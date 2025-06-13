/**
 * Neural Network Visualization System - NEON PINK EDITION
 * Ultra-optimized interactive neural network with real-time training
 * @author Amelia Enora 🌈 Marceline Chavez Barroso
 * @version 3.0 - Ultra Performance & Pink Theme
 */

class NeuralNetworkVisualization {
    constructor() {
        // Core elements
        this.canvas = document.getElementById('neural-network-canvas');
        this.ctx = this.canvas.getContext('2d');
        
        // Network state
        this.network = null;
        this.isTraining = false;
        this.isAnimating = false;
        this.isPredicting = false;
        
        // Training data
        this.trainingData = null;
        this.currentEpoch = 0;
        this.currentLoss = 0;
        this.accuracy = 0;
        this.trainingStartTime = 0;
        
        // Animation
        this.animationId = null;
        this.lastFrameTime = 0;
        this.fps = 60;
        this.frameInterval = 1000 / this.fps;
        
        // UI state
        this.hoveredNeuron = null;
        this.hoveredConnection = null;
        this.tooltip = { show: false, x: 0, y: 0, text: '' };
        
        // Performance optimization
        this.renderCache = new Map();
        this.lastRenderHash = '';
        
        // Colors - Neon Pink Theme REFINED & SUBTLE
        this.colors = {
            background: '#0f0520',
            primary: '#ff3399',        // Más suave que #ff0099
            secondary: '#ff99cc',      // Más suave que #ff66cc
            accent: '#ff66bb',         // Más suave que #ff33bb
            success: '#66ff99',        // Verde menta suave
            warning: '#ffaa44',
            error: '#ff4466',
            text: '#ffffff',
            
            // Gradients - Más sutiles
            neuronActive: ['#ff3399', '#ff99cc'],
            neuronInactive: ['#2d1b3d', '#1a0628'],
            connectionPositive: '#ff99cc',
            connectionNegative: '#ff3399',
            connectionActive: '#66ff99',
            
            // Glows - Más sutiles
            glowPrimary: 'rgba(255, 51, 153, 0.4)',     // Reducido de 0.6
            glowSecondary: 'rgba(255, 153, 204, 0.3)',  // Reducido de 0.4
            glowAccent: 'rgba(255, 102, 187, 0.25)'     // Reducido de 0.3
        };
        
        this.initialize();
    }
    
    async initialize() {
        try {
            this.setupCanvas();
            this.initializeNetwork();
            this.setupEventListeners();
            this.generateTrainingData();
            this.startRenderLoop();
        } catch (error) {
            console.error('Failed to initialize neural network:', error);
        }
    }
    
    setupCanvas() {
        // High DPI support
        const dpr = window.devicePixelRatio || 1;
        const rect = this.canvas.getBoundingClientRect();
        
        this.canvas.width = rect.width * dpr;
        this.canvas.height = rect.height * dpr;
        
        this.ctx.scale(dpr, dpr);
        this.canvas.style.width = rect.width + 'px';
        this.canvas.style.height = rect.height + 'px';
        
        // Performance optimizations
        this.ctx.imageSmoothingEnabled = true;
        this.ctx.imageSmoothingQuality = 'high';
    }
    
    initializeNetwork() {
        const architecture = document.getElementById('nn-architecture').value;
        const layers = architecture.split('-').map(n => parseInt(n));
        
        this.network = {
            layers,
            weights: this.initializeWeights(layers),
            biases: this.initializeBiases(layers),
            activations: layers.map(size => new Float32Array(size)),
            neurons: [],
            connections: [],
            gradients: {
                weights: [],
                biases: []
            }
        };
        
        this.calculateNeuronPositions();
        this.resetStats();
        this.invalidateRenderCache();
    }
    
    initializeWeights(layers) {
        const weights = [];
        for (let i = 0; i < layers.length - 1; i++) {
            const layerWeights = [];
            const fanIn = layers[i];
            const fanOut = layers[i + 1];
            const limit = Math.sqrt(6 / (fanIn + fanOut)); // Xavier initialization
            
            for (let j = 0; j < fanIn; j++) {
                const neuronWeights = new Float32Array(fanOut);
                for (let k = 0; k < fanOut; k++) {
                    neuronWeights[k] = (Math.random() * 2 - 1) * limit;
                }
                layerWeights.push(neuronWeights);
            }
            weights.push(layerWeights);
        }
        return weights;
    }
    
