# Configuration Guide

Complete reference for all configuration options.

## Configuration Methods

The gateway can be configured through:

1. **config.json** - Main configuration file
2. **Environment Variables** - Override config.json values
3. **Dashboard** - Runtime updates via web UI
4. **API** - Programmatic configuration

## config.json Reference

Located at: `config/config.json`

```json
{
 "listeners": [
 {
 "address": "0.0.0.0",
 "port": 8080,
 "https": false
 }
 ],
 "app": {
 "number_of_threads": 4,
 "enable_server_header": false,
 "log": {
 "level": "debug",
 "use_spdlog": true
 }
 },
 "gateway": {
 "backend_url": "http://httpbin.org",
 "backend_timeout_seconds": 10
 },
 "ml_model": {
 "path": "/models/abuse_detector.onnx",
 "threshold_block": 0.8,
 "threshold_throttle": 0.5,
 "throttle_max_delay_ms": 3000,
 "ban_duration_seconds": 300
 }
}
```

### Listeners Section

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `address` | string | "0.0.0.0" | Listen address (0.0.0.0 = all interfaces) |
| `port` | integer | 8080 | HTTP port to listen on |
| `https` | boolean | false | Enable HTTPS (requires cert config) |

**Examples:**

```json
// Listen only on localhost
{
 "address": "127.0.0.1",
 "port": 8080
}

// Different port
{
 "address": "0.0.0.0",
 "port": 3000
}
```

### App Section

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `number_of_threads` | integer | 4 | Worker threads (set to CPU cores) |
| `enable_server_header` | boolean | false | Include "Server" header in responses |
| `log.level` | string | "debug" | Log level: trace, debug, info, warn, error |
| `log.use_spdlog` | boolean | true | Use spdlog for logging |

**Thread Configuration:**

```json
// For 8-core CPU
{
 "number_of_threads": 8
}

// For high traffic (2x cores)
{
 "number_of_threads": 16
}
```

### Gateway Section

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `backend_url` | string | required | Your API URL to protect |
| `backend_timeout_seconds` | integer | 10 | Timeout for backend requests |

**Examples:**

```json
// Vercel backend
{
 "backend_url": "https://my-app.vercel.app"
}

// Local API
{
 "backend_url": "http://localhost:3000"
}

// With longer timeout
{
 "backend_url": "https://slow-api.com",
 "backend_timeout_seconds": 30
}
```

### ML Model Section

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `path` | string | "/models/abuse_detector.onnx" | Path to ONNX model file |
| `threshold_block` | float | 0.8 | Risk score to trigger block (0.0-1.0) |
| `threshold_throttle` | float | 0.5 | Risk score to trigger throttle (0.0-1.0) |
| `throttle_max_delay_ms` | integer | 3000 | Maximum throttle delay in ms |
| `ban_duration_seconds` | integer | 300 | How long to ban (5 minutes default) |

**Tuning Examples:**

```json
// Aggressive (strict protection)
{
 "threshold_block": 0.7,
 "threshold_throttle": 0.4,
 "ban_duration_seconds": 600
}

// Lenient (fewer false positives)
{
 "threshold_block": 0.9,
 "threshold_throttle": 0.7,
 "ban_duration_seconds": 120
}

// E-commerce (balanced)
{
 "threshold_block": 0.8,
 "threshold_throttle": 0.5,
 "ban_duration_seconds": 300
}
```

## Environment Variables

Override any config value with environment variables:

| Variable | Description | Example |
|----------|-------------|---------|
| `BACKEND_URL` | Target API URL | `https://api.example.com` |
| `PORT` | Listen port | `8080` |
| `LOG_LEVEL` | Logging level | `info` |
| `BLOCK_THRESHOLD` | Block threshold | `0.8` |
| `BAN_DURATION` | Ban time (seconds) | `300` |

**Docker Example:**

```bash
docker run -d \
 -e BACKEND_URL=https://my-api.com \
 -e BLOCK_THRESHOLD=0.9 \
 -e BAN_DURATION=180 \
 -p 8080:8080 \
 api-gateway
```

