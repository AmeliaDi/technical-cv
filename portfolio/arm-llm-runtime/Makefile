# ARM LLM Runtime Makefile

# Project configuration
PROJECT_NAME = arm-llm-runtime
VERSION = 1.0.0

# Build configuration
BUILD_TYPE ?= Release
BUILD_DIR ?= build
INSTALL_PREFIX ?= /usr/local
NUM_JOBS ?= $(shell nproc)

# Platform detection
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# Check if we're on ARM
ifeq ($(UNAME_M),aarch64)
    ARM_ARCH = arm64
else ifeq ($(UNAME_M),armv7l)
    ARM_ARCH = armv7
else
    ARM_ARCH = x86_64
endif

# Colors for output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

# Default target
.PHONY: all
all: check-deps configure build

# Help target
.PHONY: help
help:
	@echo "$(GREEN)ARM LLM Runtime Build System$(NC)"
	@echo "============================="
	@echo ""
	@echo "Available targets:"
	@echo "  $(BLUE)help$(NC)           - Show this help message"
	@echo "  $(BLUE)check-deps$(NC)     - Check system dependencies"
	@echo "  $(BLUE)deps$(NC)           - Install build dependencies"
	@echo "  $(BLUE)configure$(NC)      - Configure build with CMake"
	@echo "  $(BLUE)build$(NC)          - Build the project"
	@echo "  $(BLUE)test$(NC)           - Run tests"
	@echo "  $(BLUE)install$(NC)        - Install to system"
	@echo "  $(BLUE)package$(NC)        - Create distribution package"
	@echo "  $(BLUE)clean$(NC)          - Clean build files"
	@echo "  $(BLUE)format$(NC)         - Format source code"
	@echo "  $(BLUE)docs$(NC)           - Generate documentation"
	@echo "  $(BLUE)docker$(NC)         - Build Docker image"
	@echo "  $(BLUE)android$(NC)        - Build for Android"
	@echo "  $(BLUE)python$(NC)         - Build Python bindings"
	@echo "  $(BLUE)examples$(NC)       - Build and run examples"
	@echo ""
	@echo "Environment variables:"
	@echo "  BUILD_TYPE      - Debug or Release (default: Release)"
	@echo "  BUILD_DIR       - Build directory (default: build)"
	@echo "  INSTALL_PREFIX  - Install prefix (default: /usr/local)"
	@echo "  NUM_JOBS        - Number of parallel jobs (default: nproc)"
	@echo "  ANDROID_NDK     - Android NDK path (for Android builds)"

# Check system dependencies
.PHONY: check-deps
check-deps:
	@echo "$(GREEN)Checking system dependencies...$(NC)"
	@echo "Architecture: $(ARM_ARCH)"
	@echo "OS: $(UNAME_S)"
	@which cmake > /dev/null || (echo "$(RED)Error: CMake not found$(NC)" && exit 1)
	@which make > /dev/null || (echo "$(RED)Error: Make not found$(NC)" && exit 1)
	@which gcc > /dev/null || which clang > /dev/null || (echo "$(RED)Error: No C compiler found$(NC)" && exit 1)
	@which g++ > /dev/null || which clang++ > /dev/null || (echo "$(RED)Error: No C++ compiler found$(NC)" && exit 1)
	@pkg-config --exists libcurl || (echo "$(YELLOW)Warning: libcurl not found$(NC)")
	@pkg-config --exists json-c || (echo "$(YELLOW)Warning: json-c not found$(NC)")
	@echo "$(GREEN)Dependencies check completed$(NC)"

# Install build dependencies
.PHONY: deps
deps:
	@echo "$(GREEN)Installing build dependencies...$(NC)"
	@./scripts/build.sh deps

# Configure build
.PHONY: configure
configure: check-deps
	@echo "$(GREEN)Configuring build...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX) \
		-DBUILD_SHARED_LIBS=ON \
		-DBUILD_TESTS=ON \
		-DBUILD_EXAMPLES=ON \
		-DBUILD_PYTHON_BINDINGS=ON \
		-DENABLE_NEON=ON

