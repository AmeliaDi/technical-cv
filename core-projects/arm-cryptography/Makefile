# ARM Cryptography Suite - Advanced Makefile
# Copyright (c) 2024 Amelia Enora
#
# High-performance cryptographic library optimized for ARM processors

# ================================
# Build Configuration
# ================================

CC = gcc
CXX = g++
AR = ar
STRIP = strip

# ARM-specific optimization flags
ARCH_FLAGS = -march=armv8-a+crypto+crc -mtune=cortex-a78
NEON_FLAGS = -mfpu=neon -funsafe-math-optimizations
SVE_FLAGS = -march=armv8-a+sve
CRYPTO_FLAGS = -march=armv8-a+crypto+aes+sha2+sha3

# Performance optimization flags
OPT_FLAGS = -O3 -flto -ffast-math -funroll-loops -fprefetch-loop-arrays
OPT_FLAGS += -fomit-frame-pointer -finline-functions -falign-functions=32

# Security hardening flags
SECURITY_FLAGS = -fstack-protector-strong -D_FORTIFY_SOURCE=2
SECURITY_FLAGS += -Wformat -Wformat-security -fPIE
SECURITY_FLAGS += -fcf-protection=full -mspectre-load-hardening

# Debugging and analysis flags  
DEBUG_FLAGS = -g3 -gdwarf-4 -fno-omit-frame-pointer
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined
TIMING_FLAGS = -DCONSTANT_TIME_VALIDATION

# Warning flags
WARN_FLAGS = -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion
WARN_FLAGS += -Wcast-align -Wcast-qual -Wdisabled-optimization
WARN_FLAGS += -Wfloat-equal -Wformat=2 -Wimport -Winit-self
WARN_FLAGS += -Winvalid-pch -Wmissing-field-initializers
WARN_FLAGS += -Wmissing-format-attribute -Wmissing-include-dirs
WARN_FLAGS += -Wmissing-noreturn -Wpacked -Wpadded -Wpointer-arith
WARN_FLAGS += -Wredundant-decls -Wshadow -Wstack-protector
WARN_FLAGS += -Wstrict-aliasing=2 -Wswitch-default -Wswitch-enum
WARN_FLAGS += -Wunreachable-code -Wunused -Wvariadic-macros
WARN_FLAGS += -Wwrite-strings

# Include directories
INCLUDES = -Iinclude -Isrc -I/usr/include/openssl

# Library dependencies
LIBS = -lcrypto -lssl -lgmp -lmpfr -lpthread -lrt -ldl

# Build variants
CFLAGS_RELEASE = $(ARCH_FLAGS) $(OPT_FLAGS) $(SECURITY_FLAGS) $(WARN_FLAGS) $(INCLUDES)
CFLAGS_DEBUG = $(ARCH_FLAGS) $(DEBUG_FLAGS) $(SECURITY_FLAGS) $(WARN_FLAGS) $(INCLUDES)
CFLAGS_ASAN = $(ARCH_FLAGS) $(DEBUG_FLAGS) $(ASAN_FLAGS) $(WARN_FLAGS) $(INCLUDES)
CFLAGS_TIMING = $(ARCH_FLAGS) $(OPT_FLAGS) $(TIMING_FLAGS) $(WARN_FLAGS) $(INCLUDES)

# Default build type
BUILD_TYPE ?= release
ifeq ($(BUILD_TYPE),debug)
    CFLAGS = $(CFLAGS_DEBUG)
else ifeq ($(BUILD_TYPE),asan)
    CFLAGS = $(CFLAGS_ASAN)
else ifeq ($(BUILD_TYPE),timing)
    CFLAGS = $(CFLAGS_TIMING)
else
    CFLAGS = $(CFLAGS_RELEASE)
endif

# ================================
# Directory Structure
# ================================

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib
TEST_DIR = tests
BENCH_DIR = benchmarks
EXAMPLE_DIR = examples

# Create directories
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR))

# ================================
# Source Files
# ================================