    initializeBiases(layers) {
        const biases = [];
        for (let i = 1; i < layers.length; i++) {
            biases.push(new Float32Array(layers[i]));
        }
        return biases;
    }
    
    calculateNeuronPositions() {
        const rect = this.canvas.getBoundingClientRect();
        const margin = Math.min(rect.width, rect.height) * 0.1;
        const width = rect.width - 2 * margin;
        const height = rect.height - 2 * margin;
        
        this.network.neurons = [];
        this.network.connections = [];
        
        const layerSpacing = width / Math.max(1, this.network.layers.length - 1);
        const maxNeurons = Math.max(...this.network.layers);
        const baseRadius = Math.min(20, Math.min(width, height) / (maxNeurons * 3));
        
        // Calculate neuron positions
        for (let layer = 0; layer < this.network.layers.length; layer++) {
            const layerNeurons = [];
            const neuronCount = this.network.layers[layer];
            const neuronSpacing = height / Math.max(1, neuronCount - 1);
            const startY = margin + (height - (neuronCount - 1) * neuronSpacing) / 2;
            
            for (let neuron = 0; neuron < neuronCount; neuron++) {
                const x = margin + layer * layerSpacing;
                const y = neuronCount === 1 ? margin + height / 2 : startY + neuron * neuronSpacing;
                
                layerNeurons.push({
                    x, y,
                    activation: 0,
                    bias: layer > 0 ? this.network.biases[layer - 1][neuron] : 0,
                    radius: baseRadius,
                    layer,
                    index: neuron
                });
            }
            
            this.network.neurons.push(layerNeurons);
            
            // Create optimized connections
            if (layer < this.network.layers.length - 1) {
                const connections = [];
                for (let i = 0; i < this.network.layers[layer]; i++) {
                    for (let j = 0; j < this.network.layers[layer + 1]; j++) {
                        connections.push({
                            fromLayer: layer,
                            fromNeuron: i,
                            toLayer: layer + 1,
                            toNeuron: j,
                            weight: this.network.weights[layer][i][j],
                            active: false,
                            activation: 0
                        });
                    }
                }
                this.network.connections.push(...connections);
            }
        }
    }
    
    generateTrainingData() {
        const dataSize = 2000; // Increased for better training
        const inputSize = this.network.layers[0];
        const outputSize = this.network.layers[this.network.layers.length - 1];
        
        this.trainingData = {
            inputs: new Array(dataSize),
            outputs: new Array(dataSize)
        };
        
        // Generate more sophisticated patterns
        for (let i = 0; i < dataSize; i++) {
            const input = new Float32Array(inputSize);
            
            // Create different patterns based on input size
            if (inputSize <= 4) {
                // Simple XOR-like patterns
                for (let j = 0; j < inputSize; j++) {
                    input[j] = Math.random() > 0.5 ? 1 : -1;
                }
            } else {
                // Gaussian-like patterns
                for (let j = 0; j < inputSize; j++) {
                    input[j] = (Math.random() - 0.5) * 2;
                }
            }
            
            // Generate sophisticated target outputs
            const output = new Float32Array(outputSize);
            const sum = input.reduce((a, b) => a + b, 0);
            const variance = input.reduce((acc, val) => acc + val * val, 0) / inputSize;
            
            if (outputSize === 2) {
                // Binary classification
                output[sum > 0 ? 1 : 0] = 1;
            } else if (outputSize === 3) {
                // Ternary classification
                if (variance > 0.5) output[0] = 1;
                else if (sum > 0) output[1] = 1;
                else output[2] = 1;
            } else {
                // Multi-class based on input properties
                const index = Math.floor(((sum + inputSize) / (2 * inputSize)) * outputSize);
                output[Math.min(Math.max(index, 0), outputSize - 1)] = 1;
            }
            
            this.trainingData.inputs[i] = input;
            this.trainingData.outputs[i] = output;
        }
    }
    
