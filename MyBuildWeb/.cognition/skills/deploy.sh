#!/usr/bin/env bash
#
# deploy.sh - Deploy skills for Devin CLI and Windsurf Cascade IDE
#
# Devin:    Deploys superpowers skills only (custom skills are already
#           visible via the workspace). Symlinks to ~/.config/cognition/.
#           Sets up session-start hooks for superpowers context injection.
# Windsurf: Symlinks custom skills (non-superpowers) into
#           ~/.codeium/windsurf/skills/ so Cascade can discover them.
#
# Usage:
#   ./deploy.sh          # Full deploy (init submodule + deploy all)
#   ./deploy.sh --clean  # Remove all deployed symlinks and hooks
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VENDOR_SUPERPOWERS="${SCRIPT_DIR}/vendor/superpowers"
DEVIN_CLI_DIR="${HOME}/.config/cognition/cli"
DEVIN_SKILLS_DIR="${HOME}/.config/cognition/skills"
DEVIN_HOOKS_DIR="${HOME}/.config/cognition/hooks"
DEVIN_GLOBAL_CONFIG="${HOME}/.config/cognition/config.json"
WINDSURF_SKILLS_DIR="${HOME}/.codeium/windsurf/skills"

# Directories to skip when scanning for custom skills
SKIP_DIRS="vendor|skills|.git|.cognition|.github"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

info()    { echo -e "${GREEN}[INFO]${NC} $*"; }
warn()    { echo -e "${YELLOW}[WARN]${NC} $*"; }
error()   { echo -e "${RED}[ERROR]${NC} $*" >&2; }
section() { echo -e "\n${CYAN}=== $* ===${NC}"; }

# ─── Submodule Initialization ────────────────────────────────────────

init_submodule() {
    section "Submodule"

    if [ -d "${VENDOR_SUPERPOWERS}/skills" ]; then
        info "Submodule already initialized"
    else
        info "Initializing superpowers submodule..."
        (cd "${SCRIPT_DIR}" && git submodule update --init --recursive vendor/superpowers)
        if [ ! -d "${VENDOR_SUPERPOWERS}/skills" ]; then
            error "Failed to initialize submodule"
            exit 1
        fi
        info "Submodule initialized successfully"
    fi

    local version
    version="$(cd "${VENDOR_SUPERPOWERS}" && git describe --tags --always 2>/dev/null || echo 'unknown')"
    info "Superpowers version: ${version}"
}

# ─── Clean ───────────────────────────────────────────────────────────

clean_devin() {
    section "Clean Devin"

    # Remove superpowers repo link
    if [ -e "${DEVIN_CLI_DIR}/superpowers" ]; then
        rm -rf "${DEVIN_CLI_DIR}/superpowers"
        info "Removed: ${DEVIN_CLI_DIR}/superpowers"
    fi

    # Remove superpowers skill symlinks
    if [ -d "${DEVIN_SKILLS_DIR}" ]; then
        for link in "${DEVIN_SKILLS_DIR}"/*; do
            [ -L "$link" ] || continue
            local target
            target="$(readlink "$link")"
            if [[ "$target" == *"/cognition/cli/superpowers/"* ]] || \
               [[ "$target" == *"/vendor/superpowers/"* ]]; then
                rm "$link"
                info "Removed symlink: $(basename "$link")"
            fi
        done
    fi
}

clean_windsurf() {
    section "Clean Windsurf"

    if [ ! -d "${WINDSURF_SKILLS_DIR}" ]; then
        info "Windsurf skills dir does not exist, nothing to clean"
        return
    fi

    # Remove symlinks that point back to this repo's custom skills
    for link in "${WINDSURF_SKILLS_DIR}"/*; do
        [ -L "$link" ] || continue
        local target
        target="$(readlink -f "$link" 2>/dev/null || readlink "$link")"
        if [[ "$target" == "${SCRIPT_DIR}/"* ]]; then
            rm "$link"
            info "Removed symlink: $(basename "$link")"
        fi
    done
}

clean_hooks() {
    section "Clean Hooks"

    # Remove global hook script
    if [ -f "${DEVIN_HOOKS_DIR}/session-start.sh" ]; then
        rm "${DEVIN_HOOKS_DIR}/session-start.sh"
        info "Removed: ${DEVIN_HOOKS_DIR}/session-start.sh"
    fi

    # Remove hooks from global config.json
    if [ -f "${DEVIN_GLOBAL_CONFIG}" ] && python3 -c "import json" 2>/dev/null; then
        local result
        result="$(python3 -c "
import json
with open('${DEVIN_GLOBAL_CONFIG}', 'r') as f:
    config = json.load(f)
if 'hooks' in config:
    del config['hooks']
    with open('${DEVIN_GLOBAL_CONFIG}', 'w') as f:
        json.dump(config, f, indent=2)
        f.write('\n')
    print('removed')
else:
    print('none')
")" 
        if [ "$result" = "removed" ]; then
            info "Removed hooks from global config.json"
        else
            info "No hooks in global config.json"
        fi
    fi
}

# ─── Deploy ──────────────────────────────────────────────────────────

deploy_devin() {
    section "Deploy Devin (superpowers only)"

    mkdir -p "${DEVIN_CLI_DIR}" "${DEVIN_SKILLS_DIR}"

    # Link full superpowers repo (for hooks, agents, etc.)
    ln -sf "${VENDOR_SUPERPOWERS}" "${DEVIN_CLI_DIR}/superpowers"
    info "Linked repo: ${DEVIN_CLI_DIR}/superpowers -> vendor/superpowers"

    # Link individual superpowers skills
    local count=0
    for skill_dir in "${VENDOR_SUPERPOWERS}/skills"/*/; do
        [ -d "$skill_dir" ] || continue
        local skill_name
        skill_name="$(basename "$skill_dir")"

        # Custom skill with same name takes precedence
        if [ -d "${SCRIPT_DIR}/${skill_name}" ] && [ -f "${SCRIPT_DIR}/${skill_name}/SKILL.md" ]; then
            warn "Skipping superpowers/${skill_name}: custom skill takes precedence"
            continue
        fi

        ln -sf "${skill_dir}" "${DEVIN_SKILLS_DIR}/${skill_name}"
        count=$((count + 1))
    done

    info "Deployed ${count} superpowers skills"
}

