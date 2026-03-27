#!/usr/bin/env python3
"""
Jenkins API Skill — lightweight CLI wrapper for Jenkins REST API.

Usage:
    python3 jenkins_api.py <tool_name> '{"param": "value"}'
    python3 jenkins_api.py --server 2 <tool_name> '{"param": "value"}'
    python3 jenkins_api.py --file /tmp/payload.json <tool_name>
    python3 jenkins_api.py list_servers

No external dependencies — stdlib only (urllib, json, ssl, base64).
"""

import sys
import os
import json
import argparse
import ssl
import base64
import re
import time
import signal
import socket
import tempfile
from urllib.request import Request, urlopen
from urllib.error import HTTPError, URLError
from urllib.parse import urlencode, quote

# ---------------------------------------------------------------------------
# Timeout
# ---------------------------------------------------------------------------
DEFAULT_TIMEOUT = 30  # seconds — total wall-clock timeout per HTTP request
LOG_FETCH_TIMEOUT = 120  # seconds — longer timeout for log fetching
LOG_SAVE_THRESHOLD = 200 * 1024  # 200 KB — auto-save to file above this


class _TimeoutError(Exception):
    pass


def _timeout_handler(signum, frame):
    raise _TimeoutError("Request timed out")

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
LOCAL_ENV_FILE = os.path.join(SCRIPT_DIR, ".env")
GLOBAL_ENV_FILE = os.path.expanduser("~/.env")
ENV_FILE = LOCAL_ENV_FILE if os.path.exists(LOCAL_ENV_FILE) else GLOBAL_ENV_FILE

# ---------------------------------------------------------------------------
# Env helpers
# ---------------------------------------------------------------------------

