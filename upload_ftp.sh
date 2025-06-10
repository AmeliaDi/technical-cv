#!/bin/bash

# 🚀 FTP Upload Script for Technical CV - Complete Edition
# Deploy full technical CV with security scanner to web hosting
# @author Amelia Enora 🌈 Marceline Chavez Barroso
# @version 3.0.0 - Complete CV Deploy

# ================================
# FTP CONFIGURATION
# ================================
FTP_HOST="***REMOVED***"
FTP_USER="***REMOVED***"
FTP_PASS="***REMOVED***"
FTP_PORT="21"
REMOTE_DIR="/htdocs"

# ================================
# DEPLOYMENT SCRIPT
# ================================
echo "🛡️ ================================"
echo "🚀 Technical CV - FTP Deployment"
echo "🛡️ ================================"
echo ""
echo "📡 Connecting to: $FTP_HOST"
echo "👤 User: $FTP_USER"
echo "📁 Target directory: $REMOTE_DIR"
echo ""

# Pre-flight checks
echo "🔍 Pre-flight checks..."

# Check if main files exist
REQUIRED_FILES=(
    "index.html"
    "styles.css"
    "main.js"
    "security-scanner.js"
    "security-styles.css"
)

for file in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo "❌ Missing required file: $file"
        exit 1
    fi
done

echo "✅ All required files present"
echo ""

# Create optimized FTP commands file
echo "📝 Preparing FTP commands..."
cat > ftp_commands.txt << EOF
open $FTP_HOST $FTP_PORT
user $FTP_USER $FTP_PASS
binary
cd $REMOTE_DIR
lcd $(pwd)

# Core CV Files
put index.html
put styles.css
put main.js

# Security Scanner (NEW)
put security-scanner.js
put security-styles.css

# Algorithm & Visualization Engine
put algorithms.js
put visualizations.js
put network-tools.js
put wasm-loader.js
put i18n.js

# WebAssembly & C Code
put algorithms.c
put Makefile

# Advanced Security Tools
put web_vulnerability_scanner.c
put web_vulnerability_scanner.h
put network_scanner.c
put network_scanner.h
put utils.c
put benchmark.c

# Cryptography Implementation
put aes_crypto.h
put aes_wrapper.c
put crypto_asm.s
put test_aes.c

# Kernel Module Demo
put kernel_module.c
put test_ioctl.c

# Documentation & Config
put README.md
put CV_README.md
put CHANGELOG.md
put DEPLOYMENT.md
put config.php

quit
EOF

# Execute FTP upload with progress
echo "📤 Uploading Technical CV to production..."
echo "⏳ This may take a few moments..."
echo ""

ftp -n < ftp_commands.txt

# Check upload status
if [ $? -eq 0 ]; then
    echo ""
    echo "🎉 ================================"
    echo "✅ DEPLOYMENT SUCCESSFUL!"
    echo "🎉 ================================"
    echo ""
    echo "🌐 Your Technical CV is now LIVE at:"
    echo "   📍 http://***REMOVED***.infinityfreeapp.com/"
    echo ""
    echo "🚀 ================================"
    echo "📊 DEPLOYED FEATURES:"
    echo "🚀 ================================"
    echo "🛡️  Security Scanner (NEW!)"
    echo "   • Browser vulnerability analysis"
    echo "   • Real-time security assessment"
    echo "   • Privacy risk evaluation"
    echo "   • Network security tools"
    echo ""
    echo "🔢 Algorithm Visualizations:"
    echo "   • BogoSort (chaotic sorting)"
    echo "   • QuickSort, MergeSort, HeapSort"
    echo "   • Bubble, Selection, Insertion"
    echo "   • Radix, Counting, Shell sorts"
    echo "   • Interactive step-by-step demos"
    echo ""
    echo "🎯 Cybersecurity Expertise:"
    echo "   • CISSP, CISM, CISA certifications"
    echo "   • Ethical Hacking (CEH)"
    echo "   • Penetration Testing (OSCP)"
    echo "   • Digital Forensics (CCE)"
    echo "   • Cloud Security (CCSP)"
    echo ""
    echo "🌐 Network & Infrastructure:"
    echo "   • Cisco CCNA R&S certified"
    echo "   • Network protocol analysis"
    echo "   • Port scanning tools"
    echo "   • Traffic monitoring"
    echo ""
    echo "🐧 Linux Distributions:"
    echo "   • Arch Linux (advanced)"
    echo "   • Debian/Ubuntu (production)"
    echo "   • RedHat/CentOS (enterprise)"
    echo "   • Kali Linux (security)"
    echo ""
    echo "⚡ Technical Implementation:"
    echo "   • WebAssembly integration"
    echo "   • C/Assembly code compilation"
    echo "   • Real-time visualizations"
    echo "   • Responsive design (320px-1400px+)"
    echo "   • Multi-language support"
    echo ""
    echo "🔥 ================================"
    echo "🎯 NEXT STEPS:"
    echo "🔥 ================================"
    echo "1. 🧪 Test your live CV thoroughly"
    echo "2. 🔍 Run the security scanner"
    echo "3. 🎮 Try algorithm visualizations"
    echo "4. 📱 Test on mobile devices"
    echo "5. 🚀 Share your technical prowess!"
    echo ""
    echo "💡 Pro tip: The security scanner now shows"
    echo "   results inline instead of popups!"
    
else
    echo ""
    echo "❌ ================================"
    echo "💥 DEPLOYMENT FAILED"
    echo "❌ ================================"
    echo ""
    echo "🔧 Troubleshooting:"
    echo "   • Check internet connection"
    echo "   • Verify FTP credentials"
    echo "   • Ensure files are not locked"
    echo "   • Try again in a few minutes"
    echo ""
    echo "📞 Contact hosting support if issues persist"
    exit 1
fi

# Clean up
echo "🧹 Cleaning up temporary files..."
rm -f ftp_commands.txt

echo ""
echo "🎊 ================================"
echo "🏆 DEPLOYMENT COMPLETE!"
echo "🎊 ================================"
echo ""
echo "🌟 Your advanced technical CV is now live!"
echo "🚀 Featuring cutting-edge web technologies"
echo "🛡️ With integrated security analysis tools"
echo "⚡ Ready to impress potential employers!"
echo ""
echo "📧 Contact: enorastrokes@gmail.com"
echo "🐙 GitHub: https://github.com/AmeliaDi"
echo "💼 LinkedIn: linkedin.com/in/bogosort" 