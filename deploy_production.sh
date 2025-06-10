#!/bin/bash

# Script de despliegue producción - Amelia CV
# Limpia FTP, sube solo archivos web necesarios y actualiza GitHub

set -e  # Salir si hay error

echo "🚀 Iniciando despliegue a producción..."

# Colores para output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# Configuración FTP
FTP_HOST="***REMOVED***"
FTP_USER="***REMOVED***"
FTP_PASS="***REMOVED***"

# Lista de archivos web necesarios
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

# Archivos opcionales (si existen)
OPTIONAL_FILES=(
    "algorithms.wasm"
    "algorithms_wasm.js"
    "favicon-16.png"
    "favicon-32.png"
    "icon-192.png"
    "icon-512.png"
    "og-image.png"
    "screenshot-desktop.png"
    "screenshot-mobile.png"
)

echo -e "${YELLOW}📦 Preparando archivos para despliegue...${NC}"

# Crear directorio temporal
TEMP_DIR=$(mktemp -d)
echo "Directorio temporal: $TEMP_DIR"

# Copiar archivos necesarios
for file in "${WEB_FILES[@]}"; do
    if [ -f "$file" ]; then
        cp "$file" "$TEMP_DIR/"
        echo -e "${GREEN}✓${NC} Copiado: $file"
    else
        echo -e "${RED}✗${NC} No encontrado: $file"
    fi
done

# Copiar archivos opcionales si existen
for file in "${OPTIONAL_FILES[@]}"; do
    if [ -f "$file" ]; then
        cp "$file" "$TEMP_DIR/"
        echo -e "${GREEN}✓${NC} Copiado (opcional): $file"
    fi
done

echo -e "\n${YELLOW}🧹 Limpiando FTP...${NC}"

# Script FTP para limpiar y subir
cat > "$TEMP_DIR/ftp_commands.txt" << EOF
open $FTP_HOST
user $FTP_USER $FTP_PASS
binary
cd htdocs
prompt off

# Eliminar archivos antiguos (excepto directorios del sistema)
mdelete *.html
mdelete *.css
mdelete *.js
mdelete *.json
mdelete *.xml
mdelete *.txt
mdelete *.png
mdelete *.wasm
mdelete .htaccess

# Subir nuevos archivos
lcd $TEMP_DIR
mput *

# Verificar
ls -la

quit
EOF

# Ejecutar comandos FTP
echo -e "${YELLOW}📤 Subiendo archivos al FTP...${NC}"
ftp -inv < "$TEMP_DIR/ftp_commands.txt" > "$TEMP_DIR/ftp_log.txt" 2>&1

# Verificar resultado
if grep -q "226 Transfer complete" "$TEMP_DIR/ftp_log.txt"; then
    echo -e "${GREEN}✅ Archivos subidos correctamente al FTP${NC}"
else
    echo -e "${RED}❌ Error al subir archivos${NC}"
    echo "Ver log en: $TEMP_DIR/ftp_log.txt"
    exit 1
fi

# Actualizar GitHub
echo -e "\n${YELLOW}🐙 Actualizando GitHub...${NC}"

# Verificar estado Git
if ! git diff --quiet || ! git diff --cached --quiet; then
    echo "Cambios detectados, creando commit..."
    
    # Agregar archivos modificados
    git add .
    
    # Crear commit
    COMMIT_MSG="🚀 Deploy: Actualización de producción $(date +%Y-%m-%d)"
    git commit -m "$COMMIT_MSG" -m "- Agregadas mejoras PWA y SEO" -m "- Service Worker para funcionalidad offline" -m "- Optimizaciones de rendimiento"
    
    # Push a GitHub
    echo -e "${YELLOW}📤 Pushing a GitHub...${NC}"
    git push origin main
    
    echo -e "${GREEN}✅ GitHub actualizado${NC}"
else
    echo "No hay cambios para commitear"
fi

# Limpiar
echo -e "\n${YELLOW}🧹 Limpiando archivos temporales...${NC}"
rm -rf "$TEMP_DIR"

echo -e "\n${GREEN}✨ ¡Despliegue completado con éxito!${NC}"
echo -e "🌐 Tu sitio web está disponible en: ${GREEN}http://$FTP_USER.infinityfreeapp.com${NC}"
echo -e "🐙 GitHub: ${GREEN}https://github.com/AmeliaDi${NC}"

# Mostrar resumen
echo -e "\n${YELLOW}📊 Resumen del despliegue:${NC}"
echo "- Archivos web subidos: ${#WEB_FILES[@]}"
echo "- FTP limpiado y actualizado"
echo "- GitHub sincronizado"
echo "- Service Worker activado para funcionalidad offline"
echo "- PWA habilitada" 