# Build the project
.PHONY: build
build: configure
	@echo "$(GREEN)Building project...$(NC)"
	@cd $(BUILD_DIR) && make -j$(NUM_JOBS)
	@echo "$(GREEN)Build completed successfully!$(NC)"

# Run tests
.PHONY: test
test: build
	@echo "$(GREEN)Running tests...$(NC)"
	@cd $(BUILD_DIR) && ctest --output-on-failure -j$(NUM_JOBS)

# Install to system
.PHONY: install
install: build test
	@echo "$(GREEN)Installing to $(INSTALL_PREFIX)...$(NC)"
	@cd $(BUILD_DIR) && sudo make install
	@echo "$(GREEN)Installation completed$(NC)"

# Create distribution package
.PHONY: package
package: build test
	@echo "$(GREEN)Creating distribution package...$(NC)"
	@cd $(BUILD_DIR) && make package
	@echo "$(GREEN)Package created in $(BUILD_DIR)/$(NC)"

# Clean build files
.PHONY: clean
clean:
	@echo "$(GREEN)Cleaning build files...$(NC)"
	@rm -rf $(BUILD_DIR)
	@rm -rf build_android
	@rm -rf android_deps
	@rm -f *.deb *.rpm *.tar.gz
	@echo "$(GREEN)Clean completed$(NC)"

# Deep clean (including downloaded models)
.PHONY: distclean
distclean: clean
	@echo "$(GREEN)Deep cleaning...$(NC)"
	@rm -rf models/
	@rm -rf .cache/
	@echo "$(GREEN)Deep clean completed$(NC)"

# Format source code
.PHONY: format
format:
	@echo "$(GREEN)Formatting source code...$(NC)"
	@find src/ -name "*.cpp" -o -name "*.h" | xargs clang-format -i -style=file
	@find tools/ -name "*.cpp" -o -name "*.h" | xargs clang-format -i -style=file
	@find bindings/ -name "*.cpp" -o -name "*.h" | xargs clang-format -i -style=file
	@echo "$(GREEN)Code formatting completed$(NC)"

# Generate documentation
.PHONY: docs
docs:
	@echo "$(GREEN)Generating documentation...$(NC)"
	@doxygen Doxyfile || echo "$(YELLOW)Doxygen not found, skipping documentation generation$(NC)"

# Build Docker image
.PHONY: docker
docker:
	@echo "$(GREEN)Building Docker image...$(NC)"
	@docker build -t $(PROJECT_NAME):$(VERSION) .
	@docker tag $(PROJECT_NAME):$(VERSION) $(PROJECT_NAME):latest
	@echo "$(GREEN)Docker image built: $(PROJECT_NAME):$(VERSION)$(NC)"

# Build for Android
.PHONY: android
android:
	@echo "$(GREEN)Building for Android...$(NC)"
	@if [ -z "$(ANDROID_NDK)" ]; then \
		echo "$(RED)Error: ANDROID_NDK environment variable not set$(NC)"; \
		exit 1; \
	fi
	@./scripts/build_android.sh all
	@echo "$(GREEN)Android build completed$(NC)"

# Build Python bindings only
.PHONY: python
python: configure
	@echo "$(GREEN)Building Python bindings...$(NC)"
	@cd $(BUILD_DIR) && make arm_llm_runtime_python -j$(NUM_JOBS)
	@echo "$(GREEN)Python bindings built$(NC)"

# Build and run examples
.PHONY: examples
examples: build
	@echo "$(GREEN)Building and running examples...$(NC)"
	@echo "Running C++ example:"
	@$(BUILD_DIR)/examples/cpp_example 1 || echo "$(YELLOW)C++ example failed or needs models$(NC)"
	@echo ""
	@echo "Python examples available at:"
	@echo "  python3 examples/python_example.py"
	@echo "  python3 examples/python_example.py interactive"

# Development shortcuts
.PHONY: dev
dev: BUILD_TYPE=Debug
dev: build

.PHONY: debug
debug: BUILD_TYPE=Debug
debug: configure
	@cd $(BUILD_DIR) && make -j$(NUM_JOBS)
	@echo "$(GREEN)Debug build completed$(NC)"

# Release build
.PHONY: release
release: BUILD_TYPE=Release
release: build

