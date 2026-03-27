# Jenkins Skill — Available Tools

## System

### `list_servers`
List all configured Jenkins servers from the .env file.

**Parameters**: none

**Returns**: `{"servers": [{"index": "1", "url": "...", "user": "..."}]}`

---

### `who_am_i`
Get information about the authenticated Jenkins user.

**Parameters**: none

---

### `get_status`
Get Jenkins system status, mode, and version.

**Parameters**: none

**Returns**: `{"mode": "NORMAL", "nodeDescription": "...", "jenkinsVersion": "2.x.x", ...}`

---

## Job Management

### `list_jobs`
List Jenkins jobs at root level or within a folder.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `folder` | string | no | Folder path (e.g. `"ECS/deploy"`). Empty = root |
| `tree` | string | no | Jenkins tree filter (default: `"jobs[name,url,color,fullName]"`) |
| `depth` | int | no | API depth (default: 1) |

---

### `get_job`
Get detailed information about a specific job.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `job_name` | string | **yes** | Job full name, supports folder paths (e.g. `"folder/job-name"`) |
| `tree` | string | no | Jenkins tree filter to limit returned fields |

---

### `get_job_config`
Get the raw XML configuration of a job.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `job_name` | string | **yes** | Job full name |
| `save_to` | string | no | File path to save the XML config to |

**Returns**: XML string (Jenkins config.xml), or `{"config_file": "...", "size_bytes": N}` if `save_to` is specified

---

### `trigger_build`
Trigger a build of a job, optionally with parameters.

⚠️ **Destructive** — always confirm with the user before executing.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `job_name` | string | **yes** | Job full name |
| `parameters` | object | no | Key-value pairs for parameterized builds (e.g. `{"BRANCH": "main", "ECS_VER": "4.3.0"}`) |

**Returns**: `{"status": "triggered", "queue_url": "...", ...}`

---

## Build Information

### `get_build`
Get build details. Defaults to last build.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `job_name` | string | **yes** | Job full name |
| `build_number` | int/string | no | Build number or `"lastBuild"` (default), `"lastSuccessfulBuild"`, `"lastFailedBuild"` |
| `tree` | string | no | Jenkins tree filter (e.g. `"number,result,duration,timestamp,builtOn"`) |

---

### `get_build_log`
Get build console output, with optional tail. Uses progressive pagination to fetch the **complete** log (no truncation). Large logs (>200 KB) are automatically saved to a temp file.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `job_name` | string | **yes** | Job full name |
| `build_number` | int/string | no | Build number or `"lastBuild"` (default) |
| `start` | int | no | Byte offset to start reading from (default: 0) |
| `tail` | int | no | Return only the last N lines |
| `save_to` | string | no | File path to save the full log to |

**Returns**:
- Small logs: `{"log": "...", "job": "...", "build": "..."}`
- Large logs (auto-saved): `{"log_file": "/tmp/...", "total_lines": N, "size_bytes": N, "tail_100": "...", "note": "..."}`
- Explicit `save_to`: `{"log_file": "...", "size_bytes": N, "total_lines": N}`

---

### `search_build_log`
Search build log for a pattern (regex supported).

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `job_name` | string | **yes** | Job full name |
| `pattern` | string | **yes** | Regex pattern to search for (e.g. `"ERROR\|FATAL"`) |
| `build_number` | int/string | no | Build number or `"lastBuild"` (default) |

**Returns**: `{"matches": [{"line_number": 42, "text": "..."}], "match_count": N}`

---

### `get_running_builds`
Get all currently running builds across all executors.

**Parameters**: none

**Returns**: `{"running_builds": [...], "count": N}`

---

### `stop_build`
Stop/abort a running build.

⚠️ **Destructive** — always confirm with the user before executing.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `job_name` | string | **yes** | Job full name |
| `build_number` | int/string | no | Build number or `"lastBuild"` (default) |

---

### `get_test_report`
Get the test report (JUnit) for a build.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `job_name` | string | **yes** | Job full name |
| `build_number` | int/string | no | Build number or `"lastBuild"` (default) |
| `save_to` | string | no | File path to save the report JSON to |

**Returns**: JSON report, or `{"report_file": "...", "size_bytes": N}` if `save_to` is specified

---

## Queue

### `get_queue`
Get the current Jenkins build queue.

**Parameters**: none

---

### `cancel_queue_item`
Cancel a queued build item.

⚠️ **Destructive** — always confirm with the user before executing.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `item_id` | int | **yes** | Queue item ID |

---

## Nodes / Agents

### `list_nodes`
List all Jenkins nodes (master + agents).

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `depth` | int | no | API depth (default: 0) |

---

### `get_node`
Get details of a specific node.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `node_name` | string | **yes** | Node name (use `"(built-in)"` or `"master"` for the built-in node) |

---

## Views

### `list_views`
List all Jenkins views.

**Parameters**: none
