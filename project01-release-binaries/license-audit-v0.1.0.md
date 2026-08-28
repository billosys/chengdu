# chengdu — license and linkage audit — v0.1.0

> Written by CC, arc02 slice02 (`release-readiness`), 2026-08-06. Method
> and rule per arc-plan D5: *verify, don't assume.* Every claim below
> carries an evidence pointer — a build-log link line, an `nm`/`otool`
> reading against a real compiled artifact, or a LICENSE file path at an
> exact pinned commit SHA. No claim in this document is sourced from a
> README's say-so, a package's reputation, or memory.
>
> **Scope.** This audit covers exactly what gets *compiled into and
> shipped inside* the six binaries chengdu distributes at v0.1.0
> (`pandaPIparser`, `pandaPIgrounder`, `pandaPIengine` × `linux-x86_64`,
> `macos-arm64`). Build-time-only tools (bison, flex, gengetopt, cmake,
> the compiler itself) are out of scope — they are not linked into the
> output and impose no distribution obligation on it. Components that
> are *built* by the toolchain but *not linked* into any shipped binary
> are investigated and named, but carry no obligation either — the
> obligation trigger is distribution of the covered code, not merely
> compiling it.

## 1. Method

For each binary, the audit traces the actual link command (the
compiler/linker invocation that produces the final executable) from a
real build log, then cross-checks the result against the compiled
binary itself via `nm` (symbol table) and `otool -L` (dynamic library
dependencies), on macOS. Where the same claim needed independent
confirmation on Linux, a **fresh** CI run was triggered for this audit
(prior run logs had already aged out of GitHub's retention and were
unavailable) rather than relying on architectural inference alone,
except where the build system's own non-platform-conditional source
(a `CMakeLists.txt`, `if`/`ifeq` guard) makes the inference itself the
strongest available evidence — that is disclosed explicitly at each
such point below, not silently assumed.

Evidence artifacts referenced throughout:

- **Local macOS build**, this session, 2026-08-06: `pins.env`-pinned
  fresh `fetch-upstream.sh` + all three `build-*.sh` scripts, run on
  this machine. Full build logs retained; binaries at
  `dist/macos-arm64/`.
- **Fresh Linux CI run**, triggered for this audit:
  `gh workflow run build.yml --ref main`, run
  `https://github.com/billosys/chengdu/actions/runs/31072674216`
  (dispatch) and the immediately-preceding push run
  `https://github.com/billosys/chengdu/actions/runs/31072336131`, job
  `call-build / build (ubuntu-22.04)` (id `92522675318`), log fetched
  via `gh api repos/billosys/chengdu/actions/jobs/92522675318/logs`
  (`gh run view --log` returned empty output in this environment for
  reasons not diagnosed further; the direct API call is the evidence
  channel actually used, and is reproducible).

## 2. Per-binary linkage tables, with evidence

### 2.1 `pandaPIparser` (both platforms)

| Object linked | Source | License |
|---|---|---|
| `pandaPIparser`'s own `src/*.o` (bison/flex-generated `hddl.cpp`/`hddl-token.cpp` included) | `pandaPIparser` repo, own build | BSD 3-Clause |

**Evidence — macOS**, this session's build log:

```
g++ -O3 -lm -flto -DNDEBUG src/hddl-token.o src/hddl.o src/main.o src/sortexpansion.o
  src/parsetree.o src/util.o src/domain.o src/output.o src/parametersplitting.o
  src/cwa.o src/typeof.o src/shopWriter.o src/hpdlWriter.o src/hddlWriter.o
  src/htn2stripsWriter.o src/orderingDecomposition.o src/plan.o src/verify.o
  src/properties.o src/verification_encoding.o src/cmdline.o -o pandaPIparser
```

**Evidence — Linux**, run 31072336131, job `build (ubuntu-22.04)`:

