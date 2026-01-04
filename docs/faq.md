# Frequently Asked Questions

## General

### What is this gateway?

A high-performance C++ proxy that sits in front of your API and automatically blocks bots, scrapers, and DDoS attacks using machine learning, without requiring any changes to your backend code.

### Do I need to modify my API code?

**No!** That's the whole point. You just route traffic through the gateway. Your backend stays unchanged.

### What programming language is my API required to be in?

**Any language!** The gateway is language-agnostic. It works with Node.js, Python, Java, Go, Ruby, PHP, .NET, etc.

### Is it really free?

Yes, the gateway itself is MIT licensed (free forever). You only pay for:

- Server hosting (DigitalOcean, Railway, AWS, etc.)
- Or run it free on your own hardware

## Technical

### What's the performance overhead?

**< 5ms latency** added per request on average. The C++ implementation is highly optimized with zero-copy operations and async I/O.

### How many requests can it handle?

Tested at **10,000+ requests/second** on a 4-core server. Scales linearly with more cores.

### Is it thread-safe?

Yes! Fully thread-safe with proper mutex protection. Handles thousands of concurrent connections safely.

### What happens if the gateway crashes?

Your backend becomes unreachable (single point of failure). Solutions:

1. Run multiple gateway instances with load balancer
2. Use Docker restart policies: `--restart unless-stopped`
3. Kubernetes for auto-healing

## Detection & Blocking

### How does it detect attacks?

1. **Client fingerprinting** - IP + User-Agent + Auth token
2. **Behavioral analysis** - RPS, burstiness, endpoint diversity, error rates
3. **ML scoring** - Real-time inference calculates risk (0-1)
4. **Action** - Allow, throttle, or block based on score

### What types of attacks does it stop?

- DDoS / traffic floods
- Web scrapers / content theft
- Price monitoring bots
- Credential stuffing
- API abuse / excessive usage
- Automated form submissions

### Can legitimate users get blocked?

Possible but rare. The ML model is tuned to minimize false positives. If it happens:

1. Bans are temporary (5 min default)
2. Lower thresholds in config if needed
3. Whitelist specific IPs

### How long are bans?

**5 minutes by default**, configurable:

```json
{
 "ml_model": {
 "ban_duration_seconds": 300
 }
}
```

### Do bans persist after restart?

No, currently in-memory only. For persistence, you'd need to add Redis backend (future feature).

## Integration

### I deployed my app on Vercel. How do I integrate?

See our [complete Vercel integration guide](use-cases/vercel-integration.md). Summary:

1. Deploy gateway to Railway
2. Update frontend API calls to use gateway URL
3. Done!

### Can I use it with Next.js?

Yes! See [Vercel integration guide](use-cases/vercel-integration.md).

### Can I use it with mobile apps?

Absolutely! Your mobile app just needs to call the gateway URL instead of direct backend.

```swift
// iOS - Before
let url = "https://api.myapp.com/data"

// iOS - After
let url = "https://gateway.myapp.com/data"
```

### Does it work with GraphQL?

Yes, it's protocol-agnostic. It forwards any HTTP request.

### Does it work with WebSockets?

Not currently. HTTP/REST only. WebSocket support could be added.

## Deployment

### Where can I deploy it?

Anywhere that supports Docker:

- Railway (easiest, free tier)
- DigitalOcean (own VPS)
- AWS EC2
- Google Cloud
- Azure
- Your own server

### How much does hosting cost?

- **Railway**: Free tier (500 hrs/month) or $5/month
- **DigitalOcean**: $6/month (1GB droplet)
- **AWS EC2**: $3-10/month (t2.micro/small)
- **Self-hosted**: $0 (your hardware)

### Do I need a domain?

No, but recommended for production. You can use:

- Railway auto-generated URL
- Your IP address
- Custom domain (better UX)

### Can I run multiple instances?

Yes! Put them behind a load balancer (Nginx, HAProxy, CloudFlare).

## ML & Models

### Do I need to train a model?

**No!** It comes with a rule-based scoring system that works out-of-the-box. For advanced use, you can train custom ONNX models.

### How do I train a custom model?

See [ML Model Guide](guides/ml-models.md). Quick version:

```bash
python tools/create_sample_model.py
# Generates: models/abuse_detector.onnx
```

### What if I don't have training data?

Use the built-in rule-based scoring. It's very effective without ML.

### Can I use my own ML framework?

Yes, export to ONNX format:

- PyTorch → ONNX
- TensorFlow → ONNX
- Scikit-learn → ONNX

## Dashboard

### How do I access the dashboard?

`http://your-gateway:8080/dashboard/index.html`

You need to create an account first (signup page).

### Is the dashboard secure?

Yes, protected by JWT token authentication. Only logged-in users can view stats.

### Can I disable the dashboard?

Yes, comment out the dashboard routes in code or block the `/dashboard` path with firewall.

### Can multiple users access the dashboard?

Yes, each user creates their own account. All see the same traffic stats.

## Customization

### Can I add custom logic?

Yes! Fork the repo and modify:

- `src/gateway/ReverseProxy.cpp` - Request handling
- `src/ml/InferenceEngine.cpp` - Scoring logic
- `src/analysis/TrafficAnalyzer.cpp` - Metrics collection

### Can I add IP whitelisting?

Yes, in `src/gateway/ReverseProxy.cpp`:

```cpp
if (client_ip == "1.2.3.4") {
 // Skip all checks
 forwardRequest(req, callback);
 return;
}
```

### Can I customize the client ID?

Yes, edit `computeClientId()` in `ReverseProxy.cpp` to include more headers.

### Can I add custom headers to backend requests?

Yes:

```cpp
fwd_req->addHeader("X-Gateway-Version", "1.0");
fwd_req->addHeader("X-Client-Risk", std::to_string(risk_score));
```

## Troubleshooting

### Gateway won't start

Check logs:

```bash
docker logs gateway
```

Common issues:

- Port 8080 in use → Change port
- Config syntax error → Validate JSON
- Backend unreachable → Test with curl

### Getting 502 Bad Gateway errors

Your backend is unreachable. Verify:

```bash
curl https://your-backend-api.com/endpoint
```

### Dashboard shows 401 Unauthorized

You're not logged in:

1. Go to `/dashboard/login.html`
2. Create account if needed
3. Login to get token

### High false positive rate

Lower detection thresholds:

```json
{
 "ml_model": {
 "threshold_block": 0.9,
 "threshold_throttle": 0.7
 }
}
```

### Can't see my backend URL in dashboard

That's intentional - security. Only the backend API setting is hidden from dashboard display.

## Comparisons

### How is this different from Cloudflare?

| Feature | This Gateway | Cloudflare |
|---------|--------------|------------|
| Cost | Free (self-host) | $20-200/mo |
| Privacy | Your server | Their servers |
| Customization | Full control | Limited |
| ML Model | Trainable | Closed source |
| Deployment | Self-hosted | Cloud only |

### How is this different from rate limiting?

Rate limiting = "Only 100 req/hour per IP" 
This gateway = "Learns your normal traffic, blocks anomalies"

More intelligent, harder to bypass.

### How is this different from WAF (Web Application Firewall)?

WAF inspects request content (SQL injection, XSS). 
This gateway analyzes **behavior patterns** (bot-like activity).

They're complementary! Use both.

## Support

### Where can I get help?

1. Check this FAQ
2. Read [docs](index.md)
3. Search [GitHub Issues](https://github.com/yourusername/AI-Gateway/issues)
4. Open new issue if needed

### Can I hire you to set this up?

Contact via GitHub or email. Consulting available.

### Can I contribute?

Yes! See [Contributing Guide](contributing.md).

### Is there a community?

- GitHub Discussions
- Discord (coming soon)
- Twitter: @yourhandle

## Licensing

### Can I use this commercially?

**Yes!** MIT License = use freely in commercial projects.

### Can I sell this as a service?

Yes, you can build a SaaS on top of it.

### Do I need to open-source my modifications?

No, MIT License doesn't require that (unlike GPL).

### Can I remove the credits?

Technically yes (MIT allows it), but please keep them - it helps the project grow!

---

**Still have questions?** [Open an issue](https://github.com/yourusername/AI-Gateway/issues/new)