    setupEventListeners() {
        // Network architecture
        document.getElementById('nn-architecture').addEventListener('change', () => {
            this.initializeNetwork();
            this.generateTrainingData();
        });
        
        // Learning rate
        const lrSlider = document.getElementById('learning-rate');
        const lrValue = document.getElementById('lr-value');
        lrSlider.addEventListener('input', (e) => {
            lrValue.textContent = parseFloat(e.target.value).toFixed(2);
        });
        
        // Control buttons
        document.getElementById('nn-train').addEventListener('click', () => {
            this.isTraining ? this.stopTraining() : this.startTraining();
        });
        
        document.getElementById('nn-predict').addEventListener('click', () => {
            this.predict();
        });
        
        document.getElementById('nn-reset').addEventListener('click', () => {
            this.resetNetwork();
        });
        
        document.getElementById('nn-animate').addEventListener('click', () => {
            this.toggleAnimation();
        });
        
        // Mouse interaction
        this.canvas.addEventListener('mousemove', (e) => {
            this.handleMouseMove(e);
        });
        
        this.canvas.addEventListener('mouseleave', () => {
            this.hoveredNeuron = null;
            this.hoveredConnection = null;
            this.tooltip.show = false;
        });
        
        // Resize handling
        window.addEventListener('resize', () => {
            this.debounce(() => {
                this.setupCanvas();
                this.calculateNeuronPositions();
                this.invalidateRenderCache();
            }, 100)();
        });
    }
    
    debounce(func, wait) {
        let timeout;
        return function executedFunction(...args) {
            const later = () => {
                clearTimeout(timeout);
                func(...args);
            };
            clearTimeout(timeout);
            timeout = setTimeout(later, wait);
        };
    }
    
    // Optimized activation functions
    activationFunction(x, type = 'relu') {
        switch (type) {
            case 'relu':
                return Math.max(0, x);
            case 'sigmoid':
                return 1 / (1 + Math.exp(-Math.max(-500, Math.min(500, x)))); // Prevent overflow
            case 'tanh':
                return Math.tanh(x);
            case 'leaky_relu':
                return x > 0 ? x : 0.01 * x;
            default:
                return x;
        }
    }
    
    activationDerivative(x, type = 'relu') {
        switch (type) {
            case 'relu':
                return x > 0 ? 1 : 0;
            case 'sigmoid':
                const sig = this.activationFunction(x, 'sigmoid');
                return sig * (1 - sig);
            case 'tanh':
                const t = Math.tanh(x);
                return 1 - t * t;
            case 'leaky_relu':
                return x > 0 ? 1 : 0.01;
            default:
                return 1;
        }
    }
    
    // Optimized forward pass
    forward(input) {
        const activationType = document.getElementById('activation-function').value;
        
        // Set input layer
        this.network.activations[0].set(input);
        
        // Forward propagation
        for (let layer = 0; layer < this.network.layers.length - 1; layer++) {
            const currentActivations = this.network.activations[layer];
            const nextActivations = this.network.activations[layer + 1];
            const weights = this.network.weights[layer];
            const biases = this.network.biases[layer];
            
            // Reset next layer
            nextActivations.fill(0);
            
            // Compute weighted sum
            for (let i = 0; i < currentActivations.length; i++) {
                const activation = currentActivations[i];
                const neuronWeights = weights[i];
                
                for (let j = 0; j < nextActivations.length; j++) {
                    nextActivations[j] += activation * neuronWeights[j];
                }
            }
            
            // Add bias and apply activation
            for (let j = 0; j < nextActivations.length; j++) {
                nextActivations[j] += biases[j];
                nextActivations[j] = this.activationFunction(nextActivations[j], activationType);
            }
        }
        
        // Update neuron activations for visualization
        for (let layer = 0; layer < this.network.neurons.length; layer++) {
            for (let neuron = 0; neuron < this.network.neurons[layer].length; neuron++) {
                this.network.neurons[layer][neuron].activation = this.network.activations[layer][neuron];
            }
        }
        
        return Array.from(this.network.activations[this.network.activations.length - 1]);
    }
    
    calculateLoss(predicted, target) {
        let loss = 0;
        for (let i = 0; i < predicted.length; i++) {
            const diff = predicted[i] - target[i];
            loss += diff * diff;
        }
        return loss / predicted.length;
    }
    
