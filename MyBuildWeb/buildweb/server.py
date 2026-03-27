#!/usr/bin/env python3
"""
MyBuildWeb - A simple build dashboard inspired by Isilon Build Web.
Displays build history with metadata and artifact download links.
Uses only Python stdlib (no pip install needed).
"""

import http.server
import json
import os
import socketserver
import urllib.parse
from datetime import datetime, timezone
from pathlib import Path

BUILDS_DIR = os.environ.get("BUILDS_DIR", "/var/jenkins_home/buildweb_data")
PORT = int(os.environ.get("BUILDWEB_PORT", "8081"))

HTML_TEMPLATE = """<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>MyBuildWeb - {branch}</title>
  <style>
    * {{ box-sizing: border-box; margin: 0; padding: 0; }}
    body {{ font-family: "Helvetica Neue", Helvetica, Arial, sans-serif; font-size: 14px; color: #333; background: #f5f5f5; }}
    .container {{ max-width: 1200px; margin: 0 auto; padding: 15px; }}
    #heading h1 {{ margin: 10px 0; }}
    #heading h1 a {{ color: #333; text-decoration: none; }}
    #heading h1 span {{ font-weight: bold; }}
    .navbar-style {{ margin-bottom: 20px; }}
    nav {{ background: #f8f8f8; border: 1px solid #e7e7e7; border-radius: 4px; padding: 8px 15px; }}
    nav a {{ color: #555; text-decoration: none; margin-right: 15px; font-size: 14px; }}
    nav a:hover {{ color: #333; text-decoration: underline; }}
    .nav-right {{ float: right; color: #777; }}
    .panel {{ background: #fff; border: 1px solid #ddd; border-radius: 4px; margin-bottom: 12px; padding: 0; }}
    .succeeded_background_color {{ background-color: #DFF0D8; }}
    .failed_background_color {{ background-color: #F2DEDE; }}
    .running_background_color {{ background-color: #C9FCF9; }}
    .build_info table {{ border-collapse: collapse; width: 100%; margin: 4px 0; }}
    .build_info td {{ padding: 4px 8px; font-size: 13px; }}
    .build_info td b {{ white-space: nowrap; }}
    a.rolly {{ color: darkblue; text-decoration: none; cursor: pointer; }}
    a.rolly:hover {{ text-decoration: underline; }}
    .download-section {{ padding: 8px 8px 12px; border-top: 1px solid #ddd; background: #fafafa; }}
    .download-section a {{ display: inline-block; margin-right: 15px; padding: 4px 12px; background: #5cb85c; color: #fff; border-radius: 3px; text-decoration: none; font-size: 13px; }}
    .download-section a:hover {{ background: #449d44; }}
    .download-section a.tar {{ background: #337ab7; }}
    .download-section a.tar:hover {{ background: #286090; }}
    .empty {{ text-align: center; padding: 40px; color: #999; }}
    .badge {{ display: inline-block; padding: 2px 8px; border-radius: 3px; color: #fff; font-size: 12px; font-weight: bold; }}
    .badge-success {{ background: #5cb85c; }}
    .badge-danger {{ background: #d9534f; }}
    .badge-info {{ background: #5bc0de; }}
    .summary {{ margin-bottom: 15px; padding: 10px; background: #fff; border: 1px solid #ddd; border-radius: 4px; }}
    .summary span {{ margin-right: 20px; }}
    h4 {{ margin: 5px 8px; padding-top: 6px; color: #555; }}
  </style>
</head>
<body>
<div class="container">
  <div id="heading">
    <h1><a href="/"><span>MyBuildWeb</span></a></h1>
  </div>
  <div class="navbar-style">
    <nav>
      <a href="/">Build History</a>
      <a href="/api/builds">API (JSON)</a>
      <span class="nav-right">MyBuildWeb v1.0 | Builds: {total_builds}</span>
    </nav>
  </div>
  <div class="summary">
    <span><b>Branch:</b> {branch}</span>
    <span><b>Total Builds:</b> {total_builds}</span>
    <span><b>Succeeded:</b> <span class="badge badge-success">{succeeded}</span></span>
    <span><b>Failed:</b> <span class="badge badge-danger">{failed}</span></span>
  </div>
  {builds_html}
</div>
</body>
</html>"""

