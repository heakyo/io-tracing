# Local Jenkins CI — User Guide

> **Feynman Principle**: If you can't explain it simply, you don't understand it well enough.
> This guide teaches you how to run a local Jenkins CI server via Docker and use it to automatically build the Hello World C program — as if you were learning it for the first time.

---

## Table of Contents

1. [What Is This?](#1-what-is-this)
2. [Architecture Overview](#2-architecture-overview)
3. [Prerequisites](#3-prerequisites)
4. [Quick Start: Launch Jenkins](#4-quick-start-launch-jenkins)
5. [Access Jenkins from Another Machine](#5-access-jenkins-from-another-machine)
6. [The Build Job: Build_HelloWorld_Simple](#6-the-build-job-build_helloworld_simple)
7. [Trigger a Build](#7-trigger-a-build)
8. [View Build Results](#8-view-build-results)
9. [Common Operations](#9-common-operations)
10. [File Structure](#10-file-structure)
11. [Troubleshooting](#11-troubleshooting)
12. [Glossary](#12-glossary)

---

## 1. What Is This?

Think of Jenkins as a **robot assistant** in a factory. You tell it: "every time I say go, compile my C program, run it, and tell me if it worked." Jenkins does exactly that — automatically, consistently, and without forgetting steps.

This project sets up:
- A **Jenkins server** running inside a Docker container
- A **GCC compiler toolchain** baked into the same container
- A **pre-configured build job** that compiles and runs the Hello World C program in `src/`

### The Analogy

Imagine you have a personal chef (Jenkins) who lives in a portable kitchen (Docker container). The kitchen comes with all the tools (GCC, Make) pre-installed. You hand over a recipe (Jenkinsfile / job config), and the chef cooks the dish (compiles your code) whenever you ask.

---

## 2. Architecture Overview

```
 Your Machine (Linux Host)
 ┌─────────────────────────────────────────────────────┐
 │                                                     │
 │  docker-compose.yml                                 │
 │  ┌───────────────────────────────────────────────┐  │
 │  │  Container: jenkins-hello                     │  │
 │  │  ┌─────────────┐  ┌────────────────────────┐  │  │
 │  │  │  Jenkins     │  │  GCC 15 + Make 4.4     │  │  │
 │  │  │  (port 8080) │  │  (compiler toolchain)  │  │  │
 │  │  └─────────────┘  └────────────────────────┘  │  │
 │  │                                               │  │
 │  │  Volume mount: ./ → /var/jenkins_home/project │  │
 │  └───────────────────────────────────────────────┘  │
 │                                                     │
 │  src/main.c + Makefile  ←── your source code        │
 └─────────────────────────────────────────────────────┘

 Windows / Remote Machine
 ┌────────────────────────────┐
 │  Browser → SSH Tunnel      │──── SSH (port 22) ────→ Host:8080
 │  http://localhost:8080     │
 └────────────────────────────┘
```

### How the Pieces Fit Together

| Component | Role | Analogy |
|---|---|---|
| `docker-compose.yml` | Defines how to run Jenkins | The blueprint for the kitchen |
| `Dockerfile` | Builds custom Jenkins image with GCC | Kitchen equipment list |
| `Jenkinsfile` | Pipeline definition for the build | The recipe |
| `src/main.c` | The C program to compile | The ingredients |
| `src/Makefile` | Build instructions for Make | Cooking instructions |

---

## 3. Prerequisites

| Requirement | Check Command |
|---|---|
| Docker Engine | `docker --version` |
| Docker Compose | `docker-compose --version` |
| SSH client (for remote access) | `ssh -V` |

---

## 4. Quick Start: Launch Jenkins

**Step 1** — Clone the repo and navigate to the project:
```bash
cd /path/to/io-tracing/MyBuildWeb
```

**Step 2** — Build and start Jenkins:
```bash
docker-compose up -d
```

This does three things:
1. Builds a custom Docker image (Jenkins LTS + GCC toolchain)
2. Creates a container named `jenkins-hello`
3. Starts Jenkins on port **8080**

**Step 3** — Wait ~20 seconds for Jenkins to initialize, then verify:
```bash
curl -s -o /dev/null -w "%{http_code}" http://localhost:8080/
# Expected: 200
```

**Step 4** — Open in browser:
```
http://localhost:8080
```

> **Note**: The setup wizard is disabled (`-Djenkins.install.runSetupWizard=false`), so Jenkins is ready to use immediately — no unlock key or plugin installation needed.

---

## 5. Access Jenkins from Another Machine

### The Problem

If you're on a different machine (e.g., a Windows laptop), port 8080 may be blocked by network firewalls between the two machines, even if `ping` works.

### The Solution: SSH Tunnel

An SSH tunnel is like a **secret underground passage** — it wraps your HTTP traffic inside an encrypted SSH connection (port 22), which firewalls typically allow.

```
 Windows laptop                         Linux host
 ┌──────────┐     SSH (port 22)     ┌──────────────┐
 │ Browser   │ ═══════════════════> │ SSH server    │
 │ localhost │     (encrypted)      │    ↓          │
 │ :8080     │ <═══════════════════ │ Jenkins :8080 │
 └──────────┘                       └──────────────┘
```

#### Option A: PowerShell / Windows Terminal (OpenSSH built-in)

```powershell
ssh -L 8080:localhost:8080 root@<LINUX_HOST_IP>
```

Then open in browser: **http://localhost:8080**

#### Option B: PuTTY

1. Open PuTTY, enter Host: `<LINUX_HOST_IP>`, Port: `22`
2. Go to **Connection → SSH → Tunnels**
3. Source port: `8080`
4. Destination: `localhost:8080`
5. Click **Add**
6. Click **Open**, log in
7. Open browser: **http://localhost:8080**

#### Option C: VS Code Remote SSH

If you use VS Code with the Remote-SSH extension:
1. Connect to the Linux host
2. VS Code automatically forwards ports
3. Open **http://localhost:8080** in your local browser

#### Verify Connectivity (from Windows)

```powershell
# Test if SSH works
ssh root@<LINUX_HOST_IP> "echo OK"

# Test if the tunnel works (after establishing the SSH connection)
Test-NetConnection localhost -Port 8080
# Expected: TcpTestSucceeded : True
```

---

## 6. The Build Job: Build_HelloWorld_Simple

The job runs a shell script that performs 3 stages:

```
 Stage 1: PREPARE          Stage 2: BUILD           Stage 3: TEST
 ──────────────>           ──────────────>          ──────────────>
 Print gcc and             make clean               Run ./main
 make versions             make all                 Verify output
                           (compile main.c)
```

### What Happens Internally

```bash
# Stage 1 - Prepare
gcc --version
make --version

# Stage 2 - Build
cd /var/jenkins_home/project/src
make clean        # Remove old artifacts
make all          # Compile: main.c → main.o → main

# Stage 3 - Test
./main            # Output: "Hello, World!"
```

### Creating the Job (first time only)

If the job doesn't exist yet (e.g., after deleting the Docker volume), create it via the API:

```bash
# Get CSRF token
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
CRUMB_VALUE=$(echo "$CRUMB_JSON" | python3 -c "import sys,json; print(json.load(sys.stdin)['crumb'])")

# Create the job from XML config
curl -s -X POST "http://localhost:8080/createItem?name=Build_HelloWorld_Simple" \
  -b "$COOKIE_JAR" \
  -H "Content-Type: application/xml" \
  -H "Jenkins-Crumb: $CRUMB_VALUE" \
  --data-binary @job-config.xml
```

---

## 7. Trigger a Build

### From the Web UI

1. Open **http://localhost:8080**
2. Click **Build_HelloWorld_Simple**
3. Click **Build Now** (left sidebar)

### From the Command Line

```bash
# Get CSRF token
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
CRUMB_VALUE=$(echo "$CRUMB_JSON" | python3 -c "import sys,json; print(json.load(sys.stdin)['crumb'])")

# Trigger build
curl -s -X POST "http://localhost:8080/job/Build_HelloWorld_Simple/build?delay=0sec" \
  -b "$COOKIE_JAR" \
  -H "Jenkins-Crumb: $CRUMB_VALUE"
```

---

## 8. View Build Results

### From the Web UI

1. Click **Build_HelloWorld_Simple**
2. Click a build number (e.g., **#1**) in the Build History
3. Click **Console Output** to see the full log

### From the Command Line

```bash
# Get last build status
curl -s http://localhost:8080/job/Build_HelloWorld_Simple/lastBuild/api/json | \
  python3 -c "import sys,json; d=json.load(sys.stdin); print(f'Build #{d[\"number\"]}: {d[\"result\"]}')"

# Get full console log
curl -s http://localhost:8080/job/Build_HelloWorld_Simple/lastBuild/consoleText
```

### Expected Successful Output

```
Started by user unknown or anonymous
Running as SYSTEM
Building in workspace /var/jenkins_home/workspace/Build_HelloWorld_Simple
[Build_HelloWorld_Simple] $ /bin/sh -xe /tmp/jenkins*.sh
+ echo === Build Environment ===
=== Build Environment ===
+ gcc --version
gcc (GCC) 15.2.0
...
+ echo === Building Hello World ===
=== Building Hello World ===
+ cd /var/jenkins_home/project/src
+ make clean
rm -rf main *.o
+ make all
cc -O0 -g -c -o main.o main.c
cc -O0 -g -o main main.o
+ echo === Running Hello World ===
=== Running Hello World ===
+ ./main
Hello, World!
+ echo === Build SUCCESS ===
=== Build SUCCESS ===
Finished: SUCCESS
```

---

## 9. Common Operations

| Task | Command |
|---|---|
| Start Jenkins | `docker-compose up -d` |
| Stop Jenkins | `docker-compose down` |
| Restart Jenkins | `docker-compose restart` |
| View container logs | `docker logs jenkins-hello` |
| Rebuild image (after Dockerfile change) | `docker-compose build && docker-compose up -d` |
| Enter container shell | `docker exec -it jenkins-hello bash` |
| Test GCC inside container | `docker exec jenkins-hello gcc --version` |
| Delete all data (fresh start) | `docker-compose down -v` |

---

## 10. File Structure

```
MyBuildWeb/
├── Dockerfile              ← Custom Jenkins + GCC image definition
├── docker-compose.yml      ← Docker Compose service configuration
├── Jenkinsfile             ← Pipeline definition (for SCM-based jobs)
├── src/
│   ├── main.c              ← Hello World C source code
│   └── Makefile            ← Build rules: main.c → main.o → main
└── docs/
    ├── jenkins-local-ci-guide-en.md  ← This document (English)
    └── jenkins-local-ci-guide-zh.md  ← This document (Chinese)
```

### Dockerfile — Multi-Stage Build

The Dockerfile uses a multi-stage approach to get GCC into the Jenkins image:

```
 gcc:latest image                    jenkins/jenkins:lts image
 ┌────────────────┐                  ┌────────────────────┐
 │ /usr/local/bin/ │ ── COPY ──→     │ gcc, cc, as, ld    │
 │ /usr/include/   │ ── COPY ──→     │ C headers          │
 │ /usr/lib/       │ ── COPY ──→     │ libc, libgcc       │
 └────────────────┘                  │ + Jenkins LTS      │
                                     └────────────────────┘
                                       = mybuildweb-jenkins
```

---

## 11. Troubleshooting

### Jenkins won't start
```bash
docker logs jenkins-hello     # Check for error messages
docker-compose down && docker-compose up -d   # Restart
```

### "stdio.h: No such file" during build
The Docker image is missing C headers. Rebuild:
```bash
docker-compose down
docker-compose build --no-cache
docker-compose up -d
```

### Port 8080 not reachable from remote machine
1. Verify Jenkins is running: `curl http://localhost:8080/` on the host
2. Check firewall: `iptables -L INPUT -n`
3. Use SSH tunnel (see [Section 5](#5-access-jenkins-from-another-machine))

### "No valid crumb" error when using API
Always use session cookies with the crumb token:
```bash
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
# Then pass -b "$COOKIE_JAR" in subsequent requests
```

### Start fresh (delete all Jenkins data)
```bash
docker-compose down -v    # -v removes the named volume
docker-compose up -d      # Recreates everything
```
> **Warning**: This deletes all job configurations and build history.

---

## 12. Glossary

| Term | Definition |
|---|---|
| **Docker** | A platform that packages applications into isolated containers — like shipping containers for software |
| **Docker Compose** | A tool to define and run multi-container Docker applications using a YAML file |
| **Jenkins** | An open-source CI/CD automation server — the "robot assistant" that runs build jobs |
| **Jenkins Job** | A configured task in Jenkins (e.g., "compile this code") |
| **Freestyle Project** | A simple Jenkins job type that runs shell commands |
| **Pipeline** | A more advanced Jenkins job type defined by a Jenkinsfile |
| **CSRF Crumb** | A security token Jenkins requires for API requests to prevent cross-site request forgery |
| **GCC** | GNU Compiler Collection — the C/C++ compiler |
| **Make** | A build automation tool that reads a Makefile to compile source code |
| **SSH Tunnel** | An encrypted connection that forwards network ports through SSH |
| **Volume** | A Docker mechanism for persisting data outside the container lifecycle |
| **LTS** | Long Term Support — a stable, well-tested release of Jenkins |

---

*Generated: 2026-03-27*
