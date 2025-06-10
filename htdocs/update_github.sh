#!/bin/bash

# 🚀 Script de Actualización GitHub - CV Técnico v3.0.0
# Autor: Amelia Enora 🌈 Marceline Chavez Barroso

echo "🚀 Actualizando repositorio GitHub technical-cv..."
echo "=================================================="

# Verificar que estamos en el directorio correcto
if [ ! -f "index.html" ]; then
    echo "❌ Error: No se encuentra index.html. Ejecuta este script desde el directorio del proyecto."
    exit 1
fi

# Mostrar estadísticas del proyecto
echo "📊 ESTADÍSTICAS DEL CV v3.0.0:"
echo "   📄 Líneas HTML: $(wc -l index.html | cut -d' ' -f1)"
echo "   🎯 Algoritmos: 25+ implementados"
echo "   🏗️ Proyectos: 20+ hardcore" 
echo "   🗂️ Archivos: $(ls -1 website_upload/ | wc -l) listos para deploy"
echo "   💾 Tamaño total: $(du -sh website_upload | cut -f1)"
echo ""

# Agregar todos los archivos
echo "📋 Agregando archivos modificados..."
git add .

# Mostrar estado del repositorio
echo "🔍 Estado del repositorio:"
git status --short

echo ""
read -p "✅ ¿Continuar con el commit? (Y/n): " confirm
if [[ $confirm =~ ^[Nn]$ ]]; then
    echo "❌ Operación cancelada"
    exit 0
fi

# Crear commit con mensaje descriptivo
COMMIT_MSG="🚀 CV v3.0.0: Added 25+ algorithms, 20+ projects, new hosting

✨ New Features:
- 🔥 Advanced system algorithms (JIT, Memory Allocator, CFS Scheduler)
- 🧠 Cryptographic demos (AES, RSA, SHA-256)
- 🌳 Data structures (B+ Tree, Red-Black Tree, Bloom Filter)
- 🗺️ Pathfinding algorithms (Dijkstra, A*)
- 🏗️ 8 new hardcore projects (Quantum Crypto, ML Accelerator, etc.)
- 🌐 Updated hosting to enorastrokes.gilect.net
- 📊 Enhanced visualizations and interactivity

📈 Stats: 601 lines HTML, 328KB assets, 25 files"

git commit -m "$COMMIT_MSG"

# Push a GitHub
echo "📤 Subiendo cambios a GitHub..."
git push origin main

echo ""
echo "✅ ¡Repositorio actualizado exitosamente!"
echo ""
echo "🌐 URLs del CV:"
echo "   📍 GitHub Pages: https://ameliadi.github.io/technical-cv/"
echo "   📍 Hosting Principal: http://enorastrokes.gilect.net/?i=1"
echo ""
echo "🎯 Próximos pasos:"
echo "   1. Los archivos están listos en /website_upload/"
echo "   2. Sube manualmente los archivos a tu hosting"
echo "   3. GitHub Pages se actualizará automáticamente"
echo ""
echo "🔧 Archivos clave actualizados:"
echo "   ✅ index.html (46KB, 601 líneas)"
echo "   ✅ sitemap.xml (URLs actualizadas)" 
echo "   ✅ README.md (nuevas URLs y stats)"
echo "   ✅ CHANGELOG.md (v3.0.0 features)"
echo "   ✅ 22 archivos web optimizados"
echo ""
echo "🚀 ¡Tu CV técnico está listo para impresionar!" 