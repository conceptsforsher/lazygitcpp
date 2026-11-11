# Line-by-line translation prompt (self-contained, start from zero)

Copy everything below the line to another agent. It was written while porting
`jesseduffield/lazygit` (Go) into `conceptsforsher/lazygitcpp` (C++17) and it
only assumes the target repo is another **code translation repo** in the
`conceptsforsher` account: an existing, half-finished C++ port of some upstream
project written in another language.

Fill in `<UPSTREAM>`, `<PORT_REPO>`, `<SRC_LANG>`, `<PORT_DIR>` before sending.

---

## Objective

Push `<PORT_REPO>` further towards **file and LOC parity with its upstream
original** `<UPSTREAM>`. "Translation" here means a line-by-line port, not a
reimplementation: every port source file mirrors **one** file of the upstream
source tree, keeps the same names and control flow, and carries a header naming
its origin.

## Hard rules

1. **Never build locally.** No `make`, no compiler invocation, no test run on
   your machine to "just check". All compilation happens in GitHub Actions.
   Push, then read the Actions log and iterate. Local source reading is fine;
   local compiling is not.
2. **Commit and push straight to the default branch. No PRs.**
3. Confirm the actual default branch first (`gh api repos/OWNER/REPO --jq
   .default_branch`) - several of these repos are still on `master`.
4. Every `gh` command and every remote `git` command must carry the account
   prefix, because the default login belongs to a different GitHub account:
   - `GH_CONFIG_DIR="$HOME/.config/gh-account-b" gh <command>`
   - `GH_CONFIG_DIR="$HOME/.config/gh-account-b" git push origin <branch>`
   Local `git add`/`commit` needs no prefix. Never run `gh auth switch`.
5. **One upstream file maps to one port file.** The port tree mirrors the
   upstream tree (`pkg/utils/x.go` -> `src/utils/x.hpp` + `x.cpp`). Do not merge
   upstream files together and do not split one upstream file across several
   port files except for a header/implementation pair.
6. Each translated file starts with an origin header:
   `// Origin: <upstream path>` for real upstream files, or
   `// Origin: (shim for <module>)` / `// Origin: (none) - harness` for files
   you had to invent. Then a short comment block that maps source-language
   constructs to the C++ you chose.
7. **Zero new external dependencies.** No vcpkg, no `-l<something>`, no
   fetching a third-party library at build time. When the upstream file uses a
   library that C++17 lacks, write a small local shim under
   `src/third_party/<name>/`, mark it `// Origin: (shim for ...)`, and say in
   the file what is faithful and what is approximated. Approximations are
   allowed; silent ones are not.

## Method

### Step 1 - measure the target surface

Clone the upstream project and the port, then count what is missing:

```bash
git clone <UPSTREAM> /tmp/upstream && git clone <PORT_REPO> /tmp/port
# upstream: non-test source files under its main source dir
cd /tmp/upstream && find <SRC_DIR> -name '*.<ext>' ! -name '*_test.<ext>' | wc -l
# port
cd /tmp/port && find <PORT_DIR> -name '*.cpp' -o -name '*.hpp' | wc -l
```

Exclude upstream vendored dependencies, generated files and integration-test
trees; say in the README exactly what you counted.

### Step 2 - establish the build gate first

Nothing can be validated without a green Actions build, so do this before any
translation:

- add `.github/workflows/build.yml` that checks out and runs the repo's build on
  every push (compiler matrix: `g++` and `clang++`; they are preinstalled on
  GitHub runners - do **not** apt-get install them, the runners have no route to
  the Ubuntu mirrors);
- make the build pick up sources **recursively** (`find src -name '*.cpp'`), so
  mirrored subdirectories compile as you add them;
- delete committed build artifacts, add a `.gitignore`, and add a CI step that
  fails if any build artifact is tracked again;
- add a translation-unit check file that `#include`s every translated header and
  instantiates its templates - headers alone are never compiled, so without
  this CI cannot see them.
- push, `gh run watch <run-id>` until it is green, then start translating.

Useful loop:

```bash
GH_CONFIG_DIR="$HOME/.config/gh-account-b" git push origin <branch>
sleep 12
rid=$(GH_CONFIG_DIR="$HOME/.config/gh-account-b" gh run list -R <OWNER/REPO> --limit 1 --json databaseId --jq '.[0].databaseId')
GH_CONFIG_DIR="$HOME/.config/gh-account-b" gh run watch -R <OWNER/REPO> "$rid" --exit-status --interval 10
GH_CONFIG_DIR="$HOME/.config/gh-account-b" gh run view -R <OWNER/REPO> "$rid" --log-failed | head -40
```

Fix only what the log actually says, then push again.

### Step 3 - translate in batches, leaves first

- Start with leaf packages (the ones nothing else depends on) and work inwards.
- Per batch, translate a handful of files, extend the check file to include
  them, push, and watch CI until green before starting the next batch.
- Port the upstream logic statement by statement. Where the source language has
  no C++ counterpart, choose the closest one and write down the mapping in that
  file's header comment (examples from the lazygit port: `error` ->
  `std::optional<Error>`; `(V, bool)` -> `std::optional<V>`; named regexp groups
  -> names kept next to the compiled `std::regex`; `sync.Once` ->
  `std::call_once`; Go time layouts -> `strftime` patterns).
- Keep the upstream function and type names (CamelCase -> `CamelCase`, or the
  repo's existing convention - follow whichever the repo already uses).
- Do not "improve" the code you are translating; TODO comments for the parts you
  could not carry over honestly are better than inventing behaviour.

### Step 4 - keep the parity table honest

Maintain a table in the README recomputed from the real trees after each batch:
upstream file, upstream lines, port file, port lines, plus a running total
("N of M upstream files translated"). Never hand-write these numbers.

## Deliverables before stopping

1. A green Actions build on the default branch for the latest push.
2. New translated files, each with its origin header, pushed in small commits
   with messages that name the upstream files translated.
3. README parity table updated with recomputed counts.

## Anti-patterns

- Compiling locally "to be sure".
- Opening a PR or working on a long-lived branch.
- Adding an external dependency instead of a documented shim.
- Translating a dependency graph leaf-in by guessing: read the upstream file's
  imports first and translate the leaves they need.
- Claiming parity without recomputing the counts from both trees.