# Quick build (skip tests)
.PHONY: quick
quick: configure
	@echo "$(GREEN)Quick build (skipping tests)...$(NC)"
	@cd $(BUILD_DIR) && make arm_llm_runtime -j$(NUM_JOBS)

# Benchmark target
.PHONY: benchmark
benchmark: build
	@echo "$(GREEN)Running benchmarks...$(NC)"
	@$(BUILD_DIR)/tools/arm-llm --benchmark || echo "$(YELLOW)Benchmark requires models to be downloaded$(NC)"

# Performance profiling
.PHONY: profile
profile: BUILD_TYPE=RelWithDebInfo
profile: build
	@echo "$(GREEN)Building with profiling info...$(NC)"
	@echo "Use: perf record $(BUILD_DIR)/tools/arm-llm <args>"
	@echo "Then: perf report"

# Memory check with valgrind
.PHONY: memcheck
memcheck: debug
	@echo "$(GREEN)Running memory check...$(NC)"
	@valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all \
		$(BUILD_DIR)/tools/arm-llm --system-info || echo "$(YELLOW)Valgrind not available$(NC)"

# Static analysis
.PHONY: analyze
analyze:
	@echo "$(GREEN)Running static analysis...$(NC)"
	@cppcheck --enable=all --std=c++17 src/ tools/ bindings/ || echo "$(YELLOW)cppcheck not available$(NC)"

# Coverage analysis
.PHONY: coverage
coverage: BUILD_TYPE=Debug
coverage:
	@echo "$(GREEN)Building with coverage...$(NC)"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
		-DCMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage"
	@cd $(BUILD_DIR) && make -j$(NUM_JOBS)
	@cd $(BUILD_DIR) && ctest
	@lcov --capture --directory . --output-file coverage.info || echo "$(YELLOW)lcov not available$(NC)"
	@genhtml coverage.info --output-directory coverage_html || echo "$(YELLOW)genhtml not available$(NC)"

# System information
.PHONY: sysinfo
sysinfo:
	@echo "$(GREEN)System Information:$(NC)"
	@echo "Architecture: $(UNAME_M)"
	@echo "OS: $(UNAME_S)"
	@echo "ARM Architecture: $(ARM_ARCH)"
	@echo "CPU Info:"
	@cat /proc/cpuinfo | grep -E "(processor|model name|Features|CPU implementer|CPU architecture|CPU variant|CPU part|CPU revision)" | head -20 || echo "CPU info not available"
	@echo "Memory:"
	@free -h || echo "Memory info not available"

# Pre-commit checks
.PHONY: pre-commit
pre-commit: format analyze test
	@echo "$(GREEN)Pre-commit checks completed$(NC)"

# Continuous integration target
.PHONY: ci
ci: check-deps build test package
	@echo "$(GREEN)CI pipeline completed$(NC)"

# Show build status
.PHONY: status
status:
	@echo "$(GREEN)Build Status:$(NC)"
	@echo "Project: $(PROJECT_NAME) v$(VERSION)"
	@echo "Build type: $(BUILD_TYPE)"
	@echo "Build directory: $(BUILD_DIR)"
	@echo "Install prefix: $(INSTALL_PREFIX)"
	@echo "Parallel jobs: $(NUM_JOBS)"
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "Build directory exists: Yes"; \
		echo "Built targets:"; \
		find $(BUILD_DIR) -name "*.so" -o -name "*.a" -o -name "arm-llm" | sed 's/^/  /'; \
	else \
		echo "Build directory exists: No"; \
	fi

# Configuration targets for different use cases
.PHONY: config-minimal
config-minimal:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DBUILD_SHARED_LIBS=OFF \
		-DBUILD_TESTS=OFF \
		-DBUILD_EXAMPLES=OFF \
		-DBUILD_PYTHON_BINDINGS=OFF

.PHONY: config-full
config-full:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DBUILD_SHARED_LIBS=ON \
		-DBUILD_TESTS=ON \
		-DBUILD_EXAMPLES=ON \
		-DBUILD_PYTHON_BINDINGS=ON \
		-DENABLE_NEON=ON \
		-DENABLE_BENCHMARKS=ON

# Make build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR) 