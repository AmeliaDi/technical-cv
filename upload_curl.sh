#!/bin/bash

# 🚀 CURL FTP Upload Script for Technical CV - Complete Edition
# Using modern curl for secure and reliable FTP transfers
# @author Amelia Enora 🌈 Marceline Chavez Barroso
# @version 3.0.0 - Complete CV Deploy

# ================================
# FTP CONFIGURATION
# ================================
FTP_HOST="***REMOVED***"
FTP_USER="***REMOVED***"
FTP_PASS="***REMOVED***"
FTP_URL="ftp://$FTP_USER:$FTP_PASS@$FTP_HOST/htdocs/"

echo "🛡️ ================================"
echo "🚀 Technical CV - CURL Deployment"
echo "🛡️ ================================"
echo ""
echo "📡 Server: $FTP_HOST"
echo "👤 User: $FTP_USER"
echo "🔗 Using modern CURL for secure transfer"
echo ""

# Pre-flight check
echo "🔍 Pre-deployment checks..."
if ! command -v curl &> /dev/null; then
    echo "❌ CURL not found. Please install curl first."
    exit 1
fi

echo "✅ CURL available"
echo ""

# Comprehensive file list for technical CV
files_to_upload=(
    # Core CV Files
    "index.html"
    "styles.css"
    "main.js"
    
    # Security Scanner (NEW!)
    "security-scanner.js"
    "security-styles.css"
    
    # Interactive Features
    "algorithms.js"
    "visualizations.js"
    "network-tools.js"
    "wasm-loader.js"
    "i18n.js"
    
    # WebAssembly & C Code
    "algorithms.c"
    "Makefile"
    
    # Advanced Security Tools
    "web_vulnerability_scanner.c"
    "web_vulnerability_scanner.h"
    "network_scanner.c"
    "network_scanner.h"
    "utils.c"
    "benchmark.c"
    
    # Cryptography Implementation
    "aes_crypto.h"
    "aes_wrapper.c"
    "crypto_asm.s"
    "test_aes.c"
    
    # Kernel Module Demo
    "kernel_module.c"
    "test_ioctl.c"
    
    # Documentation
    "README.md"
    "CV_README.md"
    "CHANGELOG.md"
    "DEPLOYMENT.md"
    "config.php"
)