BUILD_CARD = """
  <div class="panel build_info {bg_class}">
    <h4>{build_name}</h4>
    <table class="{bg_class}">
      <tbody>
        <tr>
          <td align="right"><b>Name:&nbsp;</b></td>
          <td><a class="rolly" href="/build/{build_name}">{build_name}</a></td>
          <td align="right"><b>Status:&nbsp;</b></td>
          <td>{status}</td>
          <td align="right"><b>Duration:&nbsp;</b></td>
          <td>{duration}</td>
        </tr>
        <tr>
          <td align="right"><b>Start Time:&nbsp;</b></td>
          <td>{start_time}</td>
          <td align="right"><b>End Time:&nbsp;</b></td>
          <td>{end_time}</td>
          <td align="right"><b>Started By:&nbsp;</b></td>
          <td>{started_by}</td>
        </tr>
        <tr>
          <td align="right"><b>Machine:&nbsp;</b></td>
          <td>{machine}</td>
          <td align="right"><b>Last Step:&nbsp;</b></td>
          <td>{last_step}</td>
          <td align="right"><b>Build #:&nbsp;</b></td>
          <td>{build_number}</td>
        </tr>
        <tr>
          <td align="right"><b>Git Hash:&nbsp;</b></td>
          <td colspan="3"><code>{git_hash}</code></td>
          <td align="right"><b>Git Repo:&nbsp;</b></td>
          <td>{git_repo}</td>
        </tr>
      </tbody>
    </table>
    <div class="download-section">
      {download_links}
    </div>
  </div>
"""


def load_builds():
    """Load all build metadata from the builds directory."""
    builds = []
    builds_path = Path(BUILDS_DIR)
    if not builds_path.exists():
        return builds
    for build_dir in sorted(builds_path.iterdir(), reverse=True):
        meta_file = build_dir / "build_meta.json"
        if meta_file.exists():
            try:
                with open(meta_file) as f:
                    meta = json.load(f)
                meta["_dir"] = str(build_dir)
                builds.append(meta)
            except (json.JSONDecodeError, IOError):
                continue
    return builds


def format_duration(seconds):
    """Format seconds into H:MM:SS."""
    if seconds is None or seconds == "":
        return "N/A"
    s = int(float(seconds))
    h = s // 3600
    m = (s % 3600) // 60
    sec = s % 60
    return f"{h}:{m:02d}:{sec:02d}"


def get_download_links(build_dir, build_name):
    """Generate download links for artifacts in the build directory."""
    artifacts_dir = Path(build_dir) / "artifacts"
    links = []
    if artifacts_dir.exists():
        for f in sorted(artifacts_dir.iterdir()):
            if f.is_file():
                url = f"/download/{build_name}/{f.name}"
                css_class = "tar" if f.suffix in (".tar", ".gz", ".zip", ".o") else ""
                size = f.stat().st_size
                if size > 1024 * 1024:
                    size_str = f"{size / 1024 / 1024:.1f} MB"
                elif size > 1024:
                    size_str = f"{size / 1024:.1f} KB"
                else:
                    size_str = f"{size} B"
                links.append(
                    f'<a class="{css_class}" href="{url}">'
                    f'{f.name} ({size_str})</a>'
                )
    if not links:
        return "<span style='color:#999;font-size:12px;'>No artifacts</span>"
    return "\n      ".join(links)


def render_index():
    """Render the main build history page."""
    builds = load_builds()
    succeeded = sum(1 for b in builds if b.get("status") == "Succeeded")
    failed = sum(1 for b in builds if b.get("status") == "Failed")

    if not builds:
        builds_html = '<div class="empty">No builds yet. Trigger a build from Jenkins!</div>'
    else:
        cards = []
        for b in builds:
            status = b.get("status", "Unknown")
            if status == "Succeeded":
                bg = "succeeded_background_color"
            elif status == "Failed":
                bg = "failed_background_color"
            else:
                bg = "running_background_color"

            build_name = b.get("build_name", "unknown")
            cards.append(BUILD_CARD.format(
                build_name=build_name,
                bg_class=bg,
                status=status,
                duration=format_duration(b.get("duration")),
                start_time=b.get("start_time", "N/A"),
                end_time=b.get("end_time", "N/A"),
                started_by=b.get("started_by", "anonymous"),
                machine=b.get("machine", "localhost"),
                last_step=b.get("last_step", "N/A"),
                build_number=b.get("build_number", "N/A"),
                git_hash=b.get("git_hash", "N/A"),
                git_repo=b.get("git_repo", "N/A"),
                download_links=get_download_links(b.get("_dir", ""), build_name),
            ))
        builds_html = "\n".join(cards)

    return HTML_TEMPLATE.format(
        branch="hello_world",
        total_builds=len(builds),
        succeeded=succeeded,
        failed=failed,
        builds_html=builds_html,
    )


