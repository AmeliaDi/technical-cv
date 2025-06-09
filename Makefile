# Makefile for WebAssembly CV Project
# Requires Emscripten SDK for compilation

CC = emcc
CFLAGS = -O3 -s WASM=1 -s EXPORTED_FUNCTIONS='["_quicksort","_mergesort","_heapsort","_sieve_of_eratosthenes","_mandelbrot_point","_fft_real","_generate_sine_wave","_generate_square_wave","_generate_sawtooth_wave","_init_memory","_get_memory_ptr","_fill_random_array","_is_array_sorted"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' -s ALLOW_MEMORY_GROWTH=1

# Source and output files
SOURCES = algorithms.c
OUTPUT = algorithms.wasm
OUTPUT_JS = algorithms.js

# Default target
all: $(OUTPUT)

# Compile C to WebAssembly
$(OUTPUT): $(SOURCES)
	@echo "🔧 Compiling C algorithms to WebAssembly..."
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUTPUT_JS)
	@echo "✅ WebAssembly compilation complete!"
	@echo "📁 Generated files: $(OUTPUT_JS), $(OUTPUT)"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(OUTPUT) $(OUTPUT_JS)
	@echo "✅ Clean complete!"

# Install Emscripten (Linux/macOS)
install-emscripten:
	@echo "📦 Installing Emscripten SDK..."
	git clone https://github.com/emscripten-core/emsdk.git
	cd emsdk && ./emsdk install latest && ./emsdk activate latest
	@echo "✅ Emscripten installed! Run: source emsdk/emsdk_env.sh"

# Development server
serve:
	@echo "🌐 Starting development server on port 8000..."
	@which python3 > /dev/null && python3 -m http.server 8000 || python -m SimpleHTTPServer 8000

# Alternative server using Node.js
serve-node:
	@echo "🌐 Starting Node.js server on port 8000..."
	npx http-server -p 8000 -c-1

# Test compilation without Emscripten (fallback)
test-gcc:
	@echo "🧪 Testing compilation with GCC..."
	gcc -Wall -Wextra -std=c99 -lm algorithms.c -o algorithms_test
	@echo "✅ GCC compilation test passed!"
	rm -f algorithms_test

# Performance benchmark
benchmark: $(OUTPUT)
	@echo "📊 Running performance benchmark..."
	@echo "Open the web page and run: performanceBenchmark.benchmarkSortingAlgorithms()"

# Lint C code
lint:
	@echo "🔍 Linting C code..."
	@which cppcheck > /dev/null && cppcheck --enable=all algorithms.c || echo "⚠️  cppcheck not found"

# Format C code
format:
	@echo "🎨 Formatting C code..."
	@which clang-format > /dev/null && clang-format -i algorithms.c || echo "⚠️  clang-format not found"

# Create production build
production: clean $(OUTPUT)
	@echo "🚀 Creating production build..."
	@echo "📦 Optimizing files..."
	# Minify JavaScript files (requires uglify-js)
	@which uglifyjs > /dev/null && \
		uglifyjs main.js -o main.min.js && \
		uglifyjs algorithms.js -o algorithms.min.js && \
		uglifyjs visualizations.js -o visualizations.min.js && \
		uglifyjs network-tools.js -o network-tools.min.js && \
		uglifyjs wasm-loader.js -o wasm-loader.min.js && \
		echo "✅ JavaScript files minified!" || \
		echo "⚠️  uglifyjs not found, skipping JS minification"
	# Minify CSS (requires cleancss)
	@which cleancss > /dev/null && \
		cleancss -o styles.min.css styles.css && \
		echo "✅ CSS files minified!" || \
		echo "⚠️  cleancss not found, skipping CSS minification"
	@echo "🎉 Production build complete!"

# Development setup
setup:
	@echo "🛠️  Setting up development environment..."
	@echo "📋 Installing dependencies..."
	# Check for required tools
	@which gcc > /dev/null || echo "❌ GCC not found - install build tools"
	@which python3 > /dev/null || echo "❌ Python3 not found"
	@which node > /dev/null || echo "❌ Node.js not found"
	@echo "💡 To compile to WebAssembly, install Emscripten: make install-emscripten"
	@echo "✅ Setup information displayed!"

# Show help
help:
	@echo "🆘 Available commands:"
	@echo "  make all          - Compile C to WebAssembly"
	@echo "  make clean        - Clean build artifacts"
	@echo "  make serve        - Start development server"
	@echo "  make setup        - Setup development environment"
	@echo "  make lint         - Lint C code"
	@echo "  make format       - Format C code"
	@echo "  make production   - Create production build"
	@echo "  make help         - Show this help"

# Debug build with symbols
debug:
	@echo "🐛 Creating debug build..."
	$(CC) -g -s WASM=1 -s EXPORTED_FUNCTIONS='["_quicksort","_mergesort","_heapsort","_sieve_of_eratosthenes","_mandelbrot_point"]' -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' $(SOURCES) -o debug_$(OUTPUT_JS)
	@echo "✅ Debug build complete: debug_$(OUTPUT_JS)"

# Assembly output for inspection
assembly:
	@echo "🔍 Generating assembly output..."
	$(CC) -S $(CFLAGS) $(SOURCES) -o algorithms.s
	@echo "✅ Assembly generated: algorithms.s"

.PHONY: all clean serve serve-node test-gcc benchmark lint format production setup help debug assembly install-emscripten 