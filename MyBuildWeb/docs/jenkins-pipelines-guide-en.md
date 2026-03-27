# Jenkins Pipelines — A Feynman-Style Guide

> **Feynman Principle**: If you can't explain it simply, you don't understand it well enough.
> This guide explains the PowerScale (OneFS) Jenkins CI/CD pipeline system as if you were teaching it to someone who has never seen it before. We start with the big picture, then zoom in.

---

## Table of Contents

1. [What Is This Whole Thing?](#1-what-is-this-whole-thing)
2. [The Big Picture: How OneFS Gets Built](#2-the-big-picture-how-onefs-gets-built)
3. [The Main Build Pipeline — Step by Step](#3-the-main-build-pipeline--step-by-step)
4. [Packaging: Turning Build Artifacts into Deliverables](#4-packaging-turning-build-artifacts-into-deliverables)
5. [Code Signing: Making Sure Nothing Was Tampered With](#5-code-signing-making-sure-nothing-was-tampered-with)
6. [QA Test Packages: Building Test Tools](#6-qa-test-packages-building-test-tools)
7. [Cloud Builds: AMI and VHD Images](#7-cloud-builds-ami-and-vhd-images)
8. [Utility Pipelines and Scripts](#8-utility-pipelines-and-scripts)
9. [Node Management](#9-node-management)
10. [Promotion Workflow: Test → Staging → Production](#10-promotion-workflow-test--staging--production)
11. [Directory Map](#11-directory-map)
12. [Glossary](#12-glossary)

---

## 1. What Is This Whole Thing?

Imagine you're building a house. You need:

- **Raw materials** (source code)
- **Workers** (build servers)
- **A foreman** (Jenkins) who tells the workers what to do and in what order
- **Quality inspectors** (signing, testing)
- **Delivery trucks** (packaging into installer, USB stick, simulator, cloud images)

This repository is the **foreman's instruction book** — a collection of Jenkins pipeline definitions (Jenkinsfiles) that orchestrate the entire process of building, signing, packaging, and distributing Dell PowerScale's OneFS operating system.

### Why does this matter?

Without these pipelines, engineers would have to manually compile code, sign binaries, create installer packages, and upload them — all by hand. These pipelines automate thousands of steps into a single "press the button" workflow.

---

## 2. The Big Picture: How OneFS Gets Built

Think of the build process as a **relay race** with four legs:

```
   [1] BUILD          [2] SIGN           [3] PACKAGE         [4] DISTRIBUTE
   ─────────>         ─────────>         ─────────>          ─────────>
   Compile OneFS      Cryptographic      Create installer,   Upload to
   source code in     signatures on      USB stick, OVA,     artifact servers
   a FreeBSD jail     all binaries       cloud images        & notify users
```

Each "leg" is a separate Jenkins pipeline, and they are chained together automatically.

### The Analogy

Think of it like a **car factory assembly line**:
1. **BUILD** = Welding the chassis and engine (compiling code)
2. **SIGN** = Stamping the VIN number and safety certifications (cryptographic proof)
3. **PACKAGE** = Putting the car in a shipping container (creating installer packages)
4. **DISTRIBUTE** = Delivering to dealerships (uploading to servers)

---

## 3. The Main Build Pipeline — Step by Step

**File**: `Jenkinsfile_onefs_build` — The "conductor" of the orchestra.

This is the single most important file. It kicks off everything. Here's what it does, explained simply:

### Stage 1: Prep — "Getting Ready"

Like a chef preparing their ingredients before cooking:

1. **Validate the branch name** — Make sure someone didn't type "BR_MAIN" as "br-main"
2. **Get the Git SHA** — Ask GitHub: "What's the latest commit on this branch?" This is like getting a serial number for the exact version of code we're building.
3. **Register with BuildWeb** — Tell the internal build tracking system: "Hey, I'm starting build #47 of branch BR_MAIN."
4. **Create directory symlinks** — Set up the file paths where build artifacts will live.

### Stage 2: Build QA (async) — "Order the test tools"

Like ordering test equipment while the main build runs — we don't wait for it:

- Fires off a separate job to build test packages for Linux/Windows.
- `wait: false` — the main build continues immediately.

### Stage 3: Checkout OneFS — "Get the blueprints"

- Clones the OneFS source code repository.
- Uses a **reference repository** for speed (like caching — instead of downloading everything, it references a local copy).

### Stage 4: Prepare Jails — "Set up the construction site"

This is unique to FreeBSD/OneFS:

- A **jail** is a lightweight FreeBSD container. OneFS must be built inside a FreeBSD jail because it's a FreeBSD-based OS.
- Different OneFS versions need different jail types (FreeBSD 11 vs 12 vs 13).

### Stage 5: Checkout Driver — "Get the foreman's tools"

- Checks out `tools-driver`, a Python script that actually orchestrates the compilation steps inside the jail.

### Stage 6: Build OneFS — "The actual construction"

This is where the magic happens:

```
sudo jexec -n ${jail.name} sh -c "python ./driver.py $driverArgs"
```

Translation: "Enter the FreeBSD jail and run the driver script, which compiles all of OneFS."

**Key detail**: The driver also triggers code signing by making an HTTP call to the signing Jenkins job. This is how the build and signing systems communicate.

### Stage 7: Coverity Analysis (optional) — "Safety inspection"

If enabled, runs Coverity static analysis to find bugs without running the code.

### Stage 8: Package OneFS — "Put it in the box"

Triggers `Jenkinsfile_onefs_packaging` to create all the deliverables.

### Stage 9: Cloud Builds (optional) — "Build for the cloud"

Creates AMI (AWS) and VHD (Azure) images if requested.

---

## 4. Packaging: Turning Build Artifacts into Deliverables

**File**: `Jenkinsfile_onefs_packaging` — The "assembly line"

Once the raw build is done, we need to turn it into things users can actually install. This pipeline orchestrates four sub-jobs:

| What | Pipeline | Analogy |
|------|----------|---------|
| **Installer Package** | `Jenkinsfile_pkg_install` | The "setup.exe" — what you run to upgrade an existing cluster |
| **Re-image Stick** | `Jenkinsfile_pkg_reimage_stick` | A bootable USB drive to completely reinstall a node |
| **Provisioning Package** | `Jenkinsfile_pkg_provisioning` | IPS (Isilon Provisioning Service) package for pave-repave operations |
| **Simulator OVA** | `Jenkinsfile_pkg_simulator` | A VMware virtual appliance for testing OneFS without hardware |

### How it works

1. **Installer** runs first (it's the prerequisite)
2. Then **Re-image Stick**, **Provisioning**, and **Simulator** run **in parallel** — because they're independent of each other

This is like: first bake the cake (installer), then simultaneously wrap it in a box, put candles on a copy, and photograph it for the website.

---

## 5. Code Signing: Making Sure Nothing Was Tampered With

**File**: `codesign/Jenkinsfile_codesign` — The "notary public"

### Why do we sign code?

Imagine you receive a letter. How do you know it really came from who it claims? You check the wax seal. Code signing is the digital equivalent — it cryptographically proves that the software wasn't modified after it was built.

### How it works (simplified)

```
1. COPY files from the build server → signing server
2. START a Docker container with Garasign (HSM client)
3. EXPORT the private key from the HSM (Hardware Security Module)
4. SIGN each file using OpenSSL
5. VERIFY the signatures are valid
6. COPY signed files back to the build server
7. WRITE a "sentinel" file to tell the build server "I'm done"
```

### The Four Signing Modes

| Mode | What it signs | Real-world analogy |
|------|---------------|-------------------|
| **Packaging** | General files (RSA signatures) | A notary stamp on a document |
| **SecureBoot_PE** | Firmware (Authenticode format) | A government-issued safety sticker |
| **SecureBoot_RSA** | Kernel manifest | A certificate of authenticity |
| **OVA** | VMware virtual appliances | A tamper-evident seal on a package |

### The HSM Connection

The signing key never leaves the HSM (a physical security device). Instead:
1. Garasign **exports an obfuscated copy** of the key
2. OpenSSL uses this obfuscated key to sign
3. The key is deleted after use

This is like: the bank gives you a temporary keycard to access a safe deposit box, then the keycard self-destructs after use.

---

## 6. QA Test Packages: Building Test Tools

**Files**: `Jenkinsfile_qa_linux`, `Jenkinsfile_test_package_windows7`, `Jenkinsfile_qa_packaging`

These pipelines build test packages for various platforms so QA engineers can run tests against the OneFS build.

### Supported platforms (via Docker)

Each platform has its own Dockerfile in `qabuild/`:

| Dockerfile | Platform |
|-----------|----------|
| `Dockerfile.centos7` | CentOS 7 |
| `Dockerfile.centos8` | CentOS 8 |
| `Dockerfile.bionic` | Ubuntu 18.04 |
| `Dockerfile.focal` | Ubuntu 20.04 |
| `Dockerfile.jammy` | Ubuntu 22.04 |
| `Dockerfile.noble` | Ubuntu 24.04 |
| `Dockerfile.xenial` | Ubuntu 16.04 |
| `Dockerfile.opensuse15` | OpenSUSE 15 |

The test packages are built by checking out the OneFS source, running the test build inside a Docker container matching the target platform, and uploading the result.

---

## 7. Cloud Builds: AMI and VHD Images

**File**: `Jenkinsfile_cloud_packaging`

This pipeline creates cloud-native images from a successful OneFS build:

- **AMI** — Amazon Machine Image for AWS
- **VHD** — Virtual Hard Disk for Azure

Think of it like: you built a car, now you need to convert it into a boat (AMI) and a plane (VHD) for different "roads."

---

## 8. Utility Pipelines and Scripts

### Utilities Directory (`utilities/`)

| Pipeline | What it does |
|----------|-------------|
| `Jenkinsfile_helloworld` | A simple test pipeline — the "Hello World" of Jenkins |
| `Jenkinsfile_node_maintenance` | Runs maintenance tasks on build nodes |
| `Jenkinsfile_node_updater` | Updates software on build nodes |
| `Jenkinsfile_builder_upgrade` | Upgrades builder machines |
| `Jenkinsfile_heap_report` | Generates JVM heap reports for Jenkins diagnostics |
| `Jenkinsfile_ps_image_builder` | Builds PowerScale VM images |
| `Jenkinsfile_ps_image_tester` | Tests PowerScale VM images |
| `Jenkinsfile_maintenance_launcher` | Orchestrates multiple maintenance jobs |
| `Jenkinsfile_prep_for_dev` | Prepares a development environment |
| `Jenkinsfile_win7_maintenance` | Maintains Windows 7 build nodes |

### Shell Scripts

| Script | Purpose |
|--------|---------|
| `fastmerge.sh` | Quickly merge dev → stage → prod (with safety prompts). Warning: "THIS IS NOT PROPER GIT ETIQUETTE" |
| `promote-cec.sh` | Promote cec-tst → cec-staging → cec-prd (the proper way) |
| `sshcmd.sh` | SSH command helper |

---

## 9. Node Management

**File**: `nodes/nodecmd.py`

### What is it?

A Python tool for running SSH commands across multiple Jenkins build nodes at once.

### The Analogy

Imagine you manage 50 servers. Instead of SSH-ing into each one individually to run `uptime`, you tell `nodecmd.py`: "Run `uptime` on all servers labeled `builder`."

### How it works

1. Reads a Jenkins CasC (Configuration as Code) YAML file that lists all nodes
2. Filters nodes by label (e.g., `builder`, `packager`, `docker-agent`)
3. Runs the given SSH command on all matching nodes (optionally in parallel)

```bash
# Example: Check disk space on all packager nodes
python nodecmd.py -f osj-isi-02-prd.yaml -l packager -c "df -h"

# Example: Update packages on all builders in parallel
python nodecmd.py -f osj-isi-02-prd.yaml -l builder -p -c "sudo pkg update"
```

---

## 10. Promotion Workflow: Test → Staging → Production

### The Three Environments

```
  cec-tst (Test)  →  cec-staging (Staging)  →  cec-prd (Production)
```

This is a standard deployment pattern:

1. **Test (cec-tst)**: Where developers merge their feature branches. Pipelines here are experimental.
2. **Staging (cec-staging)**: A mirror of production. After testing, changes are promoted here for final validation.
3. **Production (cec-prd)**: The live pipelines that actually build and ship OneFS.

### The `promote-cec.sh` script

```
Step 1: Merge cec-tst → cec-staging, push
Step 2: PAUSE — human verifies staging works
Step 3: Merge cec-staging → cec-prd, push
Step 4: Return to cec-tst branch
```

This is like: moving a new recipe from the test kitchen → to the staging kitchen for chef review → to the main restaurant kitchen.

---

## 11. Directory Map

```
jenkins-pipelines/
├── Jenkinsfile_onefs_build          # The main orchestrator
├── Jenkinsfile_onefs_packaging      # Packaging orchestrator
├── Jenkinsfile_cloud_packaging      # Cloud image builds
├── Jenkinsfile_coverity             # Static analysis
├── Jenkinsfile_ime_onefs_launcher   # IME builds
├── Jenkinsfile_pkg_*                # Individual packaging jobs
├── Jenkinsfile_qa_*                 # QA test package jobs
├── Jenkinsfile_test_package_*       # Platform-specific test packages
│
├── codesign/                        # Code signing subsystem
│   ├── Jenkinsfile_codesign         # Core signing engine
│   ├── Jenkinsfile_isi_packager     # .isi package creation
│   ├── Jenkinsfile_manifest_signing # Manifest/patch signing
│   ├── Jenkinsfile_sign_and_package_launcher  # Signing orchestrator
│   ├── Jenkinsfile_mpa_signer       # Multi-party authorization
│   ├── Jenkinsfile_sign_simulator   # Simulator signing
│   └── container/                   # Docker image for signing
│       ├── Dockerfile               # Garasign/CSSv3 container
│       ├── config.ini               # Garasign config
│       └── ...                      # Signing certificates & configs
│
├── qabuild/                         # Docker images for QA builds
│   ├── Dockerfile.centos7           # CentOS 7 build env
│   ├── Dockerfile.focal             # Ubuntu 20.04 build env
│   └── ...                          # Other platforms
│
├── nodes/                           # Node management
│   ├── nodecmd.py                   # SSH command multiplexer
│   ├── osj-isi-02-prd.yaml         # Production node config
│   └── osj-isi-02-tst.yaml         # Test node config
│
├── utilities/                       # Maintenance & helper pipelines
├── deprecated/                      # Legacy pipelines (kept for reference)
├── ducttape/                        # Quick-fix / temporary pipelines
├── experimental/                    # Experimental/prototype pipelines
├── hc/                              # HealthCheck pipelines
├── misc/                            # Miscellaneous (NFS client tests)
├── pscale-admin/                    # Administrative tasks
│
├── promote-cec.sh                   # Promotion script (tst→staging→prd)
├── fastmerge.sh                     # Fast merge script (dev→stage→prod)
├── sshcmd.sh                        # SSH helper
└── README.md                        # Original code map
```

---

## 12. Glossary

| Term | Meaning |
|------|---------|
| **OneFS** | Dell PowerScale's clustered file system operating system |
| **BuildWeb** | Internal build tracking system (tracks build numbers, status) |
| **Jail** | FreeBSD lightweight container used for building OneFS |
| **Driver** | `tools-driver` — a Python script that orchestrates the actual compilation inside a jail |
| **Garasign** | Dell's code signing service (CSSv3) that manages HSM keys |
| **HSM** | Hardware Security Module — a physical device that stores cryptographic keys |
| **CasC** | Configuration as Code — Jenkins node definitions in YAML format |
| **Sentinel** | A watch file used to signal completion between the build and signing systems |
| **Bits** | Build artifacts (the compiled output) |
| **bitsDir** | The directory name pattern for build artifacts (e.g., `b.hexie.jdoe.001`) |
| **IPS** | Isilon Provisioning Service — used for pave-repave operations |
| **OVA** | Open Virtual Appliance — VMware virtual machine format |
| **AMI** | Amazon Machine Image — AWS virtual machine format |
| **VHD** | Virtual Hard Disk — Azure virtual machine format |
| **PE** | Portable Executable — Windows binary format (used for firmware signing) |
| **RSA** | Rivest-Shamir-Adleman — a public-key cryptographic algorithm |
| **CEC** | Corporate Engineering Center — Dell's internal infrastructure |
| **MPA** | Multi-Party Authorization — requires multiple approvals for signing |
| **Coverity** | A static code analysis tool for finding bugs |

---

*This document was generated using the Feynman learning principle: explain complex systems in simple terms, using analogies and progressive detail. Last updated: 2026-03-27.*