# Upload statistics
successful_uploads=0
failed_uploads=0
total_files=${#files_to_upload[@]}

echo "📦 Preparing to upload $total_files files..."
echo "🚀 Starting deployment..."
echo ""

# Upload each file
for file in "${files_to_upload[@]}"; do
    if [ -f "$file" ]; then
        echo "📋 Uploading: $file"
        
        # Progress and verbose output
        curl -T "$file" "${FTP_URL}$file" \
             --ftp-create-dirs \
             --progress-bar \
             --connect-timeout 30 \
             --max-time 120 \
             --retry 3 \
             --retry-delay 2
        
        if [ $? -eq 0 ]; then
            echo "   ✅ $file uploaded successfully"
            ((successful_uploads++))
        else
            echo "   ❌ Failed to upload $file"
            ((failed_uploads++))
        fi
        echo ""
    else
        echo "   ⚠️  File not found: $file (skipping)"
        # Don't count missing files as failures
    fi
done

# Final statistics
echo "🔥 ================================"
echo "📊 DEPLOYMENT SUMMARY"
echo "🔥 ================================"
echo "✅ Successful uploads: $successful_uploads files"
echo "❌ Failed uploads: $failed_uploads files"
echo "📦 Total processed: $total_files files"
echo ""

if [ $failed_uploads -eq 0 ]; then
    echo "🎉 ================================"
    echo "✅ PERFECT DEPLOYMENT!"
    echo "🎉 ================================"
    echo ""
    echo "🌐 Your Technical CV is now LIVE at:"
    echo "   📍 http://***REMOVED***.infinityfreeapp.com/"
    echo ""
    echo "🚀 ================================"
    echo "🛡️ DEPLOYED FEATURES:"
    echo "🚀 ================================"
    echo ""
    echo "🆕 Security Scanner (NEW!):"
    echo "   • Inline vulnerability analysis"
    echo "   • Browser security assessment"
    echo "   • Privacy risk evaluation"
    echo "   • Real-time threat detection"
    echo ""
    echo "🔢 Algorithm Visualizations:"
    echo "   • BogoSort (chaotic fun!)"
    echo "   • QuickSort, MergeSort, HeapSort"
    echo "   • Bubble, Selection, Insertion"
    echo "   • Radix, Counting, Shell sorts"
    echo "   • Interactive step-by-step demos"
    echo ""
    echo "🎯 Cybersecurity Expertise:"
    echo "   • CISSP, CISM, CISA certified"
    echo "   • Ethical Hacking (CEH)"
    echo "   • Penetration Testing (OSCP)"
    echo "   • Digital Forensics specialist"
    echo "   • Cloud Security (CCSP)"
    echo ""
    echo "🌐 Network Infrastructure:"
    echo "   • Cisco CCNA R&S certified"
    echo "   • Protocol analysis tools"
    echo "   • Port scanning utilities"
    echo "   • Traffic monitoring systems"
    echo ""
    echo "🐧 Linux Mastery:"
    echo "   • Arch Linux (daily driver)"
    echo "   • Debian/Ubuntu (production)"
    echo "   • RedHat/CentOS (enterprise)"
    echo "   • Kali Linux (security testing)"
    echo ""
    echo "⚡ Technical Stack:"
    echo "   • WebAssembly integration"
    echo "   • C/Assembly compilation"
    echo "   • Real-time visualizations"
    echo "   • Responsive design (320px-1400px+)"
    echo "   • Multi-language support"
    echo "   • Advanced cryptography demos"
    echo ""
    echo "🎯 ================================"
    echo "🚀 NEXT ACTIONS:"
    echo "🎯 ================================"
    echo "1. 🧪 Test all features thoroughly"
    echo "2. 🛡️ Run the new security scanner"
    echo "3. 🎮 Try algorithm visualizations"
    echo "4. 📱 Test mobile responsiveness"
    echo "5. 🔥 Share with potential employers!"
    echo ""
    echo "💡 Pro Tip: Security scanner results now appear"
    echo "   inline instead of annoying popups!"
    
elif [ $failed_uploads -lt 5 ]; then
    echo "⚠️ ================================"
    echo "🟡 PARTIAL DEPLOYMENT"
    echo "⚠️ ================================"
    echo ""
    echo "✅ Most files uploaded successfully!"
    echo "⚠️ $failed_uploads files had issues"
    echo "🌐 CV should still be functional at:"
    echo "   📍 http://***REMOVED***.infinityfreeapp.com/"
    echo ""
    echo "💡 Consider re-running deployment for missing files"
    
else
    echo "❌ ================================"
    echo "💥 DEPLOYMENT ISSUES"
    echo "❌ ================================"
    echo ""
    echo "🔧 Troubleshooting steps:"
    echo "   • Check internet connection"
    echo "   • Verify FTP credentials"
    echo "   • Try again in a few minutes"
    echo "   • Contact hosting provider"
    echo ""
    echo "📞 InfinityFree Support if needed"
fi

echo ""
echo "🎊 ================================"
echo "🏆 DEPLOYMENT COMPLETED!"
echo "🎊 ================================"
echo ""
echo "🌟 Advanced Technical CV is live!"
echo "🛡️ With integrated security tools"
echo "⚡ Ready to showcase your skills!"
echo ""
echo "📧 Contact: enorastrokes@gmail.com"
echo "🐙 GitHub: https://github.com/AmeliaDi"
echo "💼 LinkedIn: linkedin.com/in/bogosort"
echo "🌐 Live CV: http://***REMOVED***.infinityfreeapp.com/" 