```
g++ -O3 -lm -flto -static -static-libgcc -DNDEBUG src/hddl-token.o src/hddl.o
  src/main.o src/sortexpansion.o src/parsetree.o src/util.o src/domain.o
  src/output.o src/parametersplitting.o src/cwa.o src/typeof.o src/shopWriter.o
  src/hpdlWriter.o src/hddlWriter.o src/htn2stripsWriter.o
  src/orderingDecomposition.o src/plan.o src/verify.o src/properties.o
  src/verification_encoding.o src/cmdline.o -o pandaPIparser
```

Identical object set on both platforms (Linux additionally
`-static -static-libgcc`, a linking *mode* flag, not an added
dependency). **`otool -L dist/macos-arm64/pandaPIparser`**: only
`libSystem.B.dylib`, `libc++.1.dylib` (OS-provided). **`nm` grep** for
any third-party marker string (`bliss|lpsolve|h2|cpddl|boruvka`, case
insensitive) on the macOS binary: **0 matches**. `pandaPIparser` ships
no third-party code on either platform.

### 2.2 `pandaPIgrounder` (both platforms)

| Object linked | Source | License |
|---|---|---|
| `pandaPIgrounder`'s own `src/*.o` | `pandaPIgrounder` repo, own build | BSD 3-Clause |
| `h2-fd-preprocessor/src/*.cc.o` (12 files) | `galvusdamor/h2-fd-preprocessor` @ `e335433` | **GPL-3.0 (see §2.2.1 — the central finding)** |
| `cpddl/libpddl.a` | `danfis/cpddl` @ `8c142ba` | BSD 3-Clause |
| `cpddl/third-party/boruvka/libboruvka.a` | `danfis/boruvka` @ `e320cc7` | BSD 3-Clause |

**NOT linked** (built by the toolchain, absent from the link line and
from the compiled binary's symbol table — investigated and confirmed
excluded, so they carry no obligation): `cpddl/third-party/bliss`
(LGPL-3.0), `cpddl/third-party/lpsolve` (LGPL), `cpddl/third-party/opts`
(BSD 3-Clause). See §2.2.2.

**Evidence — macOS**, this session's build log, final grounder link:

```
c++  -O3 -flto  -o ../pandaPIgrounder conditional_effects.o debug.o duplicate.o
  FAMmutexes.o givenPlan.o gpg.o groundedGPG.o grounding.o h2mutexes.o
  hierarchy-typing.o liftedGPG.o main.o model.o naiveGrounding.o output.o
  parser.o postprocessing.o sasinvariants.o sasplus.o util.o rss.o cmdline.o
  ../h2-fd-preprocessor/src/axiom.cc.o ../h2-fd-preprocessor/src/causal_graph.cc.o
  ../h2-fd-preprocessor/src/domain_transition_graph.cc.o
  ../h2-fd-preprocessor/src/h2_mutexes.cc.o
  ../h2-fd-preprocessor/src/helper_functions.cc.o
  ../h2-fd-preprocessor/src/max_dag.cc.o ../h2-fd-preprocessor/src/mutex_group.cc.o
  ../h2-fd-preprocessor/src/operator.cc.o ../h2-fd-preprocessor/src/scc.cc.o
  ../h2-fd-preprocessor/src/state.cc.o
  ../h2-fd-preprocessor/src/successor_generator.cc.o
  ../h2-fd-preprocessor/src/variable.cc.o
  ../cpddl/libpddl.a ../cpddl/third-party/boruvka/libboruvka.a
```

**Evidence — Linux**, run 31072336131, job `build (ubuntu-22.04)`
(fresh, triggered for this audit):

```
g++  -O3 -flto -static -static-libgcc  -o ../pandaPIgrounder FAMmutexes.o
  conditional_effects.o debug.o duplicate.o givenPlan.o gpg.o groundedGPG.o
  grounding.o h2mutexes.o hierarchy-typing.o liftedGPG.o main.o model.o
  naiveGrounding.o output.o parser.o postprocessing.o sasinvariants.o
  sasplus.o util.o rss.o cmdline.o ../h2-fd-preprocessor/src/axiom.cc.o
  ../h2-fd-preprocessor/src/causal_graph.cc.o
  ../h2-fd-preprocessor/src/domain_transition_graph.cc.o
  ../h2-fd-preprocessor/src/h2_mutexes.cc.o
  ../h2-fd-preprocessor/src/helper_functions.cc.o
  ../h2-fd-preprocessor/src/max_dag.cc.o ../h2-fd-preprocessor/src/mutex_group.cc.o
  ../h2-fd-preprocessor/src/operator.cc.o ../h2-fd-preprocessor/src/scc.cc.o
  ../h2-fd-preprocessor/src/state.cc.o
  ../h2-fd-preprocessor/src/successor_generator.cc.o
  ../h2-fd-preprocessor/src/variable.cc.o
  ../cpddl/libpddl.a ../cpddl/third-party/boruvka/libboruvka.a
```

