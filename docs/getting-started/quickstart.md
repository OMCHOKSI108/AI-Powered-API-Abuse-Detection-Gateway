# Quick Start Guide

Get your API protected in **5 minutes**.

## Prerequisites

Choose one:

- **Docker** (Recommended - easiest)
- OR **C++20 compiler + CMake** (for development)

## Option 1: Docker (Production)

### 1. Get the Code

```bash
git clone https://github.com/yourusername/AI-Gateway.git
cd AI-Gateway
```

### 2. Build the Container

```bash
docker build -t api-gateway .
```

This takes ~5 minutes (compiles C++ code).

### 3. Run the Gateway

```bash
docker run -d \
 -p 8080:8080 \
 -e BACKEND_URL=https://your-api.com \
 --name gateway \
 api-gateway
```

Replace `https://your-api.com` with your actual API URL.

### 4. Test It

```bash
# Should proxy to your API
curl http://localhost:8080/your-endpoint

# Try abuse (will get blocked)
for i in {1..100}; do curl http://localhost:8080/test; done
```

### 5. Open Dashboard

Visit: `http://localhost:8080/dashboard/index.html`

1. Click "Sign Up" → Create account
2. Login
3. See live traffic stats!

## Option 2: Local Development

### 1. Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get install -y \
 build-essential cmake git \
 libjsoncpp-dev uuid-dev \
 libssl-dev zlib1g-dev \
 libspdlog-dev nlohmann-json3-dev

# Install Drogon
git clone https://github.com/drogonframework/drogon
cd drogon && mkdir build && cd build
cmake .. && make -j$(nproc) && sudo make install
```

**macOS:**
```bash
brew install cmake jsoncpp ossp-uuid openssl zlib spdlog nlohmann-json drogon
```

### 2. Build Project

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 3. Configure

Edit `config/config.json`:

```json
{
 "gateway": {
 "backend_url": "https://your-api.com"
 }
}
```

### 4. Run

```bash
./ai_abuse_gateway
```

Visit: `http://localhost:8080/dashboard/index.html`

## Quick Configuration

### Change Backend URL on the Fly

**Via Dashboard:**

1. Login to dashboard
2. Top of page → Enter new URL
3. Click "Update Target"

**Via Environment Variable:**

```bash
docker run -d -p 8080:8080 \
 -e BACKEND_URL=https://new-api.com \
 api-gateway
```

**Via API:**

```bash
curl -X POST http://localhost:8080/api/config/backend \
 -H "Authorization: Bearer YOUR_TOKEN" \
 -H "Content-Type: application/json" \
 -d '{"url": "https://new-api.com"}'
```

## Test Attack Detection

### Normal Traffic (Should Pass)

```bash
# One request per second - normal user
for i in {1..10}; do 
 curl http://localhost:8080/api/test
 sleep 1
done
```

 All requests should succeed.

### Bot Attack (Should Be Blocked)

```bash
# 100 rapid requests - bot behavior
for i in {1..100}; do 
 curl http://localhost:8080/api/test &
done
wait
```

 After ~50 requests, you'll see:
```
Access Denied: High Risk Detected - Temporarily Banned
```

Check dashboard to see the attacker blocked!

## Verify Protection

### Check Logs

```bash
# Docker
docker logs -f gateway

# Local
# Logs appear in console
```

Look for lines like:
```
[warn] High risk detected: score=0.85, rps=52.3, burst=3245
[warn] Client banned: abc123... | Duration: 300s | Offense: 1 | Risk: 0.85
```

### Dashboard Stats

Open dashboard and verify:

- **Active Clients** - Number of unique users
- **Requests/Sec** - Current traffic rate
- **Avg Risk Score** - Overall threat level
- **Client Table** - See top active clients with risk scores

## Next Steps

Now that it's running:

1. **Integrate with your app** - Update API calls to use gateway URL
2. **Deploy to production** - Use Railway, AWS, or DigitalOcean
3. **Customize thresholds** - Tune for your traffic patterns
4. **Add custom ML model** - Train on your specific data

 See detailed guides:

- [Protect Vercel Backend](../use-cases/vercel-integration.md)
- [Configuration Options](configuration.md)
- [Deploy to Production](../deployment/railway.md)

## Troubleshooting

### Port Already in Use

```bash
# Change port
docker run -p 9090:8080 ...
```

### Can't Connect to Backend

Check backend URL is correct and accessible:

```bash
curl https://your-api.com/test-endpoint
```

### Dashboard 401 Unauthorized

Make sure you're logged in:

1. Go to `/dashboard/login.html`
2. Login with your account
3. Token stored in localStorage

### High False Positives

Lower detection thresholds in `config/config.json`:

```json
{
 "ml_model": {
 "threshold_block": 0.9,
 "threshold_throttle": 0.6
 }
}
```

## Getting Help

- [Full Documentation](../index.md)
- [GitHub Issues](https://github.com/yourusername/AI-Gateway/issues)
- [Email Support](mailto:support@yourgateway.com)