    async startTraining() {
        if (this.isTraining) return;
        
        this.isTraining = true;
        this.trainingStartTime = Date.now();
        
        const trainBtn = document.getElementById('nn-train');
        trainBtn.textContent = '⏹️ Stop';
        trainBtn.classList.add('training');
        
        const learningRate = parseFloat(document.getElementById('learning-rate').value);
        const batchSize = 32;
        const epochs = 1000;
        
        try {
            for (let epoch = 0; epoch < epochs && this.isTraining; epoch++) {
                let totalLoss = 0;
                let correct = 0;
                
                // Shuffle data for better training
                const indices = Array.from({length: this.trainingData.inputs.length}, (_, i) => i);
                this.shuffleArray(indices);
                
                // Mini-batch training
                for (let batch = 0; batch < indices.length; batch += batchSize) {
                    const batchIndices = indices.slice(batch, batch + batchSize);
                    let batchLoss = 0;
                    
                    for (const idx of batchIndices) {
                        const input = this.trainingData.inputs[idx];
                        const target = this.trainingData.outputs[idx];
                        
                        const predicted = this.forward(input);
                        const loss = this.calculateLoss(predicted, target);
                        
                        batchLoss += loss;
                        totalLoss += loss;
                        
                        // Check accuracy
                        const predictedClass = predicted.indexOf(Math.max(...predicted));
                        const targetClass = target.indexOf(Math.max(...target));
                        if (predictedClass === targetClass) correct++;
                        
                        // Backpropagation (simplified)
                        this.backpropagate(input, target, predicted, learningRate / batchSize);
                    }
                }
                
                this.currentEpoch = epoch + 1;
                this.currentLoss = totalLoss / this.trainingData.inputs.length;
                this.accuracy = (correct / this.trainingData.inputs.length) * 100;
                
                // Update UI every 10 epochs
                if (epoch % 10 === 0) {
                    this.updateStats();
                    await new Promise(resolve => setTimeout(resolve, 1));
                }
            }
        } catch (error) {
            console.error('Training error:', error);
        } finally {
            this.stopTraining();
        }
    }
    
    stopTraining() {
        this.isTraining = false;
        const trainBtn = document.getElementById('nn-train');
        trainBtn.textContent = '🎯 Entrenar';
        trainBtn.classList.remove('training');
    }
    
    shuffleArray(array) {
        for (let i = array.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [array[i], array[j]] = [array[j], array[i]];
        }
    }
    
    // Simplified backpropagation
    backpropagate(input, target, predicted, learningRate) {
        const activationType = document.getElementById('activation-function').value;
        
        // Calculate output layer error
        const outputError = new Float32Array(predicted.length);
        for (let i = 0; i < predicted.length; i++) {
            outputError[i] = (predicted[i] - target[i]) * 
                           this.activationDerivative(predicted[i], activationType);
        }
        
        // Backpropagate errors and update weights
        let layerError = outputError;
        
        for (let layer = this.network.layers.length - 2; layer >= 0; layer--) {
            const currentActivations = this.network.activations[layer];
            const weights = this.network.weights[layer];
            const nextError = new Float32Array(currentActivations.length);
            
            // Update weights and calculate next layer error
            for (let i = 0; i < currentActivations.length; i++) {
                const activation = currentActivations[i];
                
                for (let j = 0; j < layerError.length; j++) {
                    const error = layerError[j];
                    
                    // Update weight
                    weights[i][j] -= learningRate * error * activation;
                    
                    // Accumulate error for previous layer
                    nextError[i] += error * weights[i][j];
                }
                
                // Apply activation derivative
                nextError[i] *= this.activationDerivative(activation, activationType);
            }
            
            // Update biases
            if (layer < this.network.biases.length) {
                const biases = this.network.biases[layer];
                for (let j = 0; j < biases.length; j++) {
                    biases[j] -= learningRate * layerError[j];
                }
            }
            
            layerError = nextError;
        }
    }
    
    predict() {
        if (this.isPredicting) return;
        
        this.isPredicting = true;
        
        // Generate random input
        const input = new Float32Array(this.network.layers[0]);
        for (let i = 0; i < input.length; i++) {
            input[i] = (Math.random() - 0.5) * 2;
        }
        
        const predicted = this.forward(input);
        
        // Animate prediction
        this.animatePrediction();
        
        console.log('Input:', Array.from(input).map(x => x.toFixed(3)));
        console.log('Output:', predicted.map(x => x.toFixed(3)));
        
        setTimeout(() => {
            this.isPredicting = false;
        }, 2000);
    }
    
