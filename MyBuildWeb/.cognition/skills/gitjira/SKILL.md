---
name: "gitjira"
description: "GitHub, Jira, and Confluence integration for Windsurf IDE. MANDATORY: You MUST use this skill for ALL interactions with these services. DO NOT use manual API calls (curl/wget)."
---

**ALWAYS** use `python3 path/to/gitjira/mcp_wrapper.py` for GitHub, Jira, and Confluence. **DO NOT** use `curl`, `wget`, or raw API calls — the wrapper handles authentication, SSL, and Docker networking.

## Architecture

Plain `docker run` + a client-daemon model (no Docker Compose required):
- First call spawns a Python Daemon that starts an MCP server in a Docker container via `docker run`.
- Subsequent calls connect via Unix socket — **instant** (milliseconds).
- Daemon auto-shuts down after **30 min** idle; container is automatically removed.
- Service configuration (images, env maps, commands) lives in `config.py`, separated from logic.

## Syntax

```bash
# Inline JSON payload
python3 path/to/gitjira/mcp_wrapper.py <SERVICE> call <TOOL_NAME> '{"arg": "value"}'

# File payload (recommended for text content)
python3 path/to/gitjira/mcp_wrapper.py <SERVICE> call <TOOL_NAME> --file /tmp/payload.json
```

- **SERVICE**: `github`, `atlassian`, or `gtie_atlassian`
- **TOOL_NAME**: e.g. `jira_get_issue`, `list_pull_requests`

## Routing Rules for LLM
When deciding which SERVICE to use for Confluence:
- **Default (`atlassian`)**: Use this for ALL Jira and Confluence requests by default (e.g., CEC Lab endpoints, `confluence.cec.lab.emc.com`, generic searches).
- **Exception (`gtie_atlassian`)**: ONLY use this if the user provides a link explicitly containing `gtie.dell.com` or specifically requests "GTIE".
- *(Note: Both Atlassian services share the exact same tool schema, as they use the same underlying Docker image.)*

## Tool Discovery

Cached tool schemas: **`AVAILABLE_TOOLS.md`** (read this first). 
*(Note: If the file is missing, it will be auto-generated the first time you execute any `mcp_wrapper.py` command.)*

Dynamic query (always up-to-date):
```bash
python3 path/to/gitjira/mcp_wrapper.py <SERVICE> list
```

### Cache Refresh
If you encounter "Tool not found" or "Invalid parameters", the schema cache may be outdated. Force a refresh:
```bash
python3 path/to/gitjira/generate_schema.py
```

## Examples

```bash
# GitHub
python3 path/to/gitjira/mcp_wrapper.py github call list_pull_requests '{"owner": "powerscale-misc", "repo": "skills"}'
python3 path/to/gitjira/mcp_wrapper.py github call pull_request_read '{"owner": "powerscale-misc", "repo": "skills", "pullNumber": 1, "method": "get"}'
python3 path/to/gitjira/mcp_wrapper.py github call get_gist '{"gist_id": "a1b2c3d4e5f60718293a4b5c6d7e8f90"}'
python3 path/to/gitjira/mcp_wrapper.py github call list_gists '{"username": "octocat"}'

# Atlassian
python3 path/to/gitjira/mcp_wrapper.py atlassian call jira_get_issue '{"issue_key": "OBS06A-104"}'
python3 path/to/gitjira/mcp_wrapper.py atlassian call confluence_search '{"query": "text ~ \"meeting\""}'
```

## Best Practices

### Jira Formatting
Jira uses **Atlassian Wiki Markup (Textile)**, NOT Markdown:
- Code: `{code:lang}...{code}` or `{noformat}...{noformat}` (no triple backticks)
- Headers: `h1.`, `h2.` (not `#`, `##`)
- Lists: `#` numbered, `*` bulleted
- Prefer `{noformat}` for file contents/logs to avoid parser errors.

### Complex Payloads
For text content (comments, descriptions, PR bodies), use `--file` instead of inline JSON:
```bash
cat << 'EOF' > /tmp/payload.json
{"issue_key": "OBS-123", "body": "It's a bug\nWith multiple lines"}
EOF
python3 path/to/gitjira/mcp_wrapper.py atlassian call jira_add_comment --file /tmp/payload.json
rm /tmp/payload.json
```

### Output
The wrapper **auto-unwraps** MCP JSON-RPC — output is plain JSON/text. No need to parse `result.content[0].text`.
