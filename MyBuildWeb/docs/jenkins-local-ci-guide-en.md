# Local Jenkins CI — User Guide

> **Feynman Principle**: If you can't explain it simply, you don't understand it well enough.
> This guide teaches you how to run a local Jenkins CI server via Docker, use it to automatically build the Hello World C program, and browse build history through MyBuildWeb — as if you were learning it for the first time.

---

## Table of Contents

1. [What Is This?](#1-what-is-this)
2. [Architecture Overview](#2-architecture-overview)
3. [Prerequisites](#3-prerequisites)
4. [Quick Start: Launch Jenkins](#4-quick-start-launch-jenkins)
5. [Access Jenkins from Another Machine](#5-access-jenkins-from-another-machine)
6. [The Build Job: Build_HelloWorld_Simple (Build with Parameters)](#6-the-build-job-build_helloworld_simple-build-with-parameters)
7. [Trigger a Build](#7-trigger-a-build)
8. [View Build Results](#8-view-build-results)
9. [Find Build Artifacts](#9-find-build-artifacts)
10. [MyBuildWeb: Build History Dashboard](#10-mybuildweb-build-history-dashboard)
11. [Common Operations](#11-common-operations)
12. [File Structure](#12-file-structure)
13. [Troubleshooting](#13-troubleshooting)
14. [Glossary](#14-glossary)

---

## 1. What Is This?

Think of Jenkins as a **robot assistant** in a factory. You tell it: "every time I say go, compile my C program, run it, and tell me if it worked." Jenkins does exactly that — automatically, consistently, and without forgetting steps.

This project sets up:
- A **Jenkins server** running inside a Docker container
- A **GCC compiler toolchain** baked into the same container
- A **pre-configured build job** that compiles and runs the Hello World C program in `src/`
- A **MyBuildWeb dashboard** (inspired by Isilon BuildWeb) for browsing build history and downloading artifacts

### The Analogy

Imagine you have a personal chef (Jenkins) who lives in a portable kitchen (Docker container). The kitchen comes with all the tools (GCC, Make) pre-installed. You hand over a recipe (Jenkinsfile / job config), and the chef cooks the dish (compiles your code) whenever you ask.

---

## 2. Architecture Overview

```
 Your Machine (Linux Host)
 ┌──────────────────────────────────────────────────────────────┐
 │                                                              │
 │  docker-compose.yml                                          │
 │  ┌───────────────────────────────────────────────┐           │
 │  │  Container: jenkins-hello                     │           │
 │  │  ┌─────────────┐  ┌────────────────────────┐  │           │
 │  │  │  Jenkins     │  │  GCC 15 + Make 4.4     │  │           │
 │  │  │  (port 8080) │  │  (compiler toolchain)  │  │           │
 │  │  └─────────────┘  └────────────────────────┘  │           │
 │  │                                               │           │
 │  │  Volume: jenkins_home (build data + artifacts)│           │
 │  └─────────────┬─────────────────────────────────┘           │
 │                │ reads buildweb_data/                         │
 │                ▼                                              │
 │  ┌───────────────────────────────────────────────┐           │
 │  │  MyBuildWeb Server (Python)                   │           │
 │  │  port 9090 — build history + artifact download│           │
 │  └───────────────────────────────────────────────┘           │
 │                                                              │
 │  src/main.c + Makefile  ←── your source code                 │
 └──────────────────────────────────────────────────────────────┘

 Windows / Remote Machine
 ┌─────────────────────────────────┐
 │  Browser → SSH Tunnel           │──── SSH (port 22) ────→ Host:8080 (Jenkins)
 │  http://localhost:8080 (Jenkins)│──── SSH (port 22) ────→ Host:9090 (BuildWeb)
 │  http://localhost:9090 (Build)  │
 └─────────────────────────────────┘
```

### How the Pieces Fit Together

| Component | Role | Analogy |
|---|---|---|
| `docker-compose.yml` | Defines how to run Jenkins | The blueprint for the kitchen |
| `Dockerfile` | Builds custom Jenkins image with GCC | Kitchen equipment list |
| `Jenkinsfile` | Pipeline definition for the build | The recipe |
| `src/main.c` | The C program to compile | The ingredients |
| `src/Makefile` | Build instructions for Make | Cooking instructions |
| `buildweb/server.py` | Build history dashboard + artifact server | The restaurant menu board |

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

## 6. The Build Job: Build_HelloWorld_Simple (Build with Parameters)

This job supports **Build with Parameters** — you specify a Git repository name, and Jenkins automatically clones it from your Git server and compiles it. This is similar to how `Build_OneFS_Simple` works on the production Jenkins.

### Build Parameters

| Parameter | Default Value | Description |
|---|---|---|
| **REPO_NAME** | `hello_world` | Repository name under your Git account (e.g. `hello_world`, `my_project`) |
| **BRANCH** | `main` | Branch to build |
| **GIT_BASE_URL** | `https://eos2git.cec.lab.emc.com/mam28` | Git server base URL |

### The Analogy

Think of it like ordering at a restaurant: you don't need to go into the kitchen yourself. You just fill in the order form (parameters) — which dish (repo), which style (branch) — and the chef (Jenkins) handles everything else.

### Build Stages

The job runs 4 stages automatically:

```
 Stage 1: ENVIRONMENT     Stage 2: CLONE           Stage 3: BUILD          Stage 4: TEST
 ──────────────>          ──────────────>          ──────────────>         ──────────────>
 Print gcc/make/git       git clone from           make clean              Run ./main
 versions                 eos2git server           make all                Verify output
```

### What Happens Internally

```bash
# Stage 1 - Environment
gcc --version
make --version
git --version

# Stage 2 - Clone Repository
git clone --branch ${BRANCH} --single-branch \
    ${GIT_BASE_URL}/${REPO_NAME}.git ${WORKSPACE}/repo

# Stage 3 - Build
cd ${WORKSPACE}/repo
make clean
make all          # Compile: main.c → main.o → main

# Stage 4 - Test
./main            # Output: "Hello, World!"
```

### Git Authentication

The Jenkins container uses a `.netrc` file for Git credentials. To configure (first time or after container recreate):

```bash
docker exec jenkins-hello bash -c "cat > /root/.netrc << 'EOF'
machine eos2git.cec.lab.emc.com
login <YOUR_USERNAME>
password <YOUR_PERSONAL_ACCESS_TOKEN>
EOF
chmod 600 /root/.netrc"
```

> **Note**: Generate a classic Personal Access Token at `https://eos2git.cec.lab.emc.com/settings/tokens/new` with **repo** scope.

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

### From the Web UI (Build with Parameters)

1. Open **http://localhost:8080**
2. Click **Build_HelloWorld_Simple**
3. Click **Build with Parameters** (left sidebar)
4. Fill in the parameters:
   - **REPO_NAME**: e.g. `hello_world`
   - **BRANCH**: e.g. `main`
   - **GIT_BASE_URL**: default is fine for `mam28`'s repos
5. Click **Build**

### From the Command Line

```bash
# Get CSRF token
COOKIE_JAR=/tmp/jenkins-cookies.txt
CRUMB_JSON=$(curl -s -c "$COOKIE_JAR" 'http://localhost:8080/crumbIssuer/api/json')
CRUMB_VALUE=$(echo "$CRUMB_JSON" | python3 -c "import sys,json; print(json.load(sys.stdin)['crumb'])")

# Trigger parameterized build
curl -s -X POST "http://localhost:8080/job/Build_HelloWorld_Simple/buildWithParameters" \
  -b "$COOKIE_JAR" \
  -H "Jenkins-Crumb: $CRUMB_VALUE" \
  --data-urlencode "REPO_NAME=hello_world" \
  --data-urlencode "BRANCH=main" \
  --data-urlencode "GIT_BASE_URL=https://eos2git.cec.lab.emc.com/mam28"
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
+ set -e
============================================
  Build_HelloWorld_Simple (Parameterized)
============================================
  REPO_NAME : hello_world
  BRANCH    : main
  GIT_URL   : https://eos2git.cec.lab.emc.com/mam28/hello_world.git
============================================

=== Stage 1: Environment ===
gcc (GCC) 15.2.0
GNU Make 4.4.1
git version 2.47.3

=== Stage 2: Clone Repository ===
Cloning into '/var/jenkins_home/workspace/Build_HelloWorld_Simple/repo'...

=== Stage 3: Build ===
cc -O0 -g -c -o main.o main.c
cc -O0 -g -o main main.o

=== Stage 4: Test ===
Hello, World!

============================================
  BUILD SUCCESS
============================================
Finished: SUCCESS
```

---

## 9. Find Build Artifacts

After a successful build, the compiled files are stored **inside the Jenkins container**. Think of it like a warehouse — the finished products are stored in a specific shelf (directory), and you need to know the shelf number to find them.

### Artifact Location

```
Container path:
/var/jenkins_home/workspace/Build_HelloWorld_Simple/repo/
├── main      ← Executable binary
├── main.o    ← Object file
├── main.c    ← Source code (cloned)
└── Makefile  ← Build rules (cloned)
```

### Retrieve Artifacts

#### Option A: docker cp (simplest)

Copy the compiled binary to your host machine:
```bash
# Copy the executable
docker cp jenkins-hello:/var/jenkins_home/workspace/Build_HelloWorld_Simple/repo/main ./main

# Copy all build artifacts
docker cp jenkins-hello:/var/jenkins_home/workspace/Build_HelloWorld_Simple/repo/ ./build-output/
```

#### Option B: Enter the container

```bash
docker exec -it jenkins-hello bash
cd /var/jenkins_home/workspace/Build_HelloWorld_Simple/repo/
ls -la
```

#### Option C: View via Jenkins API

```bash
# List workspace files
curl -s http://localhost:8080/job/Build_HelloWorld_Simple/ws/repo/

# Download a specific file
curl -s -O http://localhost:8080/job/Build_HelloWorld_Simple/ws/repo/main
```

### Artifact Lifecycle

- Artifacts are **overwritten** on each new build (the workspace is cleaned before clone)
- Artifacts persist as long as the Jenkins container and its volume exist
- Running `docker-compose down -v` **deletes all artifacts** along with the volume

---

## 10. MyBuildWeb: Build History Dashboard

MyBuildWeb is a lightweight HTTP server (inspired by Isilon's `build.west.isilon.com`) that provides a **web-based dashboard** for browsing build history and downloading artifacts. It reads build metadata from the Jenkins Docker volume — no database required.

### The Analogy

If Jenkins is the chef who cooks your food, MyBuildWeb is the **restaurant display window** — it shows every dish (build) the chef has ever made: when it was cooked, whether it turned out well, and lets you take the finished dish home (download artifacts).

### How It Works

```
 Jenkins (inside Docker)              MyBuildWeb (on host)
 ┌──────────────────────┐            ┌──────────────────────────────┐
 │ Build completes →    │            │                              │
 │ Saves to volume:     │            │  Reads from Docker volume:   │
 │  buildweb_data/      │───────────→│  /var/lib/docker/volumes/    │
 │   hello_world_001/   │            │   mybuildweb_jenkins_home/   │
 │    build_meta.json   │            │    _data/buildweb_data/      │
 │    artifacts/        │            │                              │
 │     main             │            │  Serves on port 9090:        │
 │     main.o           │            │  - Web UI (build history)    │
 │     *.tar.gz         │            │  - JSON API                  │
 └──────────────────────┘            │  - File downloads            │
                                     └──────────────────────────────┘
```

### Start the BuildWeb Server

```bash
# Set the builds directory to the Jenkins Docker volume
export BUILDS_DIR="/var/lib/docker/volumes/mybuildweb_jenkins_home/_data/buildweb_data"
export BUILDWEB_PORT=9090

# Start in the background
cd /path/to/io-tracing/MyBuildWeb
nohup python3 buildweb/server.py > /tmp/buildweb.log 2>&1 &

# Verify it's running
curl -s -o /dev/null -w "%{http_code}" http://localhost:9090/
# Expected: 200
```

### Access the Web UI

Open in your browser:
```
http://localhost:9090
```

You'll see a dashboard with:
- **Summary bar** — total builds, succeeded count, failed count
- **Build cards** — one card per build, color-coded:
  - Green: Succeeded
  - Red: Failed
  - Cyan: Running
- **Metadata per build** — name, status, duration, start/end time, git hash, machine, build number
- **Download buttons** — click to download any artifact (binary, object file, source tarball)

### Access from a Remote Machine (SSH Tunnel)

Just like Jenkins, MyBuildWeb requires an SSH tunnel for remote access. You can tunnel both ports in a single SSH command:

```powershell
# Forward both Jenkins (8080) and BuildWeb (9090)
ssh -L 8080:localhost:8080 -L 9090:localhost:9090 root@<LINUX_HOST_IP>
```

Then open in browser:
- Jenkins: **http://localhost:8080**
- BuildWeb: **http://localhost:9090**

### Build Detail Page

Click any build name (e.g., `hello_world_001`) to see the **build detail page**, which includes:
- Full build metadata table
- Console output log (same as Jenkins Console Output)

URL pattern: `http://localhost:9090/build/hello_world_001`

### JSON API

MyBuildWeb provides a JSON API for programmatic access:

```bash
# List all builds
curl -s http://localhost:9090/api/builds | python3 -m json.tool
```

Example response:
```json
[
  {
    "build_name": "hello_world_003",
    "status": "Succeeded",
    "duration": "12",
    "start_time": "2026-03-27 04:30:00",
    "end_time": "2026-03-27 04:30:12",
    "started_by": "anonymous",
    "machine": "sles15sp6",
    "build_number": "003",
    "git_hash": "a0cb1d6be0e4...",
    "git_repo": "hello_world",
    "last_step": "Test"
  }
]
```

### Download Artifacts

#### From the Web UI

Each build card has download buttons for all artifacts. Click to download directly.

#### From the Command Line

```bash
# Download the compiled binary
curl -s -O http://localhost:9090/download/hello_world_001/main

# Download source tarball
curl -s -O http://localhost:9090/download/hello_world_001/hello_world-src.tar.gz

# Download object file
curl -s -O http://localhost:9090/download/hello_world_001/main.o
```

#### URL Pattern

```
http://localhost:9090/download/{build_name}/{filename}
```

### Build Data Structure

Each build is stored as a directory under `buildweb_data/`:

```
buildweb_data/
├── hello_world_001/
│   ├── build_meta.json      ← Build metadata (status, time, git hash, etc.)
│   ├── console.log          ← Jenkins console output
│   └── artifacts/
│       ├── main             ← Compiled ELF binary
│       ├── main.o           ← Object file
│       └── hello_world-src.tar.gz  ← Source code tarball
├── hello_world_002/
│   ├── build_meta.json
│   ├── console.log
│   └── artifacts/
│       └── ...
└── ...
```

### Environment Variables

| Variable | Default | Description |
|---|---|---|
| `BUILDS_DIR` | `/var/jenkins_home/buildweb_data` | Path to the builds data directory |
| `BUILDWEB_PORT` | `8081` | Port to listen on (we override to `9090`) |

### Stop the BuildWeb Server

```bash
# Find the process
ps aux | grep 'buildweb/server.py' | grep -v grep

# Kill it
kill $(ps aux | grep 'buildweb/server.py' | grep -v grep | awk '{print $2}')
```

---

## 11. Common Operations

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
| Start BuildWeb server | `BUILDS_DIR=/var/lib/docker/volumes/mybuildweb_jenkins_home/_data/buildweb_data BUILDWEB_PORT=9090 nohup python3 buildweb/server.py &` |
| Stop BuildWeb server | `kill $(ps aux \| grep server.py \| grep -v grep \| awk '{print $2}')` |
| Open BuildWeb | `http://localhost:9090` |

---

## 12. File Structure

```
MyBuildWeb/
├── Dockerfile              ← Custom Jenkins + GCC image definition
├── docker-compose.yml      ← Docker Compose service configuration
├── Jenkinsfile             ← Pipeline definition (for SCM-based jobs)
├── buildweb/
│   └── server.py           ← MyBuildWeb dashboard server (Python, port 9090)
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

## 13. Troubleshooting

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

### BuildWeb shows "No builds yet"
1. Verify builds exist: `ls /var/lib/docker/volumes/mybuildweb_jenkins_home/_data/buildweb_data/`
2. Check `BUILDS_DIR` is set correctly when starting the server
3. Trigger a build from Jenkins and wait for it to complete

### BuildWeb port 9090 not reachable
1. Check the server is running: `ps aux | grep server.py`
2. Verify locally: `curl http://localhost:9090/`
3. For remote access, use SSH tunnel: `ssh -L 9090:localhost:9090 root@<LINUX_HOST_IP>`

### Start fresh (delete all Jenkins data)
```bash
docker-compose down -v    # -v removes the named volume
docker-compose up -d      # Recreates everything
```
> **Warning**: This deletes all job configurations and build history.

---

## 14. Glossary

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
| **Build with Parameters** | A Jenkins feature that lets you pass input values (like repo name, branch) when triggering a build |
| **PAT** | Personal Access Token — a password substitute for authenticating to Git servers via HTTPS |
| **.netrc** | A file that stores machine credentials for automatic login by tools like `git` and `curl` |
| **Workspace** | The directory inside Jenkins where a job's files are checked out and built |
| **MyBuildWeb** | A lightweight build history dashboard inspired by Isilon BuildWeb — shows build metadata and downloadable artifacts |
| **BuildWeb** | Isilon's internal build results website (`build.west.isilon.com`); MyBuildWeb is a simplified local version |
| **build_meta.json** | A JSON file generated by each Jenkins build containing metadata (status, timing, git hash, etc.) |

---

*Updated: 2026-03-27*