    animatePrediction() {
        const duration = 2000;
        const startTime = Date.now();
        
        const animate = () => {
            const elapsed = Date.now() - startTime;
            const progress = Math.min(elapsed / duration, 1);
            
            // Animate connections
            this.network.connections.forEach((conn, index) => {
                const delay = (index / this.network.connections.length) * duration * 0.5;
                const localProgress = Math.max(0, Math.min(1, (elapsed - delay) / (duration * 0.5)));
                
                conn.activation = Math.sin(localProgress * Math.PI) * Math.abs(conn.weight);
            });
            
            if (progress < 1) {
                requestAnimationFrame(animate);
            } else {
                // Reset animations
                this.network.connections.forEach(conn => {
                    conn.activation = 0;
                });
            }
        };
        
        animate();
    }
    
    toggleAnimation() {
        this.isAnimating = !this.isAnimating;
        const animBtn = document.getElementById('nn-animate');
        animBtn.textContent = this.isAnimating ? '⏸️ Parar' : '⚡ Animar';
        
        if (this.isAnimating) {
            this.startAnimation();
        }
    }
    
    startAnimation() {
        if (!this.isAnimating) return;
        
        const animate = () => {
            if (!this.isAnimating) return;
            
            // Animate random activations
            for (let layer = 0; layer < this.network.neurons.length; layer++) {
                for (let neuron of this.network.neurons[layer]) {
                    neuron.activation = Math.sin(Date.now() * 0.005 + neuron.x * 0.01 + neuron.y * 0.01) * 0.5 + 0.5;
                }
            }
            
            // Animate connections
            this.network.connections.forEach((conn, index) => {
                conn.activation = Math.sin(Date.now() * 0.003 + index * 0.1) * 0.5 + 0.5;
            });
            
            requestAnimationFrame(animate);
        };
        
        animate();
    }
    
    resetNetwork() {
        this.stopTraining();
        this.isAnimating = false;
        
        const animBtn = document.getElementById('nn-animate');
        animBtn.textContent = '⚡ Animar';
        
        this.initializeNetwork();
        this.generateTrainingData();
    }
    
    resetStats() {
        this.currentEpoch = 0;
        this.currentLoss = 0;
        this.accuracy = 0;
        this.trainingStartTime = 0;
        this.updateStats();
    }
    
    updateStats() {
        document.getElementById('epochs').textContent = this.currentEpoch;
        document.getElementById('loss').textContent = this.currentLoss.toFixed(4);
        document.getElementById('accuracy').textContent = this.accuracy.toFixed(1) + '%';
        
        const elapsed = this.trainingStartTime ? Date.now() - this.trainingStartTime : 0;
        document.getElementById('training-time').textContent = elapsed + 'ms';
    }
    
    handleMouseMove(e) {
        const rect = this.canvas.getBoundingClientRect();
        const x = e.clientX - rect.left;
        const y = e.clientY - rect.top;
        
        this.hoveredNeuron = null;
        this.hoveredConnection = null;
        this.tooltip.show = false;
        
        // Check neurons
        for (let layer = 0; layer < this.network.neurons.length; layer++) {
            for (let neuron of this.network.neurons[layer]) {
                const dx = x - neuron.x;
                const dy = y - neuron.y;
                const distance = Math.sqrt(dx * dx + dy * dy);
                
                if (distance <= neuron.radius) {
                    this.hoveredNeuron = neuron;
                    this.tooltip = {
                        show: true,
                        x: x + 10,
                        y: y - 10,
                        text: `Layer ${layer}, Neuron ${neuron.index}\nActivation: ${neuron.activation.toFixed(3)}\nBias: ${neuron.bias.toFixed(3)}`
                    };
                    return;
                }
            }
        }
        
        // Check connections (simplified)
        // Due to performance, we'll skip connection hover detection for large networks
        if (this.network.connections.length < 200) {
            for (let conn of this.network.connections) {
                const fromNeuron = this.network.neurons[conn.fromLayer][conn.fromNeuron];
                const toNeuron = this.network.neurons[conn.toLayer][conn.toNeuron];
                
                // Simple line distance check (approximation)
                const lineLength = Math.sqrt(
                    Math.pow(toNeuron.x - fromNeuron.x, 2) + 
                    Math.pow(toNeuron.y - fromNeuron.y, 2)
                );
                
                const distance = Math.abs(
                    (toNeuron.y - fromNeuron.y) * x - 
                    (toNeuron.x - fromNeuron.x) * y + 
                    toNeuron.x * fromNeuron.y - 
                    toNeuron.y * fromNeuron.x
                ) / lineLength;
                
                if (distance < 5) {
                    this.hoveredConnection = conn;
                    this.tooltip = {
                        show: true,
                        x: x + 10,
                        y: y - 10,
                        text: `Weight: ${conn.weight.toFixed(3)}`
                    };
                    break;
                }
            }
        }
    }
    
