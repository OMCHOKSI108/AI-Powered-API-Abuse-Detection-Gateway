# MkDocs Documentation

This directory contains the complete documentation for the AI-Powered API Abuse Detection Gateway.

## Building Documentation

### Prerequisites

```bash
pip install mkdocs mkdocs-material
```

### Local Development

```bash
# Serve locally with live reload
mkdocs serve

# Opens at: http://127.0.0.1:8000
```

### Build Static Site

```bash
# Build HTML
mkdocs build

# Output in: site/
```

### Deploy to GitHub Pages

```bash
mkdocs gh-deploy
```

## Documentation Structure

```
docs/
├── index.md                      # Home page
├── getting-started/
│   ├── quickstart.md            # 5-minute setup
│   ├── installation.md          # Detailed install
│   └── configuration.md         # Config reference
├── use-cases/
│   ├── vercel-integration.md    # sans's example
│   ├── ecommerce.md
│   ├── mobile-backend.md
│   └── microservices.md
├── guides/
│   ├── step-by-step.md
│   ├── ml-models.md
│   ├── monitoring.md
│   └── performance.md
├── api/
│   ├── dashboard.md
│   ├── configuration.md
│   └── metrics.md
├── deployment/
│   ├── docker.md
│   ├── railway.md
│   ├── aws.md
│   └── digitalocean.md
├── advanced/
│   ├── architecture.md
│   ├── thread-safety.md
│   └── security.md
├── faq.md
└── contributing.md
```

## Contributing to Docs

1. Edit markdown files in `docs/`
2. Test locally with `mkdocs serve`
3. Submit PR with changes

## Style Guide

- Use clear, concise language
- Include code examples
- Add diagrams where helpful
- Use admonitions for tips/warnings
- Keep paragraphs short

## Example Admonitions

```markdown
!!! tip
    This is a helpful tip

!!! warning
    This is a warning

!!! danger
    This is critical

!!! example
    This is an example

!!! info
    This is info
```
