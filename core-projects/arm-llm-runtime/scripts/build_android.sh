#!/bin/bash
# ARM LLM Runtime Android Build Script

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
ANDROID_NDK=${ANDROID_NDK:-}
ANDROID_ABI=${ANDROID_ABI:-arm64-v8a}
ANDROID_PLATFORM=${ANDROID_PLATFORM:-android-24}
BUILD_TYPE=${BUILD_TYPE:-Release}
BUILD_DIR=${BUILD_DIR:-build_android}
NUM_JOBS=${NUM_JOBS:-$(nproc)}

echo -e "${GREEN}ARM LLM Runtime Android Build Script${NC}"
echo "======================================"
echo "Android NDK: $ANDROID_NDK"
echo "Android ABI: $ANDROID_ABI"
echo "Android Platform: $ANDROID_PLATFORM"
echo "Build type: $BUILD_TYPE"
echo "Build directory: $BUILD_DIR"
echo "Jobs: $NUM_JOBS"
echo ""

# Validate environment
if [ -z "$ANDROID_NDK" ]; then
    echo -e "${RED}Error: ANDROID_NDK environment variable not set${NC}"
    echo "Please set ANDROID_NDK to point to your Android NDK installation"
    echo "Example: export ANDROID_NDK=/path/to/android-ndk-r25c"
    exit 1
fi

if [ ! -d "$ANDROID_NDK" ]; then
    echo -e "${RED}Error: Android NDK directory not found: $ANDROID_NDK${NC}"
    exit 1
fi

# Verify NDK version
NDK_VERSION_FILE="$ANDROID_NDK/source.properties"
if [ -f "$NDK_VERSION_FILE" ]; then
    NDK_VERSION=$(grep "Pkg.Revision" "$NDK_VERSION_FILE" | cut -d'=' -f2 | tr -d ' ')
    echo -e "${GREEN}NDK Version: $NDK_VERSION${NC}"
else
    echo -e "${YELLOW}Warning: Could not determine NDK version${NC}"
fi

# Check for required tools
echo -e "${GREEN}Checking dependencies...${NC}"

REQUIRED_TOOLS=("cmake" "make" "curl" "unzip")
for tool in "${REQUIRED_TOOLS[@]}"; do
    if ! command -v $tool &> /dev/null; then
        echo -e "${RED}Error: $tool is not installed${NC}"
        exit 1
    fi
done

# Function to download and setup dependencies for Android
setup_android_dependencies() {
    echo -e "${GREEN}Setting up Android dependencies...${NC}"
    
    DEPS_DIR="$PWD/android_deps"
    mkdir -p "$DEPS_DIR"
    
    # Download and build curl for Android
    CURL_VERSION="8.4.0"
    CURL_DIR="$DEPS_DIR/curl-$CURL_VERSION"
    
    if [ ! -d "$CURL_DIR" ]; then
        echo "Downloading curl $CURL_VERSION..."
        cd "$DEPS_DIR"
        curl -L "https://curl.se/download/curl-$CURL_VERSION.tar.gz" -o "curl-$CURL_VERSION.tar.gz"
        tar -xzf "curl-$CURL_VERSION.tar.gz"
        cd "$CURL_DIR"
        
        # Configure curl for Android
        ./configure \
            --host=aarch64-linux-android \
            --target=aarch64-linux-android \
            --prefix="$DEPS_DIR/curl_install" \
            --enable-shared=no \
            --enable-static=yes \
            --with-ssl \
            --without-ca-bundle \
            --without-ca-path \
            CC="$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang" \
            CXX="$ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android24-clang++"
        
        make -j$NUM_JOBS
        make install
        cd "$PWD"
    fi
    
    # Download and build json-c for Android
    JSONC_VERSION="0.16"
    JSONC_DIR="$DEPS_DIR/json-c-$JSONC_VERSION"
    
    if [ ! -d "$JSONC_DIR" ]; then
        echo "Downloading json-c $JSONC_VERSION..."
        cd "$DEPS_DIR"
        curl -L "https://github.com/json-c/json-c/archive/json-c-$JSONC_VERSION-20220414.tar.gz" -o "json-c-$JSONC_VERSION.tar.gz"
        tar -xzf "json-c-$JSONC_VERSION.tar.gz"
        mv "json-c-json-c-$JSONC_VERSION-"* "$JSONC_DIR"
        cd "$JSONC_DIR"
        
        # Build json-c for Android
        mkdir -p build_android
        cd build_android
        
        cmake .. \
            -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
            -DANDROID_ABI="$ANDROID_ABI" \
            -DANDROID_PLATFORM="$ANDROID_PLATFORM" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX="$DEPS_DIR/jsonc_install" \
            -DBUILD_SHARED_LIBS=OFF \
            -DBUILD_TESTING=OFF
        
        make -j$NUM_JOBS
        make install
        cd "$PWD"
    fi
    
    echo -e "${GREEN}Android dependencies setup complete${NC}"
}

