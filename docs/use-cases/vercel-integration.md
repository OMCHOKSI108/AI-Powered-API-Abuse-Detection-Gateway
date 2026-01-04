# Protecting Your Vercel Backend - Complete Guide

!!! example "Real Scenario"
 **sans** built her e-commerce website with Next.js (frontend) + Node.js API (backend), deployed on Vercel. Bots started scraping her product catalog. Here's how she fixed it in 10 minutes.

## sans's Setup

**Before Protection:**

- Frontend: `https://sans-shop.vercel.app` (Next.js)
- Backend API: `https://sans-shop.vercel.app/api/*`
- Problem: API getting hammered by scrapers (1000+ req/min)

**After Protection:**

- Frontend: Still at `https://sans-shop.vercel.app`
- Backend: Protected by Gateway
- Bots: Blocked automatically

## Step-by-Step Integration

### Step 1: Deploy the Gateway

sans has 3 options. She chose Railway (easiest):

=== "Railway (Recommended - 5 min)"

 ```bash
 # 1. Fork this repo to your GitHub
 # 2. Go to railway.app and login
 # 3. Click "New Project" → "Deploy from GitHub"
 # 4. Select your forked repo
 # 5. Add environment variable:
 # BACKEND_URL = https://sans-shop.vercel.app
 # 6. Railway auto-deploys and gives you a URL:
 # https://gateway-production.up.railway.app
 ```

=== "DigitalOcean (Own VPS)"

 ```bash
 # SSH into your droplet
 ssh root@your-droplet-ip

 # Install Docker
 curl -fsSL https://get.docker.com | sh

 # Clone & deploy
 git clone https://github.com/youruser/AI-Gateway.git
 cd AI-Gateway

 docker build -t gateway .
 docker run -d -p 80:8080 \
 -e BACKEND_URL=https://sans-shop.vercel.app \
 --restart unless-stopped \
 gateway

 # Your gateway is now at: http://your-droplet-ip
 ```

=== "Local Testing"

 ```bash
 # Test on your laptop first
 git clone https://github.com/youruser/AI-Gateway.git
 cd AI-Gateway

 docker build -t gateway .
 docker run -p 8080:8080 \
 -e BACKEND_URL=https://sans-shop.vercel.app \
 gateway

 # Test: http://localhost:8080
 ```

### Step 2: Update Frontend to Use Gateway

sans needs to change her API calls to go through the gateway:

**Before (Vulnerable):**

```javascript
// components/ProductList.jsx
async function fetchProducts() {
 const response = await fetch('https://sans-shop.vercel.app/api/products');
 return response.json();
}
```

**After (Protected):**

```javascript
// components/ProductList.jsx
const GATEWAY_URL = 'https://gateway-production.up.railway.app';

async function fetchProducts() {
 const response = await fetch(`${GATEWAY_URL}/api/products`);
 return response.json();
}
```

!!! tip "Environment Variables"
 Better approach - use environment variable:

 ```bash
 # .env.local
 NEXT_PUBLIC_API_GATEWAY=https://gateway-production.up.railway.app
 ```

 ```javascript
 // lib/api.js
 const API_BASE = process.env.NEXT_PUBLIC_API_GATEWAY;

 export async function fetchProducts() {
 const res = await fetch(`${API_BASE}/api/products`);
 return res.json();
 }
 ```

### Step 3: Keep Vercel API Private (Optional but Recommended)

Now that traffic goes through the gateway, make Vercel API private:

**Option A: IP Whitelist (Vercel Pro)**

In Vercel dashboard → Settings → Firewall:
```
Allowed IPs: <your-gateway-ip-only>
```

**Option B: Secret Header**

Add verification in your Vercel API:

```javascript
// pages/api/products.js (Vercel)
export default function handler(req, res) {
 const secret = req.headers['x-gateway-secret'];

 if (secret !== process.env.GATEWAY_SECRET) {
 return res.status(403).json({ error: 'Direct access forbidden' });
 }

 // Your normal API logic
 res.json({ products: [...] });
}
```

Then configure gateway to send that header:

```cpp
// In ReverseProxy.cpp, add to forwarded headers:
fwd_req->addHeader("X-Gateway-Secret", "your-secret-key-here");
```

### Step 4: Test the Protection

sans runs these tests:

**Test 1: Normal User (Should Work)**

```bash
# Single request - should pass through
curl https://gateway-production.up.railway.app/api/products
```

 Expected: Get products JSON

**Test 2: Bot Attack (Should Be Blocked)**

```bash
# Rapid fire 100 requests
for i in {1..100}; do 
 curl https://gateway-production.up.railway.app/api/products &
done
```

 Expected after ~50 requests:
```json
{"error": "Access Denied: High Risk Detected - Temporarily Banned"}
```

**Test 3: Monitor Dashboard**

Visit: `https://gateway-production.up.railway.app/dashboard/index.html`

1. Create account (signup page)
2. Login
3. See live traffic stats
4. Watch attackers get blocked in real-time

