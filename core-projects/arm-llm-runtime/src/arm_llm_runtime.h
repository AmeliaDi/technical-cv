#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#ifdef __ARM_NEON
#include <arm_neon.h>
#endif

namespace arm_llm {

// Forward declarations
class ModelLoader;
class HuggingFaceClient;
class QuantizationEngine;
class MemoryManager;
class InferenceEngine;

// Enums and configuration structures
enum class QuantizationType {
    NONE,
    Q4_0,    // 4-bit quantization
    Q4_K,    // 4-bit quantization with K-quants
    Q8_0,    // 8-bit quantization
    Q8_K,    // 8-bit quantization with K-quants
    CUSTOM
};

enum class ModelType {
    LLAMA,
    GPT,
    MISTRAL,
    GEMMA,
    PHI,
    MULTIMODAL
};

enum class DataType {
    FLOAT32,
    FLOAT16,
    INT8,
    INT4
};

struct QuantizationConfig {
    QuantizationType method = QuantizationType::Q4_K;
    int bits = 4;
    int group_size = 128;
    std::string calibration_dataset;
    bool symmetric = false;
    double scale_factor = 1.0;
};

struct RuntimeConfig {
    // Memory configuration
    size_t memory_pool_size = 8ULL * 1024 * 1024 * 1024; // 8GB
    bool enable_mmap = true;
    bool enable_kv_cache_compression = true;
    
    // Threading configuration
    int num_threads = std::thread::hardware_concurrency();
    bool use_thread_pool = true;
    
    // Optimization configuration
    bool use_neon = true;
    bool use_flash_attention = true;
    bool enable_speculative_decoding = false;
    
    // Cache configuration
    std::string cache_dir = "./models";
    size_t max_cache_size = 50ULL * 1024 * 1024 * 1024; // 50GB
    
    // Inference configuration
    int max_batch_size = 1;
    int max_context_length = 4096;
    float temperature = 0.7f;
    int top_k = 50;
    float top_p = 0.9f;
};

struct GenerationConfig {
    int max_tokens = 100;
    float temperature = 0.7f;
    int top_k = 50;
    float top_p = 0.9f;
    float repetition_penalty = 1.1f;
    std::vector<std::string> stop_sequences;
    bool stream = false;
    int seed = -1;
};

struct ModelInfo {
    std::string name;
    std::string path;
    ModelType type;
    size_t size_bytes;
    QuantizationType quantization;
    int vocab_size;
    int hidden_size;
    int num_layers;
    int num_heads;
    int context_length;
    std::unordered_map<std::string, std::string> metadata;
};

struct GenerationResult {
    std::string text;
    std::vector<float> logits;
    int tokens_generated;
    double generation_time;
    double tokens_per_second;
    size_t memory_used;
};

// Tensor class for efficient memory management
class Tensor {
public:
    Tensor() = default;
    Tensor(const std::vector<int64_t>& shape, DataType dtype);
    ~Tensor();
    
    // Move constructor and assignment
    Tensor(Tensor&& other) noexcept;
    Tensor& operator=(Tensor&& other) noexcept;
    
    // Copy is disabled to prevent accidental copies
    Tensor(const Tensor&) = delete;
    Tensor& operator=(const Tensor&) = delete;
    
    // Accessors
    void* data() const { return data_; }
    const std::vector<int64_t>& shape() const { return shape_; }
    DataType dtype() const { return dtype_; }
    size_t size() const { return size_; }
    size_t bytes() const { return bytes_; }
    
    // Tensor operations
    void reshape(const std::vector<int64_t>& new_shape);
    void zero();
    void fill(float value);
    
    // NEON optimized operations
    void add(const Tensor& other);
    void mul(const Tensor& other);
    void matmul(const Tensor& other, Tensor& result);
    
private:
    void* data_ = nullptr;
    std::vector<int64_t> shape_;
    DataType dtype_ = DataType::FLOAT32;
    size_t size_ = 0;
    size_t bytes_ = 0;
    bool owns_data_ = true;
    
    void allocate();
    void deallocate();
};

// Memory manager for efficient tensor allocation
class MemoryManager {
public:
    MemoryManager(size_t pool_size);
    ~MemoryManager();
    
    void* allocate(size_t bytes, size_t alignment = 32);
    void deallocate(void* ptr);
    void reset();
    
    size_t total_size() const { return pool_size_; }
    size_t used_size() const { return used_size_; }
    size_t available_size() const { return pool_size_ - used_size_; }
    
private:
    void* pool_ = nullptr;
    size_t pool_size_;
    size_t used_size_ = 0;
    std::mutex mutex_;
    
    struct Block {
        void* ptr;
        size_t size;
        bool free;
    };
    std::vector<Block> blocks_;
};

// HuggingFace client for model downloading
class HuggingFaceClient {
public:
    HuggingFaceClient(const std::string& cache_dir = "./models");
    
    // Download model from HuggingFace Hub
    std::string downloadModel(const std::string& model_id, 
                             const std::string& revision = "main");
    
    // Check if model exists locally
    bool hasModel(const std::string& model_id, 
                  const std::string& revision = "main");
    
    // Get model info
    ModelInfo getModelInfo(const std::string& model_id);
    
private:
    std::string cache_dir_;
    std::unordered_map<std::string, ModelInfo> model_cache_;
    
    std::string download_file(const std::string& url, const std::string& path);
    ModelInfo parse_config(const std::string& config_path);
};

// Quantization engine for model compression
class QuantizationEngine {
public:
    QuantizationEngine() = default;
    
    // Quantize a model
    bool quantizeModel(const std::string& input_path,
                      const std::string& output_path,
                      const QuantizationConfig& config);
    
    // Quantize tensor in-place
    void quantizeTensor(Tensor& tensor, const QuantizationConfig& config);
    
