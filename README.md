# 🚀 Marcy's Technical CV - WebAssembly Portfolio

Una página web interactiva que funciona como CV técnico, desarrollada con **WebAssembly**, **C**, **JavaScript** y diseñada con un estilo terminal profesional. Demuestra habilidades en programación de bajo nivel, algoritmos, matemáticas computacionales y análisis de redes.

## 🎯 Características Principales

### 💻 **Algoritmos en WebAssembly**
- **Algoritmos de ordenamiento**: QuickSort, MergeSort, HeapSort
- **Criba de Eratóstenes** para números primos
- **Visualización en tiempo real** con estadísticas de rendimiento
- **Benchmarking automático** de complejidad temporal

### 🔢 **Visualizaciones Matemáticas**
- **Conjunto de Mandelbrot** interactivo con zoom
- **Transformada Rápida de Fourier (FFT)** en tiempo real
- **Generación de ondas**: seno, cuadrada, diente de sierra
- **Renderizado optimizado** en Canvas

### 🌐 **Herramientas de Red**
- **Simulador de captura de paquetes** en tiempo real
- **Escáner de puertos** con análisis de servicios
- **Análisis de tráfico** con estadísticas detalladas
- **Calculadora de subredes** y utilidades de red

### 🎨 **Interfaz Terminal**
- **Tema dark profesional** con sintaxis highlighting
- **Efectos Matrix** sutiles en el fondo
- **Navegación por teclado** (Ctrl+1-6)
- **Animaciones de terminal** realistas

## 🛠️ Instalación y Configuración

### Prerrequisitos

```bash
# Linux (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential python3 nodejs npm git

# macOS (con Homebrew)
brew install gcc python3 node git

# Arch Linux
sudo pacman -S base-devel python nodejs npm git
```

### Configuración Rápida

```bash
# 1. Clonar el repositorio
git clone <tu-repositorio>
cd caloa

# 2. Verificar dependencias
make setup

# 3. Iniciar servidor de desarrollo
make serve
```

La página estará disponible en `http://localhost:8000`

### Compilación a WebAssembly (Opcional)

Para compilar el código C a WebAssembly real:

```bash
# Instalar Emscripten SDK
make install-emscripten
source emsdk/emsdk_env.sh

# Compilar algoritmos a WASM
make all

# Verificar compilación
ls -la algorithms.wasm algorithms.js
```

## 📁 Estructura del Proyecto

```
├── index.html              # Página principal con estructura HTML
├── styles.css              # Estilos CSS con tema terminal
├── main.js                 # Controlador principal de la aplicación
├── algorithms.js           # Visualización de algoritmos
├── visualizations.js       # Visualizaciones matemáticas
├── network-tools.js        # Herramientas de red
├── wasm-loader.js          # Cargador de WebAssembly
├── algorithms.c            # Algoritmos en C para WASM
├── Makefile               # Scripts de compilación
└── README.md              # Esta documentación
```

## 🔧 Comandos Disponibles

### Desarrollo
```bash
make serve          # Servidor de desarrollo (Python)
make serve-node     # Servidor alternativo (Node.js)
make setup          # Verificar configuración
```

### Compilación
```bash
make all            # Compilar C a WebAssembly
make clean          # Limpiar archivos generados
make debug          # Build de debug con símbolos
make test-gcc       # Verificar compilación C
```

### Calidad de Código
```bash
make lint           # Análisis estático del código C
make format         # Formatear código C
make production     # Build optimizado para producción
```

### Utilidades
```bash
make benchmark      # Ejecutar benchmarks de rendimiento
make assembly       # Generar código ensamblador
make help           # Mostrar ayuda completa
```

## 🎮 Uso Interactivo

### Navegación por Teclado
- **Ctrl + 1-6**: Navegación rápida entre secciones
- **F12**: Consola de desarrollador
- **ESC**: Limpiar animaciones

### Consola de Desarrollador
Presiona **F12** y ejecuta:

```javascript
// Benchmark de algoritmos
performanceBenchmark.benchmarkSortingAlgorithms()

// Matemáticas
MathUtils.factorial(10)
MathUtils.generateFibonacci(20)

// Análisis de red
NetworkAnalyzer.calculateSubnet('192.168.1.0', 24)
NetworkAnalyzer.generateMACAddress()

// WebAssembly directo
wasmLoader.callFunction('sieve_of_eratosthenes', 1000)
```

