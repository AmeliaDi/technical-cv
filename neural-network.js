/**
 * Neural Network Visualization System
 * Interactive neural network with real-time training visualization
 */

class NeuralNetworkVisualization {
    constructor() {
        this.canvas = document.getElementById('neural-network-canvas');
        this.ctx = this.canvas.getContext('2d');
        this.network = null;
        this.isTraining = false;
        this.isAnimating = false;
        this.trainingData = null;
        this.currentEpoch = 0;
        this.currentLoss = 0;
        this.accuracy = 0;
        this.animationId = null;
        
        this.initializeNetwork();
        this.setupEventListeners();
        this.generateTrainingData();
        this.render();
    }
    
    initializeNetwork() {
        const architecture = document.getElementById('nn-architecture').value;
        const layers = architecture.split('-').map(n => parseInt(n));
        
        this.network = {
            layers: layers,
            weights: [],
            biases: [],
            activations: [],
            neurons: [],
            connections: []
        };
        
        // Initialize weights and biases
        for (let i = 0; i < layers.length - 1; i++) {
            const weights = [];
            for (let j = 0; j < layers[i]; j++) {
                const neuronWeights = [];
                for (let k = 0; k < layers[i + 1]; k++) {
                    neuronWeights.push((Math.random() - 0.5) * 2);
                }
                weights.push(neuronWeights);
            }
            this.network.weights.push(weights);
            
            const biases = [];
            for (let j = 0; j < layers[i + 1]; j++) {
                biases.push((Math.random() - 0.5) * 2);
            }
            this.network.biases.push(biases);
        }
        
        // Initialize activations
        for (let i = 0; i < layers.length; i++) {
            this.network.activations.push(new Array(layers[i]).fill(0));
        }
        
        this.calculateNeuronPositions();
        this.resetStats();
    }
    
    calculateNeuronPositions() {
        const margin = 80;
        const width = this.canvas.width - 2 * margin;
        const height = this.canvas.height - 2 * margin;
        
        this.network.neurons = [];
        this.network.connections = [];
        
        const layerSpacing = width / (this.network.layers.length - 1);
        
        for (let layer = 0; layer < this.network.layers.length; layer++) {
            const layerNeurons = [];
            const neuronCount = this.network.layers[layer];
            const neuronSpacing = height / (neuronCount + 1);
            
            for (let neuron = 0; neuron < neuronCount; neuron++) {
                const x = margin + layer * layerSpacing;
                const y = margin + (neuron + 1) * neuronSpacing;
                
                layerNeurons.push({
                    x: x,
                    y: y,
                    activation: 0,
                    bias: layer > 0 ? this.network.biases[layer - 1][neuron] : 0,
                    radius: Math.min(15, 300 / Math.max(...this.network.layers))
                });
            }
            
            this.network.neurons.push(layerNeurons);
            
            // Create connections to next layer
            if (layer < this.network.layers.length - 1) {
                for (let i = 0; i < this.network.layers[layer]; i++) {
                    for (let j = 0; j < this.network.layers[layer + 1]; j++) {
                        this.network.connections.push({
                            from: { layer: layer, neuron: i },
                            to: { layer: layer + 1, neuron: j },
                            weight: this.network.weights[layer][i][j],
                            active: false
                        });
                    }
                }
            }
        }
    }
    
    generateTrainingData() {
        const dataSize = 1000;
        this.trainingData = {
            inputs: [],
            outputs: []
        };
        
        const inputSize = this.network.layers[0];
        const outputSize = this.network.layers[this.network.layers.length - 1];
        
        for (let i = 0; i < dataSize; i++) {
            const input = [];
            for (let j = 0; j < inputSize; j++) {
                input.push(Math.random() * 2 - 1);
            }
            
            // Generate target output based on simple pattern
            const output = new Array(outputSize).fill(0);
            const sum = input.reduce((a, b) => a + b, 0);
            if (outputSize === 2) {
                output[sum > 0 ? 1 : 0] = 1;
            } else {
                const index = Math.floor((sum + inputSize) / (2 * inputSize) * outputSize);
                output[Math.min(Math.max(index, 0), outputSize - 1)] = 1;
            }
            
            this.trainingData.inputs.push(input);
            this.trainingData.outputs.push(output);
        }
    }
    