    startRenderLoop() {
        const render = (currentTime) => {
            if (currentTime - this.lastFrameTime >= this.frameInterval) {
                this.render();
                this.lastFrameTime = currentTime;
            }
            requestAnimationFrame(render);
        };
        requestAnimationFrame(render);
    }
    
    invalidateRenderCache() {
        this.renderCache.clear();
        this.lastRenderHash = '';
    }
    
    getRenderHash() {
        return JSON.stringify({
            neurons: this.network.neurons.map(layer => 
                layer.map(n => [n.x, n.y, n.activation.toFixed(2)])
            ),
            connections: this.network.connections.slice(0, 50).map(c => 
                [c.weight.toFixed(2), c.activation.toFixed(2)]
            ),
            hover: this.hoveredNeuron ? 
                `${this.hoveredNeuron.layer}-${this.hoveredNeuron.index}` : null
        });
    }
    
    render() {
        const currentHash = this.getRenderHash();
        if (currentHash === this.lastRenderHash && !this.isAnimating && !this.isPredicting) {
            return; // Skip rendering if nothing changed
        }
        this.lastRenderHash = currentHash;
        
        // Clear canvas with neon background
        this.ctx.fillStyle = this.colors.background;
        this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
        
        // Add subtle grid pattern
        this.renderGrid();
        
        // Render network components
        this.renderConnections();
        this.renderNeurons();
        this.renderLabels();
        
        // Render UI elements
        if (this.tooltip.show) {
            this.renderTooltip();
        }
    }
    
    renderGrid() {
        const rect = this.canvas.getBoundingClientRect();
        const gridSize = 60;                    // Ligeramente más grande
        
        this.ctx.strokeStyle = 'rgba(255, 51, 153, 0.03)';  // Más sutil
        this.ctx.lineWidth = 0.3;               // Líneas más finas
        this.ctx.beginPath();
        
        for (let x = 0; x < rect.width; x += gridSize) {
            this.ctx.moveTo(x, 0);
            this.ctx.lineTo(x, rect.height);
        }
        
        for (let y = 0; y < rect.height; y += gridSize) {
            this.ctx.moveTo(0, y);
            this.ctx.lineTo(rect.width, y);
        }
        
        this.ctx.stroke();
    }
    
    renderConnections() {
        for (let conn of this.network.connections) {
            const fromNeuron = this.network.neurons[conn.fromLayer][conn.fromNeuron];
            const toNeuron = this.network.neurons[conn.toLayer][conn.toNeuron];
            
            const weight = conn.weight;
            const isHovered = this.hoveredConnection === conn;
            const activation = conn.activation || 0;
            
            // Connection style based on weight and activation
            const absWeight = Math.abs(weight);
            const opacity = Math.min(0.8, Math.max(0.1, absWeight));
            const lineWidth = Math.max(0.5, Math.min(4, absWeight * 3));
            
            // Color based on weight sign and activation
            let color;
            if (activation > 0) {
                color = this.colors.connectionActive;
            } else if (weight > 0) {
                color = this.colors.connectionPositive;
            } else {
                color = this.colors.connectionNegative;
            }
            
            this.ctx.strokeStyle = isHovered ? 
                this.colors.accent : 
                this.adjustOpacity(color, opacity + activation * 0.3);
            this.ctx.lineWidth = isHovered ? lineWidth + 1 : lineWidth;
            
            // Add glow for active connections
            if (activation > 0 || isHovered) {
                this.ctx.shadowColor = color;
                this.ctx.shadowBlur = 10;
            }
            
            this.ctx.beginPath();
            this.ctx.moveTo(fromNeuron.x, fromNeuron.y);
            this.ctx.lineTo(toNeuron.x, toNeuron.y);
            this.ctx.stroke();
            
            this.ctx.shadowBlur = 0;
        }
    }
    
