# 🚀 Instrucciones de Despliegue en GitHub

## Pasos para subir tu CV técnico a GitHub

### 1. 📋 Preparación

Tu repositorio local ya está configurado y listo. Los datos actualizados son:
- **GitHub**: [https://github.com/AmeliaDi](https://github.com/AmeliaDi)
- **Email**: enorastrokes@gmail.com
- **LinkedIn**: [linkedin.com/in/bogosort](https://linkedin.com/in/bogosort)

### 2. 🌐 Crear Repositorio en GitHub

1. Ve a [https://github.com/AmeliaDi](https://github.com/AmeliaDi)
2. Click en **"New repository"** (botón verde)
3. Nombra el repositorio: `technical-cv`
4. Descripción: `"Interactive Technical CV with WebAssembly algorithms and visualizations"`
5. Mantén como **público**
6. **NO** inicializar con README, .gitignore o license (ya los tenemos)
7. Click **"Create repository"**

### 3. 🔗 Conectar Repositorio Local con GitHub

En tu terminal, ejecuta:

```bash
cd /home/marcy/Escritorio/caloa
git remote add origin https://github.com/AmeliaDi/technical-cv.git
git branch -M main
git push -u origin main
```

### 4. 🚀 Despliegue Automático

Usa el script que he creado:

```bash
./deploy.sh
```

### 5. 🌐 Configurar GitHub Pages

1. Ve a tu repositorio: `https://github.com/AmeliaDi/technical-cv`
2. Click en **"Settings"**
3. Scroll down hasta **"Pages"** en el menú lateral
4. En **"Source"**, selecciona **"Deploy from a branch"**
5. Selecciona **"gh-pages"** branch
6. Selecciona **"/ (root)"** folder
7. Click **"Save"**

### 6. ✅ Verificar Despliegue

Tu CV estará disponible en:
**https://ameliadi.github.io/technical-cv/**

⏱️ Puede tomar 5-10 minutos en estar disponible.

## 🛠️ Comandos Útiles

### Actualizar el CV
```bash
# Después de hacer cambios
git add .
git commit -m "📝 Update CV content"
./deploy.sh
```

### Ver estado del repositorio
```bash
git status
git log --oneline -5
```

### Compilar WebAssembly (opcional)
```bash
# Si tienes Emscripten instalado
make all
```

### Servidor local para testing
```bash
make serve
# O manualmente:
python3 -m http.server 8000
```

## 🎯 Características del CV

Tu CV incluye:

- ✅ **Algoritmos interactivos** (QuickSort, MergeSort, HeapSort)
- ✅ **Visualizaciones matemáticas** (Mandelbrot, FFT)
- ✅ **Herramientas de red** (packet capture, port scanner)
- ✅ **Interfaz terminal** profesional
- ✅ **Responsive design**
- ✅ **WebAssembly ready** (con fallback JavaScript)

## 🔧 Personalización Futura

Para personalizar tu CV:

1. **Contenido**: Edita `index.html`
2. **Estilos**: Modifica `styles.css`
3. **Algoritmos**: Añade funciones en `algorithms.c`
4. **Visualizaciones**: Expande `visualizations.js`

## 📱 Optimización para Mobile

El CV ya está optimizado para:
- ✅ Dispositivos móviles
- ✅ Tablets
- ✅ Desktop
- ✅ Diferentes resoluciones

## 🎨 Branding Personal

Los colores y tema pueden personalizarse en `styles.css`:

```css
:root {
    --text-primary: #00ff00;    /* Verde terminal */
    --accent-blue: #00aaff;     /* Azul acentos */
    --bg-dark: #0a0a0a;         /* Fondo oscuro */
}
```

## 🚀 ¡Listo para Impresionar!

Tu CV técnico está diseñado para mostrar:

1. **Habilidades de programación de bajo nivel**
2. **Conocimiento en algoritmos y matemáticas**
3. **Experiencia en sistemas y redes**
4. **Capacidad de crear interfaces modernas**
5. **Dominio de tecnologías web avanzadas**

¡Perfecto para posiciones en desarrollo de sistemas, ingeniería de software, y roles técnicos especializados! 