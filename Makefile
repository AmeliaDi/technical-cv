# Advanced Web Vulnerability Scanner Makefile
# Author: AmeliaDi <enorastrokes@gmail.com>

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -D_GNU_SOURCE
LDFLAGS = -lcurl -ljson-c -lxml2 -lpthread -lpcre -lm
TARGET = webvulnscan
SOURCES = web_vulnerability_scanner.c crawler.c vulnerability_tests.c http_utils.c payloads.c utils.c
OBJECTS = $(SOURCES:.c=.o)
TEST_TARGET = test_webvulnscan
HEADERS = web_vulnerability_scanner.h

# Default target
all: $(TARGET)

# Main executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Test suite
$(TEST_TARGET): test_webvulnscan.c $(OBJECTS)
	$(CC) $(CFLAGS) test_webvulnscan.c $(filter-out web_vulnerability_scanner.o,$(OBJECTS)) -o $(TEST_TARGET) $(LDFLAGS)

# Static analysis
static-analysis:
	cppcheck --enable=all --std=c99 $(SOURCES)
	@echo "Static analysis complete"

# Code formatting
format:
	@which clang-format > /dev/null && clang-format -i $(SOURCES) $(HEADERS) || echo "clang-format not found"

# Install
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/
	sudo chmod +x /usr/local/bin/$(TARGET)
	@echo "Installed to /usr/local/bin/$(TARGET)"

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Clean
clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET)
	rm -f *.log scan_results.* vulnerability_report.*

# Development build with debug symbols
debug: CFLAGS += -g -DDEBUG -fsanitize=address
debug: LDFLAGS += -fsanitize=address
debug: $(TARGET)

# Performance build
performance: CFLAGS += -O3 -march=native -DNDEBUG
performance: $(TARGET)

# Test
test: $(TEST_TARGET)
	@echo "Running web vulnerability scanner tests..."
	./$(TEST_TARGET)

# Integration test
integration-test: $(TARGET)
	@echo "Running integration tests..."
	./$(TARGET) -u http://testphp.vulnweb.com -d 2 -t 5 -f text -o test_results.txt

# Valgrind memory check
memcheck: debug
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./$(TARGET) -u http://example.com -d 1

# Create test targets file
create-test-targets:
	echo "http://testphp.vulnweb.com" > test_targets.txt
	echo "http://demo.testfire.net" >> test_targets.txt
	echo "http://zero.webappsecurity.com" >> test_targets.txt

# Benchmark
benchmark: $(TARGET)
	@echo "Running performance benchmark..."
	time ./$(TARGET) -u http://testphp.vulnweb.com -d 3 -t 10 -a

# Security scan of binary
security-scan: $(TARGET)
	@which checksec > /dev/null && checksec --file=$(TARGET) || echo "checksec not found"

# Generate documentation
docs:
	@which doxygen > /dev/null && doxygen Doxyfile || echo "doxygen not found"

# Package for distribution
package: clean all
	tar -czf web-vulnerability-scanner.tar.gz $(SOURCES) $(HEADERS) Makefile README.md test_*.c payloads/ docs/

# Docker build
docker-build:
	docker build -t webvulnscan .

# Docker run
docker-run:
	docker run -it --rm webvulnscan -u http://testphp.vulnweb.com

# Help
help:
	@echo "Available targets:"
	@echo "  all              - Build the web vulnerability scanner"
	@echo "  debug            - Build with debug symbols and AddressSanitizer"
	@echo "  performance      - Build optimized version"
	@echo "  test             - Run unit tests"
	@echo "  integration-test - Run integration tests"
	@echo "  install          - Install to system (requires sudo)"
	@echo "  uninstall        - Remove from system (requires sudo)"
	@echo "  clean            - Remove build artifacts"
	@echo "  format           - Format source code"
	@echo "  static-analysis  - Run static code analysis"
	@echo "  memcheck         - Run memory leak detection"
	@echo "  benchmark        - Run performance benchmark"
	@echo "  security-scan    - Scan binary for security issues"
	@echo "  package          - Create distribution package"
	@echo "  docker-build     - Build Docker image"
	@echo "  docker-run       - Run in Docker container"
	@echo "  help             - Show this help"

.PHONY: all clean debug performance test integration-test install uninstall static-analysis format memcheck benchmark package help docs security-scan create-test-targets docker-build docker-run 