**Railway/Render:**

```
Environment Variables:
BACKEND_URL = https://my-api.vercel.app
BLOCK_THRESHOLD = 0.85
LOG_LEVEL = info
```

## Runtime Configuration

### Via Dashboard

1. Login to dashboard: `http://your-gateway:8080/dashboard/index.html`
2. Top of page → "Enter Target Backend URL"
3. Type new URL and click "Update Target"

Changes take effect immediately, no restart needed!

### Via API

```bash
# Update backend URL
curl -X POST http://localhost:8080/api/config/backend \
 -H "Authorization: Bearer YOUR_TOKEN" \
 -H "Content-Type: application/json" \
 -d '{"url": "https://new-backend.com"}'
```

## Use Case Configurations

### E-commerce Site

```json
{
 "ml_model": {
 "threshold_block": 0.75,
 "threshold_throttle": 0.45,
 "ban_duration_seconds": 600
 }
}
```

**Why:**
- Lower thresholds catch price scrapers
- Longer bans discourage persistent bots

### Mobile App API

```json
{
 "ml_model": {
 "threshold_block": 0.85,
 "threshold_throttle": 0.6,
 "ban_duration_seconds": 180
 }
}
```

**Why:**
- Higher thresholds for burst traffic from app refreshes
- Shorter bans for legit users on flaky networks

### Internal Microservices

```json
{
 "ml_model": {
 "threshold_block": 0.95,
 "threshold_throttle": 0.8,
 "ban_duration_seconds": 60
 }
}
```

**Why:**
- Very high thresholds (mostly trusted traffic)
- Short bans for debugging failed services

### Public API

```json
{
 "ml_model": {
 "threshold_block": 0.8,
 "threshold_throttle": 0.5,
 "ban_duration_seconds": 300
 }
}
```

**Why:**
- Balanced protection for unknown users
- Standard ban duration

## Advanced Options

### Custom Client ID Hashing

Edit `src/gateway/ReverseProxy.cpp`:

```cpp
// Include more headers for fingerprinting
std::string ua = req->getHeader("User-Agent");
std::string accept = req->getHeader("Accept-Language");
std::string encoding = req->getHeader("Accept-Encoding");

std::string combined = ip + "|" + auth + "|" + ua + "|" + accept;
```

### Whitelist IPs

```cpp
// In processRequest() before checks
std::string client_ip = req->peerAddr().toIp();

// Whitelist (never block these)
if (client_ip == "203.0.113.1" || 
 client_ip == "203.0.113.2") {
 // Skip all checks, forward directly
 forwardRequest(req, callback);
 return;
}
```

### Custom Metrics Window

Edit `src/analysis/TrafficAnalyzer.cpp`:

```cpp
// Change from 60s to 120s window
stats = std::make_shared<ClientStats>();
stats->window = SlidingWindow(120); // 2 minutes
```

## Performance Tuning

### High Traffic (1000+ rps)

```json
{
 "app": {
 "number_of_threads": 16
 }
}
```

### Low Latency (< 1ms overhead)

```json
{
 "app": {
 "number_of_threads": 2,
 "log": {
 "level": "warn"
 }
 }
}
```

### Memory Optimization

```cpp
// In TrafficAnalyzer.cpp, limit stored payloads
if (stats->payload_sizes.size() > 100) { // Instead of 1000
 stats->payload_sizes.erase(stats->payload_sizes.begin());
}
```

## Validation

After changing config, validate it:

```bash
# Test syntax
python3 -m json.tool config/config.json

# Test gateway starts
docker run --rm api-gateway ./ai_abuse_gateway --test-config
```

## Backup & Restore

```bash
# Backup current config
cp config/config.json config/config.backup.json

# Restore
cp config/config.backup.json config/config.json
docker restart gateway
```

## Next Steps

- [Deploy to Production](../deployment/railway.md)
- [Custom ML Models](../guides/ml-models.md)
- [Performance Tuning](../guides/performance.md)
