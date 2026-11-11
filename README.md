# lazygitcpp — Git Terminal UI (C++ port of lazygit)

A zero-dependency C++ port of [lazygit](https://github.com/jesseduffield/lazygit) — a keyboard-driven terminal UI for everyday git operations.

## Why lazygitcpp?

The original [lazygit](https://github.com/jesseduffield/lazygit) requires Go plus dozens of modules. lazygitcpp compiles with a single `make` using only C++17 and pthreads.

## Quick Start

```bash
make
./lazygitcpp
```

## Features

- **Status panel** — staged/unstaged files with color coding
- **Branches panel** — branch list sorted by last commit date
- **Log panel** — `git log --oneline --graph` (30 entries)
- **Stash panel** — stash list

## Keyboard Controls

| Key | Action |
|-----|--------|
| 1-4 | Switch panels (Status/Branches/Log/Stash) |
| j/k | Move selection down/up |
| Space | Stage/unstage file (Status) or checkout branch (Branches) |
| c | Commit with editor prompt |
| p | Push |
| P | Pull |
| q | Quit |

## Limitations

- No interactive rebase, cherry-pick, or diff viewer
- No horizontal split for diff display
- No submodule support

## Translation status

The port is a line-by-line translation, not a reimplementation: every file in
`src/` mirrors one file of upstream `pkg/` and starts with an `// Origin: pkg/...`
header naming it. Upstream's non-test `pkg/` (excluding `pkg/integration` and
vendored dependencies) is the target surface: **410 files / 87,279 lines**.

| Upstream file | Upstream lines | Port file | Port lines |
|---|---:|---|---:|
| `pkg/utils/thread_safe_map.go` | 90 | `src/utils/thread_safe_map.hpp` | 115 |
| `pkg/utils/history_buffer.go` | 38 | `src/utils/history_buffer.hpp` | 61 |
| `pkg/utils/stack.go` | 28 | `src/utils/stack.hpp` | 45 |
| `pkg/utils/string_pool.go` | 14 | `src/utils/string_pool.hpp` | 44 |
| `pkg/utils/once_writer.go` | 31 | `src/utils/once_writer.hpp` | 45 |
| `pkg/utils/errors.go` | 14 | `src/utils/errors.hpp` | 36 |
| `pkg/utils/io.go` | 30 | `src/utils/io.hpp`, `src/utils/io.cpp` | 67 |
| `pkg/utils/template.go` | 33 | `src/utils/template.hpp`, `src/utils/template.cpp` | 136 |
| `pkg/utils/regexp.go` | 17 | `src/utils/regexp.hpp`, `src/utils/regexp.cpp` | 97 |
| `pkg/utils/date.go` | 67 | `src/utils/date.hpp`, `src/utils/date.cpp` | 207 |
| `pkg/utils/slice.go` | 179 | `src/utils/slice.hpp` | 233 |
| `pkg/utils/search.go` | 95 | `src/utils/search.hpp`, `src/utils/search.cpp` + `src/third_party/fuzzy/*` | 339 |
| `pkg/utils/project_root.go` | 69 | `src/utils/project_root.hpp`, `src/utils/project_root.cpp` | 178 |
| **13 files translated** | **705** | **18 files (+ 2-file fuzzy shim + 1 CI check)** | **1603** |

Translated so far: 13 of 410 upstream files. C++ carries more lines than the Go
original because translation splits declarations into headers, and because Go
constructs that have no one-line C++ counterpart (errors without values,
`sync.Map`, named capture groups) need an explicit type or shim.

Not translated as-is, and marked in the file itself:

- `ResolveTemplate` supports the `{{name}}` / `{{.name}}` subset lazygit feeds
  it with `missingkey=error` semantics, not all of Go's `text/template`.
- `WrapError` cannot capture a stack trace in C++17 (`std::stacktrace` is C++23);
  the nil-in/nil-out behaviour upstream relies on is preserved.
- Named regexp groups are kept alongside the compiled expression, because
  `std::regex` has no notion of `SubexpNames()`.
- `fuzzy.Find` scoring is an approximation of `github.com/sahilm/fuzzy` (see
  `src/third_party/fuzzy/fuzzy.hpp`); ordering is right, the score value is not.
- `MustFindLazygitRootDirectory` writes to stderr and exits 1 where upstream
  calls `log.Fatal`, because a C++ exception would change every caller.

## Build

```bash
make
```
Requires: GCC 10+ or Clang 12+, GNU Make, `git` in PATH

Builds run in GitHub Actions (`.github/workflows/build.yml`, g++ and clang++ on
every push). `src/utils/utils_check.cpp` includes every translated header so CI
compiles them too - headers alone are not translation units.
