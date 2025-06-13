#!/bin/bash

# Script para compilar WebAssembly
# Autor: Amelia Enora 🌈 Marceline Chavez Barroso

echo "🚀 Compilando WebAssembly..."

# Verificar si emscripten está instalado
if ! command -v emcc &> /dev/null; then
    echo "⚠️  Emscripten no está instalado."
    echo "Por favor instala Emscripten siguiendo las instrucciones en:"
    echo "https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

# Compilar algorithms.c a WASM
echo "📦 Compilando algorithms.c..."
emcc algorithms.c -O3 -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_malloc","_free","_quicksort","_mergesort","_heapsort","_radixsort","_countingsort","_shellsort","_cocktailsort","_gnomesort","_pancakesort","_bogosort","_sieve_of_eratosthenes","_mandelbrot_point","_generate_random_array","_is_sorted"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME='AlgorithmsModule' \
    -o algorithms.wasm

if [ $? -eq 0 ]; then
    echo "✅ algorithms.wasm compilado exitosamente"
    
    # Generar archivo JavaScript de soporte
    echo "📝 Generando archivo de soporte JavaScript..."
    cat > algorithms_wasm.js << 'EOF'
// Archivo generado automáticamente para cargar algorithms.wasm
let algorithmsModule = null;

async function loadAlgorithmsWASM() {
    if (!algorithmsModule) {
        algorithmsModule = await AlgorithmsModule();
    }
    return algorithmsModule;
}

// Exportar funciones para uso global
window.loadAlgorithmsWASM = loadAlgorithmsWASM;
EOF
    
    echo "✅ Compilación completada"
else
    echo "❌ Error al compilar algorithms.wasm"
    exit 1
fi

# Verificar el tamaño del archivo
if [ -f "algorithms.wasm" ]; then
    SIZE=$(du -h algorithms.wasm | cut -f1)
    echo "📊 Tamaño de algorithms.wasm: $SIZE"
fi 