Same object/archive set on both platforms. **Binary-level confirmation
(macOS only — Linux binaries were not downloaded for symbol inspection
in this audit pass; the link-line evidence above is direct and
sufficient on its own)**: `nm dist/macos-arm64/pandaPIgrounder | grep
-i h2` returns FD-preprocessor-specific symbols including
`compute_h2_mutexes`, `CausalGraph::weigh_graph_from_ops`,
`H2Mutexes::init_h2_operators`, `strip_axioms` — unambiguously the
h2-fd-preprocessor code, compiled in. The same grep for
`bliss|lpsolve` returns **0 matches**.

#### 2.2.1 The central finding: `h2-fd-preprocessor` is GPL-3.0-derived, no LICENSE file of its own

`h2-fd-preprocessor`'s own repository (`galvusdamor/h2-fd-preprocessor`
@ `e335433`) carries **no LICENSE or COPYING file anywhere in its tree**
— confirmed by an exhaustive `find` across the checkout and a `git log`
across all reachable history for those filenames (neither present at
any point). Its `README.md`, verbatim:

> This repository contains the preprocessor code that used to be part
> of Fast Downward (http://www.fast-downward.org/) and the
> h^2-mutex-pruning code by Vidal Alcázar and Álvaro Torralba.

Fast Downward's own canonical repository
(`aibasel/downward`, `LICENSE.md`, fetched 2026-08-06) is unambiguous:
**GNU General Public License, Version 3**, header line "GNU GENERAL
PUBLIC LICENSE Version 3, 29 June 2007", copyright Free Software
Foundation.

**Conclusion, stated as a judgment, not a certainty presented as fact:**
this vendored copy inherits Fast Downward's license by clear provenance
(the README says so explicitly) but was extracted without carrying its
own license grant forward — itself a compliance gap in the vendored
copy, independent of what chengdu does next. The defensible, correct,
and *conservative* posture — providing more compliance than strictly
provable is required does not create risk; providing less does — is to
treat the twelve `h2-fd-preprocessor` object files compiled into
`pandaPIgrounder` as **GPL-3.0-covered code**, and the `pandaPIgrounder`
binary that statically links them as a work whose distribution triggers
GPL-3.0's obligations. This is the single most consequential finding in
this audit, exactly as arc-plan D5 anticipated.

**Mere aggregation, not contamination:** `pandaPIparser` and
`pandaPIengine` do not link any `h2-fd-preprocessor` object code
(confirmed: zero matching symbols in either binary, §2.1, §2.3) and are
separate executables communicating with `pandaPIgrounder` only through
intermediate files on disk (the architecture the runbook itself
describes as "Erlang-port-style isolation"). Distributing them
alongside a GPL-covered `pandaPIgrounder` in the same release tarball is
"mere aggregation" under GPL-3.0 §5's own terms, not a derivative work —
their own BSD 3-Clause terms are unaffected. **Only `pandaPIgrounder`
carries the GPL-3.0 obligation.**

#### 2.2.2 Confirmed NOT linked: `bliss`, `lpsolve`, `opts`

All three are built by `cpddl`'s Makefile (`bliss` and `lpsolve`
explicitly, per `build-grounder.sh`'s own mechanization of the runbook;
`opts` is cpddl's own getopt-alternative dependency) but **do not
appear on either platform's grounder link line above**, and — checked
independently at the binary level on macOS — contribute **zero symbols**
to the compiled `pandaPIgrounder` (`nm | grep -i 'bliss\|lpsolve'`: 0
matches). This directly confirms the suggestion arc01 flagged and this
slice's evidence-based mandate to verify rather than assume:

| Component | License found | LICENSE path @ SHA | Linked? |
|---|---|---|---|
| `bliss` 0.73 | LGPL-3.0 | `bliss/COPYING.LESSER` (vendored zip; header confirms "GNU LESSER GENERAL PUBLIC LICENSE Version 3") | **No** |
| `lpsolve` | LGPL (lp_solve v5, "released under the LGPL license" per `lp_lib.h`'s own header) | no standalone file found; license statement is in-source (`lp_lib.h`) | **No** |
| `opts` | BSD 3-Clause | `cpddl/third-party/opts/BSD-LICENSE` @ `819d90e` | **No** |

Because none of the three is distributed in compiled form, none
imposes an obligation on chengdu's binaries. Named here for the
transparency the audit method demands, not because they require an
entry in `THIRD-PARTY-LICENSES`.

### 2.3 `pandaPIengine` (both platforms)

| Object linked | Source | License |
|---|---|---|
| `pandaPIengine`'s own object files + its own internal modules (`search`, `symbolic_search`, `heuristics`, `hrc` [rcHeuristics], `hlm` [landmarks], `translation`, `intDataStructures`) | `pandaPIengine` repo, own build | BSD 3-Clause |

**No third-party component is linked.** Evidence, two independent
forms:

1. **macOS `link.txt`** (CMake's own generated link command, read
   directly from `upstream/pandaPIengine/build/CMakeFiles/
   pandaPIengine.dir/link.txt` after this session's build):

   ```
   c++ -O3 -DNDEBUG -arch arm64 ... CMakeFiles/pandaPIengine.dir/Debug.cpp.o
     CMakeFiles/pandaPIengine.dir/Invariants.cpp.o ... -o pandaPIengine
     search/libsearch.a symbolic_search/libsymbolic_search.a
     heuristics/libheuristics.a heuristics/rcHeuristics/libhrc.a
     heuristics/landmarks/libhlm.a translation/libtranslation.a
     intDataStructures/libintDataStructures.a
     heuristics/libheuristics.a heuristics/rcHeuristics/libhrc.a
   ```

   Every archive named is a subdirectory of `pandaPIengine`'s own repo
   (confirmed by path — none point outside `upstream/pandaPIengine/`).

2. **`upstream/pandaPIengine/src/CMakeLists.txt`, read directly**
   (lines 82–108): the only *unconditional* `target_link_libraries`
   calls are the seven internal modules above. The three
   third-party-capable options are each behind a guard that our build
   never satisfies:
   - `libcudd.a` (BDD/symbolic planner) — inside `if (BDD)`;
     `build-engine.sh` invokes `cmake ../src -DCMAKE_BUILD_TYPE=Release`
     only, so `BDD` stays its documented default `OFF`.
   - `ilpheuristics` (CPLEX) — inside `if (CPLEX_SOURCE_DIR)`; never set.
   - `sat` — inside `if (SAT)`; default `OFF`, never set.

   This CMake logic carries **no platform conditional** around any of
   these three guards — the guard state (all closed) is identical on
   Linux and macOS by construction, which is why this file reading
   stands as evidence for the Linux binary too, alongside the direct
   macOS confirmation below.

**Binary-level confirmation (macOS)**: `otool -L
dist/macos-arm64/pandaPIengine`: only `libSystem.B.dylib`,
`libc++.1.dylib`. `nm | grep -i 'bliss\|lpsolve\|cplex\|cudd'`: **0
matches**.

## 3. License identification, every component enumerated in §2

| Component | License | Identified from | SHA |
|---|---|---|---|
| `pandaPIparser` | BSD 3-Clause (c) 2018–2021 Gregor Behnke | `pandaPIparser/LICENSE` | `88c0995` |
| `pandaPIgrounder` (own code) | BSD 3-Clause (c) 2020 Alexander Schmid, 2020–2021 Gregor Behnke | `pandaPIgrounder/LICENSE` | `b04ff7b` |
| `pandaPIengine` (own code) | BSD 3-Clause (c) 2017–2023 Daniel Höller, Gregor Behnke, David Speck, Felipe Meneguzzi, Florian Pollitt, Pascal Bercher | `pandaPIengine/LICENSE` | `810f043` |
| `h2-fd-preprocessor` | **GPL-3.0** (inherited from Fast Downward; no LICENSE file in this vendored copy — see §2.2.1) | `aibasel/downward`'s `LICENSE.md` (upstream provenance per this repo's own README) | `e335433` |
| `cpddl` | BSD 3-Clause (c) 2016 Daniel Fišer, CTU Prague | `cpddl/BSD-LICENSE` | `8c142ba` |
| `boruvka` | BSD 3-Clause (c) 2010– Daniel Fišer | `cpddl/third-party/boruvka/BSD-LICENSE` | `e320cc7` |
| `bliss` (not linked) | LGPL-3.0 | `bliss/COPYING.LESSER` (vendored in `cpddl/third-party/bliss-0.73.zip`) | n/a (vendored zip, not a pinned repo) |
| `lpsolve` (not linked) | LGPL | `lp_lib.h`'s own header text | n/a (vendored in `cpddl` third-party tree) |
| `opts` (not linked) | BSD 3-Clause | `cpddl/third-party/opts/BSD-LICENSE` | `819d90e` |

chengdu's own patches (`patches/bliss-0.73-cxx11-string-literal.patch`
and the two boruvka macOS patches, which ship inside the
`pandaPIgrounder` repo itself) touch only BSD-licensed code and are
themselves trivial, disclosed, single-purpose fixes — no separate
license question.

## 4. Obligations, and what satisfies them

**BSD 3-Clause components** (`pandaPIparser`, `pandaPIgrounder`'s own
code, `pandaPIengine`, `cpddl`, `boruvka`): the license's own condition
2 requires reproducing the copyright notice and license text "in the
documentation and/or other materials provided with the distribution."
Satisfied by: each component's full license text, verbatim, in
`THIRD-PARTY-LICENSES` (L-3).

**`h2-fd-preprocessor`, GPL-3.0** (linked into `pandaPIgrounder` only):
three obligations, all documentation-based per RQ2's default (no build
change):