## ⚡ Optimizaciones de Rendimiento

### WebAssembly
- **Compilación optimizada** con `-O3`
- **Gestión de memoria** eficiente
- **Funciones exportadas** específicas
- **Fallback JavaScript** automático

### Visualizaciones
- **RequestAnimationFrame** para animaciones fluidas
- **Canvas optimizado** para renderizado
- **Throttling inteligente** de actualizaciones
- **Worker threads** para cálculos pesados (futuro)

### Interfaz
- **CSS Grid** y **Flexbox** modernos
- **Smooth scrolling** nativo
- **Intersection Observer** para navegación
- **Event delegation** eficiente

## 🔍 Detalles Técnicos

### Algoritmos Implementados

#### Ordenamiento
```c
// QuickSort con optimizaciones
int32_t quicksort(int32_t* arr, int32_t size);

// MergeSort estable
int32_t mergesort(int32_t* arr, int32_t size);

// HeapSort in-place
int32_t heapsort(int32_t* arr, int32_t size);
```

#### Matemáticas
```c
// Mandelbrot con iteraciones variables
int32_t mandelbrot_point(double x, double y, int32_t max_iter);

// FFT optimizada Cooley-Tukey
void fft_real(double* real, double* imag, int32_t n);

// Criba de Eratóstenes
int32_t sieve_of_eratosthenes(int32_t limit);
```

### Arquitectura del Código

```javascript
// Patrón de módulos ES6
class AlgorithmVisualizer {
    constructor() { /* ... */ }
    async runSortingAlgorithm(algorithm) { /* ... */ }
}

// Sistema de eventos reactivo
document.addEventListener('DOMContentLoaded', async () => {
    await wasmLoader.loadModule('algorithms.wasm');
    algorithmVisualizer.init();
});
```

## 🎨 Personalización

### Temas de Color
Modifica las variables CSS en `styles.css`:

```css
:root {
    --bg-dark: #0a0a0a;
    --text-primary: #00ff00;
    --accent-blue: #00aaff;
    /* ... más variables */
}
```

### Algoritmos Personalizados
Añade funciones en `algorithms.c`:

```c
WASM_EXPORT
int32_t mi_algoritmo(int32_t* data, int32_t size) {
    // Tu implementación aquí
    return resultado;
}
```

## 🚀 Despliegue

### GitHub Pages
```bash
# Build de producción
make production

# Configurar GitHub Pages
# Usar branch gh-pages o carpeta docs/
```

### Servidor Propio
```bash
# Nginx config
server {
    listen 80;
    server_name tu-dominio.com;
    root /path/to/caloa;
    
    # MIME types para WASM
    location ~* \.wasm$ {
        add_header Content-Type application/wasm;
    }
}
```

## 🤝 Contribuciones

¡Las contribuciones son bienvenidas! Por favor:

1. **Fork** el proyecto
2. **Crea** una rama feature (`git checkout -b feature/nueva-funcionalidad`)
3. **Commit** tus cambios (`git commit -am 'Añadir nueva funcionalidad'`)
4. **Push** a la rama (`git push origin feature/nueva-funcionalidad`)
5. **Crea** un Pull Request

### Áreas de Mejora
- [ ] **Más algoritmos**: A*, Dijkstra, algoritmos de grafos
- [ ] **Web Workers**: Para cálculos paralelos
- [ ] **WebGL**: Renderizado acelerado por GPU
- [ ] **PWA**: Service workers y funcionamiento offline
- [ ] **Tests**: Suite de testing automatizado

## 📝 Licencia

Este proyecto está bajo la **Licencia MIT**. Ver `LICENSE` para más detalles.

## 👨‍💻 Autor

**AmeliaDi** - Desarrollador de Sistemas y Programación de Bajo Nivel

- **Email**: enorastrokes@gmail.com
- **GitHub**: [github.com/AmeliaDi](https://github.com/AmeliaDi)
- **LinkedIn**: [linkedin.com/in/bogosort](https://linkedin.com/in/bogosort)

## 🙏 Agradecimientos

- **Emscripten Project** - Por hacer WebAssembly accesible
- **Mozilla MDN** - Documentación excelente de Web APIs
- **Community** - Por las librerías y herramientas open source

---

<div align="center">

**⭐ Si te gusta este proyecto, dale una estrella en GitHub ⭐**

**💡 ¿Preguntas o sugerencias? Abre un issue**

</div> 