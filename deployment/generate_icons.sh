#!/bin/bash

# Script para generar iconos placeholder
echo "🎨 Generando iconos placeholder..."

# Verificar si ImageMagick está instalado
if ! command -v convert &> /dev/null; then
    echo "⚠️  ImageMagick no está instalado."
    echo "Instálalo con: sudo pacman -S imagemagick (Arch) o sudo apt install imagemagick (Debian/Ubuntu)"
    exit 1
fi

# Colores del tema
BG_COLOR="#000000"
FG_COLOR="#00ff00"

# Generar iconos con las iniciales "AE"
echo "Generando favicon-16.png..."
convert -size 16x16 xc:$BG_COLOR -fill $FG_COLOR -gravity center \
    -pointsize 10 -font "JetBrains-Mono-Bold" -annotate +0+0 "A" \
    favicon-16.png 2>/dev/null || \
    convert -size 16x16 xc:$BG_COLOR -fill $FG_COLOR -gravity center \
    -pointsize 10 -annotate +0+0 "A" favicon-16.png

echo "Generando favicon-32.png..."
convert -size 32x32 xc:$BG_COLOR -fill $FG_COLOR -gravity center \
    -pointsize 20 -font "JetBrains-Mono-Bold" -annotate +0+0 "AE" \
    favicon-32.png 2>/dev/null || \
    convert -size 32x32 xc:$BG_COLOR -fill $FG_COLOR -gravity center \
    -pointsize 20 -annotate +0+0 "AE" favicon-32.png

echo "Generando icon-192.png..."
convert -size 192x192 xc:$BG_COLOR -fill $FG_COLOR -gravity center \
    -pointsize 80 -font "JetBrains-Mono-Bold" -annotate +0+0 "AE" \
    -stroke $FG_COLOR -strokewidth 2 \
    icon-192.png 2>/dev/null || \
    convert -size 192x192 xc:$BG_COLOR -fill $FG_COLOR -gravity center \
    -pointsize 80 -annotate +0+0 "AE" icon-192.png

echo "Generando icon-512.png..."
convert -size 512x512 xc:$BG_COLOR -fill $FG_COLOR -gravity center \
    -pointsize 200 -font "JetBrains-Mono-Bold" -annotate +0+0 "AE" \
    -stroke $FG_COLOR -strokewidth 4 \
    icon-512.png 2>/dev/null || \
    convert -size 512x512 xc:$BG_COLOR -fill $FG_COLOR -gravity center \
    -pointsize 200 -annotate +0+0 "AE" icon-512.png

echo "Generando og-image.png..."
convert -size 1200x630 xc:$BG_COLOR \
    -fill $FG_COLOR -gravity center -pointsize 80 \
    -annotate +0-50 "Amelia Enora" \
    -pointsize 40 -annotate +0+50 "Systems & Assembly Developer" \
    og-image.png

echo "✅ Iconos generados exitosamente" 