class BuildWebHandler(http.server.BaseHTTPRequestHandler):
    def log_message(self, format, *args):
        print(f"[{datetime.now(timezone.utc).strftime('%Y-%m-%d %H:%M:%S UTC')}] {args[0]}")

    def do_GET(self):
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path

        if path == "/" or path.startswith("/BR_") or path == "/index":
            self._serve_html(render_index())

        elif path == "/api/builds":
            builds = load_builds()
            for b in builds:
                b.pop("_dir", None)
            self._serve_json(builds)

        elif path.startswith("/download/"):
            self._serve_download(path)

        elif path.startswith("/build/"):
            build_name = path.split("/build/")[1].rstrip("/")
            self._serve_build_detail(build_name)

        else:
            self.send_error(404, "Not Found")

    def _serve_html(self, html):
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.end_headers()
        self.wfile.write(html.encode("utf-8"))

    def _serve_json(self, data):
        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.end_headers()
        self.wfile.write(json.dumps(data, indent=2).encode("utf-8"))

    def _serve_download(self, path):
        parts = path.split("/download/")[1].split("/", 1)
        if len(parts) != 2:
            self.send_error(404)
            return
        build_name, filename = parts
        filepath = Path(BUILDS_DIR) / build_name / "artifacts" / filename
        if not filepath.exists() or not filepath.is_file():
            self.send_error(404, f"File not found: {filename}")
            return
        self.send_response(200)
        self.send_header("Content-Type", "application/octet-stream")
        self.send_header("Content-Disposition", f'attachment; filename="{filename}"')
        self.send_header("Content-Length", str(filepath.stat().st_size))
        self.end_headers()
        with open(filepath, "rb") as f:
            self.wfile.write(f.read())

    def _serve_build_detail(self, build_name):
        meta_file = Path(BUILDS_DIR) / build_name / "build_meta.json"
        if not meta_file.exists():
            self.send_error(404, f"Build not found: {build_name}")
            return
        with open(meta_file) as f:
            meta = json.load(f)
        # Read console log if available
        log_file = Path(BUILDS_DIR) / build_name / "console.log"
        console_log = ""
        if log_file.exists():
            with open(log_file) as f:
                console_log = f.read()
        html = f"""<!doctype html>
<html><head><meta charset="utf-8"><title>{build_name}</title>
<style>
body {{ font-family: monospace; margin: 20px; background: #f5f5f5; }}
h1 {{ font-family: sans-serif; }}
a {{ color: darkblue; }}
.meta {{ background: #fff; padding: 15px; border: 1px solid #ddd; border-radius: 4px; margin: 15px 0; }}
.meta td {{ padding: 3px 10px; }}
pre {{ background: #1e1e1e; color: #d4d4d4; padding: 15px; border-radius: 4px; overflow-x: auto; max-height: 600px; overflow-y: auto; }}
</style></head><body>
<h1><a href="/">MyBuildWeb</a> / {build_name}</h1>
<div class="meta"><table>
<tr><td><b>Status:</b></td><td>{meta.get('status','N/A')}</td></tr>
<tr><td><b>Duration:</b></td><td>{format_duration(meta.get('duration'))}</td></tr>
<tr><td><b>Start:</b></td><td>{meta.get('start_time','N/A')}</td></tr>
<tr><td><b>End:</b></td><td>{meta.get('end_time','N/A')}</td></tr>
<tr><td><b>Git Hash:</b></td><td><code>{meta.get('git_hash','N/A')}</code></td></tr>
<tr><td><b>Git Repo:</b></td><td>{meta.get('git_repo','N/A')}</td></tr>
</table></div>
<h2>Console Output</h2>
<pre>{console_log if console_log else 'No console log available.'}</pre>
</body></html>"""
        self._serve_html(html)


def main():
    os.makedirs(BUILDS_DIR, exist_ok=True)
    with socketserver.TCPServer(("", PORT), BuildWebHandler) as httpd:
        print(f"MyBuildWeb running on http://0.0.0.0:{PORT}")
        print(f"Builds directory: {BUILDS_DIR}")
        httpd.serve_forever()


if __name__ == "__main__":
    main()
