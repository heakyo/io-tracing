# GitHub and Jira MCP Integration for Windsurf (CLI Wrapper)

## Overview
This skill enables integration between Windsurf IDE and GitHub, Jira, and Confluence services using a local Python wrapper script that communicates with MCP servers running in Docker.

**Note**: Due to Windsurf's security restrictions on custom MCP servers, this integration uses a CLI-based approach via `mcp_wrapper.py` instead of native IDE integration.

## Architecture
* **Client**: `mcp_wrapper.py` (Python script, client-daemon model)
* **Config**: `config.py` (service images, env mappings, commands — decoupled from logic)
* **Servers**: Docker containers started via plain `docker run` (no Docker Compose required)
* **Communication**: JSON-RPC over Stdio

## Features
* GitHub repository access and pull request management
* Jira ticket retrieval and status monitoring
* Confluence page creation and editing

## Setup Instructions
1. Ensure Docker is installed and available (just `docker`, no `docker compose` needed).
2. Ensure `~/.env` (or local `.env` in the skill directory) contains valid credentials (see `.env.example`):
   - `GITHUB_TOKEN`, `GITHUB_API_URL`
   - `JIRA_URL`, `JIRA_TOKEN`
   - `CONFLUENCE_URL`, `CONFLUENCE_TOKEN`
3. Ensure `mcp_wrapper.py` and `config.py` exist alongside each other.
4. Use the skill via `run_command` in Windsurf.

## Usage Examples

### Get Jira Ticket
```bash
python3 mcp_wrapper.py atlassian --method tools/call --params '{"name": "jira_get_issue", "arguments": {"issue_key": "OBS06A-104"}}'
```

### List GitHub PRs
```bash
python3 mcp_wrapper.py github --method tools/call --params '{"name": "list_pull_requests", "arguments": {"owner": "ECS", "repo": "nile-hal"}}'
```

## Troubleshooting
* **Docker Errors**: Ensure Docker daemon is running and you have pulled the images. Docker Compose is **not** required.
* **Container Conflicts**: If a stale container exists, the wrapper auto-removes it. You can also run `docker rm -f mcp-wrapper-<service>-<uid>`.
* **Network Errors**: The wrapper uses `--network host` to access internal labs. Ensure you are on the VPN/network.
* **Token Errors**: Check `~/.env` for correct variable names and valid tokens.
