#!/usr/bin/env python3
import sys
import json
import subprocess
import os
import argparse
import socket
import time
import threading
import signal
import atexit

# Import configuration (images, env mappings, commands, etc.)
from config import (
    SERVICES, SOCK_DIR, IDLE_TIMEOUT, ENV_FILE, CONTAINER_PREFIX,
)

def get_socket_path(service):
    # Unique socket per user and service
    uid = os.getuid()
    return os.path.join(SOCK_DIR, f"mcp-wrapper-{service}-{uid}.sock")

def _load_env_vars():
    """Parse ~/.env into a dict."""
    env = {}
    try:
        with open(ENV_FILE, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                if '=' in line:
                    k, v = line.split('=', 1)
                    env[k.strip()] = v.strip()
    except FileNotFoundError:
        print(f"[Wrapper] Env file not found: {ENV_FILE}", file=sys.stderr)
    return env

def _container_name(service):
    uid = os.getuid()
    return f"{CONTAINER_PREFIX}-{service}-{uid}"

def _build_docker_run_cmd(service):
    """Build a `docker run` command list for the given service."""
    cfg = SERVICES[service]
    env_vars = _load_env_vars()
    name = _container_name(service)

    cmd = [
        "docker", "run", "--rm", "-i",
        "--name", name,
    ]

    # Network mode
    if cfg.get("network_mode"):
        cmd += ["--network", cfg["network_mode"]]

    # Volumes
    for vol in cfg.get("volumes", []):
        cmd += ["-v", vol]

    # Environment variables – mapped from env file
    missing_vars = []
    for container_var, env_file_var in cfg.get("env_map", {}).items():
        if env_file_var is None:
            continue  # handled by env_literals
        value = env_vars.get(env_file_var, "").strip()
        
        # Check if the environment variable has "TOKEN" in its name and is empty
        if not value and "TOKEN" in env_file_var.upper():
            missing_vars.append(env_file_var)
            
        cmd += ["-e", f"{container_var}={value}"]

    if missing_vars:
        print(f"Error: Missing required Personal Access Token(s) in {ENV_FILE}: {', '.join(missing_vars)}", file=sys.stderr)
        print(f"Please add these variables to {ENV_FILE} before running.", file=sys.stderr)
        sys.exit(1)

    # Environment variables – literal values
    for container_var, value in cfg.get("env_literals", {}).items():
        cmd += ["-e", f"{container_var}={value}"]

    # Entrypoint override
    if cfg.get("entrypoint"):
        cmd += ["--entrypoint", cfg["entrypoint"]]

    # Image
    cmd.append(cfg["image"])

    # Args (passed after image name)
    cmd += cfg.get("args", [])

    # Extra args (e.g. --gh-host)
    extra_fn = cfg.get("extra_args")
    if callable(extra_fn):
        cmd += extra_fn(env_vars)

    return cmd

def _stop_container(service):
    """Stop and remove a running container for the service (best-effort)."""
    name = _container_name(service)
    subprocess.run(
        ["docker", "rm", "-f", name],
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL
    )

class MCPDaemon:
    def __init__(self, service):
        self.service = service
        self.sock_path = get_socket_path(service)
        self.process = None
        self.last_activity = time.time()
        self.running = True
        self.lock = threading.Lock() # Ensure serial access to Docker process

    def _is_process_alive(self):
        """Check if the Docker subprocess is still running."""
        return self.process is not None and self.process.poll() is None

    def _start_docker(self):
        """Start (or restart) the Docker container and perform handshake."""
        _stop_container(self.service)
        cmd = _build_docker_run_cmd(self.service)
        self.process = subprocess.Popen(
            cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=sys.stderr,
            universal_newlines=True,
            bufsize=0
        )
        if not self._handshake():
            raise RuntimeError("Handshake failed after (re)starting container")

    def run(self):
        if self.service not in SERVICES:
            print(f"[Daemon] Unknown service: {self.service}", file=sys.stderr)
            return

        # 1. Start MCP service via `docker run`
        try:
            self._start_docker()
        except Exception as e:
            print(f"[Daemon] Failed to start container: {e}", file=sys.stderr)
            self.cleanup()
            return

        # 2. Start Socket Server
        if os.path.exists(self.sock_path):
            try:
                os.remove(self.sock_path)
            except OSError:
                pass
        
        server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        server.bind(self.sock_path)
        server.listen(1)
        server.settimeout(1.0) # Check for idle/shutdown every second
        
        while self.running:
            try:
                # Check idle timeout
                if time.time() - self.last_activity > IDLE_TIMEOUT:
                    print(f"[Daemon] Idle timeout reached, shutting down", file=sys.stderr)
                    break

                # If Docker process died (e.g. docker stop), exit cleanly.
                # The client will spawn a fresh daemon on the next request.
                if not self._is_process_alive():
                    print(f"[Daemon] Docker process died, exiting", file=sys.stderr)
                    break

                try:
                    conn, _ = server.accept()
                except socket.timeout:
                    continue
                
                self.last_activity = time.time()
                threading.Thread(target=self._handle_client, args=(conn,)).start()

            except Exception as e:
                print(f"[Daemon] Error in loop: {e}", file=sys.stderr)
                break
        
        self.cleanup()
        server.close()
        if os.path.exists(self.sock_path):
            try:
                os.remove(self.sock_path)
            except OSError:
                pass

    def _handshake(self):
        try:
            init_req = {
                "jsonrpc": "2.0",
                "method": "initialize",
                "id": 0,
                "params": {
                    "protocolVersion": "2024-11-05",
                    "capabilities": {},
                    "clientInfo": {"name": "windsurf-cli", "version": "1.0"}
                }
            }
            self.process.stdin.write(json.dumps(init_req) + "\n")
            self.process.stdin.flush()

            # Read initialize response
            while True:
                line = self.process.stdout.readline()
                if not line: return False
                try:
                    resp = json.loads(line)
                    if resp.get("id") == 0:
                        break
                except: continue

            # Send initialized notification
            self.process.stdin.write(json.dumps({
                "jsonrpc": "2.0",
                "method": "notifications/initialized"
            }) + "\n")
            self.process.stdin.flush()
            return True
        except Exception as e:
            print(f"[Daemon] Handshake failed: {e}", file=sys.stderr)
            return False

    def _handle_client(self, conn):
        with self.lock: # Serial processing to Docker stdin
            try:
                f = conn.makefile('rw')
                line = f.readline()
                if not line:
                    conn.close()
                    return
                
                try:
                    req = json.loads(line)
                except json.JSONDecodeError:
                    conn.close()
                    return

                req_id = req.get("id")

                # Forward to Docker
                self.process.stdin.write(json.dumps(req) + "\n")
                self.process.stdin.flush()

                # Read from Docker until we get response with matching ID
                while True:
                    d_line = self.process.stdout.readline()
                    if not d_line:
                        # Docker process EOF — return error to client
                        error_resp = {"jsonrpc": "2.0", "id": req_id,
                                      "error": {"code": -1, "message": "Docker process terminated unexpectedly"}}
                        f.write(json.dumps(error_resp) + "\n")
                        f.flush()
                        break
                    try:
                        resp = json.loads(d_line)
                        if resp.get("id") == req_id:
                            f.write(json.dumps(resp) + "\n")
                            f.flush()
                            break
                        # Ignore other messages (logs/notifications)
                    except:
                        continue
            except Exception as e:
                print(f"[Daemon] Client handling error: {e}", file=sys.stderr)
            finally:
                conn.close()

    def cleanup(self):
        self.running = False
        if self.process:
            # Terminate docker run process
            try:
                self.process.stdin.close()
            except Exception:
                pass
            self.process.terminate()
            try:
                self.process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                self.process.kill()
        # Stop the container (--rm will auto-remove, but force-clean just in case)
        _stop_container(self.service)

class MCPClient:
    def __init__(self, service):
        self.service = service
        self.sock_path = get_socket_path(service)

    def run(self, method, params_dict=None):
        if not self._ensure_daemon():
            print(json.dumps({"error": "Failed to connect to daemon"}))
            sys.exit(1)
            
        try:
            client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
            client.connect(self.sock_path)
            
            req_id = int(time.time() * 1000)
            req = {
                "jsonrpc": "2.0",
                "method": method,
                "id": req_id
            }
            if params_dict:
                req["params"] = params_dict
            
            client.sendall((json.dumps(req) + "\n").encode('utf-8'))
            
            f = client.makefile('r')
            resp_line = f.readline()
            if resp_line:
                try:
                    resp = json.loads(resp_line)
                    
                    # Auto-Unwrap logic for tools/call
                    if method == "tools/call" and "result" in resp and "content" in resp["result"]:
                        content = resp["result"]["content"]
                        if isinstance(content, list) and len(content) > 0:
                            for item in content:
                                if item.get("type") == "text" and "text" in item:
                                    raw_text = item["text"]
                                    # Try to parse the inner text as JSON
                                    try:
                                        inner_json = json.loads(raw_text)
                                        print(json.dumps(inner_json, indent=2))
                                    except json.JSONDecodeError:
                                        # If it's not JSON, just print the text
                                        print(raw_text)
                                elif item.get("type") == "resource" and "resource" in item and "text" in item["resource"]:
                                    print(item["resource"]["text"])
                        else:
                            print(json.dumps(resp["result"], indent=2))
                    # For tools/list, just print the result nicely
                    elif method == "tools/list" and "result" in resp:
                        print(json.dumps(resp["result"], indent=2))
                    elif "error" in resp:
                        print(json.dumps(resp["error"], indent=2))
                    else:
                        # Pass through other responses nicely formatted
                        print(json.dumps(resp, indent=2))
                except json.JSONDecodeError:
                    print(resp_line.strip())
            
            client.close()
        except Exception as e:
            print(json.dumps({"error": str(e)}))
            sys.exit(1)

    def _ensure_daemon(self):
        # 1. Check if socket exists and is responsive
        if os.path.exists(self.sock_path):
            try:
                s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
                s.connect(self.sock_path)
                s.close()
                return True
            except:
                # Stale socket
                try: os.remove(self.sock_path)
                except: pass

        # 2. Start daemon
        try:
            # We must use subprocess.Popen to detach completely
            # Using sys.executable to run the same script
            # setsid to detach from TTY
            cmd = [sys.executable, os.path.abspath(__file__), "daemon", "--service", self.service]
            
            # Use preexec_fn=os.setsid to ensure it doesn't get killed when parent exits
            # Redirect all stdio to devnull to avoid hanging
            subprocess.Popen(
                cmd,
                start_new_session=True,
                stdin=subprocess.DEVNULL,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL
            )
            
            # 3. Wait for socket
            start = time.time()
            while time.time() - start < 10: # Wait up to 10s for slow Docker start
                if os.path.exists(self.sock_path):
                    try:
                        s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
                        s.connect(self.sock_path)
                        s.close()
                        return True
                    except:
                        time.sleep(0.1)
                        continue
                time.sleep(0.1)
        except Exception as e:
            # Debug info to stderr, but don't crash main output
            print(f"DEBUG: Failed to spawn daemon: {e}", file=sys.stderr)
        
        return False

def _ensure_schema_exists():
    """Auto-generate AVAILABLE_TOOLS.md on first run if it doesn't exist."""
    if os.environ.get("MCP_NO_AUTO_GEN") == "1":
        return
        
    script_dir = os.path.dirname(os.path.abspath(__file__))
    schema_file = os.path.join(script_dir, "AVAILABLE_TOOLS.md")
    
    if not os.path.exists(schema_file):
        gen_script = os.path.join(script_dir, "generate_schema.py")
        if os.path.exists(gen_script):
            print("[Wrapper] First run detected. Auto-generating AVAILABLE_TOOLS.md...", file=sys.stderr)
            # Run in background to not block the current request too much, 
            # or just run it synchronously since it's a one-time cost. Let's do sync to be safe.
            try:
                subprocess.run([sys.executable, gen_script], check=True, stdout=subprocess.DEVNULL, stderr=sys.stderr)
                print("[Wrapper] Schema generation complete.", file=sys.stderr)
            except Exception as e:
                print(f"[Wrapper] Warning: Failed to auto-generate schema: {e}", file=sys.stderr)

def main():
    _ensure_schema_exists()
    parser = argparse.ArgumentParser(description="MCP Client Wrapper with Daemon")
    subparsers = parser.add_subparsers(dest="command")
    
    # Daemon command (internal use)
    d_parser = subparsers.add_parser("daemon")
    d_parser.add_argument("--service", choices=list(SERVICES.keys()), required=True)

    # Client commands
    for service in SERVICES:
        s_parser = subparsers.add_parser(service)
        s_subparsers = s_parser.add_subparsers(dest="action")
        
        # Action: list
        l_parser = s_subparsers.add_parser("list", help="List available tools")
        
        # Action: call
        c_parser = s_subparsers.add_parser("call", help="Call a tool")
        c_parser.add_argument("tool_name", help="Name of the tool to call")
        
        group = c_parser.add_mutually_exclusive_group()
        group.add_argument("params", nargs="?", help="JSON string of arguments for the tool")
        group.add_argument("--file", help="Path to a JSON file containing arguments")

    args = parser.parse_args()

    if args.command == "daemon":
        daemon = MCPDaemon(args.service)
        daemon.run()
    elif args.command in SERVICES:
        client = MCPClient(args.command)
        
        if not getattr(args, 'action', None):
            # Fallback if no action provided
            client.run("tools/list")
            return
            
        if args.action == "list":
            client.run("tools/list")
        elif args.action == "call":
            params_dict = {}
            if getattr(args, 'file', None):
                with open(args.file, 'r') as f:
                    params_dict = json.load(f)
            elif getattr(args, 'params', None):
                params_dict = json.loads(args.params)
                
            # Wrap in the MCP format for tools/call
            mcp_params = {
                "name": args.tool_name,
                "arguments": params_dict
            }
            client.run("tools/call", mcp_params)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
