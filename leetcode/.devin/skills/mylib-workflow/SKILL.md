# mylib-workflow

## Description

Workflow rules for creating and maintaining C libraries under the `mylib/` directory. Each library is organized by functionality into its own subdirectory, built as a dynamic shared library (`.so`), and includes a demo showing usage.

## Rules

### 1. Directory Structure

Each library lives in its own subdirectory under `mylib/`, organized by functionality:

```
mylib/<library_name>/
├── include/
│   └── <library_name>.h    # public header
├── src/
│   └── <library_name>.c    # implementation
├── demo/
│   ├── main.c              # demo program showing how to use the library
│   └── Makefile             # builds demo, links against the .so
├── Makefile                 # builds the dynamic library (.so)
├── readme.md                # English documentation
└── readme_cn.md             # Chinese documentation
```

### 2. Library Build

- The library **must** be built as a **dynamic shared library** (`.so` on Linux).
- The top-level `Makefile` in the library directory compiles with `-fPIC` and links with `-shared`.
- Naming convention: `lib<library_name>.so` (e.g. `libhashtable.so`).
- Use `-O0 -g` for debug builds by default.

### 3. Demo

- Every library **must** include a `demo/` subdirectory.
- `demo/main.c` demonstrates all major API functions of the library.
- `demo/Makefile` builds the demo, linking against the `.so` from the parent directory.
- The demo must be runnable with `LD_LIBRARY_PATH` pointing to the library directory or via `-rpath`.
- The demo should print clear output showing each API call and its result.

### 4. Documentation

Each library directory must contain:

- `readme.md` — **English** documentation.
- `readme_cn.md` — **Chinese** (中文) documentation.

Both files must include:

- **Table of Contents** (linked headings).
- **Overview** — What the library does and when to use it.
- **API Reference** — Every public function with signature, parameters, return value, and description.
- **Build Instructions** — How to compile the library and the demo.
- **Usage Example** — Code snippet showing typical usage.

### 5. Git Commit

- All changes under `mylib/` **must** end with `git commit -s`.
- Commit message style: `mylib/<name>: <short description>`.
- **NEVER** include `Co-Authored-By: Devin` or any similar auto-generated co-author lines.
- Any updates to this skill must also be committed with `git commit -s`.

### 6. Checklist

Before finishing any mylib task, verify:

- [ ] Library compiles to a `.so` file successfully.
- [ ] `demo/main.c` builds, runs, and demonstrates all major API functions.
- [ ] `readme.md` (English) and `readme_cn.md` (中文) both exist with API reference.
- [ ] `git commit -s` has been executed for all changes.
