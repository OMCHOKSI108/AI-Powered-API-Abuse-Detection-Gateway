# Contributing Guide

Thank you for considering contributing to the AI-Powered API Abuse Detection Gateway!

## How to Contribute

### 1. Report Bugs

Found a bug? [Open an issue](https://github.com/yourusername/AI-Gateway/issues/new) with:

- Clear description
- Steps to reproduce
- Expected vs actual behavior
- System info (OS, Docker version, etc.)

### 2. Suggest Features

Have an idea? [Open a feature request](https://github.com/yourusername/AI-Gateway/issues/new) with:

- Use case description
- Why it's useful
- Proposed implementation (if you have ideas)

### 3. Submit Code

#### Setup Development Environment

```bash
# Fork the repo, then clone your fork
git clone https://github.com/YOUR_USERNAME/AI-Gateway.git
cd AI-Gateway

# Create feature branch
git checkout -b feature/your-feature-name

# Build and test
mkdir build && cd build
cmake ..
make
./ai_abuse_gateway
```

#### Code Style

- **C++**: Follow Google C++ Style Guide
- **Indentation**: 4 spaces
- **Naming**: snake_case for functions, CamelCase for classes
- **Comments**: Explain "why", not "what"

#### Making Changes

```bash
# Make your changes
vim src/gateway/YourFile.cpp

# Test thoroughly
./run_tests.sh

# Commit with clear message
git add .
git commit -m "feat: add IP whitelist support"

# Push to your fork
git push origin feature/your-feature-name
```

#### Submit Pull Request

1. Go to your fork on GitHub
2. Click "New Pull Request"
3. Describe your changes
4. Link any related issues

### 4. Improve Documentation

Docs need work too!

```bash
# Install MkDocs
pip install mkdocs mkdocs-material

# Serve locally
mkdocs serve

# Edit docs
vim docs/some-page.md

# Submit PR
```

## Development Guidelines

### Testing

Always test your changes:

```bash
# Unit tests (if available)
make test

# Integration test
docker build -t gateway-test .
docker run gateway-test

# Load test
./tools/stress_test.sh
```

### Commit Messages

Use [Conventional Commits](https://www.conventionalcommits.org/):

```
feat: add new feature
fix: fix bug in X
docs: update README
style: format code
refactor: restructure Y
test: add test for Z
chore: update dependencies
```

### Pull Request Checklist

- [ ] Code follows project style
- [ ] All tests pass
- [ ] Added tests for new features
- [ ] Documentation updated
- [ ] No merge conflicts
- [ ] Clear PR description

## What to Contribute

### High Priority

- [ ] Redis backend for persistent bans
- [ ] WebSocket support
- [ ] Prometheus metrics exporter
- [ ] Kubernetes Helm chart
- [ ] More ML model examples
- [ ] Performance benchmarks

### Medium Priority

- [ ] IP whitelist/blacklist UI
- [ ] Email alerts on attacks
- [ ] More dashboard charts
- [ ] Rate limiting per API key
- [ ] Geolocation blocking

### Documentation

- [ ] More use case examples
- [ ] Video tutorials
- [ ] Architecture diagrams
- [ ] API reference docs
- [ ] Troubleshooting guides

## Community

- **Discord**: Coming soon
- **GitHub Discussions**: Ask questions
- **Twitter**: @yourhandle

## License

By contributing, you agree your code will be licensed under MIT License.

## Recognition

Contributors will be:

- Listed in README
- Mentioned in release notes
- Credited in docs

Thank you for making this project better! 
