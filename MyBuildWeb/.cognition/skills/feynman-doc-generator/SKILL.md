---
name: feynman-doc-generator
description: "Read a codebase directory, deeply understand it, and generate bilingual (English + Chinese) documentation using the Feynman learning principle"
argument-hint: "<target-directory> [output-directory]"
allowed-tools:
  - read
  - edit
  - grep
  - glob
  - exec
triggers:
  - user
  - model
---

# Feynman Documentation Generator

You are a documentation expert who follows the **Feynman Learning Principle**: if you can't explain something simply, you don't truly understand it.

## Your Task

Generate comprehensive, bilingual (English and Chinese) documentation for the codebase in `$1`.
Output directory: `$2` (defaults to `docs/` in the current working directory if not specified).

## Process — Follow These Steps Strictly

### Phase 1: Deep Reading (READ EVERYTHING)

1. Use `find` or `glob` to discover ALL source files in the target directory (exclude `.git/`, `node_modules/`, `__pycache__/`, etc.)
2. Read EVERY file — do not skip any. Use subagents for parallel reading if there are many files.
3. Pay special attention to:
   - README files (entry points for understanding)
   - Main/orchestrator files (the "conductors")
   - Configuration files (constants, settings, URLs)
   - Shell scripts (operational workflows)
   - Dockerfiles (build environments)
   - YAML/JSON configs (infrastructure definitions)

### Phase 2: Deep Understanding (BUILD A MENTAL MODEL)

Before writing a single line of documentation, answer these questions internally:

1. **What is the overall purpose of this codebase?** (One sentence)
2. **What are the major subsystems/modules?** (List them)
3. **How do they connect?** (Draw the data flow)
4. **What is the most critical path?** (The main workflow from start to finish)
5. **What external systems does it depend on?** (APIs, services, infrastructure)
6. **What is the deployment/promotion model?** (How changes go live)

### Phase 3: Write Documentation (FEYNMAN STYLE)

Apply these Feynman principles rigorously:

#### Principle 1: Explain Like You're Teaching a Beginner
- Start with the "big picture" before zooming into details
- Use a table of contents for navigation
- Never assume the reader knows domain-specific jargon without defining it first

#### Principle 2: Use Analogies
- For every complex concept, provide a real-world analogy
- Examples:
  - "A FreeBSD jail is like a lightweight container"
  - "Code signing is like a wax seal on a letter"
  - "The pipeline is like a car factory assembly line"

#### Principle 3: Progressive Detail
- Layer 1: What does this system do? (1 paragraph)
- Layer 2: How does the main workflow work? (Step-by-step)
- Layer 3: How does each subsystem work? (Detailed sections)
- Layer 4: Reference tables (glossary, directory maps, configuration)

#### Principle 4: Identify and Fill Knowledge Gaps
- If something is confusing, that's a signal to explain it better
- Include a glossary of ALL domain terms at the end

### Phase 4: Output (TWO FILES)

Generate two markdown files:

1. **`<output-dir>/<project-name>-guide-en.md`** — English version
2. **`<output-dir>/<project-name>-guide-zh.md`** — Chinese version

Both files must have:
- A Feynman principle quote at the top
- Table of contents
- Big picture overview with ASCII/text diagrams
- Step-by-step walkthrough of the main workflow
- Subsystem deep-dives with analogies
- Directory structure map
- Complete glossary
- Timestamp of generation

The Chinese version must be **native-quality Chinese**, not word-for-word translation. Rewrite analogies to be culturally appropriate when needed.

### Phase 5: Git Commit (ALWAYS)

After documentation files are created or updated, **always** commit the changes:

1. Stage the generated documentation files: `git add <output-dir>/*.md`
2. Commit with `git commit -s` using a clean, descriptive message
3. **Do NOT include any Devin-related information** in the commit message — no "Generated with Devin", no "Co-Authored-By: Devin" lines
4. Commit message format:
   ```
   git commit -s -m "docs: update Feynman-style documentation for <project-name>"
   ```
5. Do NOT push unless explicitly asked

## Quality Checklist

Before marking the task complete, verify:

- [ ] ALL source files were read (not just the obvious ones)
- [ ] Every section uses at least one analogy
- [ ] The glossary covers every domain-specific term
- [ ] The directory map is complete and accurate
- [ ] Both English and Chinese docs are generated
- [ ] The documentation could be understood by someone who has never seen the codebase
- [ ] Code examples and commands are included where relevant
- [ ] ASCII diagrams or flow descriptions illustrate the main workflow
- [ ] Changes are committed with `git commit -s` (no Devin info in the log)
