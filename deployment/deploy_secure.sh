#!/bin/bash

# 🔒 SECURE DEPLOYMENT SCRIPT
# No credentials hardcoded - use environment variables!

echo "🚀 Secure CV Deployment Script"
echo "================================"

# Check for environment variables
if [ -z "$FTP_HOST" ] || [ -z "$FTP_USER" ] || [ -z "$FTP_PASS" ]; then
    echo "❌ ERROR: Please set environment variables:"
    echo "   export FTP_HOST=your_host"
    echo "   export FTP_USER=your_user"
    echo "   export FTP_PASS=your_password"
    echo ""
    echo "💡 TIP: Add them to ~/.bashrc or use a .env file"
    exit 1
fi

# Web files to deploy
WEB_FILES=(
    "index.html"
    "styles.css"
    "security-styles.css"
    "main.js"
    "algorithms.js"
    "visualizations.js"
    "network-tools.js"
    "security-scanner.js"
    "i18n.js"
    "wasm-loader.js"
    "vulnerability-scanner.js"
    "donut-arch.js"
)

# Deploy to GitHub Pages (always safe)
echo "📤 Deploying to GitHub Pages..."
git add . && git commit -m "🚀 Deploy: Updated CV $(date)" && git push origin main

# Optional FTP deployment (with secure credentials)
read -p "🔒 Deploy to FTP? (y/N): " deploy_ftp
if [[ $deploy_ftp =~ ^[Yy]$ ]]; then
    echo "📤 Deploying to FTP..."
    for file in "${WEB_FILES[@]}"; do
        if [ -f "$file" ]; then
            echo "Uploading $file..."
            curl -s --ftp-create-dirs -T "$file" "ftp://$FTP_USER:$FTP_PASS@$FTP_HOST/htdocs/$file"
        fi
    done
fi

echo "✅ Deployment completed!"
echo "🌐 GitHub Pages: https://ameliadi.github.io/technical-cv/" 