#!/bin/bash

# Stop Amelia Enora Technical CV Server

echo "🛑 Stopping Amelia Enora Technical CV Server"
echo "============================================="

# Find and kill Python HTTP server processes
PIDS=$(pgrep -f "python3 -m http.server 8080")

if [ -z "$PIDS" ]; then
    echo "ℹ️  No server processes found running on port 8080"
else
    echo "🔍 Found server processes: $PIDS"
    echo "🛑 Stopping server..."
    
    # Kill the processes
    echo "$PIDS" | xargs kill
    
    # Wait a moment and check
    sleep 2
    
    # Force kill if still running
    REMAINING=$(pgrep -f "python3 -m http.server 8080")
    if [ ! -z "$REMAINING" ]; then
        echo "🔨 Force stopping remaining processes..."
        echo "$REMAINING" | xargs kill -9
    fi
    
    echo "✅ Server stopped successfully!"
fi

# Check if port is now free
if ! lsof -Pi :8080 -sTCP:LISTEN -t >/dev/null; then
    echo "✅ Port 8080 is now available"
else
    echo "⚠️  Port 8080 might still be in use by another process"
    echo "   Use: lsof -i :8080 to check"
fi 