    renderNeurons() {
        for (let layer = 0; layer < this.network.neurons.length; layer++) {
            for (let neuron of this.network.neurons[layer]) {
                const isHovered = this.hoveredNeuron === neuron;
                const activation = Math.max(0, Math.min(1, neuron.activation));
                
                // Neuron colors
                const gradient = this.ctx.createRadialGradient(
                    neuron.x, neuron.y, 0,
                    neuron.x, neuron.y, neuron.radius
                );
                
                if (activation > 0.1) {
                    gradient.addColorStop(0, this.adjustOpacity(this.colors.neuronActive[0], activation));
                    gradient.addColorStop(1, this.adjustOpacity(this.colors.neuronActive[1], activation * 0.7));
                } else {
                    gradient.addColorStop(0, this.colors.neuronInactive[0]);
                    gradient.addColorStop(1, this.colors.neuronInactive[1]);
                }
                
                this.ctx.fillStyle = gradient;
                
                // Add glow for active neurons
                if (activation > 0.5 || isHovered) {
                    this.ctx.shadowColor = this.colors.glowPrimary;
                    this.ctx.shadowBlur = isHovered ? 20 : 15;
                }
                
                this.ctx.beginPath();
                this.ctx.arc(neuron.x, neuron.y, neuron.radius, 0, 2 * Math.PI);
                this.ctx.fill();
                
                // Border
                this.ctx.strokeStyle = isHovered ? 
                    this.colors.accent : 
                    this.adjustOpacity(this.colors.primary, 0.8);
                this.ctx.lineWidth = isHovered ? 3 : 2;
                this.ctx.stroke();
                
                this.ctx.shadowBlur = 0;
                
                // Activation value text for highly active neurons
                if (activation > 0.7) {
                    this.ctx.fillStyle = this.colors.text;
                    this.ctx.font = '10px JetBrains Mono';
                    this.ctx.textAlign = 'center';
                    this.ctx.fillText(
                        activation.toFixed(1), 
                        neuron.x, 
                        neuron.y + 3
                    );
                }
            }
        }
    }
    
    renderLabels() {
        this.ctx.fillStyle = this.colors.secondary;
        this.ctx.font = 'bold 14px JetBrains Mono';
        this.ctx.textAlign = 'center';
        
        const labels = ['Input', ...Array(this.network.layers.length - 2).fill('Hidden'), 'Output'];
        
        for (let layer = 0; layer < this.network.neurons.length; layer++) {
            if (this.network.neurons[layer].length > 0) {
                const neuron = this.network.neurons[layer][0];
                const y = neuron.y - this.network.neurons[layer][0].radius - 20;
                
                this.ctx.fillText(labels[layer], neuron.x, y);
            }
        }
    }
    
    renderTooltip() {
        if (!this.tooltip.show) return;
        
        const lines = this.tooltip.text.split('\n');
        const padding = 10;
        const lineHeight = 16;
        const width = Math.max(...lines.map(line => 
            this.ctx.measureText(line).width
        )) + padding * 2;
        const height = lines.length * lineHeight + padding * 2;
        
        // Tooltip background
        this.ctx.fillStyle = 'rgba(0, 0, 0, 0.9)';
        this.ctx.fillRect(this.tooltip.x, this.tooltip.y, width, height);
        
        // Tooltip border
        this.ctx.strokeStyle = this.colors.primary;
        this.ctx.lineWidth = 2;
        this.ctx.strokeRect(this.tooltip.x, this.tooltip.y, width, height);
        
        // Tooltip text
        this.ctx.fillStyle = this.colors.text;
        this.ctx.font = '12px JetBrains Mono';
        this.ctx.textAlign = 'left';
        
        lines.forEach((line, index) => {
            this.ctx.fillText(
                line,
                this.tooltip.x + padding,
                this.tooltip.y + padding + (index + 1) * lineHeight
            );
        });
    }
    
    adjustOpacity(color, opacity) {
        if (color.startsWith('#')) {
            const r = parseInt(color.slice(1, 3), 16);
            const g = parseInt(color.slice(3, 5), 16);
            const b = parseInt(color.slice(5, 7), 16);
            return `rgba(${r}, ${g}, ${b}, ${opacity})`;
        }
        return color;
    }
}

// Initialize when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    if (document.getElementById('neural-network-canvas')) {
        new NeuralNetworkVisualization();
    }
}); 