1. The full GPL-3.0 license text accompanies the binary —
   `THIRD-PARTY-LICENSES`.
2. A **Source availability** section in the `v0.1.0` release notes,
   naming: `h2-fd-preprocessor`'s exact repo URL and SHA
   (`https://github.com/galvusdamor/h2-fd-preprocessor` @ `e335433`,
   already public, unmodified by chengdu), plus — for completeness of
   what "corresponding source" means for the whole `pandaPIgrounder`
   binary — `pandaPIgrounder`'s own repo+SHA, `cpddl`'s, `boruvka`'s,
   and chengdu's own patch set (`patches/`, already committed and
   public in this repo). GPL-3.0 §6 is satisfied by equivalent access
   to already-public source at no further charge; nothing here needs
   new hosting.
3. No copyleft-triggered build change — chengdu does not modify
   `h2-fd-preprocessor`'s source, so no additional source-disclosure
   burden beyond naming where it already lives.

**Not linked** (`bliss`, `lpsolve`, `opts`): no obligation; not included
in `THIRD-PARTY-LICENSES`.

## 5. RQ2 disposition (recommended)

Per arc-plan RQ2: the audit confirms a copyleft component
(`h2-fd-preprocessor`, GPL-3.0) is linked into one binary
(`pandaPIgrounder`, both platforms). Recommended disposition, for the
tracked arc-plan amendment: **documentation-based compliance**
(§4) — ship `THIRD-PARTY-LICENSES` with the GPL-3.0 text and a
Source-availability section in the release notes; no build change. This
is the default the slice-doc and cc-prompt both name, and nothing found
here makes a build change *necessary* (only a build change would be
required if chengdu modified GPL-covered source without disclosing it,
or if GPL's terms were somehow incompatible with the BSD components'
terms in a way that blocked distribution — neither applies: BSD and
GPL-3.0 combine without conflict when the GPL component's own terms are
met).
