#!/bin/bash

# CURL FTP Upload Script for CV WebAssembly
# Using curl for more reliable FTP transfers

FTP_HOST="***REMOVED***"
FTP_USER="***REMOVED***"
FTP_PASS="***REMOVED***"
FTP_URL="ftp://$FTP_USER:$FTP_PASS@$FTP_HOST/htdocs/"

echo "🚀 Starting FTP upload using CURL to $FTP_HOST..."
echo "📁 Uploading CV WebAssembly files..."

# Array of files to upload
files_to_upload=(
    "index.html"
    "styles.css"
    "main.js"
    "algorithms.js"
    "algorithms.c"
    "visualizations.js"
    "network-tools.js"
    "wasm-loader.js"
    "CHANGELOG.md"
    "CV_README.md"
    "DEPLOYMENT.md"
    ".gitignore"
    "deploy.sh"
    "Makefile"
)

# Track upload results
successful_uploads=0
failed_uploads=0

echo "📤 Starting file uploads..."
echo ""

for file in "${files_to_upload[@]}"; do
    if [ -f "$file" ]; then
        echo "📋 Uploading: $file"
        
        # Special handling for CV_README.md -> README.md
        if [ "$file" = "CV_README.md" ]; then
            curl -T "$file" "${FTP_URL}README.md" --ftp-create-dirs -v
        else
            curl -T "$file" "${FTP_URL}$file" --ftp-create-dirs -v
        fi
        
        if [ $? -eq 0 ]; then
            echo "   ✅ $file uploaded successfully"
            ((successful_uploads++))
        else
            echo "   ❌ Failed to upload $file"
            ((failed_uploads++))
        fi
        echo ""
    else
        echo "   ⚠️  File not found: $file"
        ((failed_uploads++))
    fi
done

echo "📊 Upload Summary:"
echo "   ✅ Successful: $successful_uploads files"
echo "   ❌ Failed: $failed_uploads files"
echo ""

if [ $failed_uploads -eq 0 ]; then
    echo "🎉 All files uploaded successfully!"
    echo "🌐 Your CV should now be live at: http://***REMOVED***.infinityfreeapp.com/"
    echo ""
    echo "🎯 Your technical CV featuring:"
    echo "   • BogoSort and 9 other sorting algorithms"
    echo "   • 16+ cybersecurity certifications (CISM, CISA, MITRE ATT&CK)"
    echo "   • Cisco CCNA networking expertise"
    echo "   • Linux distributions mastery (Arch, Debian, Red Hat, Gentoo)"
    echo "   • Fully responsive design (320px-1400px+)"
    echo "   • Interactive algorithm visualizations"
    echo "   • Mathematical fractals and FFT"
    echo "   • Network analysis tools"
         echo "   • Updated with feminine pronouns and full name: Amelia Enora Marceline Chavez Barroso"
    echo ""
    echo "💡 Test the live site to ensure all features work correctly!"
else
    echo "⚠️  Some files failed to upload. Please check the connection and try again."
fi

echo ""
echo "🔗 Access your CV at: http://***REMOVED***.infinityfreeapp.com/"
echo "📧 Contact: enorastrokes@gmail.com"
echo "🐙 GitHub: github.com/AmeliaDi" 