    // Dequantize tensor
    void dequantizeTensor(const Tensor& quantized, Tensor& dequantized);
    
private:
    void quantize_q4_k(const float* input, void* output, size_t size);
    void quantize_q8_0(const float* input, void* output, size_t size);
    void dequantize_q4_k(const void* input, float* output, size_t size);
    void dequantize_q8_0(const void* input, float* output, size_t size);
};

// Main model class
class Model {
public:
    Model(const ModelInfo& info, std::shared_ptr<MemoryManager> memory_manager);
    ~Model();
    
    // Generation methods
    GenerationResult generate(const std::string& prompt, 
                            const GenerationConfig& config = {});
    
    std::vector<GenerationResult> generateBatch(
        const std::vector<std::string>& prompts,
        const GenerationConfig& config = {});
    
    // Streaming generation
    void generateStream(const std::string& prompt,
                       std::function<void(const std::string&)> callback,
                       const GenerationConfig& config = {});
    
    // Model info
    const ModelInfo& info() const { return info_; }
    
    // Tokenization
    std::vector<int> tokenize(const std::string& text);
    std::string detokenize(const std::vector<int>& tokens);
    
private:
    ModelInfo info_;
    std::shared_ptr<MemoryManager> memory_manager_;
    std::unique_ptr<InferenceEngine> inference_engine_;
    
    // Model weights and parameters
    std::unordered_map<std::string, Tensor> weights_;
    std::unordered_map<std::string, std::string> tokenizer_config_;
    
    // KV cache for efficient generation
    struct KVCache {
        std::vector<Tensor> keys;
        std::vector<Tensor> values;
        int current_length = 0;
    };
    KVCache kv_cache_;
    
    void load_weights(const std::string& model_path);
    void load_tokenizer(const std::string& tokenizer_path);
    std::vector<float> forward(const std::vector<int>& tokens);
    int sample_token(const std::vector<float>& logits, const GenerationConfig& config);
};

// Inference engine with ARM optimizations
class InferenceEngine {
public:
    InferenceEngine(const RuntimeConfig& config);
    ~InferenceEngine();
    
    // Forward pass
    std::vector<float> forward(const std::vector<int>& tokens,
                              const std::unordered_map<std::string, Tensor>& weights,
                              const ModelInfo& model_info);
    
    // NEON optimized operations
    void matmul_neon(const float* a, const float* b, float* c,
                     int m, int n, int k);
    void add_neon(const float* a, const float* b, float* c, int n);
    void relu_neon(const float* input, float* output, int n);
    void softmax_neon(const float* input, float* output, int n);
    void layer_norm_neon(const float* input, const float* gamma,
                        const float* beta, float* output, int n);
    
    // Attention mechanisms
    void multi_head_attention(const Tensor& query, const Tensor& key,
                             const Tensor& value, Tensor& output,
                             int num_heads);
    
    void flash_attention(const Tensor& query, const Tensor& key,
                        const Tensor& value, Tensor& output,
                        int num_heads);
    
private:
    RuntimeConfig config_;
    std::unique_ptr<MemoryManager> memory_manager_;
    
    // Thread pool for parallel operations
    std::vector<std::thread> thread_pool_;
    std::queue<std::function<void()>> task_queue_;
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_threads_ = false;
    
    void worker_thread();
    void submit_task(std::function<void()> task);
    void initialize_thread_pool();
    void shutdown_thread_pool();
};

// Main runtime class
class ArmLLMRuntime {
public:
    ArmLLMRuntime(const RuntimeConfig& config = {});
    ~ArmLLMRuntime();
    
    // Model management
    std::shared_ptr<Model> loadModel(const std::string& model_id,
                                   QuantizationType quantization = QuantizationType::Q4_K);
    
    std::shared_ptr<Model> loadModelFromFile(const std::string& model_path,
                                           QuantizationType quantization = QuantizationType::Q4_K);
    
    void unloadModel(const std::string& model_id);
    void unloadAllModels();
    
    // Model quantization
    bool quantizeModel(const std::string& input_path,
                      const std::string& output_path,
                      const QuantizationConfig& config);
    
    // Configuration
    void configure(const RuntimeConfig& config);
    const RuntimeConfig& config() const { return config_; }
    
    // Utilities
    std::vector<std::string> listCachedModels() const;
    void clearCache();
    
    // Performance monitoring
    struct PerformanceMetrics {
        double total_inference_time = 0.0;
        double avg_tokens_per_second = 0.0;
        size_t total_tokens_generated = 0;
        size_t memory_used = 0;
        size_t cache_hits = 0;
        size_t cache_misses = 0;
    };
    
    PerformanceMetrics getMetrics() const { return metrics_; }
    void resetMetrics();
    
private:
    RuntimeConfig config_;
    std::shared_ptr<MemoryManager> memory_manager_;
    std::unique_ptr<HuggingFaceClient> hf_client_;
    std::unique_ptr<QuantizationEngine> quantization_engine_;
    
    // Model cache
    std::unordered_map<std::string, std::shared_ptr<Model>> model_cache_;
    std::mutex cache_mutex_;
    
    // Performance monitoring
    mutable std::mutex metrics_mutex_;
    PerformanceMetrics metrics_;
    
    void update_metrics(const GenerationResult& result);
    std::string get_cache_key(const std::string& model_id, QuantizationType quantization);
};

// Utility functions
namespace utils {
    std::string get_system_info();
    bool has_neon_support();
    size_t get_available_memory();
    std::string format_bytes(size_t bytes);
    double get_time_ms();
    
    // ARM-specific optimizations
    void prefetch_data(const void* ptr, size_t size);
    void memory_barrier();
}

} // namespace arm_llm 