# Core cryptographic implementations
CORE_SRCS = $(wildcard $(SRC_DIR)/core/*.c)
CORE_OBJS = $(CORE_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Post-quantum cryptography
PQC_SRCS = $(wildcard $(SRC_DIR)/pqc/*.c)
PQC_OBJS = $(PQC_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Homomorphic encryption
HE_SRCS = $(wildcard $(SRC_DIR)/homomorphic/*.c)
HE_OBJS = $(HE_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Zero-knowledge proofs
ZK_SRCS = $(wildcard $(SRC_DIR)/zkproofs/*.c)
ZK_OBJS = $(ZK_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ARM assembly optimizations
ASM_SRCS = $(wildcard $(SRC_DIR)/asm/*.s)
ASM_OBJS = $(ASM_SRCS:$(SRC_DIR)/%.s=$(OBJ_DIR)/%.o)

# Utilities and helpers
UTIL_SRCS = $(wildcard $(SRC_DIR)/utils/*.c)
UTIL_OBJS = $(UTIL_SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# All objects
ALL_OBJS = $(CORE_OBJS) $(PQC_OBJS) $(HE_OBJS) $(ZK_OBJS) $(ASM_OBJS) $(UTIL_OBJS)

# Test files
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BIN_DIR)/test_%)

# Benchmark files
BENCH_SRCS = $(wildcard $(BENCH_DIR)/*.c)
BENCH_BINS = $(BENCH_SRCS:$(BENCH_DIR)/%.c=$(BIN_DIR)/bench_%)

# Example files
EXAMPLE_SRCS = $(wildcard $(EXAMPLE_DIR)/*.c)
EXAMPLE_BINS = $(EXAMPLE_SRCS:$(EXAMPLE_DIR)/%.c=$(BIN_DIR)/%)

# ================================
# Build Targets
# ================================

.PHONY: all clean install uninstall test benchmark examples docs
.PHONY: release debug asan timing
.PHONY: check-deps check-hardware lint format
.PHONY: fuzz coverage static-analysis

# Default target
all: check-deps check-hardware $(LIB_DIR)/libarm_crypto.a $(LIB_DIR)/libarm_crypto.so

# Static library
$(LIB_DIR)/libarm_crypto.a: $(ALL_OBJS)
	@echo "Creating static library: $@"
	$(AR) rcs $@ $^
	@echo "✅ Static library built successfully"

# Shared library
$(LIB_DIR)/libarm_crypto.so: $(ALL_OBJS)
	@echo "Creating shared library: $@"
	$(CC) -shared -o $@ $^ $(LIBS)
	@echo "✅ Shared library built successfully"

# Object file compilation rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling: $<"
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(dir $@)
	@echo "Assembling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# ================================
# Build Variants
# ================================

release:
	@$(MAKE) BUILD_TYPE=release all
	@echo "🚀 Release build completed"

debug:
	@$(MAKE) BUILD_TYPE=debug all
	@echo "🐛 Debug build completed"

asan:
	@$(MAKE) BUILD_TYPE=asan all
	@echo "🛡️ AddressSanitizer build completed"

timing:
	@$(MAKE) BUILD_TYPE=timing all
	@echo "⏱️ Timing analysis build completed"

# ================================
# Testing & Validation
# ================================

test: $(TEST_BINS)
	@echo "🧪 Running test suite..."
	@for test in $(TEST_BINS); do \
		echo "Running $$test"; \
		$$test || exit 1; \
	done
	@echo "✅ All tests passed"

$(BIN_DIR)/test_%: $(TEST_DIR)/%.c $(LIB_DIR)/libarm_crypto.a
	@echo "Building test: $@"
	$(CC) $(CFLAGS) $< -o $@ -L$(LIB_DIR) -larm_crypto $(LIBS)

# Performance benchmarks
benchmark: $(BENCH_BINS)
	@echo "⚡ Running benchmarks..."
	@for bench in $(BENCH_BINS); do \
		echo "Running $$bench"; \
		$$bench; \
	done

$(BIN_DIR)/bench_%: $(BENCH_DIR)/%.c $(LIB_DIR)/libarm_crypto.a
	@echo "Building benchmark: $@"
	$(CC) $(CFLAGS) $< -o $@ -L$(LIB_DIR) -larm_crypto $(LIBS)

# Example programs
examples: $(EXAMPLE_BINS)
	@echo "📚 Built example programs"

$(BIN_DIR)/%: $(EXAMPLE_DIR)/%.c $(LIB_DIR)/libarm_crypto.a
	@echo "Building example: $@"
	$(CC) $(CFLAGS) $< -o $@ -L$(LIB_DIR) -larm_crypto $(LIBS)

# ================================
# Security & Quality Assurance
# ================================

# Static analysis with cppcheck
static-analysis:
	@echo "🔍 Running static analysis..."
	cppcheck --enable=all --std=c11 --platform=unix64 \
		--suppress=missingIncludeSystem \
		--inline-suppr $(SRC_DIR) $(INCLUDE_DIR)
	@echo "✅ Static analysis completed"

# Code formatting with clang-format
format:
	@echo "🎨 Formatting code..."
	find $(SRC_DIR) $(INCLUDE_DIR) $(TEST_DIR) $(BENCH_DIR) $(EXAMPLE_DIR) \
		-name "*.c" -o -name "*.h" | xargs clang-format -i
	@echo "✅ Code formatting completed"

# Linting with clang-tidy
lint:
	@echo "🔍 Running linter..."
	find $(SRC_DIR) -name "*.c" | xargs clang-tidy --checks='-*,readability-*,performance-*,security-*'
	@echo "✅ Linting completed"

# Fuzzing with AFL++
fuzz:
	@echo "🐛 Building for fuzzing..."
	AFL_CC=$(CC) AFL_CXX=$(CXX) $(MAKE) BUILD_TYPE=asan
	@echo "✅ Fuzzing build ready"

# Code coverage with gcov
coverage:
	@echo "📊 Generating coverage report..."
	$(MAKE) BUILD_TYPE=debug CFLAGS="$(CFLAGS_DEBUG) --coverage"
	@$(MAKE) test
	gcov $(SRC_DIR)/*.c
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_html
	@echo "✅ Coverage report generated in coverage_html/"

# ================================
# Hardware & Dependency Checks
# ================================

check-hardware:
	@echo "🔍 Checking ARM hardware capabilities..."
	@if [ -f /proc/cpuinfo ]; then \
		echo "CPU Info:"; \
		grep -E "(processor|model name|Features|CPU architecture)" /proc/cpuinfo | head -20; \
		echo ""; \
		if grep -q "aes" /proc/cpuinfo; then \
			echo "✅ ARM Crypto Extensions detected"; \
		else \
			echo "❌ ARM Crypto Extensions not found"; \
		fi; \
		if grep -q "neon" /proc/cpuinfo; then \
			echo "✅ NEON SIMD support detected"; \
		else \
			echo "❌ NEON SIMD support not found"; \
		fi; \
		if grep -q "sve" /proc/cpuinfo; then \
			echo "✅ SVE support detected"; \
		else \
			echo "⚠️ SVE support not detected (optional)"; \
		fi; \
	else \
		echo "⚠️ Cannot detect ARM capabilities (not on Linux)"; \
	fi

check-deps:
	@echo "🔍 Checking dependencies..."
	@pkg-config --exists openssl || (echo "❌ OpenSSL not found" && exit 1)
	@which gcc > /dev/null || (echo "❌ GCC not found" && exit 1)
	@which ar > /dev/null || (echo "❌ ar not found" && exit 1)
	@echo "✅ All dependencies satisfied"

# ================================
# Documentation
# ================================

docs:
	@echo "📚 Generating documentation..."
	doxygen Doxyfile
	@echo "✅ Documentation generated in docs/"

# ================================
# Installation
# ================================

PREFIX ?= /usr/local
INCLUDEDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
BINDIR = $(PREFIX)/bin

install: all
	@echo "📦 Installing ARM Cryptography Suite..."
	install -d $(DESTDIR)$(INCLUDEDIR)/arm_crypto
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(BINDIR)
	
	# Install headers
	install -m 644 $(INCLUDE_DIR)/*.h $(DESTDIR)$(INCLUDEDIR)/arm_crypto/
	
	# Install libraries
	install -m 644 $(LIB_DIR)/libarm_crypto.a $(DESTDIR)$(LIBDIR)/
	install -m 755 $(LIB_DIR)/libarm_crypto.so $(DESTDIR)$(LIBDIR)/
	
	# Install example binaries
	if [ -d $(BIN_DIR) ]; then \
		find $(BIN_DIR) -type f -executable | while read bin; do \
			install -m 755 $$bin $(DESTDIR)$(BINDIR)/; \
		done \
	fi
	
	# Update library cache
	ldconfig
	@echo "✅ Installation completed"

uninstall:
	@echo "🗑️ Uninstalling ARM Cryptography Suite..."
	rm -rf $(DESTDIR)$(INCLUDEDIR)/arm_crypto
	rm -f $(DESTDIR)$(LIBDIR)/libarm_crypto.*
	rm -f $(DESTDIR)$(BINDIR)/arm_crypto_*
	ldconfig
	@echo "✅ Uninstallation completed"

# ================================
# Performance Profiling
# ================================

profile:
	@echo "📊 Building with profiling support..."
	$(MAKE) BUILD_TYPE=debug CFLAGS="$(CFLAGS_DEBUG) -pg"
	@echo "✅ Profiling build ready. Run 'gprof' after execution."

perf-test:
	@echo "⚡ Running performance tests..."
	perf record -g $(BIN_DIR)/bench_crypto
	perf report
	@echo "✅ Performance analysis completed"

# ================================
# Cleanup
# ================================

clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	rm -f *.gcda *.gcno *.gcov coverage.info
	rm -rf coverage_html docs
	@echo "✅ Cleanup completed"

clean-all: clean
	@echo "🧹 Deep cleaning..."
	rm -rf .git/hooks/pre-commit
	find . -name "*~" -delete
	find . -name "*.bak" -delete
	@echo "✅ Deep cleanup completed"

# ================================
# Help System
# ================================

help:
	@echo "🛡️ ARM Cryptography Suite Build System"
	@echo ""
	@echo "Main targets:"
	@echo "  all          - Build all libraries (default)"
	@echo "  release      - Optimized release build"
	@echo "  debug        - Debug build with symbols"
	@echo "  asan         - Build with AddressSanitizer"
	@echo "  timing       - Build with timing validation"
	@echo ""
	@echo "Testing & Quality:"
	@echo "  test         - Run test suite"
	@echo "  benchmark    - Run performance benchmarks"
	@echo "  examples     - Build example programs"
	@echo "  lint         - Run code linter"
	@echo "  format       - Format source code"
	@echo "  static-analysis - Run static analysis"
	@echo "  coverage     - Generate coverage report"
	@echo "  fuzz         - Build for fuzzing"
	@echo ""
	@echo "Installation:"
	@echo "  install      - Install to system (PREFIX=/usr/local)"
	@echo "  uninstall    - Remove from system"
	@echo ""
	@echo "Utilities:"
	@echo "  docs         - Generate documentation"
	@echo "  clean        - Remove build artifacts"
	@echo "  check-deps   - Check dependencies"
	@echo "  check-hardware - Check ARM capabilities"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Environment variables:"
	@echo "  BUILD_TYPE   - release|debug|asan|timing (default: release)"
	@echo "  PREFIX       - Install prefix (default: /usr/local)"
	@echo "  CC           - C compiler (default: gcc)"

# ================================
# Special Targets
# ================================

# Prevent make from deleting intermediate files
.SECONDARY:

# Disable built-in rules
.SUFFIXES: 