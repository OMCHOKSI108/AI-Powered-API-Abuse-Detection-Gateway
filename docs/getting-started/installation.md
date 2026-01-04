# Installation Guide

Complete installation instructions for all platforms.

## Prerequisites

Choose your installation method:

- **Docker** (Recommended) - Easiest, works everywhere
- **Package Manager** - For development
- **From Source** - Full customization

## Docker Installation (Recommended)

### Install Docker

=== "Ubuntu/Debian"

 ```bash
 curl -fsSL https://get.docker.com | sh
 sudo usermod -aG docker $USER
 # Log out and back in
 ```

=== "macOS"

 ```bash
 brew install docker
 # Or download Docker Desktop from docker.com
 ```

=== "Windows"

 Download Docker Desktop from [docker.com](https://www.docker.com/products/docker-desktop/)

### Build Gateway Image

```bash
# Clone repository
git clone https://github.com/yourusername/AI-Gateway.git
cd AI-Gateway

# Build image
docker build -t api-gateway .

# This takes ~5 minutes (compiles C++ code)
```

### Run Container

```bash
docker run -d \
 --name gateway \
 -p 8080:8080 \
 -e BACKEND_URL=https://your-api.com \
 --restart unless-stopped \
 api-gateway

# Verify running
docker ps
```

## Native Installation

### Ubuntu/Debian

```bash
# Update packages
sudo apt-get update

# Install dependencies
sudo apt-get install -y \
 build-essential \
 cmake \
 git \
 libjsoncpp-dev \
 uuid-dev \
 libssl-dev \
 zlib1g-dev \
 libspdlog-dev \
 nlohmann-json3-dev

# Install Drogon framework
cd /tmp
git clone https://github.com/drogonframework/drogon
cd drogon
git checkout v1.9.0
git submodule update --init
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
sudo ldconfig

# Build gateway
cd ~/
git clone https://github.com/yourusername/AI-Gateway.git
cd AI-Gateway
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./ai_abuse_gateway
```

### macOS

```bash
# Install Homebrew if needed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake jsoncpp ossp-uuid openssl zlib spdlog nlohmann-json

# Install Drogon
brew install drogon

# Build gateway
git clone https://github.com/yourusername/AI-Gateway.git
cd AI-Gateway
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)

# Run
./ai_abuse_gateway
```

### Windows (WSL2)

Use WSL2 with Ubuntu, then follow Ubuntu instructions above.

## Verify Installation

```bash
# Check if running
curl http://localhost:8080/api/stats

# Should return JSON (may need auth)

# Check dashboard
open http://localhost:8080/dashboard/index.html
```

## Post-Installation

### Configure Backend

Edit `config/config.json`:

```json
{
 "gateway": {
 "backend_url": "https://your-actual-api.com"
 }
}
```

Restart gateway:

```bash
# Docker
docker restart gateway

# Native
pkill ai_abuse_gateway
./ai_abuse_gateway
```

### Create Dashboard Account

1. Visit: `http://localhost:8080/dashboard/signup.html`
2. Create account
3. Login

### Test Protection

```bash
# Normal request
curl http://localhost:8080/test

# Attack simulation
for i in {1..100}; do curl http://localhost:8080/test & done
```

## Troubleshooting

### Port Already in Use

```bash
# Find what's using port 8080
sudo lsof -i :8080

# Use different port
docker run -p 9090:8080 ...
```

### Build Errors

```bash
# Clean build
rm -rf build
mkdir build && cd build
cmake ..
make clean
make -j$(nproc)
```

### Can't Connect to Backend

```bash
# Test backend directly
curl https://your-api.com/endpoint

# Check DNS
ping your-api.com

# Check firewall
sudo ufw status
```

## Upgrading

```bash
# Pull latest code
cd AI-Gateway
git pull

# Rebuild
docker build -t api-gateway .

# Restart with new image
docker stop gateway
docker rm gateway
docker run -d --name gateway -p 8080:8080 api-gateway
```

## Uninstallation

```bash
# Docker
docker stop gateway
docker rm gateway
docker rmi api-gateway

# Native
sudo rm /usr/local/bin/ai_abuse_gateway
sudo rm -rf /opt/AI-Gateway
```

## Next Steps

- [Quick Start Guide](quickstart.md)
- [Configuration](configuration.md)
- [Deploy to Production](../deployment/railway.md)
