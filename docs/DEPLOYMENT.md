# ☁️ How to Deploy the AI Gateway to Cloud

Since this project is fully **Dockerized**, you can deploy it to almost any cloud provider.

Here are the 3 easiest ways to deploy it so anyone can use it.

---

## 🚀 Option 1: Railway (Easiest)
**Best for:** Beginners, Git integration, Quick start.

1. **Push your code to GitHub** (You already did this).
2. Go to [Railway.app](https://railway.app/) and Login.
3. Click **"New Project"** -> **"Deploy from GitHub repo"**.
4. Select your repo: `OMCHOKSI108/AI-Powered-API-Abuse-Detection-Gateway`.
5. Railway will automatically detect the `Dockerfile` and start building.
6. **Configuration**:
   - Go to "Variables".
   - Add `BACKEND_URL` = `httsp://your-real-api.com` (The API you want to protect).
   - If you don't set this, it defaults to the test API.
7. **Public Access**:
   - Go to "Settings" -> "Networking" -> "Generate Domain".
   - You will get a URL like: `https://ai-gateway-production.up.railway.app`.

**Usage:**
- Dashboard: `https://<your-url>/dashboard/index.html`
- API Proxy: `https://<your-url>/<your-endpoint>`

---

## 🐳 Option 2: Render
**Best for:** Free tier experiments.

1. Go to [Render.com](https://render.com/).
2. Create **"New Web Service"**.
3. Connect your GitHub repo.
4. Runtime: Select **"Docker"**.
5. Environment Variables:
   - Key: `BACKEND_URL`
   - Value: `https://jsonplaceholder.typicode.com` (or your API).
6. Click **"Create Web Service"**.

---

## 💻 Option 3: VPS (DigitalOcean / AWS EC2)
**Best for:** Performance, Control, Lower cost at scale.

1. **SSH into your server**.
2. **Install Docker**:
   ```bash
   curl -fsSL https://get.docker.com | sh
   ```
3. **Clone & Run**:
   ```bash
   git clone https://github.com/OMCHOKSI108/AI-Powered-API-Abuse-Detection-Gateway.git
   cd AI-Powered-API-Abuse-Detection-Gateway
   
   # Build
   docker build -t gateway .
   
   # Run (Protecting Google.com as an example)
   docker run -d \
     -p 80:8080 \
     -e BACKEND_URL="https://www.google.com" \
     --name gateway \
     gateway
   ```
4. Access via your server's IP: `http://<server-ip>/dashboard/index.html`.

---

## ⚙️ Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `BACKEND_URL` | The upstream API to protect | `http://httpbin.org` |
| `PORT` | (Internal) Listening port | `8080` (Configured in json) |
