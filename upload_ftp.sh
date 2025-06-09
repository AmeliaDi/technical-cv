#!/bin/bash

# FTP Upload Script for CV WebAssembly
# Credentials from InfinityFree hosting

FTP_HOST="***REMOVED***"
FTP_USER="***REMOVED***"
FTP_PASS="3JCqt7Uk4ED"
FTP_PORT="21"
REMOTE_DIR="/htdocs"

echo "🚀 Starting FTP upload to $FTP_HOST..."
echo "📁 Uploading CV WebAssembly files..."

# Create temporary FTP commands file
cat > ftp_commands.txt << EOF
open $FTP_HOST $FTP_PORT
user $FTP_USER $FTP_PASS
binary
cd $REMOTE_DIR
lcd $(pwd)
put index.html
put styles.css
put main.js
put algorithms.js
put algorithms.c
put visualizations.js
put network-tools.js
put wasm-loader.js
put CV_README.md README.md
put CHANGELOG.md
put DEPLOYMENT.md
put .gitignore
put deploy.sh
put Makefile
quit
EOF

# Execute FTP upload
echo "📤 Uploading files via FTP..."
ftp -n < ftp_commands.txt

# Check if upload was successful
if [ $? -eq 0 ]; then
    echo "✅ Upload completed successfully!"
    echo "🌐 Your CV is now live at: http://***REMOVED***.infinityfreeapp.com/"
    echo ""
    echo "📊 Files uploaded:"
    echo "   • index.html (main CV page)"
    echo "   • styles.css (responsive styling)"
    echo "   • main.js (interactive features)"
    echo "   • algorithms.js (algorithm visualizations)"
    echo "   • visualizations.js (math visualizations)"
    echo "   • network-tools.js (network tools)"
    echo "   • wasm-loader.js (WebAssembly loader)"
    echo "   • README.md (project documentation)"
    echo "   • CHANGELOG.md (version history)"
    echo "   • And more supporting files..."
    echo ""
    echo "🎯 Your technical CV featuring:"
    echo "   • BogoSort and 9 other sorting algorithms"
    echo "   • 16+ cybersecurity certifications"
    echo "   • Cisco CCNA networking expertise"
    echo "   • Linux distributions mastery"
    echo "   • Fully responsive design (320px-1400px+)"
    echo "   • Interactive algorithm visualizations"
    echo "   • Mathematical fractals and FFT"
    echo "   • Network analysis tools"
else
    echo "❌ Upload failed. Please check your connection and credentials."
    exit 1
fi

# Clean up temporary files
rm -f ftp_commands.txt

echo ""
echo "🎉 Deployment complete!"
echo "💡 Remember to test your live site to ensure everything works correctly." 