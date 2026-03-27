# Jenkins CI/CD Skill for Windsurf IDE

## Overview

Lightweight Python CLI wrapper for the Jenkins REST API. Enables AI assistants in Windsurf to check builds, view logs, trigger jobs, manage queues, and inspect nodes — all via a single script with **zero external dependencies** (stdlib only).

## Architecture

```
Windsurf (run_command) → jenkins_api.py → Jenkins REST API
                              ↑
                         .env credentials
```

- **No Docker, no MCP server, no pip install** — just Python 3 + urllib
- Supports **multiple Jenkins servers** (`_1`, `_2`, `_3` suffixes in .env)
- Auto-handles **CSRF crumbs** for POST requests
- Auto-handles **self-signed SSL** via `JENKINS_SSL_VERIFY=false`
- Folder-path jobs (e.g. `folder/sub/job`) auto-encoded to Jenkins URL format

## Setup

1. Copy `.env.example` to `.env` in this directory (or add variables to `~/.env`):

```bash
cp .env.example .env
# Edit .env with your Jenkins credentials
```

2. **Update Windsurf Memory** — Create or update a memory entry so the AI assistant knows to use this skill for Jenkins operations. Suggested memory content:

> **Title**: Jenkins CI/CD Skill  
> **Content**: Jenkins CI/CD integration for Windsurf IDE. Use this skill for ALL Jenkins operations — checking builds, viewing logs, triggering jobs, managing nodes, etc. Always use `python3 path/to/jenkins/jenkins_api.py` for Jenkins operations. DO NOT use raw curl calls. The wrapper handles authentication, SSL (self-signed certs), CSRF crumbs, and multi-server routing. Credentials are stored in the skill's .env file. The skill contains 6 files: SKILL.md, AVAILABLE_TOOLS.md, jenkins_api.py, .env, .env.example, README.md.  
> **Tags**: `jenkins`, `ci_cd`, `skill`, `integration`

3. That's it. No other setup needed.

## Files

| File | Purpose |
|------|---------|
| `SKILL.md` | LLM instructions (loaded by Windsurf as skill) |
| `AVAILABLE_TOOLS.md` | Detailed tool/parameter reference |
| `jenkins_api.py` | Main CLI wrapper script |
| `.env.example` | Credential template |
| `.env` | Your actual credentials (gitignored) |
| `README.md` | This file |

## Quick Test

```bash
# List configured servers
python3 jenkins_api.py list_servers

# Check connectivity
python3 jenkins_api.py who_am_i

# List tools
python3 jenkins_api.py list_tools
```

## Available Tools (17)

| Category | Tools |
|----------|-------|
| **System** | `list_servers`, `who_am_i`, `get_status` |
| **Jobs** | `list_jobs`, `get_job`, `get_job_config`, `trigger_build` |
| **Builds** | `get_build`, `get_build_log`, `search_build_log`, `get_running_builds`, `stop_build`, `get_test_report` |
| **Queue** | `get_queue`, `cancel_queue_item` |
| **Nodes** | `list_nodes`, `get_node` |
| **Views** | `list_views` |

## Comparison with MCP Approach

This skill uses **direct REST API calls** instead of MCP. See the main conversation for a detailed comparison. In short:

| | This skill (curl/REST) | MCP approach |
|--|------------------------|--------------|
| **Dependencies** | None (Python stdlib) | Docker + MCP server |
| **Setup** | Copy .env, done | Install plugin or run container |
| **Debugging** | Direct HTTP, easy | Multi-layer, harder |
| **Flexibility** | Can call any Jenkins API | Limited to exposed MCP tools |
| **Cross-IDE** | Windsurf only | Any MCP client |