### Step 5: Update DNS (For Custom Domain)

If sans wants `api.sans-shop.com` → Gateway:

**DNS Records:**

```
Type: CNAME
Name: api
Value: gateway-production.up.railway.app
```

Then update frontend:

```javascript
const API_BASE = 'https://api.sans-shop.com';
```

## Complete Code Example

Here's sans's full integration:

=== "Frontend API Service"

 ```typescript
 // lib/api.ts
 const API_BASE = process.env.NEXT_PUBLIC_API_GATEWAY 
 || 'https://gateway-production.up.railway.app';

 interface Product {
 id: string;
 name: string;
 price: number;
 }

 export class APIService {
 private static async request<T>(
 endpoint: string, 
 options?: RequestInit
 ): Promise<T> {
 const response = await fetch(`${API_BASE}${endpoint}`, {
 ...options,
 headers: {
 'Content-Type': 'application/json',
 ...options?.headers,
 },
 });

 if (!response.ok) {
 if (response.status === 429) {
 throw new Error('Too many requests. Please slow down.');
 }
 throw new Error(`API Error: ${response.statusText}`);
 }

 return response.json();
 }

 static async getProducts(): Promise<Product[]> {
 return this.request<Product[]>('/api/products');
 }

 static async getProduct(id: string): Promise<Product> {
 return this.request<Product>(`/api/products/${id}`);
 }

 static async createOrder(data: any): Promise<any> {
 return this.request('/api/orders', {
 method: 'POST',
 body: JSON.stringify(data),
 });
 }
 }
 ```

=== "React Component"

 ```tsx
 // components/ProductList.tsx
 import { useEffect, useState } from 'react';
 import { APIService } from '@/lib/api';

 export function ProductList() {
 const [products, setProducts] = useState([]);
 const [loading, setLoading] = useState(true);
 const [error, setError] = useState(null);

 useEffect(() => {
 APIService.getProducts()
 .then(setProducts)
 .catch(err => {
 if (err.message.includes('Too many requests')) {
 setError('Please slow down and try again.');
 } else {
 setError('Failed to load products');
 }
 })
 .finally(() => setLoading(false));
 }, []);

 if (loading) return <div>Loading...</div>;
 if (error) return <div className="error">{error}</div>;

 return (
 <div className="grid grid-cols-3 gap-4">
 {products.map(product => (
 <ProductCard key={product.id} product={product} />
 ))}
 </div>
 );
 }
 ```

=== "Environment Setup"

 ```bash
 # .env.local (Development)
 NEXT_PUBLIC_API_GATEWAY=http://localhost:8080

 # .env.production (Production)
 NEXT_PUBLIC_API_GATEWAY=https://gateway-production.up.railway.app
 ```

## Monitoring & Maintenance

### Check Gateway Health

```bash
# Quick health check
curl https://gateway-production.up.railway.app/api/stats

# Expected response:
{
 "global": {
 "active_clients": 15,
 "total_rps": 23.5,
 "avg_risk_score": 0.12
 }
}
```

### View Logs

**Railway:**
```
Dashboard → Logs tab → Real-time logs
```

**Docker:**
```bash
docker logs -f <container-id>
```

### Adjust Thresholds

If too many false positives:

```json
// config/config.json
{
 "ml_model": {
 "threshold_block": 0.9, // ↑ Less aggressive (was 0.8)
 "threshold_throttle": 0.6, // ↑ Less aggressive (was 0.5)
 "ban_duration_seconds": 180 // ↓ Shorter bans (was 300)
 }
}
```

## Cost Comparison

**Before Gateway (sans's bills):**

- Vercel: $150/month (excessive bandwidth from bots)
- Total: **$150/month**

**After Gateway:**

- Vercel: $20/month (normal traffic only)
- Railway: $5/month (gateway hosting)
- Total: **$25/month**

**Savings: $125/month (83% reduction!)**

## Common Issues & Solutions

### Issue: "CORS Error"

**Solution:** Add CORS headers in gateway

```cpp
// In ReverseProxy.cpp
my_resp->addHeader("Access-Control-Allow-Origin", "*");
my_resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
```

### Issue: "Dashboard won't load"

**Problem:** Trying to access `/dashboard` without `/index.html`

**Solution:** Always use full path:
```
https://your-gateway.com/dashboard/index.html
```

### Issue: "Legitimate users getting blocked"

**Solution:** Lower thresholds in config.json or whitelist IPs:

```cpp
// Add to ReverseProxy.cpp
if (client_ip == "1.2.3.4") return; // Skip check
```

## Next Steps

- Gateway deployed and protecting Vercel API
- [ ] [Add custom ML model](../guides/ml-models.md) for better accuracy
- [ ] [Setup monitoring alerts](../guides/monitoring.md) to Slack
- [ ] [Scale to multiple instances](../deployment/aws.md) for high traffic

## Questions?

- Check [FAQ](../faq.md)
- Ask on [GitHub Discussions](https://github.com/youruser/AI-Gateway/discussions)
- See more [use cases](ecommerce.md)
