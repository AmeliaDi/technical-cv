# ARM Neural Network Makefile
# Supports both AArch64 and ARMv7-A

# Compiler toolchains
AARCH64_AS = aarch64-linux-gnu-as
AARCH64_LD = aarch64-linux-gnu-ld
AARCH64_GCC = aarch64-linux-gnu-gcc
AARCH64_OBJDUMP = aarch64-linux-gnu-objdump

ARM_AS = arm-linux-gnueabihf-as
ARM_LD = arm-linux-gnueabihf-ld
ARM_GCC = arm-linux-gnueabihf-gcc
ARM_OBJDUMP = arm-linux-gnueabihf-objdump

# Build flags
AARCH64_ASFLAGS = -march=armv8-a
ARM_ASFLAGS = -march=armv7-a -mfpu=neon

# Targets
AARCH64_TARGET = neural_net_aarch64
ARM_TARGET = neural_net_armv7

# Source files
AARCH64_SRC = neural_net.s
ARM_SRC = neural_net_v7.s
COMMON_SRC = matrix_ops.s activations.s memory_pool.s

.PHONY: all clean aarch64 armv7 test install help

all: aarch64 armv7

# AArch64 build
aarch64: $(AARCH64_TARGET)

$(AARCH64_TARGET): $(AARCH64_SRC)
	@echo "Building AArch64 version..."
	$(AARCH64_AS) $(AARCH64_ASFLAGS) -o $(AARCH64_TARGET).o $(AARCH64_SRC)
	$(AARCH64_LD) -o $(AARCH64_TARGET) $(AARCH64_TARGET).o
	@echo "AArch64 build complete: $(AARCH64_TARGET)"

# ARMv7-A build
armv7: $(ARM_TARGET)

$(ARM_TARGET): $(ARM_SRC)
	@echo "Building ARMv7-A version..."
	$(ARM_AS) $(ARM_ASFLAGS) -o $(ARM_TARGET).o $(ARM_SRC)
	$(ARM_LD) -o $(ARM_TARGET) $(ARM_TARGET).o
	@echo "ARMv7-A build complete: $(ARM_TARGET)"

# Create ARMv7 version (simplified from AArch64)
neural_net_v7.s: neural_net.s
	@echo "Generating ARMv7-A version..."
	@sed 's/.arch armv8-a/.arch armv7-a/g; s/x0/r0/g; s/x1/r1/g; s/x2/r2/g; s/x3/r3/g; s/x4/r4/g; s/x5/r5/g; s/x6/r6/g; s/x7/r7/g; s/x8/r8/g' $< > $@

# Disassembly for analysis
disasm: $(AARCH64_TARGET) $(ARM_TARGET)
	@echo "Generating disassembly..."
	$(AARCH64_OBJDUMP) -d $(AARCH64_TARGET) > $(AARCH64_TARGET).disasm
	$(ARM_OBJDUMP) -d $(ARM_TARGET) > $(ARM_TARGET).disasm

# Test on QEMU
test-aarch64: $(AARCH64_TARGET)
	@echo "Testing AArch64 version on QEMU..."
	qemu-aarch64 ./$(AARCH64_TARGET)

test-arm: $(ARM_TARGET)
	@echo "Testing ARMv7 version on QEMU..."
	qemu-arm ./$(ARM_TARGET)

test: test-aarch64 test-arm

# Performance benchmark
benchmark: $(AARCH64_TARGET)
	@echo "Running performance benchmark..."
	@time -p ./$(AARCH64_TARGET)

# Memory analysis with Valgrind (if available)
memcheck: $(AARCH64_TARGET)
	@echo "Running memory analysis..."
	valgrind --tool=memcheck --leak-check=full ./$(AARCH64_TARGET)

# Install to system (requires sudo)
install: $(AARCH64_TARGET) $(ARM_TARGET)
	@echo "Installing neural network binaries..."
	sudo cp $(AARCH64_TARGET) /usr/local/bin/
	sudo cp $(ARM_TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(AARCH64_TARGET)
	sudo chmod +x /usr/local/bin/$(ARM_TARGET)

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f *.o $(AARCH64_TARGET) $(ARM_TARGET)
	rm -f *.disasm neural_net_v7.s
	rm -f test_results.txt benchmark_results.txt

# Help target
help:
	@echo "ARM Neural Network Build System"
	@echo "Available targets:"
	@echo "  all       - Build both AArch64 and ARMv7 versions"
	@echo "  aarch64   - Build AArch64 version only"
	@echo "  armv7     - Build ARMv7-A version only"
	@echo "  disasm    - Generate disassembly files"
	@echo "  test      - Test both versions on QEMU"
	@echo "  benchmark - Run performance benchmark"
	@echo "  memcheck  - Run memory analysis with Valgrind"
	@echo "  install   - Install binaries to /usr/local/bin"
	@echo "  clean     - Remove all build artifacts"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Requirements:"
	@echo "  - GCC ARM cross-compilation toolchain"
	@echo "  - QEMU user-mode emulation (for testing)"
	@echo "  - Valgrind (optional, for memory analysis)"

# Development utilities
size: $(AARCH64_TARGET) $(ARM_TARGET)
	@echo "Binary sizes:"
	@ls -lh $(AARCH64_TARGET) $(ARM_TARGET)

debug: $(AARCH64_TARGET)
	@echo "Starting GDB debug session..."
	gdb-multiarch $(AARCH64_TARGET)

# Check for required tools
check-tools:
	@echo "Checking for required tools..."
	@which $(AARCH64_AS) > /dev/null || echo "Warning: $(AARCH64_AS) not found"
	@which $(ARM_AS) > /dev/null || echo "Warning: $(ARM_AS) not found"
	@which qemu-aarch64 > /dev/null || echo "Warning: qemu-aarch64 not found"
	@which qemu-arm > /dev/null || echo "Warning: qemu-arm not found"
	@echo "Tool check complete" 