    setupEventListeners() {
        document.getElementById('nn-architecture').addEventListener('change', () => {
            this.initializeNetwork();
            this.generateTrainingData();
            this.render();
        });
        
        document.getElementById('learning-rate').addEventListener('input', (e) => {
            document.getElementById('lr-value').textContent = e.target.value;
        });
        
        document.getElementById('nn-train').addEventListener('click', () => {
            this.startTraining();
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
        
        this.canvas.addEventListener('mousemove', (e) => {
            this.handleMouseMove(e);
        });
    }
    
    activationFunction(x, type = 'relu') {
        switch (type) {
            case 'relu':
                return Math.max(0, x);
            case 'sigmoid':
                return 1 / (1 + Math.exp(-x));
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
    
    forward(input) {
        const activationType = document.getElementById('activation-function').value;
        
        // Set input layer
        this.network.activations[0] = [...input];
        
        // Forward propagation
        for (let layer = 1; layer < this.network.layers.length; layer++) {
            for (let neuron = 0; neuron < this.network.layers[layer]; neuron++) {
                let sum = this.network.biases[layer - 1][neuron];
                
                for (let prevNeuron = 0; prevNeuron < this.network.layers[layer - 1]; prevNeuron++) {
                    sum += this.network.activations[layer - 1][prevNeuron] * 
                           this.network.weights[layer - 1][prevNeuron][neuron];
                }
                
                this.network.activations[layer][neuron] = 
                    this.activationFunction(sum, activationType);
            }
        }
        
        // Update neuron activations for visualization
        for (let layer = 0; layer < this.network.layers.length; layer++) {
            for (let neuron = 0; neuron < this.network.layers[layer]; neuron++) {
                this.network.neurons[layer][neuron].activation = 
                    this.network.activations[layer][neuron];
            }
        }
        
        return this.network.activations[this.network.layers.length - 1];
    }
    
    calculateLoss(predicted, target) {
        let loss = 0;
        for (let i = 0; i < predicted.length; i++) {
            loss += Math.pow(predicted[i] - target[i], 2);
        }
        return loss / predicted.length;
    }
    
    async startTraining() {
        if (this.isTraining) return;
        
        this.isTraining = true;
        const trainButton = document.getElementById('nn-train');
        trainButton.textContent = '⏹️ Stop';
        trainButton.classList.add('training');
        
        const learningRate = parseFloat(document.getElementById('learning-rate').value);
        const batchSize = 32;
        const maxEpochs = 1000;
        
        const startTime = Date.now();
        
        for (let epoch = 0; epoch < maxEpochs && this.isTraining; epoch++) {
            let totalLoss = 0;
            let correct = 0;
            
            // Shuffle training data
            const indices = Array.from({length: this.trainingData.inputs.length}, (_, i) => i);
            for (let i = indices.length - 1; i > 0; i--) {
                const j = Math.floor(Math.random() * (i + 1));
                [indices[i], indices[j]] = [indices[j], indices[i]];
            }
            
            // Training batches
            for (let batch = 0; batch < indices.length; batch += batchSize) {
                const batchEnd = Math.min(batch + batchSize, indices.length);
                
                for (let i = batch; i < batchEnd; i++) {
                    const idx = indices[i];
                    const input = this.trainingData.inputs[idx];
                    const target = this.trainingData.outputs[idx];
                    
                    // Forward pass
                    const predicted = this.forward(input);
                    
                    // Calculate loss
                    const loss = this.calculateLoss(predicted, target);
                    totalLoss += loss;
                    
                    // Check accuracy
                    const predictedClass = predicted.indexOf(Math.max(...predicted));
                    const targetClass = target.indexOf(Math.max(...target));
                    if (predictedClass === targetClass) correct++;
                    
                    // Simplified backpropagation (gradient approximation)
                    this.approximateGradients(input, target, predicted, learningRate);
                }
            }
            
            // Update statistics
            this.currentEpoch = epoch + 1;
            this.currentLoss = totalLoss / this.trainingData.inputs.length;
            this.accuracy = (correct / this.trainingData.inputs.length) * 100;
            
            this.updateStats();
            
            // Render every few epochs for performance
            if (epoch % 5 === 0) {
                this.render();
                await new Promise(resolve => setTimeout(resolve, 10));
            }
            
            // Early stopping
            if (this.currentLoss < 0.01) break;
        }
        
        const endTime = Date.now();
        document.getElementById('training-time').textContent = `${endTime - startTime}ms`;
        
        this.isTraining = false;
        trainButton.textContent = '🎯 Entrenar';
        trainButton.classList.remove('training');
    }
    
    approximateGradients(input, target, predicted, learningRate) {
        const epsilon = 0.001;
        
        // Update weights using finite difference approximation
        for (let layer = 0; layer < this.network.weights.length; layer++) {
            for (let i = 0; i < this.network.weights[layer].length; i++) {
                for (let j = 0; j < this.network.weights[layer][i].length; j++) {
                    // Save original weight
                    const originalWeight = this.network.weights[layer][i][j];
                    
                    // Calculate gradient approximation
                    this.network.weights[layer][i][j] += epsilon;
                    const loss1 = this.calculateLoss(this.forward(input), target);
                    
                    this.network.weights[layer][i][j] = originalWeight - epsilon;
                    const loss2 = this.calculateLoss(this.forward(input), target);
                    
                    const gradient = (loss1 - loss2) / (2 * epsilon);
                    
                    // Update weight
                    this.network.weights[layer][i][j] = originalWeight - learningRate * gradient;
                }
            }
        }
        
        // Update biases
        for (let layer = 0; layer < this.network.biases.length; layer++) {
            for (let i = 0; i < this.network.biases[layer].length; i++) {
                const originalBias = this.network.biases[layer][i];
                
                this.network.biases[layer][i] += epsilon;
                const loss1 = this.calculateLoss(this.forward(input), target);
                
                this.network.biases[layer][i] = originalBias - epsilon;
                const loss2 = this.calculateLoss(this.forward(input), target);
                
                const gradient = (loss1 - loss2) / (2 * epsilon);
                this.network.biases[layer][i] = originalBias - learningRate * gradient;
            }
        }
    }
    
    predict() {
        const inputSize = this.network.layers[0];
        const randomInput = Array.from({length: inputSize}, () => Math.random() * 2 - 1);
        
        const output = this.forward(randomInput);
        const prediction = output.indexOf(Math.max(...output));
        
        // Animate prediction
        this.animatePrediction();
        
        console.log('Input:', randomInput);
        console.log('Output:', output);
        console.log('Prediction:', prediction);
    }
    
    animatePrediction() {
        // Reset connection activity
        this.network.connections.forEach(conn => conn.active = false);
        
        // Activate connections based on signal flow
        let delay = 0;
        for (let layer = 0; layer < this.network.layers.length - 1; layer++) {
            setTimeout(() => {
                for (let i = 0; i < this.network.layers[layer]; i++) {
                    for (let j = 0; j < this.network.layers[layer + 1]; j++) {
                        const connIndex = this.getConnectionIndex(layer, i, j);
                        if (connIndex >= 0) {
                            this.network.connections[connIndex].active = true;
                        }
                    }
                }
                this.render();
            }, delay);
            
            delay += 200;
        }
        
        // Reset after animation
        setTimeout(() => {
            this.network.connections.forEach(conn => conn.active = false);
            this.render();
        }, delay + 500);
    }
    
    getConnectionIndex(layer, fromNeuron, toNeuron) {
        let index = 0;
        for (let l = 0; l < layer; l++) {
            index += this.network.layers[l] * this.network.layers[l + 1];
        }
        index += fromNeuron * this.network.layers[layer + 1] + toNeuron;
        return index;
    }
    
    toggleAnimation() {
        this.isAnimating = !this.isAnimating;
        const button = document.getElementById('nn-animate');
        
        if (this.isAnimating) {
            button.textContent = '⏹️ Stop';
            this.startAnimation();
        } else {
            button.textContent = '⚡ Animar';
            if (this.animationId) {
                cancelAnimationFrame(this.animationId);
            }
        }
    }
    
    startAnimation() {
        if (!this.isAnimating) return;
        
        // Generate random input every 500ms
        if (Date.now() % 500 < 16) {
            this.predict();
        }
        
        this.animationId = requestAnimationFrame(() => this.startAnimation());
    }
    
    resetNetwork() {
        this.isTraining = false;
        this.initializeNetwork();
        this.generateTrainingData();
        this.render();
    }
    
    resetStats() {
        this.currentEpoch = 0;
        this.currentLoss = 0;
        this.accuracy = 0;
        this.updateStats();
    }
    
    updateStats() {
        document.getElementById('epochs').textContent = this.currentEpoch;
        document.getElementById('loss').textContent = this.currentLoss.toFixed(4);
        document.getElementById('accuracy').textContent = this.accuracy.toFixed(1) + '%';
    }
    
    handleMouseMove(e) {
        const rect = this.canvas.getBoundingClientRect();
        const mouseX = e.clientX - rect.left;
        const mouseY = e.clientY - rect.top;
        
        // Check if mouse is over a neuron
        let hoveredNeuron = null;
        for (let layer = 0; layer < this.network.neurons.length; layer++) {
            for (let neuron = 0; neuron < this.network.neurons[layer].length; neuron++) {
                const n = this.network.neurons[layer][neuron];
                const distance = Math.sqrt((mouseX - n.x) ** 2 + (mouseY - n.y) ** 2);
                if (distance <= n.radius) {
                    hoveredNeuron = { layer, neuron, data: n };
                    break;
                }
            }
        }
        
        this.hoveredNeuron = hoveredNeuron;
        this.render();
    }
    
    render() {
        const ctx = this.ctx;
        
        // Clear canvas
        ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
        
        // Draw background
        const gradient = ctx.createLinearGradient(0, 0, this.canvas.width, this.canvas.height);
        gradient.addColorStop(0, 'rgba(0, 20, 0, 0.1)');
        gradient.addColorStop(1, 'rgba(0, 40, 0, 0.1)');
        ctx.fillStyle = gradient;
        ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);
        
        // Draw connections
        this.renderConnections();
        
        // Draw neurons
        this.renderNeurons();
        
        // Draw labels
        this.renderLabels();
        
        // Draw tooltip
        if (this.hoveredNeuron) {
            this.renderTooltip();
        }
    }
    
    renderConnections() {
        const ctx = this.ctx;
        
        this.network.connections.forEach(conn => {
            const from = this.network.neurons[conn.from.layer][conn.from.neuron];
            const to = this.network.neurons[conn.to.layer][conn.to.neuron];
            
            const weight = Math.abs(conn.weight);
            const maxWeight = 2;
            const opacity = Math.min(weight / maxWeight, 1);
            
            // Connection color based on weight sign
            const color = conn.weight > 0 ? '0, 255, 0' : '255, 50, 50';
            
            ctx.strokeStyle = conn.active ? 
                `rgba(255, 255, 0, ${opacity})` : 
                `rgba(${color}, ${opacity * 0.6})`;
            ctx.lineWidth = conn.active ? 3 : Math.max(opacity * 2, 0.5);
            
            ctx.beginPath();
            ctx.moveTo(from.x, from.y);
            ctx.lineTo(to.x, to.y);
            ctx.stroke();
        });
    }
    
    renderNeurons() {
        const ctx = this.ctx;
        
        this.network.neurons.forEach((layer, layerIndex) => {
            layer.forEach((neuron, neuronIndex) => {
                const activation = Math.abs(neuron.activation);
                const maxActivation = 1;
                const intensity = Math.min(activation / maxActivation, 1);
                
                // Neuron color based on activation
                const baseColor = layerIndex === 0 ? '100, 150, 255' : 
                                 layerIndex === this.network.neurons.length - 1 ? '255, 100, 100' : 
                                 '100, 255, 100';
                
                // Draw neuron glow
                const glowRadius = neuron.radius + 5;
                const glowGradient = ctx.createRadialGradient(
                    neuron.x, neuron.y, 0,
                    neuron.x, neuron.y, glowRadius
                );
                glowGradient.addColorStop(0, `rgba(${baseColor}, ${intensity * 0.8})`);
                glowGradient.addColorStop(1, `rgba(${baseColor}, 0)`);
                
                ctx.fillStyle = glowGradient;
                ctx.beginPath();
                ctx.arc(neuron.x, neuron.y, glowRadius, 0, Math.PI * 2);
                ctx.fill();
                
                // Draw neuron body
                ctx.fillStyle = `rgba(${baseColor}, ${0.3 + intensity * 0.7})`;
                ctx.strokeStyle = `rgba(${baseColor}, 1)`;
                ctx.lineWidth = 2;
                
                ctx.beginPath();
                ctx.arc(neuron.x, neuron.y, neuron.radius, 0, Math.PI * 2);
                ctx.fill();
                ctx.stroke();
                
                // Draw activation value
                if (neuron.radius > 10) {
                    ctx.fillStyle = '#ffffff';
                    ctx.font = `${Math.min(neuron.radius * 0.8, 12)}px 'JetBrains Mono'`;
                    ctx.textAlign = 'center';
                    ctx.textBaseline = 'middle';
                    ctx.fillText(
                        activation.toFixed(1),
                        neuron.x,
                        neuron.y
                    );
                }
            });
        });
    }
    
    renderLabels() {
        const ctx = this.ctx;
        ctx.fillStyle = '#00ff00';
        ctx.font = '14px "JetBrains Mono"';
        ctx.textAlign = 'center';
        
        // Layer labels
        const layerNames = ['Input', ...Array(this.network.layers.length - 2).fill('Hidden'), 'Output'];
        
        this.network.neurons.forEach((layer, index) => {
            if (layer.length > 0) {
                const y = layer[0].y - 40;
                ctx.fillText(
                    `${layerNames[index]} (${layer.length})`,
                    layer[0].x,
                    y
                );
            }
        });
    }
    
    renderTooltip() {
        const ctx = this.ctx;
        const neuron = this.hoveredNeuron;
        
        const tooltipX = neuron.data.x + 30;
        const tooltipY = neuron.data.y - 30;
        
        const text = [
            `Layer: ${neuron.layer}`,
            `Neuron: ${neuron.neuron}`,
            `Activation: ${neuron.data.activation.toFixed(3)}`,
            `Bias: ${neuron.data.bias.toFixed(3)}`
        ];
        
        // Draw tooltip background
        const padding = 8;
        const lineHeight = 16;
        const width = 150;
        const height = text.length * lineHeight + padding * 2;
        
        ctx.fillStyle = 'rgba(0, 0, 0, 0.8)';
        ctx.fillRect(tooltipX, tooltipY, width, height);
        
        ctx.strokeStyle = '#00ff00';
        ctx.lineWidth = 1;
        ctx.strokeRect(tooltipX, tooltipY, width, height);
        
        // Draw tooltip text
        ctx.fillStyle = '#00ff00';
        ctx.font = '12px "JetBrains Mono"';
        ctx.textAlign = 'left';
        
        text.forEach((line, index) => {
            ctx.fillText(
                line,
                tooltipX + padding,
                tooltipY + padding + (index + 1) * lineHeight
            );
        });
    }
}

// Initialize when page loads
document.addEventListener('DOMContentLoaded', () => {
    if (document.getElementById('neural-network-canvas')) {
        window.neuralNetworkViz = new NeuralNetworkVisualization();
    }
}); 