def _load_env():
    """Parse env file into a dict."""
    env = {}
    try:
        with open(ENV_FILE, "r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                if "=" in line:
                    k, v = line.split("=", 1)
                    env[k.strip()] = v.strip().strip('"').strip("'")
    except FileNotFoundError:
        _die(f"Env file not found: {ENV_FILE}")
    return env


def _discover_servers(env):
    """Return list of {index, url, user, token} dicts from env."""
    servers = []
    for key in sorted(env.keys()):
        m = re.match(r"^JENKINS_URL_(\d+)$", key)
        if m:
            idx = m.group(1)
            url = env.get(f"JENKINS_URL_{idx}", "").rstrip("/")
            user = env.get(f"JENKINS_USER_{idx}", "")
            token = env.get(f"JENKINS_TOKEN_{idx}", "")
            if url:
                servers.append({"index": idx, "url": url, "user": user, "token": token})
    return servers


def _get_server(env, index="1"):
    """Get a specific server config by index."""
    url = env.get(f"JENKINS_URL_{index}", "").rstrip("/")
    user = env.get(f"JENKINS_USER_{index}", "")
    token = env.get(f"JENKINS_TOKEN_{index}", "")
    if not url:
        _die(f"JENKINS_URL_{index} not found in {ENV_FILE}")
    if not user or not token:
        _die(f"JENKINS_USER_{index} or JENKINS_TOKEN_{index} missing in {ENV_FILE}")
    ssl_verify = env.get("JENKINS_SSL_VERIFY", "true").lower() not in ("false", "0", "no")
    return {"url": url, "user": user, "token": token, "ssl_verify": ssl_verify}

# ---------------------------------------------------------------------------
# HTTP helpers
# ---------------------------------------------------------------------------

def _make_request(server, path, method="GET", data=None, content_type=None, raw=False, timeout=None):
    """Make an authenticated HTTP request to Jenkins with total wall-clock timeout."""
    url = f"{server['url']}{path}"
    cred = base64.b64encode(f"{server['user']}:{server['token']}".encode()).decode()
    timeout = timeout or server.get("timeout", DEFAULT_TIMEOUT)

    headers = {"Authorization": f"Basic {cred}"}
    if content_type:
        headers["Content-Type"] = content_type

    body = None
    if data is not None:
        if isinstance(data, str):
            body = data.encode("utf-8")
        elif isinstance(data, bytes):
            body = data
        else:
            body = json.dumps(data).encode("utf-8")
            if not content_type:
                headers["Content-Type"] = "application/json"

    req = Request(url, data=body, headers=headers, method=method)

    ctx = None
    if not server.get("ssl_verify", True):
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE

    # Set total wall-clock timeout via signal.alarm (Linux)
    old_handler = signal.signal(signal.SIGALRM, _timeout_handler)
    signal.alarm(timeout)
    try:
        resp = urlopen(req, context=ctx, timeout=timeout)
        body_bytes = resp.read()
        signal.alarm(0)  # Cancel alarm on success
        if raw:
            return body_bytes.decode("utf-8", errors="replace")
        try:
            return json.loads(body_bytes)
        except (json.JSONDecodeError, ValueError):
            return body_bytes.decode("utf-8", errors="replace")
    except _TimeoutError:
        _die(f"Request timed out after {timeout}s: {url}")
    except HTTPError as e:
        signal.alarm(0)
        body_text = ""
        try:
            body_text = e.read().decode("utf-8", errors="replace")
        except Exception:
            pass
        _die(f"HTTP {e.code} {e.reason}: {url}\n{body_text}")
    except (URLError, socket.timeout) as e:
        signal.alarm(0)
        reason = getattr(e, 'reason', str(e))
        _die(f"Connection error: {reason} ({url})")
    finally:
        signal.alarm(0)
        signal.signal(signal.SIGALRM, old_handler)


def _get_crumb(server):
    """Fetch a CSRF crumb token (needed for POST requests on some Jenkins)."""
    try:
        data = _make_request(server, "/crumbIssuer/api/json")
        if isinstance(data, dict) and "crumbRequestField" in data:
            return {data["crumbRequestField"]: data["crumb"]}
    except SystemExit:
        pass
    return {}

# ---------------------------------------------------------------------------
# Utility
# ---------------------------------------------------------------------------

def _die(msg):
    print(json.dumps({"error": str(msg)}), file=sys.stdout)
    sys.exit(1)


def _output(data):
    if isinstance(data, str):
        # Try to parse as JSON for pretty print
        try:
            parsed = json.loads(data)
            print(json.dumps(parsed, indent=2, ensure_ascii=False))
        except (json.JSONDecodeError, ValueError):
            print(data)
    else:
        print(json.dumps(data, indent=2, ensure_ascii=False))


def _encode_job_path(job_name):
    """Convert 'folder/subfolder/job-name' into Jenkins URL path 'job/folder/job/subfolder/job/job-name'."""
    parts = job_name.strip("/").split("/")
    return "/".join(f"job/{quote(p, safe='')}" for p in parts)

# ===========================================================================
# TOOLS — each function takes (server, params_dict) and returns output
# ===========================================================================

# --- System ----------------------------------------------------------------

def tool_list_servers(server, params):
    """List all configured Jenkins servers."""
    env = _load_env()
    servers = _discover_servers(env)
    result = []
    for s in servers:
        result.append({
            "index": s["index"],
            "url": s["url"],
            "user": s["user"],
        })
    return {"servers": result}


def tool_who_am_i(server, params):
    """Get information about the authenticated user."""
    return _make_request(server, "/me/api/json")


def tool_get_status(server, params):
    """Get Jenkins system status and version."""
    # Use the top-level api to get basic info
    data = _make_request(server, "/api/json?tree=mode,nodeDescription,useSecurity,quietingDown")
    # Also try to get version from headers
    try:
        url = f"{server['url']}/api/json"
        cred = base64.b64encode(f"{server['user']}:{server['token']}".encode()).decode()
        req = Request(url, headers={"Authorization": f"Basic {cred}"}, method="HEAD")
        ctx = None
        if not server.get("ssl_verify", True):
            ctx = ssl.create_default_context()
            ctx.check_hostname = False
            ctx.verify_mode = ssl.CERT_NONE
        resp = urlopen(req, context=ctx, timeout=10)
        version = resp.headers.get("X-Jenkins", "unknown")
        if isinstance(data, dict):
            data["jenkinsVersion"] = version
    except Exception:
        pass
    return data

# --- Job Management --------------------------------------------------------

def tool_list_jobs(server, params):
    """List Jenkins jobs. Supports folder path."""
    folder = params.get("folder", "")
    tree = params.get("tree", "jobs[name,url,color,fullName]")
    depth = int(params.get("depth", 1))

    if folder:
        path = f"/{_encode_job_path(folder)}/api/json?tree={tree}&depth={depth}"
    else:
        path = f"/api/json?tree={tree}&depth={depth}"
    return _make_request(server, path)


def tool_get_job(server, params):
    """Get detailed information about a specific job."""
    job_name = params.get("job_name")
    if not job_name:
        _die("'job_name' is required")
    tree = params.get("tree", "")
    path = f"/{_encode_job_path(job_name)}/api/json"
    if tree:
        path += f"?tree={tree}"
    return _make_request(server, path)


def tool_get_job_config(server, params):
    """Get job XML configuration. Supports save_to file."""
    job_name = params.get("job_name")
    if not job_name:
        _die("'job_name' is required")
    save_to = params.get("save_to", "")
    text = _make_request(server, f"/{_encode_job_path(job_name)}/config.xml", raw=True)
    if save_to:
        with open(save_to, "w") as f:
            f.write(text)
        return {"config_file": save_to, "size_bytes": len(text), "job": job_name}
    return text


def tool_trigger_build(server, params):
    """Trigger a build with optional parameters."""
    job_name = params.get("job_name")
    if not job_name:
        _die("'job_name' is required")

    build_params = params.get("parameters", {})
    crumb = _get_crumb(server)

    if build_params:
        # Use buildWithParameters
        query = urlencode(build_params)
        path = f"/{_encode_job_path(job_name)}/buildWithParameters?{query}"
    else:
        path = f"/{_encode_job_path(job_name)}/build"

    # POST with crumb headers
    url = f"{server['url']}{path}"
    cred = base64.b64encode(f"{server['user']}:{server['token']}".encode()).decode()
    headers = {"Authorization": f"Basic {cred}"}
    headers.update(crumb)

    req = Request(url, data=b"", headers=headers, method="POST")

    ctx = None
    if not server.get("ssl_verify", True):
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE

    try:
        resp = urlopen(req, context=ctx, timeout=30)
        location = resp.headers.get("Location", "")
        return {
            "status": "triggered",
            "job": job_name,
            "parameters": build_params,
            "queue_url": location,
            "http_status": resp.status,
        }
    except HTTPError as e:
        body_text = ""
        try:
            body_text = e.read().decode("utf-8", errors="replace")
        except Exception:
            pass
        _die(f"Failed to trigger build: HTTP {e.code} {e.reason}\n{body_text}")

# --- Build Information -----------------------------------------------------

# Default tree filter for get_build — keeps response small and fast
_BUILD_DEFAULT_TREE = (
    "number,result,duration,timestamp,builtOn,building,estimatedDuration,"
    "url,fullDisplayName,displayName,description,"
    "actions[parameters[name,value],causes[shortDescription,userId,userName]],"
    "changeSets[items[msg,author[fullName]]]"
)


def tool_get_build(server, params):
    """Get build details. Use build_number='lastBuild' for the latest."""
    job_name = params.get("job_name")
    if not job_name:
        _die("'job_name' is required")
    build_number = str(params.get("build_number", "lastBuild"))
    tree = params.get("tree", _BUILD_DEFAULT_TREE)
    path = f"/{_encode_job_path(job_name)}/{build_number}/api/json"
    # tree="*" or tree="all" means no filter (fetch everything)
    if tree and tree not in ("*", "all"):
        path += f"?tree={tree}"
    return _make_request(server, path)


def _fetch_full_log(server, job_name, build_number, start=0):
    """Fetch the complete console log using progressiveText with pagination.

    Loops using X-Text-Size / X-More-Data headers so that large or
    in-progress logs are never silently truncated.
    """
    job_path = _encode_job_path(job_name)
    timeout = max(server.get("timeout", DEFAULT_TIMEOUT), LOG_FETCH_TIMEOUT)

    cred = base64.b64encode(f"{server['user']}:{server['token']}".encode()).decode()
    ctx = None
    if not server.get("ssl_verify", True):
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE

    chunks = []
    offset = int(start)
    max_iterations = 200  # safety cap to avoid infinite loops

    for _ in range(max_iterations):
        url = f"{server['url']}/{job_path}/{build_number}/logText/progressiveText?start={offset}"
        req = Request(url, headers={"Authorization": f"Basic {cred}"}, method="GET")

        old_handler = signal.signal(signal.SIGALRM, _timeout_handler)
        signal.alarm(timeout)
        try:
            resp = urlopen(req, context=ctx, timeout=timeout)
            chunk = resp.read().decode("utf-8", errors="replace")
            x_text_size = int(resp.headers.get("X-Text-Size", str(offset)))
            x_more_data = (resp.headers.get("X-More-Data") or "").lower() == "true"
            signal.alarm(0)
        except _TimeoutError:
            chunks.append(
                f"\n[WARN] Log fetch timed out after {timeout}s at offset {offset}. "
                "Log may be incomplete.\n"
            )
            break
        except HTTPError as e:
            signal.alarm(0)
            body_text = ""
            try:
                body_text = e.read().decode("utf-8", errors="replace")
            except Exception:
                pass
            _die(f"HTTP {e.code} {e.reason} fetching log\n{body_text}")
        except (URLError, socket.timeout) as e:
            signal.alarm(0)
            reason = getattr(e, 'reason', str(e))
            _die(f"Connection error fetching log: {reason}")
        finally:
            signal.alarm(0)
            signal.signal(signal.SIGALRM, old_handler)

        if chunk:
            chunks.append(chunk)

        # Stop if no more data or offset didn't advance
        if not x_more_data or x_text_size <= offset:
            break
        offset = x_text_size

    return "".join(chunks)


def _save_log_to_file(text, job_name, build_number, save_to=None):
    """Save log text to a file. Returns the file path."""
    if save_to:
        path = save_to
    else:
        safe_name = job_name.replace("/", "_")
        fd, path = tempfile.mkstemp(
            prefix=f"jenkins_log_{safe_name}_{build_number}_", suffix=".txt"
        )
        os.close(fd)
    with open(path, "w") as f:
        f.write(text)
    return path


def tool_get_build_log(server, params):
    """Get build console output. Supports tail, save_to file, and auto-save for large logs."""
    job_name = params.get("job_name")
    if not job_name:
        _die("'job_name' is required")
    build_number = str(params.get("build_number", "lastBuild"))
    start = params.get("start", 0)
    save_to = params.get("save_to", "")

    text = _fetch_full_log(server, job_name, build_number, start=start)

    # Optionally tail last N lines
    tail = params.get("tail")
    if tail:
        lines = text.strip().split("\n")
        text = "\n".join(lines[-int(tail):])

    # Save to file if explicitly requested
    if save_to:
        path = _save_log_to_file(text, job_name, build_number, save_to=save_to)
        return {
            "log_file": path,
            "size_bytes": len(text),
            "total_lines": text.count("\n") + 1,
            "job": job_name,
            "build": build_number,
        }

    # Auto-save large logs (unless already tailed)
    if not tail and len(text) > LOG_SAVE_THRESHOLD:
        path = _save_log_to_file(text, job_name, build_number)
        lines = text.strip().split("\n")
        total_lines = len(lines)
        tail_text = "\n".join(lines[-100:])
        return {
            "log_file": path,
            "total_lines": total_lines,
            "size_bytes": len(text),
            "tail_100": tail_text,
            "job": job_name,
            "build": build_number,
            "note": (
                f"Full log saved to {path} ({total_lines} lines, {len(text)} bytes). "
                "Showing last 100 lines in 'tail_100'. "
                "Use 'tail' param or read the file for more."
            ),
        }

    return {"log": text, "job": job_name, "build": build_number}


def tool_search_build_log(server, params):
    """Search build log for a pattern (regex supported)."""
    job_name = params.get("job_name")
    if not job_name:
        _die("'job_name' is required")
    pattern = params.get("pattern")
    if not pattern:
        _die("'pattern' is required")
    build_number = str(params.get("build_number", "lastBuild"))

    text = _fetch_full_log(server, job_name, build_number)

    try:
        regex = re.compile(pattern, re.IGNORECASE)
    except re.error as e:
        _die(f"Invalid regex pattern: {e}")

    matches = []
    for i, line in enumerate(text.split("\n"), 1):
        if regex.search(line):
            matches.append({"line_number": i, "text": line.strip()})

    return {
        "job": job_name,
        "build": build_number,
        "pattern": pattern,
        "match_count": len(matches),
        "matches": matches[:200],  # Cap at 200 to avoid huge output
    }


def tool_get_running_builds(server, params):
    """Get all currently running builds across all executors."""
    data = _make_request(
        server,
        "/computer/api/json?tree=computer[displayName,executors[currentExecutable[url,number,fullDisplayName,timestamp,building]],oneOffExecutors[currentExecutable[url,number,fullDisplayName,timestamp,building]]]"
    )
    running = []
    if isinstance(data, dict) and "computer" in data:
        for node in data["computer"]:
            node_name = node.get("displayName", "unknown")
            for executor_group in ("executors", "oneOffExecutors"):
                for executor in node.get(executor_group, []):
                    exe = executor.get("currentExecutable")
                    if exe and exe.get("building"):
                        exe["node"] = node_name
                        running.append(exe)
    return {"running_builds": running, "count": len(running)}


def tool_stop_build(server, params):
    """Stop/abort a running build."""
    job_name = params.get("job_name")
    if not job_name:
        _die("'job_name' is required")
    build_number = str(params.get("build_number", "lastBuild"))
    crumb = _get_crumb(server)

    url = f"{server['url']}/{_encode_job_path(job_name)}/{build_number}/stop"
    cred = base64.b64encode(f"{server['user']}:{server['token']}".encode()).decode()
    headers = {"Authorization": f"Basic {cred}"}
    headers.update(crumb)

    req = Request(url, data=b"", headers=headers, method="POST")
    ctx = None
    if not server.get("ssl_verify", True):
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE

    try:
        urlopen(req, context=ctx, timeout=30)
        return {"status": "stopped", "job": job_name, "build": build_number}
    except HTTPError as e:
        body_text = ""
        try:
            body_text = e.read().decode("utf-8", errors="replace")
        except Exception:
            pass
        _die(f"Failed to stop build: HTTP {e.code}\n{body_text}")


def tool_get_test_report(server, params):
    """Get test report for a build. Supports save_to file."""
    job_name = params.get("job_name")
    if not job_name:
        _die("'job_name' is required")
    build_number = str(params.get("build_number", "lastBuild"))
    save_to = params.get("save_to", "")
    path = f"/{_encode_job_path(job_name)}/{build_number}/testReport/api/json"
    data = _make_request(server, path)
    if save_to:
        with open(save_to, "w") as f:
            if isinstance(data, str):
                f.write(data)
            else:
                json.dump(data, f, indent=2, ensure_ascii=False)
        size = os.path.getsize(save_to)
        return {"report_file": save_to, "size_bytes": size, "job": job_name, "build": build_number}
    return data

# --- Queue -----------------------------------------------------------------

def tool_get_queue(server, params):
    """Get the current build queue."""
    return _make_request(server, "/queue/api/json")


def tool_cancel_queue_item(server, params):
    """Cancel a queued build by queue item ID."""
    item_id = params.get("item_id")
    if not item_id:
        _die("'item_id' is required")
    crumb = _get_crumb(server)

    url = f"{server['url']}/queue/cancelItem?id={item_id}"
    cred = base64.b64encode(f"{server['user']}:{server['token']}".encode()).decode()
    headers = {"Authorization": f"Basic {cred}"}
    headers.update(crumb)

    req = Request(url, data=b"", headers=headers, method="POST")
    ctx = None
    if not server.get("ssl_verify", True):
        ctx = ssl.create_default_context()
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
    try:
        urlopen(req, context=ctx, timeout=30)
        return {"status": "cancelled", "item_id": item_id}
    except HTTPError as e:
        body_text = ""
        try:
            body_text = e.read().decode("utf-8", errors="replace")
        except Exception:
            pass
        _die(f"Failed to cancel queue item: HTTP {e.code}\n{body_text}")

# --- Nodes / Agents --------------------------------------------------------

def tool_list_nodes(server, params):
    """List all Jenkins nodes (agents)."""
    depth = int(params.get("depth", 0))
    return _make_request(
        server,
        f"/computer/api/json?depth={depth}"
    )


def tool_get_node(server, params):
    """Get details of a specific node."""
    node_name = params.get("node_name")
    if not node_name:
        _die("'node_name' is required")
    if node_name.lower() in ("master", "built-in", "(built-in)"):
        node_name = "(built-in)"
    encoded = quote(node_name, safe="()")
    return _make_request(server, f"/computer/{encoded}/api/json")

# --- Views -----------------------------------------------------------------

def tool_list_views(server, params):
    """List all views."""
    return _make_request(server, "/api/json?tree=views[name,url,description]")

# ===========================================================================
# Tool registry
# ===========================================================================

TOOLS = {
    # System
    "list_servers":       tool_list_servers,
    "who_am_i":           tool_who_am_i,
    "get_status":         tool_get_status,
    # Job Management
    "list_jobs":          tool_list_jobs,
    "get_job":            tool_get_job,
    "get_job_config":     tool_get_job_config,
    "trigger_build":      tool_trigger_build,
    # Build Information
    "get_build":          tool_get_build,
    "get_build_log":      tool_get_build_log,
    "search_build_log":   tool_search_build_log,
    "get_running_builds": tool_get_running_builds,
    "stop_build":         tool_stop_build,
    "get_test_report":    tool_get_test_report,
    # Queue
    "get_queue":          tool_get_queue,
    "cancel_queue_item":  tool_cancel_queue_item,
    # Nodes
    "list_nodes":         tool_list_nodes,
    "get_node":           tool_get_node,
    # Views
    "list_views":         tool_list_views,
}


def list_tools():
    """Print available tools and their docstrings."""
    result = {}
    for name, fn in TOOLS.items():
        result[name] = (fn.__doc__ or "").strip()
    return result

# ===========================================================================
# CLI entry point
# ===========================================================================

def main():
    parser = argparse.ArgumentParser(
        description="Jenkins API Skill — CLI wrapper for Jenkins REST API",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Examples:\n"
               "  python3 jenkins_api.py list_tools\n"
               "  python3 jenkins_api.py list_servers\n"
               '  python3 jenkins_api.py get_job \'{"job_name": "my-folder/my-job"}\'\n'
               '  python3 jenkins_api.py --server 2 get_build \'{"job_name": "deploy", "build_number": 42}\'\n'
               '  python3 jenkins_api.py trigger_build --file /tmp/params.json\n',
    )
    parser.add_argument("--server", "-s", default="1",
                        help="Jenkins server index (default: 1)")
    parser.add_argument("--timeout", "-t", type=int, default=None,
                        help=f"Total timeout in seconds (default: {DEFAULT_TIMEOUT})")
    parser.add_argument("--file", "-f",
                        help="Read JSON params from file instead of CLI arg")
    parser.add_argument("tool", help="Tool name to invoke (or 'list_tools')")
    parser.add_argument("params", nargs="?", default="{}",
                        help="JSON string of parameters")

    args = parser.parse_args()

    # Special: list_tools (no server needed)
    if args.tool == "list_tools":
        _output(list_tools())
        return

    # Load env and get server config
    env = _load_env()

    # list_servers is special — doesn't need a valid server
    if args.tool == "list_servers":
        result = tool_list_servers(None, {})
        _output(result)
        return

    server = _get_server(env, args.server)

    # Apply CLI timeout override
    if args.timeout is not None:
        server["timeout"] = args.timeout

    # Parse params
    if args.file:
        with open(args.file, "r") as f:
            params = json.load(f)
    else:
        try:
            params = json.loads(args.params)
        except json.JSONDecodeError as e:
            _die(f"Invalid JSON params: {e}")

    # Find and call tool
    if args.tool not in TOOLS:
        _die(f"Unknown tool: {args.tool}. Run 'list_tools' to see available tools.")

    result = TOOLS[args.tool](server, params)
    _output(result)


if __name__ == "__main__":
    main()
