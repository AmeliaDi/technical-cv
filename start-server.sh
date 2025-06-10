#!/bin/bash

# Amelia Enora Technical CV - Local Development Server
# Quick start script for local preview

echo "🚀 Starting Amelia Enora Technical CV Server"
echo "============================================="

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "❌ Python3 is not installed"
    echo "   Install with: sudo apt install python3"
    exit 1
fi

# Check if port 8080 is available
if lsof -Pi :8080 -sTCP:LISTEN -t >/dev/null; then
    echo "⚠️  Port 8080 is already in use"
    echo "   Stopping existing server..."
    pkill -f "python3 -m http.server 8080" || true
    sleep 2
fi

# Start the server
echo "🌐 Starting HTTP server on port 8080..."
echo "📁 Serving from: $(pwd)"
echo ""

# Start server in background
python3 -m http.server 8080 --bind 0.0.0.0 &
SERVER_PID=$!

# Wait for server to start
sleep 2

# Check if server started successfully
if curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/ | grep -q "200"; then
    echo "✅ Server started successfully!"
    echo ""
    echo "🔗 Access URLs:"
    echo "   Local:    http://localhost:8080"
    echo "   Network:  http://$(hostname -I | awk '{print $1}'):8080"
    echo ""
    echo "🎯 Features available:"
    echo "   • Interactive neural network visualization"
    echo "   • Algorithm sorting demos"
    echo "   • Mathematical visualizations (Mandelbrot, FFT)"
    echo "   • Network tools (packet analyzer, port scanner)"
    echo "   • Complete portfolio with GitHub links"
    echo ""
    echo "📋 Controls:"
    echo "   • Press Ctrl+C to stop the server"
    echo "   • Or run: ./stop-server.sh"
    echo ""
    echo "🚀 Opening browser..."
    
    # Try to open browser
    if command -v xdg-open &> /dev/null; then
        xdg-open http://localhost:8080 &
    elif command -v firefox &> /dev/null; then
        firefox http://localhost:8080 &
    elif command -v chromium &> /dev/null; then
        chromium http://localhost:8080 &
    else
        echo "   Manual: Open http://localhost:8080 in your browser"
    fi
    
    echo ""
    echo "📊 Server logs:"
    echo "=================="
    
    # Keep server running and show logs
    wait $SERVER_PID
else
    echo "❌ Failed to start server"
    kill $SERVER_PID 2>/dev/null
    exit 1
fi 