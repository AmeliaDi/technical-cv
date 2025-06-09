# 🚀 CV Técnico WebAssembly - Amelia Enora Marceline Chavez Barroso

Portfolio técnico interactivo desarrollado en WebAssembly, mostrando expertise en programación de sistemas, Assembly, ciberseguridad y algoritmos de bajo nivel.

## 👩‍💻 Sobre Amelia

**Desarrolladora de Sistemas y Low-Level Programming Specialist**

Especializada en programación de bajo nivel, optimización de algoritmos y sistemas embebidos. Experiencia extensa en x86, x64, ARM con NASM y FASM.

## 🎯 Características Principales

### 🎲 Algoritmos Interactivos
- **10+ algoritmos de ordenamiento** implementados en C/WebAssembly
- **BogoSort** (O(∞)) - El algoritmo más ineficiente del mundo
- **Visualizaciones en tiempo real** con métricas de rendimiento
- **Descripciones técnicas detalladas** de cada algoritmo

### 🔐 Expertise en Ciberseguridad
- **16+ certificaciones activas** (CISM, CISA, MITRE ATT&CK)
- **Penetration testing** con Kali Linux y Metasploit
- **Análisis de vulnerabilidades** siguiendo OWASP Top 10
- **Implementaciones criptográficas** en Assembly

### 🌐 Networking & Cisco CCNA
- **Routing & Switching avanzado** (OSPF, EIGRP, BGP)
- **VLANs, STP y seguridad de red**
- **Redes inalámbricas** 802.11
- **Automatización con Python** y REST APIs

### 🐧 Distribuciones Linux Master
- **Arch Linux**: AUR, pacman, builds personalizados
- **Debian/Ubuntu**: systemd, configuración empresarial
- **Red Hat/CentOS**: yum/dnf, sistemas corporativos
- **Gentoo**: compilación basada en fuentes

### 📱 Diseño Responsive Completo
- **320px → 1400px+** soporte total
- **Mobile-first CSS Grid** adaptativo
- **Touch-optimized controls**
- **High contrast & dark theme** automático

## 🛠️ Tecnologías Utilizadas

### Frontend
- **HTML5 semántico** con estructura accessible
- **CSS3 moderno** con variables y Grid Layout
- **JavaScript ES6+** modular y optimizado
- **Canvas API** para visualizaciones

### Backend/Systems
- **C/C++** para algoritmos de alto rendimiento
- **Assembly** (x86/x64/ARM) con NASM/FASM
- **WebAssembly** para cálculos intensivos
- **Linux kernel modules** y drivers

### Tools & Infrastructure
- **Git/GitHub** para control de versiones
- **Emscripten** para compilación WebAssembly
- **Performance monitoring** en tiempo real
- **Automated deployment** con scripts bash

## 🚀 Instalación y Uso

### Requisitos Previos
```bash
# Emscripten (opcional para compilar WebAssembly)
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest

# Python 3 para servidor local
python3 --version
```

### Ejecución Local
```bash
# Clonar el repositorio
git clone https://github.com/AmeliaDi/cv-webassembly.git
cd cv-webassembly

# Servir localmente
python3 -m http.server 8000

# Abrir en navegador
# http://localhost:8000
```

### Compilación WebAssembly (Opcional)
```bash
# Compilar algoritmos a WebAssembly
make wasm

# O manualmente
emcc algorithms.c -O3 -s WASM=1 \
  -s EXPORTED_FUNCTIONS="['_quicksort','_mergesort','_bogosort']" \
  -o algorithms.js
```

## 🎮 Características Interactivas

### ⌨️ Atajos de Teclado
- **Ctrl+1-6**: Navegación rápida por secciones
- **F12**: Consola de desarrolladora con ASCII art
- **ESC**: Limpiar animaciones en curso

### 🎯 Algoritmos Disponibles
1. **QuickSort** - Divide y vencerás (O(n log n))
2. **MergeSort** - Estable y eficiente (O(n log n))
3. **HeapSort** - In-place garantizado (O(n log n))
4. **RadixSort** - No comparativo (O(kn))
5. **CountingSort** - Rango limitado (O(n+k))
6. **ShellSort** - Gap sequence (O(n²))
7. **CocktailSort** - Bidireccional (O(n²))
8. **GnomeSort** - Gnomo del jardín (O(n²))
9. **PancakeSort** - Voltear panqueques (O(n²))
10. **BogoSort** - ¡La locura pura! (O(∞))

### 🧮 Visualizaciones Matemáticas
- **Fractales de Mandelbrot** con zoom interactivo
- **Transformada de Fourier** con generación de ondas
- **Criba de Eratóstenes** para números primos

### 🌐 Herramientas de Red
- **Analizador de paquetes** simulado
- **Port scanner** con detección de servicios
- **Network topology discovery**

## 📊 Métricas de Rendimiento

El CV incluye monitoreo en tiempo real de:
- **Comparaciones y swaps** en algoritmos
- **Tiempo de ejecución** preciso
- **Memory usage** y optimizaciones
- **Rendering performance** en visualizaciones

## 🔧 Arquitectura del Proyecto

```
cv-webassembly/
├── index.html              # Estructura principal
├── styles.css              # Estilos responsive
├── main.js                 # Controlador principal
├── algorithms.js           # Visualizador de algoritmos
├── algorithms.c            # Implementaciones en C
├── visualizations.js       # Matemáticas interactivas
├── network-tools.js        # Herramientas de red
├── wasm-loader.js          # Cargador WebAssembly
├── Makefile               # Scripts de compilación
├── deploy.sh              # Deployment automatizado
└── CHANGELOG.md           # Historial de cambios
```

## 🎨 Características de UI/UX

### Terminal-Style Interface
- **Prompt dinámico** que refleja la navegación
- **Syntax highlighting** para código Assembly/C
- **Matrix rain effect** sutil en el fondo
- **Smooth scrolling** y transiciones fluidas

### Responsive Design
- **Mobile-first approach** desde 320px
- **Adaptive layouts** para tablets y desktop
- **Touch-friendly controls** en dispositivos móviles
- **High contrast support** para accesibilidad

## 📈 Estadísticas del Proyecto

- **16+ certificaciones** en ciberseguridad y networking
- **10 algoritmos** con visualizaciones interactivas
- **4 nuevos algoritmos** añadidos (incluye BogoSort)
- **100% responsive** desde móvil hasta 4K
- **8 años** de experiencia en sistemas
- **20+ archivos** de código fuente

## 🌟 Próximas Características

- [ ] **Más algoritmos exóticos** (TimSort, IntroSort)
- [ ] **Visualizaciones 3D** con WebGL
- [ ] **Tests automatizados** con Jest
- [ ] **Multi-idioma** (ES/EN)
- [ ] **Dark/Light theme toggle**
- [ ] **Modo presentación** fullscreen

## 📞 Contacto

- **Email**: [enorastrokes@gmail.com](mailto:enorastrokes@gmail.com)
- **GitHub**: [github.com/AmeliaDi](https://github.com/AmeliaDi)
- **LinkedIn**: [linkedin.com/in/bogosort](https://linkedin.com/in/bogosort)

---

**Desarrollado con ❤️ por Amelia Enora Marceline Chavez Barroso**  
*Especialista en Sistemas y Programación de Bajo Nivel*

> "La programación no es solo escribir código, es resolver problemas de manera elegante y eficiente." 