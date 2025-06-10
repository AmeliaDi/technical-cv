#!/bin/bash

# Script alternativo de subida FTP usando CURL
# Más confiable para algunos servidores FTP

echo "🚀 Subiendo archivos al FTP con CURL..."

# Configuración
FTP_HOST="***REMOVED***"
FTP_USER="***REMOVED***"
FTP_PASS="***REMOVED***"
FTP_URL="ftp://$FTP_USER:$FTP_PASS@$FTP_HOST/htdocs/"

# Lista de archivos web
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
    "manifest.json"
    "service-worker.js"
    "robots.txt"
    "sitemap.xml"
    ".htaccess"
)

# Archivos de imagen
IMAGE_FILES=(
    "favicon-16.png"
    "favicon-32.png"
    "icon-192.png"
    "icon-512.png"
    "og-image.png"
)

# Contador de éxitos
SUCCESS=0
TOTAL=0

# Subir archivos web
echo "📤 Subiendo archivos web..."
for file in "${WEB_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo -n "Subiendo $file... "
        if curl -s --ftp-create-dirs -T "$file" "$FTP_URL$file"; then
            echo "✅"
            ((SUCCESS++))
        else
            echo "❌"
        fi
        ((TOTAL++))
    fi
done

# Subir imágenes si existen
echo -e "\n📤 Subiendo imágenes..."
for file in "${IMAGE_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo -n "Subiendo $file... "
        if curl -s --ftp-create-dirs -T "$file" "$FTP_URL$file"; then
            echo "✅"
            ((SUCCESS++))
        else
            echo "❌"
        fi
        ((TOTAL++))
    fi
done

# Resumen
echo -e "\n📊 Resumen de subida:"
echo "- Archivos subidos exitosamente: $SUCCESS/$TOTAL"
echo "- URL del sitio: http://$FTP_USER.infinityfreeapp.com"

if [ $SUCCESS -eq $TOTAL ]; then
    echo -e "\n✨ ¡Todos los archivos subidos correctamente!"
    exit 0
else
    echo -e "\n⚠️  Algunos archivos no se pudieron subir"
    exit 1
fi 