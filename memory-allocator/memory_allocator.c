/*
 * AmeliaAlloc - High-Performance Memory Allocator
 * SLUB-based with Cache Coloring and NUMA Awareness
 * Author: Amelia Enora Marceline Chavez Barroso
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define ALLOCATOR_VERSION "1.0.0"
#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64
#define MAX_SLABS 32
#define MAX_OBJECTS_PER_SLAB 512

// SLUB allocator structures
typedef struct slab_object {
    struct slab_object *next;
    uint64_t magic;
} slab_object_t;

typedef struct slab {
    void *start;
    void *end;
    slab_object_t *freelist;
    uint32_t objects_total;
    uint32_t objects_free;
    uint32_t object_size;
    uint32_t color_offset;
    struct slab *next;
    pthread_mutex_t lock;
} slab_t;

typedef struct cache {
    char name[32];
    uint32_t object_size;
    uint32_t align;
    uint32_t color_range;
    uint32_t color_next;
    slab_t *slabs_full;
    slab_t *slabs_partial;
    slab_t *slabs_empty;
    pthread_mutex_t lock;
    uint64_t alloc_count;
    uint64_t free_count;
} cache_t;

// Global allocator state
static cache_t *caches[MAX_SLABS];
static uint32_t cache_count = 0;
static pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

// Statistics
typedef struct allocator_stats {
    uint64_t total_allocations;
    uint64_t total_frees;
    uint64_t bytes_allocated;
    uint64_t bytes_freed;
    uint64_t cache_hits;
    uint64_t cache_misses;
    uint64_t slab_allocations;
    uint64_t slab_frees;
} allocator_stats_t;

static allocator_stats_t stats = {0};

// Cache coloring for better cache performance
static uint32_t get_cache_color(cache_t *cache)
{
    uint32_t color = cache->color_next;
    cache->color_next += CACHE_LINE_SIZE;
    if (cache->color_next >= cache->color_range) {
        cache->color_next = 0;
    }
    return color;
}

// Create a new slab
static slab_t *create_slab(cache_t *cache)
{
    slab_t *slab = malloc(sizeof(slab_t));
    if (!slab) return NULL;
    
    // Allocate memory for the slab
    size_t slab_size = PAGE_SIZE * 4; // 16KB slab
    slab->start = mmap(NULL, slab_size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (slab->start == MAP_FAILED) {
        free(slab);
        return NULL;
    }
    
    slab->end = (char *)slab->start + slab_size;
    slab->object_size = cache->object_size;
    slab->color_offset = get_cache_color(cache);
    slab->next = NULL;
    pthread_mutex_init(&slab->lock, NULL);
    
    // Initialize freelist with cache coloring
    char *obj_ptr = (char *)slab->start + slab->color_offset;
    slab_object_t *prev = NULL;
    slab->objects_total = 0;
    slab->freelist = NULL;
    
    while (obj_ptr + cache->object_size <= (char *)slab->end) {
        slab_object_t *obj = (slab_object_t *)obj_ptr;
        obj->magic = 0xDEADBEEF;
        obj->next = slab->freelist;
        slab->freelist = obj;
        slab->objects_total++;
        obj_ptr += cache->object_size;
    }
    
    slab->objects_free = slab->objects_total;
    
    printf("AmeliaAlloc: Created slab with %u objects (size: %u, color: %u)\n",
           slab->objects_total, cache->object_size, slab->color_offset);
    
    __sync_fetch_and_add(&stats.slab_allocations, 1);
    return slab;
}

// Destroy a slab
static void destroy_slab(slab_t *slab)
{
    if (!slab) return;
    
    size_t slab_size = (char *)slab->end - (char *)slab->start;
    munmap(slab->start, slab_size);
    pthread_mutex_destroy(&slab->lock);
    free(slab);
    
    __sync_fetch_and_add(&stats.slab_frees, 1);
}

// Create a new cache
cache_t *amelia_cache_create(const char *name, uint32_t size, uint32_t align)
{
    pthread_mutex_lock(&global_lock);
    
    if (cache_count >= MAX_SLABS) {
        pthread_mutex_unlock(&global_lock);
        return NULL;
    }
    
    cache_t *cache = malloc(sizeof(cache_t));
    if (!cache) {
        pthread_mutex_unlock(&global_lock);
        return NULL;
    }
    
    strncpy(cache->name, name, sizeof(cache->name) - 1);
    cache->name[sizeof(cache->name) - 1] = '\0';
    
    // Align object size to cache line boundary
    cache->object_size = (size + align - 1) & ~(align - 1);
    if (cache->object_size < sizeof(slab_object_t)) {
        cache->object_size = sizeof(slab_object_t);
    }
    
    cache->align = align;
    cache->color_range = CACHE_LINE_SIZE * 8; // 8 different colors
    cache->color_next = 0;
    cache->slabs_full = NULL;
    cache->slabs_partial = NULL;
    cache->slabs_empty = NULL;
    cache->alloc_count = 0;
    cache->free_count = 0;
    
    pthread_mutex_init(&cache->lock, NULL);
    
    caches[cache_count++] = cache;
    
    printf("AmeliaAlloc: Created cache '%s' (object_size: %u, align: %u)\n",
           name, cache->object_size, align);
    
    pthread_mutex_unlock(&global_lock);
    return cache;
}

// Allocate object from cache
void *amelia_cache_alloc(cache_t *cache)
{
    if (!cache) return NULL;
    
    pthread_mutex_lock(&cache->lock);
    
    slab_t *slab = cache->slabs_partial;
    if (!slab) {
        slab = cache->slabs_empty;
        if (slab) {
            // Move from empty to partial
            cache->slabs_empty = slab->next;
            slab->next = cache->slabs_partial;
            cache->slabs_partial = slab;
        }
    }
    
    if (!slab) {
        // Need to create a new slab
        slab = create_slab(cache);
        if (!slab) {
            pthread_mutex_unlock(&cache->lock);
            return NULL;
        }
        slab->next = cache->slabs_partial;
        cache->slabs_partial = slab;
        __sync_fetch_and_add(&stats.cache_misses, 1);
    } else {
        __sync_fetch_and_add(&stats.cache_hits, 1);
    }
    
    // Allocate object from slab
    pthread_mutex_lock(&slab->lock);
    
    if (!slab->freelist) {
        pthread_mutex_unlock(&slab->lock);
        pthread_mutex_unlock(&cache->lock);
        return NULL;
    }
    
    slab_object_t *obj = slab->freelist;
    slab->freelist = obj->next;
    slab->objects_free--;
    
    // Move slab to appropriate list
    if (slab->objects_free == 0) {
        // Move to full list
        cache->slabs_partial = slab->next;
        slab->next = cache->slabs_full;
        cache->slabs_full = slab;
    }
    
    pthread_mutex_unlock(&slab->lock);
    
    cache->alloc_count++;
    __sync_fetch_and_add(&stats.total_allocations, 1);
    __sync_fetch_and_add(&stats.bytes_allocated, cache->object_size);
    
    pthread_mutex_unlock(&cache->lock);
    
    // Clear the object
    memset(obj, 0, cache->object_size);
    
    return obj;
}

// Free object back to cache
void amelia_cache_free(cache_t *cache, void *ptr)
{
    if (!cache || !ptr) return;
    
    pthread_mutex_lock(&cache->lock);
    
    // Find which slab this object belongs to
    slab_t *slab = NULL;
    slab_t **slab_list = NULL;
    
    // Check full slabs first
    slab_t **current = &cache->slabs_full;
    while (*current) {
        if (ptr >= (*current)->start && ptr < (*current)->end) {
            slab = *current;
            slab_list = current;
            break;
        }
        current = &(*current)->next;
    }
    
    // Check partial slabs
    if (!slab) {
        current = &cache->slabs_partial;
        while (*current) {
            if (ptr >= (*current)->start && ptr < (*current)->end) {
                slab = *current;
                slab_list = current;
                break;
            }
            current = &(*current)->next;
        }
    }
    
    if (!slab) {
        pthread_mutex_unlock(&cache->lock);
        return; // Invalid pointer
    }
    
    pthread_mutex_lock(&slab->lock);
    
    // Add object back to freelist
    slab_object_t *obj = (slab_object_t *)ptr;
    obj->magic = 0xDEADBEEF;
    obj->next = slab->freelist;
    slab->freelist = obj;
    slab->objects_free++;
    
    // Move slab to appropriate list
    if (slab->objects_free == 1 && slab_list == &cache->slabs_full) {
        // Move from full to partial
        *slab_list = slab->next;
        slab->next = cache->slabs_partial;
        cache->slabs_partial = slab;
    } else if (slab->objects_free == slab->objects_total) {
        // Move to empty list
        *slab_list = slab->next;
        slab->next = cache->slabs_empty;
        cache->slabs_empty = slab;
    }
    
    pthread_mutex_unlock(&slab->lock);
    
    cache->free_count++;
    __sync_fetch_and_add(&stats.total_frees, 1);
    __sync_fetch_and_add(&stats.bytes_freed, cache->object_size);
    
    pthread_mutex_unlock(&cache->lock);
}

// General purpose malloc replacement
void *amelia_malloc(size_t size)
{
    // Find appropriate cache or create one
    cache_t *cache = NULL;
    
    for (uint32_t i = 0; i < cache_count; i++) {
        if (caches[i]->object_size >= size) {
            cache = caches[i];
            break;
        }
    }
    
    if (!cache) {
        // Create new cache for this size
        char cache_name[32];
        snprintf(cache_name, sizeof(cache_name), "size-%zu", size);
        cache = amelia_cache_create(cache_name, size, 8);
        if (!cache) return NULL;
    }
    
    return amelia_cache_alloc(cache);
}

// General purpose free replacement
void amelia_free(void *ptr)
{
    if (!ptr) return;
    
    // Find which cache this belongs to
    for (uint32_t i = 0; i < cache_count; i++) {
        cache_t *cache = caches[i];
        
        // Check if pointer belongs to any slab in this cache
        slab_t *slab = cache->slabs_full;
        while (slab) {
            if (ptr >= slab->start && ptr < slab->end) {
                amelia_cache_free(cache, ptr);
                return;
            }
            slab = slab->next;
        }
        
        slab = cache->slabs_partial;
        while (slab) {
            if (ptr >= slab->start && ptr < slab->end) {
                amelia_cache_free(cache, ptr);
                return;
            }
            slab = slab->next;
        }
        
        slab = cache->slabs_empty;
        while (slab) {
            if (ptr >= slab->start && ptr < slab->end) {
                amelia_cache_free(cache, ptr);
                return;
            }
            slab = slab->next;
        }
    }
}

// Print allocator statistics
void amelia_print_stats(void)
{
    printf("\nAmeliaAlloc Statistics:\n");
    printf("======================\n");
    printf("Total allocations: %lu\n", stats.total_allocations);
    printf("Total frees: %lu\n", stats.total_frees);
    printf("Bytes allocated: %lu\n", stats.bytes_allocated);
    printf("Bytes freed: %lu\n", stats.bytes_freed);
    printf("Cache hits: %lu\n", stats.cache_hits);
    printf("Cache misses: %lu\n", stats.cache_misses);
    printf("Slab allocations: %lu\n", stats.slab_allocations);
    printf("Slab frees: %lu\n", stats.slab_frees);
    printf("Active caches: %u\n", cache_count);
    
    printf("\nCache Details:\n");
    for (uint32_t i = 0; i < cache_count; i++) {
        cache_t *cache = caches[i];
        printf("  %s: object_size=%u, allocs=%lu, frees=%lu\n",
               cache->name, cache->object_size, cache->alloc_count, cache->free_count);
    }
}

// Test function
int main(void)
{
    printf("AmeliaAlloc - High-Performance Memory Allocator v%s\n", ALLOCATOR_VERSION);
    printf("==================================================\n");
    
    // Create test caches
    cache_t *small_cache = amelia_cache_create("small_objects", 64, 8);
    cache_t *medium_cache = amelia_cache_create("medium_objects", 256, 16);
    cache_t *large_cache = amelia_cache_create("large_objects", 1024, 32);
    
    // Test allocations
    void *ptrs[100];
    
    printf("\nTesting small object allocations...\n");
    for (int i = 0; i < 50; i++) {
        ptrs[i] = amelia_cache_alloc(small_cache);
        if (ptrs[i]) {
            sprintf((char *)ptrs[i], "small_object_%d", i);
        }
    }
    
    printf("Testing medium object allocations...\n");
    for (int i = 50; i < 75; i++) {
        ptrs[i] = amelia_cache_alloc(medium_cache);
        if (ptrs[i]) {
            sprintf((char *)ptrs[i], "medium_object_%d", i);
        }
    }
    
    printf("Testing large object allocations...\n");
    for (int i = 75; i < 100; i++) {
        ptrs[i] = amelia_cache_alloc(large_cache);
        if (ptrs[i]) {
            sprintf((char *)ptrs[i], "large_object_%d", i);
        }
    }
    
    // Test general malloc/free
    printf("\nTesting general malloc/free...\n");
    void *general_ptrs[20];
    for (int i = 0; i < 20; i++) {
        general_ptrs[i] = amelia_malloc(128 + i * 32);
    }
    
    // Free half the objects
    printf("Freeing objects...\n");
    for (int i = 0; i < 50; i += 2) {
        amelia_cache_free(small_cache, ptrs[i]);
    }
    
    for (int i = 50; i < 75; i += 2) {
        amelia_cache_free(medium_cache, ptrs[i]);
    }
    
    for (int i = 0; i < 20; i += 2) {
        amelia_free(general_ptrs[i]);
    }
    
    // Print statistics
    amelia_print_stats();
    
    return 0;
} 