# Function to build the project for Android
build_android_project() {
    echo -e "${GREEN}Building ARM LLM Runtime for Android...${NC}"
    
    # Setup dependencies
    setup_android_dependencies
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Set dependency paths
    DEPS_DIR="$PWD/../android_deps"
    CURL_PREFIX="$DEPS_DIR/curl_install"
    JSONC_PREFIX="$DEPS_DIR/jsonc_install"
    
    # Configure with CMake for Android
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
        -DANDROID_ABI="$ANDROID_ABI" \
        -DANDROID_PLATFORM="$ANDROID_PLATFORM" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DBUILD_SHARED_LIBS=ON \
        -DBUILD_TESTS=OFF \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_PYTHON_BINDINGS=OFF \
        -DENABLE_NEON=ON \
        -DCURL_ROOT="$CURL_PREFIX" \
        -DJSON_C_ROOT="$JSONC_PREFIX" \
        -DCMAKE_FIND_ROOT_PATH="$CURL_PREFIX;$JSONC_PREFIX" \
        -DCMAKE_CXX_FLAGS="-std=c++17 -fPIC" \
        -DCMAKE_C_FLAGS="-fPIC"
    
    # Build
    make -j$NUM_JOBS
    
    echo -e "${GREEN}Android build completed successfully!${NC}"
    
    # Show output files
    echo -e "${GREEN}Output files:${NC}"
    find . -name "*.so" -o -name "*.a" | while read file; do
        echo "  $file"
        file "$file"
    done
}

# Function to create Android AAR package
create_android_aar() {
    echo -e "${GREEN}Creating Android AAR package...${NC}"
    
    AAR_DIR="$BUILD_DIR/aar"
    mkdir -p "$AAR_DIR/jni/$ANDROID_ABI"
    mkdir -p "$AAR_DIR/java/com/armllm/runtime"
    
    # Copy native libraries
    cp "$BUILD_DIR/libarm_llm_runtime.so" "$AAR_DIR/jni/$ANDROID_ABI/"
    
    # Create Android manifest
    cat > "$AAR_DIR/AndroidManifest.xml" << EOF
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.armllm.runtime">
    
    <uses-sdk android:minSdkVersion="24" android:targetSdkVersion="33" />
    
</manifest>
EOF
    
    # Create Java wrapper class
    cat > "$AAR_DIR/java/com/armllm/runtime/ArmLLMNative.java" << 'EOF'
package com.armllm.runtime;

public class ArmLLMNative {
    static {
        System.loadLibrary("arm_llm_runtime");
    }
    
    // Native method declarations
    public native long createRuntime();
    public native void destroyRuntime(long runtimePtr);
    public native long loadModel(long runtimePtr, String modelId, int quantization);
    public native void unloadModel(long runtimePtr, long modelPtr);
    public native String generate(long modelPtr, String prompt, int maxTokens, float temperature);
    public native void generateStream(long modelPtr, String prompt, int maxTokens, float temperature, StreamCallback callback);
    
    // Callback interface for streaming
    public interface StreamCallback {
        void onToken(String token);
    }
    
    // Simple wrapper methods
    private long runtimePtr = 0;
    
    public ArmLLMNative() {
        runtimePtr = createRuntime();
    }
    
    public void close() {
        if (runtimePtr != 0) {
            destroyRuntime(runtimePtr);
            runtimePtr = 0;
        }
    }
    
    public long loadModel(String modelId, String quantization) {
        int quantType = 1; // Default to Q4_K
        if ("Q4_0".equals(quantization)) quantType = 0;
        else if ("Q4_K".equals(quantization)) quantType = 1;
        else if ("Q8_0".equals(quantization)) quantType = 2;
        else if ("Q8_K".equals(quantization)) quantType = 3;
        
        return loadModel(runtimePtr, modelId, quantType);
    }
    
    public String generate(long modelPtr, String prompt) {
        return generate(modelPtr, prompt, 100, 0.7f);
    }
    
    public String generate(long modelPtr, String prompt, int maxTokens) {
        return generate(modelPtr, prompt, maxTokens, 0.7f);
    }
}
EOF
    
    # Create classes.jar (empty for now)
    jar cf "$AAR_DIR/classes.jar" -C "$AAR_DIR/java" .
    
    # Create R.txt (empty)
    touch "$AAR_DIR/R.txt"
    
    # Create AAR file
    cd "$AAR_DIR"
    zip -r "../arm_llm_runtime.aar" .
    cd ..
    
    echo -e "${GREEN}AAR package created: $BUILD_DIR/arm_llm_runtime.aar${NC}"
}

