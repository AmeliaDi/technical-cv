#!/bin/bash

# Deploy script for GitHub Pages
# Usage: ./deploy.sh

set -e

echo "🚀 Deploying Technical CV to GitHub Pages..."

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo -e "${YELLOW}⚠️  Not in a git repository. Initializing...${NC}"
    git init
    git config user.email "enorastrokes@gmail.com"
    git config user.name "AmeliaDi"
fi

# Check if remote origin exists
if ! git remote | grep -q "origin"; then
    echo -e "${YELLOW}⚠️  No remote origin found. Please add it manually:${NC}"
    echo "git remote add origin https://github.com/AmeliaDi/technical-cv.git"
    exit 1
fi

# Build production version if Makefile exists
if [ -f "Makefile" ]; then
    echo -e "${BLUE}🔧 Building production version...${NC}"
    make clean
    # Only build WebAssembly if Emscripten is available
    if command -v emcc &> /dev/null; then
        make all
        echo -e "${GREEN}✅ WebAssembly modules compiled${NC}"
    else
        echo -e "${YELLOW}⚠️  Emscripten not found, using JavaScript fallback${NC}"
    fi
fi

# Add all files
echo -e "${BLUE}📦 Adding files to git...${NC}"
git add .

# Commit if there are changes
if git diff --staged --quiet; then
    echo -e "${YELLOW}ℹ️  No changes to commit${NC}"
else
    echo -e "${BLUE}💾 Committing changes...${NC}"
    git commit -m "🚀 Deploy: $(date '+%Y-%m-%d %H:%M:%S')"
fi

# Push to main branch
echo -e "${BLUE}⬆️  Pushing to main branch...${NC}"
git branch -M main
git push -u origin main

# Create and push gh-pages branch for GitHub Pages
echo -e "${BLUE}🌐 Setting up GitHub Pages...${NC}"

# Check if gh-pages branch exists
if git show-ref --verify --quiet refs/heads/gh-pages; then
    echo -e "${BLUE}📄 Updating existing gh-pages branch...${NC}"
    git checkout gh-pages
    git merge main --no-edit
else
    echo -e "${BLUE}📄 Creating new gh-pages branch...${NC}"
    git checkout -b gh-pages
fi

# Push gh-pages
git push origin gh-pages

# Return to main branch
git checkout main

echo -e "${GREEN}✅ Deployment complete!${NC}"
echo -e "${GREEN}🌐 Your CV will be available at: https://ameliadi.github.io/technical-cv/${NC}"
echo -e "${BLUE}ℹ️  It may take a few minutes for GitHub Pages to update${NC}"

# Instructions for first-time setup
echo ""
echo -e "${YELLOW}📋 If this is your first deployment, follow these steps:${NC}"
echo "1. Go to https://github.com/AmeliaDi/technical-cv"
echo "2. Click on Settings > Pages"
echo "3. Under 'Source', select 'Deploy from a branch'"
echo "4. Select 'gh-pages' branch and '/ (root)' folder"
echo "5. Click Save"
echo ""
echo -e "${GREEN}🎉 Your technical CV is ready to impress!${NC}" 