deploy_windsurf() {
    section "Deploy Windsurf (custom skills only)"

    mkdir -p "${WINDSURF_SKILLS_DIR}"

    # If the repo IS the Windsurf skills dir, skills are already visible
    local repo_real windsurf_real
    repo_real="$(readlink -f "${SCRIPT_DIR}")"
    windsurf_real="$(readlink -f "${WINDSURF_SKILLS_DIR}" 2>/dev/null || echo "")"

    if [ "${repo_real}" = "${windsurf_real}" ]; then
        info "Repo is already at Windsurf skills path — custom skills are natively visible"
        info "No symlinks needed (vendor/ is excluded by directory structure)"
        return
    fi

    # Symlink each custom skill directory into Windsurf skills dir
    local count=0
    for dir in "${SCRIPT_DIR}"/*/; do
        [ -d "$dir" ] || continue
        local name
        name="$(basename "$dir")"

        # Skip non-skill directories
        if [[ "$name" =~ ^(${SKIP_DIRS})$ ]]; then
            continue
        fi

        # Must have a SKILL.md to be a valid skill
        if [ ! -f "${dir}/SKILL.md" ]; then
            continue
        fi

        ln -sf "${dir}" "${WINDSURF_SKILLS_DIR}/${name}"
        info "Linked skill: ${name}"
        count=$((count + 1))
    done

    info "Deployed ${count} custom skills to Windsurf"
}

deploy_hooks() {
    section "Deploy Hooks (session-start)"

    mkdir -p "${DEVIN_HOOKS_DIR}"

    # ── 1. Create global hook script ──
    #
    # This script is executed by Devin on session_start / SessionStart.
    # It reads the using-superpowers SKILL.md and outputs JSON to inject
    # the skill content into the conversation context.
    #
    cat > "${DEVIN_HOOKS_DIR}/session-start.sh" << 'HOOKEOF'
#!/usr/bin/env bash
# Superpowers session-start hook for Devin CLI
# Injects using-superpowers skill content at the start of every session.
# Deployed by: skills/deploy.sh

set -euo pipefail

SKILL_FILE="${HOME}/.config/cognition/skills/using-superpowers/SKILL.md"

# Silently exit if skill not deployed
if [ ! -f "$SKILL_FILE" ]; then
    exit 0
fi

content=$(cat "$SKILL_FILE")

# Escape for JSON embedding
escape_for_json() {
    local s="$1"
    s="${s//\\/\\\\}"
    s="${s//\"/\\\"}"
    s="${s//$'\n'/\\n}"
    s="${s//$'\r'/\\r}"
    s="${s//$'\t'/\\t}"
    printf '%s' "$s"
}

escaped=$(escape_for_json "$content")

# Output JSON — Devin reads "add_context" from command stdout
printf '{\n  "add_context": "<EXTREMELY_IMPORTANT>\\nYou have superpowers.\\n\\n%s\\n</EXTREMELY_IMPORTANT>"\n}\n' "$escaped"

exit 0
HOOKEOF
    chmod +x "${DEVIN_HOOKS_DIR}/session-start.sh"
    info "Created: ${DEVIN_HOOKS_DIR}/session-start.sh"

    # ── 2. Update global config.json with hooks (Claude-format) ──
    #
    # Devin supports Claude-format hooks in config.json:
    #   SessionStart → fires on session start/resume/clear/compact
    #
    if [ -f "${DEVIN_GLOBAL_CONFIG}" ] && python3 -c "import json" 2>/dev/null; then
        python3 << PYEOF
import json

config_path = "${DEVIN_GLOBAL_CONFIG}"
hook_script = "${DEVIN_HOOKS_DIR}/session-start.sh"

with open(config_path, "r") as f:
    config = json.load(f)

config["hooks"] = {
    "SessionStart": [
        {
            "matcher": "",
            "hooks": [
                {
                    "type": "command",
                    "command": hook_script,
                    "timeout": 5
                }
            ]
        }
    ]
}

with open(config_path, "w") as f:
    json.dump(config, f, indent=2)
    f.write("\n")
PYEOF
        info "Updated: ${DEVIN_GLOBAL_CONFIG} (added SessionStart hook)"
    else
        warn "Could not update global config.json — manual setup needed"
    fi

}

# ─── Main ────────────────────────────────────────────────────────────

main() {
    echo -e "${CYAN}Skills Deploy Script${NC}"
    echo "Repo: ${SCRIPT_DIR}"

    local clean_only=false
    if [ "${1:-}" = "--clean" ]; then
        clean_only=true
    fi

    # Always clean first
    clean_devin
    clean_windsurf
    clean_hooks

    if $clean_only; then
        echo ""
        info "Clean-only mode. Done."
        exit 0
    fi

    # Init submodule (auto-clone if needed)
    init_submodule

    # Deploy
    deploy_devin
    deploy_windsurf
    deploy_hooks

    section "Summary"
    info "Devin:    superpowers skills deployed to ${DEVIN_SKILLS_DIR}"
    info "Windsurf: custom skills deployed to ${WINDSURF_SKILLS_DIR}"
    info "Hooks:    session-start configured (global)"
    info "Done."
}

main "$@"