# Function to run Android-specific tests
run_android_tests() {
    echo -e "${GREEN}Running Android compatibility tests...${NC}"
    
    # Check if libraries are properly linked
    if [ -f "$BUILD_DIR/libarm_llm_runtime.so" ]; then
        echo "Checking library dependencies..."
        readelf -d "$BUILD_DIR/libarm_llm_runtime.so" | grep NEEDED
        
        echo "Checking symbols..."
        nm -D "$BUILD_DIR/libarm_llm_runtime.so" | grep -E "(arm_llm|create|load)" | head -10
        
        echo -e "${GREEN}Library checks passed${NC}"
    else
        echo -e "${RED}Error: Library not found${NC}"
        exit 1
    fi
}

# Function to install to Android device
install_to_device() {
    echo -e "${GREEN}Installing to Android device...${NC}"
    
    if ! command -v adb &> /dev/null; then
        echo -e "${RED}Error: adb not found. Please install Android SDK platform tools.${NC}"
        exit 1
    fi
    
    # Check if device is connected
    if ! adb devices | grep -q "device$"; then
        echo -e "${RED}Error: No Android device connected${NC}"
        echo "Please connect an Android device and enable USB debugging"
        exit 1
    fi
    
    # Push library to device
    adb push "$BUILD_DIR/libarm_llm_runtime.so" "/data/local/tmp/"
    
    # Push test executable if it exists
    if [ -f "$BUILD_DIR/arm-llm" ]; then
        adb push "$BUILD_DIR/arm-llm" "/data/local/tmp/"
        adb shell "chmod +x /data/local/tmp/arm-llm"
        
        echo -e "${GREEN}Test installation on device:${NC}"
        adb shell "/data/local/tmp/arm-llm --system-info" || true
    fi
    
    echo -e "${GREEN}Installation to device completed${NC}"
}

# Function to clean build files
clean_android() {
    echo -e "${GREEN}Cleaning Android build files...${NC}"
    rm -rf "$BUILD_DIR"
    rm -rf "android_deps"
    echo -e "${GREEN}Clean completed${NC}"
}

# Main script logic
case "${1:-build}" in
    deps)
        setup_android_dependencies
        ;;
    build)
        build_android_project
        ;;
    aar)
        build_android_project
        create_android_aar
        ;;
    test)
        run_android_tests
        ;;
    install)
        install_to_device
        ;;
    clean)
        clean_android
        ;;
    all)
        build_android_project
        create_android_aar
        run_android_tests
        ;;
    *)
        echo "Usage: $0 {deps|build|aar|test|install|clean|all}"
        echo ""
        echo "Commands:"
        echo "  deps     - Download and build Android dependencies"
        echo "  build    - Build the project for Android"
        echo "  aar      - Build and create Android AAR package"
        echo "  test     - Run Android compatibility tests"
        echo "  install  - Install to connected Android device"
        echo "  clean    - Clean build files"
        echo "  all      - Build, create AAR, and test"
        echo ""
        echo "Environment variables:"
        echo "  ANDROID_NDK      - Path to Android NDK (required)"
        echo "  ANDROID_ABI      - Target ABI (default: arm64-v8a)"
        echo "  ANDROID_PLATFORM - Target platform (default: android-24)"
        echo "  BUILD_TYPE       - Debug or Release (default: Release)"
        echo "  BUILD_DIR        - Build directory (default: build_android)"
        echo "  NUM_JOBS         - Number of parallel jobs (default: nproc)"
        echo ""
        echo "Supported ABIs: arm64-v8a, armeabi-v7a"
        echo "Minimum Android version: API 24 (Android 7.0)"
        exit 1
        ;;
esac

echo -e "${GREEN}Android build script completed successfully!${NC}" 