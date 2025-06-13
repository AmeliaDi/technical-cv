#!/bin/bash
# ARM LLM Runtime Build Script

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
BUILD_TYPE=${BUILD_TYPE:-Release}
BUILD_DIR=${BUILD_DIR:-build}
INSTALL_PREFIX=${INSTALL_PREFIX:-/usr/local}
NUM_JOBS=${NUM_JOBS:-$(nproc)}

# Options
BUILD_TESTS=${BUILD_TESTS:-ON}
BUILD_EXAMPLES=${BUILD_EXAMPLES:-ON}
BUILD_PYTHON_BINDINGS=${BUILD_PYTHON_BINDINGS:-ON}
ENABLE_NEON=${ENABLE_NEON:-ON}

echo -e "${GREEN}ARM LLM Runtime Build Script${NC}"
echo "=================================="
echo "Build type: $BUILD_TYPE"
echo "Build directory: $BUILD_DIR"
echo "Install prefix: $INSTALL_PREFIX"
echo "Jobs: $NUM_JOBS"
echo "Tests: $BUILD_TESTS"
echo "Examples: $BUILD_EXAMPLES"
echo "Python bindings: $BUILD_PYTHON_BINDINGS"
echo "NEON: $ENABLE_NEON"
echo ""

# Check if we're on ARM
ARCH=$(uname -m)
if [[ "$ARCH" =~ ^(arm|aarch64) ]]; then
    echo -e "${GREEN}Detected ARM architecture: $ARCH${NC}"
else
    echo -e "${YELLOW}Warning: This project is optimized for ARM architectures.${NC}"
    echo -e "${YELLOW}Current architecture: $ARCH${NC}"
fi

# Check for NEON support
if [ "$ENABLE_NEON" = "ON" ]; then
    if [ -f /proc/cpuinfo ]; then
        if grep -q "neon" /proc/cpuinfo; then
            echo -e "${GREEN}NEON support detected${NC}"
        else
            echo -e "${YELLOW}Warning: NEON support not detected in /proc/cpuinfo${NC}"
        fi
    fi
fi

# Function to check dependencies
check_dependencies() {
    echo -e "${GREEN}Checking dependencies...${NC}"
    
    # Check for required tools
    for tool in cmake make gcc g++; do
        if ! command -v $tool &> /dev/null; then
            echo -e "${RED}Error: $tool is not installed${NC}"
            exit 1
        fi
    done
    
    # Check for pkg-config
    if ! command -v pkg-config &> /dev/null; then
        echo -e "${RED}Error: pkg-config is not installed${NC}"
        exit 1
    fi
    
    # Check for curl development libraries
    if ! pkg-config --exists libcurl; then
        echo -e "${RED}Error: libcurl development libraries not found${NC}"
        echo "Install with: sudo apt-get install libcurl4-openssl-dev"
        exit 1
    fi
    
    # Check for json-c development libraries
    if ! pkg-config --exists json-c; then
        echo -e "${RED}Error: json-c development libraries not found${NC}"
        echo "Install with: sudo apt-get install libjson-c-dev"
        exit 1
    fi
    
    # Check for Python development headers if building Python bindings
    if [ "$BUILD_PYTHON_BINDINGS" = "ON" ]; then
        if ! command -v python3-config &> /dev/null; then
            echo -e "${YELLOW}Warning: python3-config not found. Python bindings may not build.${NC}"
        fi
    fi
    
    echo -e "${GREEN}All dependencies satisfied${NC}"
}

