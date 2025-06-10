#!/bin/bash

# 🚀 Quick CV Upload - Essential Files Only
# Fast deployment of core CV files
# @author Amelia Enora 🌈 Marceline Chavez Barroso

# ================================
# FTP CONFIGURATION  
# ================================
FTP_HOST="***REMOVED***"
FTP_USER="***REMOVED***"
FTP_PASS="***REMOVED***"
FTP_URL="ftp://$FTP_USER:$FTP_PASS@$FTP_HOST/htdocs/"

echo "⚡ ================================"
echo "🚀 Quick CV Upload - Essential Files"
echo "⚡ ================================"
echo ""

# Essential files only for fast upload
essential_files=(
    "index.html"
    "styles.css"
    "main.js"
    "security-scanner.js"
    "security-styles.css"
    "algorithms.js"
    "visualizations.js"
    "network-tools.js"
    "wasm-loader.js"
)

successful=0
failed=0

echo "📦 Uploading ${#essential_files[@]} essential files..."
echo ""

for file in "${essential_files[@]}"; do
    if [ -f "$file" ]; then
        echo "📋 Uploading: $file"
        
        # Quick upload with shorter timeouts
        curl -T "$file" "${FTP_URL}$file" \
             --ftp-create-dirs \
             --connect-timeout 15 \
             --max-time 60 \
             --retry 2 \
             --retry-delay 1 \
             --silent \
             --show-error
        
        if [ $? -eq 0 ]; then
            echo "   ✅ $file uploaded"
            ((successful++))
        else
            echo "   ❌ $file failed"
            ((failed++))
        fi
    else
        echo "   ⚠️ $file not found"
        ((failed++))
    fi
done

echo ""
echo "📊 Results: ✅ $successful uploaded, ❌ $failed failed"
echo ""

if [ $successful -gt 0 ]; then
    echo "🎉 CV uploaded successfully!"
    echo "🌐 Live at: http://***REMOVED***.infinityfreeapp.com/"
    echo ""
    echo "🛡️ Features deployed:"
    echo "   • Security Scanner (inline results)"
    echo "   • Algorithm Visualizations"
    echo "   • Network Tools"
    echo "   • Responsive Design"
    echo ""
else
    echo "❌ Upload failed. Check connection."
fi

echo "📧 Contact: enorastrokes@gmail.com"
echo "🐙 GitHub: https://github.com/AmeliaDi" 