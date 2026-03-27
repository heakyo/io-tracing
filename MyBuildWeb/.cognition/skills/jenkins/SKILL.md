---
name: "jenkins"
description: "Jenkins CI/CD integration for Windsurf IDE. Use this skill for ALL Jenkins operations — checking builds, viewing logs, triggering jobs, managing nodes, etc."
---

**ALWAYS** use `python3 path/to/jenkins/jenkins_api.py` for Jenkins operations. **DO NOT** use raw `curl` calls — the wrapper handles authentication, SSL (self-signed certs), CSRF crumbs, and multi-server routing.

## Architecture

Lightweight Python CLI wrapper (stdlib only, no pip install needed):
- Reads credentials from `.env` (local) or `~/.env` (global fallback)
- Supports **multiple Jenkins servers** via `_1`, `_2`, `_3` suffixes
- Handles CSRF crumb tokens automatically for POST requests
- Handles self-signed SSL certificates via `JENKINS_SSL_VERIFY=false`
- Folder-path jobs like `folder/subfolder/job-name` are auto-encoded

## Syntax

```bash
# Basic usage (uses server 1 by default)
python3 path/to/jenkins/jenkins_api.py <TOOL_NAME> '{"param": "value"}'

# Target a specific server
python3 path/to/jenkins/jenkins_api.py --server 2 <TOOL_NAME> '{"param": "value"}'

# File payload (for complex params)
python3 path/to/jenkins/jenkins_api.py <TOOL_NAME> --file /tmp/payload.json

# List all available tools
python3 path/to/jenkins/jenkins_api.py list_tools
```

## Multi-Server Routing

When multiple Jenkins servers are configured (`JENKINS_URL_1`, `JENKINS_URL_2`, ...):
1. **Default**: All commands target server `1` unless `--server N` is specified.
2. **Discovery**: Use `list_servers` to show all configured servers.
3. **Prompt**: If the user doesn't specify which server, ask them first.

## Tool Discovery

See **`AVAILABLE_TOOLS.md`** for detailed parameter schemas.

Quick list:
```bash
python3 path/to/jenkins/jenkins_api.py list_tools
```

## Examples

```bash
# List all configured servers
python3 path/to/jenkins/jenkins_api.py list_servers

# List jobs at root level
python3 path/to/jenkins/jenkins_api.py list_jobs '{}'

# List jobs in a folder
python3 path/to/jenkins/jenkins_api.py list_jobs '{"folder": "ECS/deploy"}'

# Get job details
python3 path/to/jenkins/jenkins_api.py get_job '{"job_name": "ECS/deploy/standup"}'

# Get last build status
python3 path/to/jenkins/jenkins_api.py get_build '{"job_name": "my-job"}'

# Get specific build
python3 path/to/jenkins/jenkins_api.py get_build '{"job_name": "my-job", "build_number": 42}'

# Get last 50 lines of build log
python3 path/to/jenkins/jenkins_api.py get_build_log '{"job_name": "my-job", "tail": 50}'

# Search build log for errors
python3 path/to/jenkins/jenkins_api.py search_build_log '{"job_name": "my-job", "pattern": "ERROR|FATAL"}'

# Trigger a build with parameters
python3 path/to/jenkins/jenkins_api.py trigger_build '{"job_name": "deploy", "parameters": {"BRANCH": "main", "ECS_VER": "4.3.0"}}'

# Trigger on server 2
python3 path/to/jenkins/jenkins_api.py --server 2 trigger_build '{"job_name": "deploy"}'

# Get running builds
python3 path/to/jenkins/jenkins_api.py get_running_builds '{}'

# Stop a build
python3 path/to/jenkins/jenkins_api.py stop_build '{"job_name": "my-job", "build_number": 123}'

# Get test report
python3 path/to/jenkins/jenkins_api.py get_test_report '{"job_name": "my-job", "build_number": "lastBuild"}'

# List nodes
python3 path/to/jenkins/jenkins_api.py list_nodes '{}'
```

## Safety Rules

- **trigger_build** and **stop_build** are destructive operations — **always confirm with the user** before executing.
- **cancel_queue_item** removes a queued build — confirm before executing.
- Read-only tools (get_*, list_*, search_*, who_am_i) are safe to run freely.

## Output

All output is JSON (pretty-printed). Errors are returned as `{"error": "message"}`.
Build logs (`get_build_log`) return `{"log": "...", "job": "...", "build": "..."}`.