# Function to install dependencies
install_dependencies() {
    echo -e "${GREEN}Installing dependencies...${NC}"
    
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            libcurl4-openssl-dev \
            libjson-c-dev \
            pkg-config
            
        if [ "$BUILD_PYTHON_BINDINGS" = "ON" ]; then
            sudo apt-get install -y \
                python3-dev \
                python3-pip \
                python3-numpy
            
            # Install pybind11
            pip3 install pybind11[global] --user
        fi
        
    elif command -v yum &> /dev/null; then
        sudo yum install -y \
            gcc-c++ \
            cmake \
            libcurl-devel \
            json-c-devel \
            pkgconfig
            
        if [ "$BUILD_PYTHON_BINDINGS" = "ON" ]; then
            sudo yum install -y \
                python3-devel \
                python3-pip \
                python3-numpy
            
            pip3 install pybind11[global] --user
        fi
        
    elif command -v brew &> /dev/null; then
        brew install cmake curl json-c
        
        if [ "$BUILD_PYTHON_BINDINGS" = "ON" ]; then
            brew install python
            pip3 install pybind11[global] numpy
        fi
    else
        echo -e "${YELLOW}Package manager not detected. Please install dependencies manually.${NC}"
    fi
}

# Function to build the project
build_project() {
    echo -e "${GREEN}Building ARM LLM Runtime...${NC}"
    
    # Create build directory
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    
    # Configure with CMake
    cmake .. \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
        -DBUILD_TESTS=$BUILD_TESTS \
        -DBUILD_EXAMPLES=$BUILD_EXAMPLES \
        -DBUILD_PYTHON_BINDINGS=$BUILD_PYTHON_BINDINGS \
        -DENABLE_NEON=$ENABLE_NEON
    
    # Build
    make -j$NUM_JOBS
    
    echo -e "${GREEN}Build completed successfully!${NC}"
}

# Function to run tests
run_tests() {
    if [ "$BUILD_TESTS" = "ON" ]; then
        echo -e "${GREEN}Running tests...${NC}"
        cd $BUILD_DIR
        ctest --output-on-failure
        echo -e "${GREEN}All tests passed!${NC}"
    fi
}

# Function to install
install_project() {
    echo -e "${GREEN}Installing ARM LLM Runtime...${NC}"
    cd $BUILD_DIR
    
    if [ "$EUID" -eq 0 ]; then
        make install
    else
        sudo make install
    fi
    
    # Update library cache
    if [ -f /etc/ld.so.conf ]; then
        sudo ldconfig
    fi
    
    echo -e "${GREEN}Installation completed!${NC}"
}

# Function to create package
create_package() {
    echo -e "${GREEN}Creating package...${NC}"
    cd $BUILD_DIR
    
    # Create binary package
    cpack
    
    echo -e "${GREEN}Package created!${NC}"
}

# Main script logic
case "${1:-all}" in
    deps)
        install_dependencies
        ;;
    check)
        check_dependencies
        ;;
    build)
        check_dependencies
        build_project
        ;;
    test)
        run_tests
        ;;
    install)
        install_project
        ;;
    package)
        create_package
        ;;
    clean)
        echo -e "${GREEN}Cleaning build directory...${NC}"
        rm -rf $BUILD_DIR
        echo -e "${GREEN}Clean completed!${NC}"
        ;;
    all)
        check_dependencies
        build_project
        run_tests
        ;;
    *)
        echo "Usage: $0 {deps|check|build|test|install|package|clean|all}"
        echo ""
        echo "Commands:"
        echo "  deps     - Install dependencies"
        echo "  check    - Check dependencies"
        echo "  build    - Build the project"
        echo "  test     - Run tests"
        echo "  install  - Install the project"
        echo "  package  - Create distribution package"
        echo "  clean    - Clean build directory"
        echo "  all      - Check, build, and test (default)"
        echo ""
        echo "Environment variables:"
        echo "  BUILD_TYPE           - Debug or Release (default: Release)"
        echo "  BUILD_DIR            - Build directory (default: build)"
        echo "  INSTALL_PREFIX       - Install prefix (default: /usr/local)"
        echo "  NUM_JOBS             - Number of parallel jobs (default: nproc)"
        echo "  BUILD_TESTS          - Build tests (default: ON)"
        echo "  BUILD_EXAMPLES       - Build examples (default: ON)"
        echo "  BUILD_PYTHON_BINDINGS - Build Python bindings (default: ON)"
        echo "  ENABLE_NEON          - Enable NEON optimizations (default: ON)"
        exit 1
        ;;
esac

echo -e "${GREEN}Script completed successfully!${NC}" 