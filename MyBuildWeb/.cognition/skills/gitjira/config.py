"""
Service configuration for MCP wrapper.

All Docker images, environment variable mappings, container commands,
and service-specific settings are defined here — separated from logic.
"""

import os

# ---------------------------------------------------------------------------
# Paths & Timeouts
# ---------------------------------------------------------------------------
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
SOCK_DIR = "/tmp"
IDLE_TIMEOUT = 1800  # 30 minutes

# Env file: prefer local .env (user-placed), fallback to ~/.env
GLOBAL_ENV_FILE = os.path.expanduser("~/.env")
LOCAL_ENV_FILE = os.path.join(SCRIPT_DIR, ".env")
ENV_FILE = LOCAL_ENV_FILE if os.path.exists(LOCAL_ENV_FILE) else GLOBAL_ENV_FILE

# ---------------------------------------------------------------------------
# Per-service Docker configuration
# ---------------------------------------------------------------------------
# Each service entry contains:
#   image        – Docker image to pull / run
#   env_map      – {CONTAINER_VAR: ENV_FILE_VAR} mapping
#   entrypoint   – override image entrypoint (string)
#   args         – arguments passed after the image name (list)
#   volumes      – list of "-v" bind-mount strings
#   network_mode – Docker network mode
#   extra_args   – callable(env_vars) -> list of extra command args (optional)

def _gh_extra_args(env_vars):
    """Derive --gh-host from GITHUB_API_URL when present."""
    api_url = env_vars.get("GITHUB_API_URL", "")
    if api_url:
        host = api_url.replace("/api/v3", "").rstrip("/")
        if host:
            return ["--gh-host", host]
    return []

SERVICES = {
    "github": {
        "image": "ghcr.io/github/github-mcp-server:latest",
        "env_map": {
            "GITHUB_PERSONAL_ACCESS_TOKEN": "GITHUB_TOKEN",
            "GITHUB_API_URL": "GITHUB_API_URL",
        },
        "env_literals": {
            "GITHUB_TOOLSETS": "default,gists",
        },
        "entrypoint": "/server/github-mcp-server",
        "args": ["stdio"],
        "volumes": ["/etc/ssl/certs:/etc/ssl/certs:ro"],
        "network_mode": "host",
        "extra_args": _gh_extra_args,
    },
    "atlassian": {
        "image": "ghcr.io/sooperset/mcp-atlassian:latest",
        "env_map": {
            "CONFLUENCE_URL": "CONFLUENCE_URL",
            "CONFLUENCE_PERSONAL_TOKEN": "CONFLUENCE_TOKEN",
            "JIRA_URL": "JIRA_URL",
            "JIRA_PERSONAL_TOKEN": "JIRA_TOKEN",
            "JIRA_SSL_VERIFY": None,       # literal value below
            "CONFLUENCE_SSL_VERIFY": None,  # literal value below
        },
        # Literal values for env vars whose source is not the env file
        "env_literals": {
            "JIRA_SSL_VERIFY": "false",
            "CONFLUENCE_SSL_VERIFY": "false",
        },
        "entrypoint": "mcp-atlassian",
        "args": [],
        "volumes": [],
        "network_mode": "host",
    },
    "gtie_atlassian": {
        "image": "ghcr.io/sooperset/mcp-atlassian:latest",
        "env_map": {
            "CONFLUENCE_URL": "GTIE_CONFLUENCE_URL",
            "CONFLUENCE_PERSONAL_TOKEN": "GTIE_CONFLUENCE_TOKEN",
            "JIRA_URL": "JIRA_URL",
            "JIRA_PERSONAL_TOKEN": "JIRA_TOKEN",
            "JIRA_SSL_VERIFY": None,       # literal value below
            "CONFLUENCE_SSL_VERIFY": None,  # literal value below
        },
        # Literal values for env vars whose source is not the env file
        "env_literals": {
            "JIRA_SSL_VERIFY": "false",
            "CONFLUENCE_SSL_VERIFY": "false",
        },
        "entrypoint": "mcp-atlassian",
        "args": [],
        "volumes": [],
        "network_mode": "host",
    },
}

# Container name prefix (used to identify / reuse containers)
CONTAINER_PREFIX = "mcp-wrapper"
