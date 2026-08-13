# Source-quality selector and compile database surfaces for Arc07.

.PHONY: source-quality-surface
source-quality-surface:
	printf '%b\n' "$(BLUE)Writing source-quality selector surface for $(PLATFORM)...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" surface "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality surface report: $(SOURCE_QUALITY_DIR)/source-quality-surface.md$(RESET)"

.PHONY: source-quality-profile-map
source-quality-profile-map:
	printf '%b\n' "$(BLUE)Writing source-quality profile/source map for $(PLATFORM)...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" profile-map "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality profile map: $(SOURCE_QUALITY_DIR)/source-quality-profile-map.md$(RESET)"

.PHONY: source-quality-naming-report
source-quality-naming-report:
	printf '%b\n' "$(BLUE)Writing source-quality naming normalization report...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" naming-report "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality naming report: docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md$(RESET)"

.PHONY: source-quality-naming-check
source-quality-naming-check:
	printf '%b\n' "$(BLUE)Checking first-party source naming policy...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" naming-check "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality naming check passed$(RESET)"

.PHONY: compile-db-parser
compile-db-parser:
	printf '%b\n' "$(BLUE)Generating parser source-quality compile database...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" compile-db parser "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Parser compile database: $(SOURCE_QUALITY_COMPILE_DB_DIR)/parser/compile_commands.json$(RESET)"

.PHONY: compile-db-grounder
compile-db-grounder:
	printf '%b\n' "$(BLUE)Generating grounder source-quality compile database...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" compile-db grounder "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Grounder compile database: $(SOURCE_QUALITY_COMPILE_DB_DIR)/grounder/compile_commands.json$(RESET)"

.PHONY: compile-db-engine
compile-db-engine:
	printf '%b\n' "$(BLUE)Generating engine source-quality compile database...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" compile-db engine "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Engine compile database: $(SOURCE_QUALITY_COMPILE_DB_DIR)/engine/compile_commands.json$(RESET)"

.PHONY: compile-db-first-party
compile-db-first-party: compile-db-parser compile-db-grounder compile-db-engine
	printf '%b\n' "$(BLUE)Writing first-party compile database summary...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" compile-db-summary "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)First-party compile database summary: $(SOURCE_QUALITY_COMPILE_DB_DIR)/first-party-selected.txt$(RESET)"

.PHONY: source-quality-gate-report
source-quality-gate-report: source-quality-surface source-quality-profile-map source-quality-naming-check compile-db-first-party
	printf '%b\n' "$(BLUE)Writing first-party source-quality gate scaffold report...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" gate-report "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality gate scaffold report: docs/design-v0.3.0/arc07-source-quality-expansion/source-quality-gate-scaffold.md$(RESET)"

.PHONY: format-check-first-party
format-check-first-party: source-quality-gate-report format-check
	printf '%b\n' "$(GREEN)First-party format scaffold passed$(RESET)"

.PHONY: static-analysis-first-party
static-analysis-first-party: source-quality-gate-report static-analysis-cpp
	printf '%b\n' "$(GREEN)First-party static-analysis scaffold passed$(RESET)"

.PHONY: coverage-first-party
coverage-first-party: source-quality-gate-report coverage
	printf '%b\n' "$(GREEN)First-party coverage scaffold passed$(RESET)"

.PHONY: warning-inventory-first-party
warning-inventory-first-party: source-quality-gate-report warning-inventory
	printf '%b\n' "$(GREEN)First-party warning inventory scaffold passed$(RESET)"

.PHONY: generated-warning-triage
generated-warning-triage: source-quality-gate-report
	printf '%b\n' "$(BLUE)Writing generated-warning triage scaffold...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" generated-warning-triage "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Generated-warning triage scaffold: $(SOURCE_QUALITY_DIR)/generated-warning-triage.md$(RESET)"

.PHONY: generated-warning-triage-parser
generated-warning-triage-parser:
	printf '%b\n' "$(BLUE)Writing parser generated-warning root-cause evidence...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" generated-warning-triage-parser "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Parser generated-warning report: docs/design-v0.3.0/arc07-source-quality-expansion/parser-generated-warning-root-cause.md$(RESET)"
	printf '%b\n' "$(GREEN)Parser generated-warning evidence: $(SOURCE_QUALITY_DIR)/generated-warning-triage-parser.md$(RESET)"

.PHONY: format-check-grounder
format-check-grounder: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Checking grounder first-party format baseline...$(RESET)"; \
	$(resolve_clang_format); \
	mkdir -p "$(SOURCE_QUALITY_DIR)"; \
	files="$(SOURCE_QUALITY_DIR)/format-check-grounder-files.txt"; \
	drift="$(SOURCE_QUALITY_DIR)/format-check-grounder-drift.txt"; \
	find "$(GROUNDER_SOURCE_DIR)" "$(GROUNDER_TEST_DIR)" -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' -o -name '*.c' -o -name '*.cxx' \) \
	  ! -name 'cmdline.c' \
	  ! -name 'cmdline.h' \
	  -print | sort > "$$files"; \
	: > "$$drift"; \
	while IFS= read -r file; do \
	  [ -n "$$file" ] || continue; \
	  tmp="$$(mktemp)"; \
	  "$$CLANG_FORMAT_BIN" "$$file" > "$$tmp"; \
	  if ! cmp -s "$$file" "$$tmp"; then \
	    printf '%s\n' "$$file" >> "$$drift"; \
	  fi; \
	  rm -f "$$tmp"; \
	done < "$$files"; \
	file_count="$$(wc -l < "$$files" | tr -d ' ')"; \
	drift_count="$$(wc -l < "$$drift" | tr -d ' ')"; \
	{ \
	  printf '%s\n' "# format-check-grounder evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: format-check-grounder"; \
	  printf '%s\n' "Component: grounder"; \
	  printf '%s\n' "Source class: first-party maintained, including rss.c copied-origin RSS utility."; \
	  printf '%s\n' "Selector: $(GROUNDER_SOURCE_DIR) and $(GROUNDER_TEST_DIR), excluding generated cmdline.c/cmdline.h, cpddl, h2-fd-preprocessor, third-party, copied build roots, and BUILD_OUTPUT roots."; \
	  printf '%s\n' "Tool: $$CLANG_FORMAT_VERSION"; \
	  printf '%s\n' "Files checked: $$file_count"; \
	  printf '%s\n' "Current baseline drift entries: $$drift_count"; \
	  printf '%s\n' "Evidence files: $$files and $$drift"; \
	  printf '%s\n' "Policy: executable baseline check; re-entry converts recorded drift to strict formatting once a grounder formatting burndown is accepted."; \
	} > "$(GROUNDER_FORMAT_REPORT)"; \
	if [ "$$drift_count" -eq 0 ]; then \
	  printf '%b\n' "$(GREEN)Grounder format baseline has no drift$(RESET)"; \
	else \
	  printf '%b\n' "$(YELLOW)Grounder format baseline recorded $$drift_count drift entries: $$drift$(RESET)"; \
	fi; \
	printf '%b\n' "$(GREEN)Grounder format report: $(GROUNDER_FORMAT_REPORT)$(RESET)"

.PHONY: static-analysis-grounder
static-analysis-grounder: compile-db-grounder source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Running grounder first-party static-analysis baseline...$(RESET)"; \
	find_tool() { \
	  local name="$$1"; \
	  if command -v "$$name" >/dev/null 2>&1; then command -v "$$name"; return 0; fi; \
	  if command -v xcrun >/dev/null 2>&1; then xcrun --find "$$name" 2>/dev/null || true; fi; \
	  for path in "/opt/homebrew/opt/llvm/bin/$$name" "/opt/homebrew/opt/llvm@21/bin/$$name" "/usr/local/opt/llvm/bin/$$name"; do \
	    if [ -x "$$path" ]; then printf '%s\n' "$$path"; return 0; fi; \
	  done; \
	}; \
	mkdir -p "$(GROUNDER_STATIC_ANALYSIS_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	CLANG_TIDY_BIN="$${CLANG_TIDY:-$$(find_tool clang-tidy)}"; \
	report_md="$(SOURCE_QUALITY_DIR)/static-analysis-grounder.md"; \
	if [ -z "$$CLANG_TIDY_BIN" ]; then \
	  { \
	    printf '%s\n' "# static-analysis-grounder evidence"; \
	    printf '%s\n' ""; \
	    printf '%s\n' "Target: static-analysis-grounder"; \
	    printf '%s\n' "Status: SKIP, clang-tidy not found."; \
	    printf '%s\n' "Checks: $(CLANG_TIDY_CHECKS), including clang-analyzer."; \
	    printf '%s\n' "Input: compile-db-grounder first-party selected grounder translation units; generated cmdline output, cpddl, h2-fd-preprocessor, third-party, copied build roots, and BUILD_OUTPUT roots are excluded."; \
	    printf '%s\n' "Re-entry: install clang-tidy or set CLANG_TIDY=/path/to/clang-tidy and rerun make static-analysis-grounder."; \
	  } > "$$report_md"; \
	  printf '%b\n' "$(YELLOW)static-analysis-grounder: SKIP: missing clang-tidy$(RESET)"; \
	  exit 0; \
	fi; \
	files="$$(sed "s#^#$(CURDIR)/#" "$(SOURCE_QUALITY_COMPILE_DB_DIR)/grounder/first-party-selected.txt")"; \
	if [ -z "$$files" ]; then \
	  printf '%b\n' "$(RED)static-analysis-grounder: no grounder first-party selected files found$(RESET)" >&2; \
	  exit 1; \
	fi; \
	tmp="$(GROUNDER_STATIC_ANALYSIS_REPORT).tmp"; \
	if "$$CLANG_TIDY_BIN" -p "$(SOURCE_QUALITY_COMPILE_DB_DIR)/grounder" \
	  -checks="$(CLANG_TIDY_CHECKS)" \
	  -header-filter='(^|.*/)pandaPI/grounder/src/.*' \
	  $$files > "$$tmp" 2>&1; then \
	  tidy_status=0; \
	else \
	  tidy_status="$$?"; \
	fi; \
	mv "$$tmp" "$(GROUNDER_STATIC_ANALYSIS_REPORT)"; \
	finding_count="$$(awk '/warning:|error:/ { count++ } END { print count + 0 }' "$(GROUNDER_STATIC_ANALYSIS_REPORT)")"; \
	{ \
	  printf '%s\n' "# static-analysis-grounder evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: static-analysis-grounder"; \
	  printf '%s\n' "Tool: $$("$$CLANG_TIDY_BIN" --version | head -1)"; \
	  printf '%s\n' "Checks: $(CLANG_TIDY_CHECKS), including clang-analyzer."; \
	  printf '%s\n' "Input: compile-db-grounder first-party selected grounder translation units."; \
	  printf '%s\n' "Selector policy: generated cmdline output, cpddl, h2-fd-preprocessor, third-party, copied build roots, and BUILD_OUTPUT roots are excluded."; \
	  printf '%s\n' "Generated header policy: gengetopt cmdline.h and cpddl/boruvka config/template headers are generated under build/source-quality for analysis only; generated output is not committed or first-party maintained."; \
	  printf '%s\n' "clang-tidy exit status: $$tidy_status"; \
	  printf '%s\n' "Reported finding lines: $$finding_count"; \
	  printf '%s\n' "Raw report: $(GROUNDER_STATIC_ANALYSIS_REPORT)"; \
	  printf '%s\n' "Policy: executable reported baseline; re-entry decides which analyzer findings become strict grounder release blockers."; \
	} > "$$report_md"; \
	if [ "$$tidy_status" -eq 0 ]; then \
	  printf '%b\n' "$(GREEN)Grounder static-analysis baseline completed with no clang-tidy errors$(RESET)"; \
	else \
	  printf '%b\n' "$(YELLOW)Grounder static-analysis baseline reported clang-tidy status $$tidy_status: $(GROUNDER_STATIC_ANALYSIS_REPORT)$(RESET)"; \
	fi

.PHONY: test-unit-grounder
test-unit-grounder: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Running grounder unit seam tests...$(RESET)"; \
	mkdir -p "$(GROUNDER_UNIT_BUILD_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	exe="$(GROUNDER_UNIT_BUILD_DIR)/grounder_top_sort_seam_test"; \
	"$${CXX:-c++}" -std=gnu++20 -Wall -Wextra -pedantic -O0 -g \
	  -I"$(GROUNDER_SOURCE_DIR)" \
	  "$(GROUNDER_TEST_DIR)/top_sort_seam_test.cpp" \
	  "$(GROUNDER_SOURCE_DIR)/util.cpp" \
	  -o "$$exe"; \
	"$$exe"; \
	{ \
	  printf '%s\n' "# test-unit-grounder evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: test-unit-grounder"; \
	  printf '%s\n' "Scope: grounder first-party unit seam."; \
	  printf '%s\n' "Seam: util.cpp topsort dependency ordering contract."; \
	  printf '%s\n' "Harness: direct compiled seam test at $(GROUNDER_TEST_DIR)/top_sort_seam_test.cpp."; \
	  printf '%s\n' "CTest: not required for this smallest grounder seam."; \
	  printf '%s\n' "Catch2: not required for this smallest grounder seam."; \
	  printf '%s\n' "Contract boundary: managed process fixtures remain black-box evidence, not this unit seam."; \
	} > "$(GROUNDER_TEST_UNIT_REPORT)"; \
	printf '%b\n' "$(GREEN)Grounder unit seam report: $(GROUNDER_TEST_UNIT_REPORT)$(RESET)"

.PHONY: coverage-grounder
coverage-grounder: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Running grounder topsort seam coverage for $(PLATFORM)...$(RESET)"; \
	find_tool() { \
	  local name="$$1"; \
	  if command -v "$$name" >/dev/null 2>&1; then command -v "$$name"; return 0; fi; \
	  if command -v xcrun >/dev/null 2>&1; then xcrun --find "$$name" 2>/dev/null || true; fi; \
	}; \
	CXX_BIN="$${CXX:-$$(find_tool clang++)}"; \
	LLVM_PROFDATA_BIN="$${LLVM_PROFDATA:-$$(find_tool llvm-profdata)}"; \
	LLVM_COV_BIN="$${LLVM_COV:-$$(find_tool llvm-cov)}"; \
	if [ -z "$$CXX_BIN" ] || [ -z "$$LLVM_PROFDATA_BIN" ] || [ -z "$$LLVM_COV_BIN" ]; then \
	  printf '%b\n' "$(RED)coverage-grounder: clang++, llvm-profdata, and llvm-cov are required$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$(GROUNDER_COVERAGE_BUILD_DIR)" "$(GROUNDER_COVERAGE_REPORT_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	exe="$(GROUNDER_COVERAGE_BUILD_DIR)/grounder_top_sort_seam_test"; \
	COVERAGE_FLAGS="-O0 -g -fprofile-instr-generate -fcoverage-mapping"; \
	"$$CXX_BIN" -std=gnu++20 -Wall -Wextra -pedantic $$COVERAGE_FLAGS \
	  -I"$(GROUNDER_SOURCE_DIR)" \
	  "$(GROUNDER_TEST_DIR)/top_sort_seam_test.cpp" \
	  "$(GROUNDER_SOURCE_DIR)/util.cpp" \
	  -o "$$exe"; \
	PROFILE_DIR="$$(mktemp -d "$(CURDIR)/$(GROUNDER_COVERAGE_DIR)/profiles.XXXXXX")"; \
	LLVM_PROFILE_FILE="$$PROFILE_DIR/%m-%p.profraw" "$$exe"; \
	PROFILE_LIST="$(GROUNDER_COVERAGE_DIR)/profiles.txt"; \
	find "$$PROFILE_DIR" -name '*.profraw' -print | sort > "$$PROFILE_LIST"; \
	if [ ! -s "$$PROFILE_LIST" ]; then \
	  printf '%b\n' "$(RED)coverage-grounder: no .profraw files were produced by grounder seam test$(RESET)" >&2; \
	  exit 1; \
	fi; \
	"$$LLVM_PROFDATA_BIN" merge -sparse --input-files="$$PROFILE_LIST" -o "$(GROUNDER_COVERAGE_PROFDATA)"; \
	ignore_regex='(^|/)(build|dist|release)(/|$$)|(^|/)pandaPI/grounder/(cpddl|h2-fd-preprocessor)(/|$$)|(^|/)pandaPI/grounder/src/cmdline\.(c|h)$$'; \
	"$$LLVM_COV_BIN" report "$$exe" \
	  -instr-profile="$(GROUNDER_COVERAGE_PROFDATA)" \
	  -ignore-filename-regex="$$ignore_regex" > "$(GROUNDER_COVERAGE_SUMMARY)"; \
	"$$LLVM_COV_BIN" show "$$exe" \
	  -instr-profile="$(GROUNDER_COVERAGE_PROFDATA)" \
	  -ignore-filename-regex="$$ignore_regex" \
	  -format=text > "$(GROUNDER_COVERAGE_DETAIL)"; \
	{ \
	  printf '%s\n' "# coverage-grounder evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: coverage-grounder"; \
	  printf '%s\n' "Scope: grounder first-party topsort unit seam baseline."; \
	  printf '%s\n' "Profile map: source-quality-profile-map maps copied build roots back to canonical source."; \
	  printf '%s\n' "Source class policy: first-party maintained grounder source only; generated cmdline output, cpddl, h2-fd-preprocessor, third-party, copied roots, dependency, dist, release, and BUILD_OUTPUT roots are excluded."; \
	  printf '%s\n' "Summary: $(GROUNDER_COVERAGE_SUMMARY)"; \
	  printf '%s\n' "Detail: $(GROUNDER_COVERAGE_DETAIL)"; \
	  printf '%s\n' "Policy: measured baseline only; no release floor."; \
	} > "$(SOURCE_QUALITY_DIR)/coverage-grounder.md"; \
	printf '%b\n' "$(GREEN)Grounder coverage summary: $(GROUNDER_COVERAGE_SUMMARY)$(RESET)"; \
	cat "$(GROUNDER_COVERAGE_SUMMARY)"

.PHONY: warning-inventory-grounder
warning-inventory-grounder: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Capturing grounder compiler warning inventory...$(RESET)"; \
	mkdir -p "$(WARNING_INVENTORY_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	tmp_log="$(WARNING_INVENTORY_DIR)/grounder-build.log.tmp"; \
	grounder_log="$(WARNING_INVENTORY_DIR)/grounder-build.log"; \
	grounder_inventory="$(WARNING_INVENTORY_DIR)/grounder-warning-inventory.txt"; \
	if $(MAKE) build-grounder > "$$tmp_log" 2>&1; then \
	  build_status=0; \
	else \
	  build_status="$$?"; \
	fi; \
	mv "$$tmp_log" "$$grounder_log"; \
	{ \
	  printf '%s\n' "chengdu grounder compiler warning inventory"; \
	  printf '%s\n' "command: make build-grounder"; \
	  printf '%s\n' "platform: $(PLATFORM)"; \
	  printf '%s\n' "commit: $(GIT_COMMIT)"; \
	  printf '%s\n' "log: $$grounder_log"; \
	  printf '%s\n' ""; \
	  awk '/warning:|warnings generated|ld: warning/ { printf "%d:%s\n", NR, $$0 }' "$$grounder_log"; \
	} > "$$grounder_inventory"; \
	first_party_matches="$$(rg -n -- 'src/(conditional_effects|debug|duplicate|fam_mutexes|given_plan|gpg|grounded_gpg|grounding|h2_mutexes|hierarchy_typing|lifted_gpg|main|model|naive_grounding|output|pandapi_grounder_native|parser|postprocessing|rss|sasinvariants|sasplus|util)\\.(cpp|c):.*warning:' "$$grounder_inventory" || true)"; \
	first_party_count="$$(printf '%s\n' "$$first_party_matches" | sed '/^$$/d' | wc -l | tr -d ' ')"; \
	rss_status="closed: rss.c first-party maintained copied-origin RSS utility has no remaining warning"; \
	if rg -n -- 'src/rss\\.c:.*warning:|rss\\.c.*warning:' "$$grounder_inventory" >/dev/null 2>&1; then \
	  rss_status="blocked: rss.c first-party maintained warning remains"; \
	fi; \
	dependency_count="$$(rg -n -- 'mergesort\\.c|tasks\\.c|task-pool\\.c|fifo-sem\\.c|ring_queue\\.c|opts\\.(c|h)|test2?\\.c|lp_|rss\\.c|h2_mutexes\\.cc|warnings generated|-O9' "$$grounder_inventory" | wc -l | tr -d ' ' || true)"; \
	{ \
	  printf '%s\n' "# warning-inventory-grounder evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: warning-inventory-grounder"; \
	  printf '%s\n' "Scope: grounder warning inventory with source-class routing."; \
	  printf '%s\n' "Input command: make build-grounder"; \
	  printf '%s\n' "Inventory: $$grounder_inventory"; \
	  printf '%s\n' "First-party maintained warning count: $$first_party_count"; \
	  printf '%s\n' "RSS chosen disposition: first-party maintained copied-origin RSS utility; $$rss_status."; \
	  printf '%s\n' "Generated cmdline disposition: cmdline.c and cmdline.h are generated from options.ggo through gengetopt; managed native grounder behavior owns process-contract evidence."; \
	  printf '%s\n' "Dependency warning classes: cpddl, boruvka, opts, lpsolve, h2-fd-preprocessor, third-party, copied, and BUILD_OUTPUT paths are routed separately if present; observed dependency-class evidence lines: $$dependency_count."; \
	  printf '%s\n' "Re-entry: dependency audit, supported FAM/H2 decision, strict generated-code policy, or first-party warning recurrence."; \
	} > "$(GROUNDER_WARNING_REPORT)"; \
	cat "$$grounder_inventory"; \
	if [ "$$build_status" -ne 0 ]; then \
	  printf '%b\n' "$(RED)warning-inventory-grounder: build failed; inspect $$grounder_log$(RESET)" >&2; \
	  exit "$$build_status"; \
	fi; \
	if [ "$$first_party_count" -ne 0 ]; then \
	  printf '%s\n' "$$first_party_matches" >&2; \
	  printf '%b\n' "$(RED)warning-inventory-grounder: first-party grounder warnings remain$(RESET)" >&2; \
	  exit 1; \
	fi; \
	printf '%b\n' "$(GREEN)Grounder warning inventory report: $(GROUNDER_WARNING_REPORT)$(RESET)"

.PHONY: sanitize-grounder
sanitize-grounder: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Running grounder sanitizer seam tests...$(RESET)"; \
	find_tool() { \
	  local name="$$1"; \
	  if command -v "$$name" >/dev/null 2>&1; then command -v "$$name"; return 0; fi; \
	  if command -v xcrun >/dev/null 2>&1; then xcrun --find "$$name" 2>/dev/null || true; fi; \
	}; \
	CXX_BIN="$${CXX:-$$(find_tool clang++)}"; \
	if [ -z "$$CXX_BIN" ]; then \
	  printf '%b\n' "$(RED)sanitize-grounder: clang++ is required for ASan/UBSan$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$(GROUNDER_SANITIZE_BUILD_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	exe="$(GROUNDER_SANITIZE_BUILD_DIR)/grounder_top_sort_seam_test"; \
	"$$CXX_BIN" -std=gnu++20 -Wall -Wextra -pedantic $(BINARY_SANITIZER_FLAGS) \
	  -I"$(GROUNDER_SOURCE_DIR)" \
	  "$(GROUNDER_TEST_DIR)/top_sort_seam_test.cpp" \
	  "$(GROUNDER_SOURCE_DIR)/util.cpp" \
	  $(BINARY_SANITIZER_LINK_FLAGS) \
	  -o "$$exe"; \
	ASAN_OPTIONS="$(BINARY_SANITIZER_ASAN_OPTIONS)" UBSAN_OPTIONS="$(BINARY_SANITIZER_UBSAN_OPTIONS)" "$$exe"; \
	{ \
	  printf '%s\n' "# sanitize-grounder evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: sanitize-grounder"; \
	  printf '%s\n' "Scope: grounder first-party topsort unit seam under ASan/UBSan."; \
	  printf '%s\n' "ASan: enabled through $(BINARY_SANITIZER_FLAGS)."; \
	  printf '%s\n' "UBSan: enabled through $(BINARY_SANITIZER_FLAGS)."; \
	  printf '%s\n' "LSan: $(BINARY_SANITIZER_LSAN_STATUS)"; \
	  printf '%s\n' "TSan: re-entry only; no accepted grounder concurrency workload in this source-quality slice."; \
	  printf '%s\n' "Binary/source class policy: managed binary sanitizer findings still map copied paths back to first-party, generated, dependency, h2-fd-preprocessor, cpddl, third-party, copied, or BUILD_OUTPUT classes before ownership is assigned."; \
	  printf '%s\n' "Re-entry: ASan, UBSan, LSan, TSan, binary, source class, copied, dependency, or third-party finding in grounder evidence."; \
	} > "$(GROUNDER_SANITIZE_REPORT)"; \
	printf '%b\n' "$(GREEN)Grounder sanitizer report: $(GROUNDER_SANITIZE_REPORT)$(RESET)"

.PHONY: test-unit
test-unit: source-quality-gate-report test-runtime
	printf '%b\n' "$(GREEN)Unit/seam test scaffold passed$(RESET)"

.PHONY: sanitize-first-party
sanitize-first-party: source-quality-gate-report sanitize-runtime
	printf '%b\n' "$(GREEN)First-party sanitizer scaffold passed$(RESET)"

.PHONY: format-check-engine
format-check-engine: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Checking engine first-party format baseline...$(RESET)"; \
	$(resolve_clang_format); \
	mkdir -p "$(SOURCE_QUALITY_DIR)"; \
	files="$(SOURCE_QUALITY_DIR)/format-check-engine-files.txt"; \
	drift="$(SOURCE_QUALITY_DIR)/format-check-engine-drift.txt"; \
	find "$(ENGINE_SOURCE_DIR)" -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \) \
	  ! -path '*/symbolic_search/cudd-3.0.0/*' \
	  ! -name 'cmdline.c' \
	  ! -name 'cmdline.h' \
	  -print | sort > "$$files"; \
	: > "$$drift"; \
	while IFS= read -r file; do \
	  [ -n "$$file" ] || continue; \
	  tmp="$$(mktemp)"; \
	  "$$CLANG_FORMAT_BIN" "$$file" > "$$tmp"; \
	  if ! cmp -s "$$file" "$$tmp"; then \
	    printf '%s\n' "$$file" >> "$$drift"; \
	  fi; \
	  rm -f "$$tmp"; \
	done < "$$files"; \
	file_count="$$(wc -l < "$$files" | tr -d ' ')"; \
	drift_count="$$(wc -l < "$$drift" | tr -d ' ')"; \
	{ \
	  printf '%s\n' "# format-check-engine evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: format-check-engine"; \
	  printf '%s\n' "Component: engine"; \
	  printf '%s\n' "Source class: first-party maintained"; \
	  printf '%s\n' "Selector: $(ENGINE_SOURCE_DIR), excluding generated cmdline.c/cmdline.h, CUDD third-party source, copied build roots, and BUILD_OUTPUT roots."; \
	  printf '%s\n' "Tool: $$CLANG_FORMAT_VERSION"; \
	  printf '%s\n' "Files checked: $$file_count"; \
	  printf '%s\n' "Current baseline drift entries: $$drift_count"; \
	  printf '%s\n' "Evidence files: $$files and $$drift"; \
	  printf '%s\n' "Policy: executable baseline check; re-entry converts recorded drift to strict formatting once the engine formatting burndown is accepted."; \
	} > "$(ENGINE_FORMAT_REPORT)"; \
	if [ "$$drift_count" -eq 0 ]; then \
	  printf '%b\n' "$(GREEN)Engine format baseline has no drift$(RESET)"; \
	else \
	  printf '%b\n' "$(YELLOW)Engine format baseline recorded $$drift_count drift entries: $$drift$(RESET)"; \
	fi; \
	printf '%b\n' "$(GREEN)Engine format report: $(ENGINE_FORMAT_REPORT)$(RESET)"

.PHONY: static-analysis-engine
static-analysis-engine: compile-db-engine source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Running engine first-party static-analysis baseline...$(RESET)"; \
	find_tool() { \
	  local name="$$1"; \
	  if command -v "$$name" >/dev/null 2>&1; then command -v "$$name"; return 0; fi; \
	  if command -v xcrun >/dev/null 2>&1; then xcrun --find "$$name" 2>/dev/null || true; fi; \
	  for path in "/opt/homebrew/opt/llvm/bin/$$name" "/opt/homebrew/opt/llvm@21/bin/$$name" "/usr/local/opt/llvm/bin/$$name"; do \
	    if [ -x "$$path" ]; then printf '%s\n' "$$path"; return 0; fi; \
	  done; \
	}; \
	mkdir -p "$(ENGINE_STATIC_ANALYSIS_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	CLANG_TIDY_BIN="$${CLANG_TIDY:-$$(find_tool clang-tidy)}"; \
	report_md="$(SOURCE_QUALITY_DIR)/static-analysis-engine.md"; \
	if [ -z "$$CLANG_TIDY_BIN" ]; then \
	  { \
	    printf '%s\n' "# static-analysis-engine evidence"; \
	    printf '%s\n' ""; \
	    printf '%s\n' "Target: static-analysis-engine"; \
	    printf '%s\n' "Status: SKIP, clang-tidy not found."; \
	    printf '%s\n' "Checks: $(CLANG_TIDY_CHECKS), including clang-analyzer."; \
	    printf '%s\n' "Input: compile-db-engine first-party selected engine translation units; generated cmdline output and CUDD are excluded."; \
	    printf '%s\n' "Re-entry: install clang-tidy or set CLANG_TIDY=/path/to/clang-tidy and rerun make static-analysis-engine."; \
	  } > "$$report_md"; \
	  printf '%b\n' "$(YELLOW)static-analysis-engine: SKIP: missing clang-tidy$(RESET)"; \
	  exit 0; \
	fi; \
	files="$$(sed "s#^#$(CURDIR)/#" "$(SOURCE_QUALITY_COMPILE_DB_DIR)/engine/first-party-selected.txt")"; \
	if [ -z "$$files" ]; then \
	  printf '%b\n' "$(RED)static-analysis-engine: no engine first-party selected files found$(RESET)" >&2; \
	  exit 1; \
	fi; \
	tmp="$(ENGINE_STATIC_ANALYSIS_REPORT).tmp"; \
	if "$$CLANG_TIDY_BIN" -p "$(SOURCE_QUALITY_COMPILE_DB_DIR)/engine" \
	  -checks="$(CLANG_TIDY_CHECKS)" \
	  -header-filter='(^|.*/)pandaPI/engine/src/.*' \
	  $$files > "$$tmp" 2>&1; then \
	  tidy_status=0; \
	else \
	  tidy_status="$$?"; \
	fi; \
	mv "$$tmp" "$(ENGINE_STATIC_ANALYSIS_REPORT)"; \
	finding_count="$$(awk '/warning:|error:/ { count++ } END { print count + 0 }' "$(ENGINE_STATIC_ANALYSIS_REPORT)")"; \
	{ \
	  printf '%s\n' "# static-analysis-engine evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: static-analysis-engine"; \
	  printf '%s\n' "Tool: $$("$$CLANG_TIDY_BIN" --version | head -1)"; \
	  printf '%s\n' "Checks: $(CLANG_TIDY_CHECKS), including clang-analyzer."; \
	  printf '%s\n' "Input: compile-db-engine first-party selected engine translation units."; \
	  printf '%s\n' "Selector policy: generated cmdline output, CUDD third-party source, copied build roots, and BUILD_OUTPUT roots are excluded."; \
	  printf '%s\n' "clang-tidy exit status: $$tidy_status"; \
	  printf '%s\n' "Reported finding lines: $$finding_count"; \
	  printf '%s\n' "Raw report: $(ENGINE_STATIC_ANALYSIS_REPORT)"; \
	  printf '%s\n' "Policy: executable reported baseline; re-entry decides which analyzer findings become strict engine release blockers."; \
	} > "$$report_md"; \
	if [ "$$tidy_status" -eq 0 ]; then \
	  printf '%b\n' "$(GREEN)Engine static-analysis baseline completed with no clang-tidy errors$(RESET)"; \
	else \
	  printf '%b\n' "$(YELLOW)Engine static-analysis baseline reported clang-tidy status $$tidy_status: $(ENGINE_STATIC_ANALYSIS_REPORT)$(RESET)"; \
	fi

.PHONY: test-unit-engine
test-unit-engine: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Running engine CTest seam tests...$(RESET)"; \
	mkdir -p "$(ENGINE_UNIT_BUILD_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	cmake -S "$(ENGINE_SOURCE_DIR)" -B "$(ENGINE_UNIT_BUILD_DIR)" \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -DPANDAPI_RUNTIME_INCLUDE="$(CURDIR)/$(RUNTIME_INCLUDE_DIR)"; \
	cmake --build "$(ENGINE_UNIT_BUILD_DIR)" --target pandapi_engine_visited_list_payload_test --parallel; \
	( cd "$(ENGINE_UNIT_BUILD_DIR)" && ctest --output-on-failure -C Debug -R pandapi_engine_visited_list_payload ); \
	{ \
	  printf '%s\n' "# test-unit-engine evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: test-unit-engine"; \
	  printf '%s\n' "Scope: engine first-party unit seam."; \
	  printf '%s\n' "Seam: visited_list payload representation encode/decode contract."; \
	  printf '%s\n' "Harness: CTest target pandapi_engine_visited_list_payload."; \
	  printf '%s\n' "Catch2: not required for this smallest engine seam."; \
	  printf '%s\n' "Contract boundary: managed process fixtures remain black-box evidence, not this unit seam."; \
	} > "$(ENGINE_TEST_UNIT_REPORT)"; \
	printf '%b\n' "$(GREEN)Engine unit seam report: $(ENGINE_TEST_UNIT_REPORT)$(RESET)"

.PHONY: coverage-engine
coverage-engine: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Running engine visited_list seam coverage for $(PLATFORM)...$(RESET)"; \
	find_tool() { \
	  local name="$$1"; \
	  if command -v "$$name" >/dev/null 2>&1; then command -v "$$name"; return 0; fi; \
	  if command -v xcrun >/dev/null 2>&1; then xcrun --find "$$name" 2>/dev/null || true; fi; \
	}; \
	CXX_BIN="$${CXX:-$$(find_tool clang++)}"; \
	LLVM_PROFDATA_BIN="$${LLVM_PROFDATA:-$$(find_tool llvm-profdata)}"; \
	LLVM_COV_BIN="$${LLVM_COV:-$$(find_tool llvm-cov)}"; \
	if [ -z "$$CXX_BIN" ] || [ -z "$$LLVM_PROFDATA_BIN" ] || [ -z "$$LLVM_COV_BIN" ]; then \
	  printf '%b\n' "$(RED)coverage-engine: clang++, llvm-profdata, and llvm-cov are required$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$(ENGINE_COVERAGE_BUILD_DIR)" "$(ENGINE_COVERAGE_REPORT_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	COVERAGE_FLAGS="-O0 -g -fprofile-instr-generate -fcoverage-mapping"; \
	cmake -S "$(ENGINE_SOURCE_DIR)" -B "$(ENGINE_COVERAGE_BUILD_DIR)" \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -DCMAKE_CXX_COMPILER="$$CXX_BIN" \
	  -DCMAKE_CXX_FLAGS="$$COVERAGE_FLAGS" \
	  -DCMAKE_EXE_LINKER_FLAGS="-fprofile-instr-generate -fcoverage-mapping" \
	  -DCMAKE_SHARED_LINKER_FLAGS="-fprofile-instr-generate -fcoverage-mapping" \
	  -DPANDAPI_RUNTIME_INCLUDE="$(CURDIR)/$(RUNTIME_INCLUDE_DIR)"; \
	cmake --build "$(ENGINE_COVERAGE_BUILD_DIR)" --target pandapi_engine_visited_list_payload_test --parallel; \
	PROFILE_DIR="$$(mktemp -d "$(CURDIR)/$(ENGINE_COVERAGE_DIR)/profiles.XXXXXX")"; \
	( cd "$(ENGINE_COVERAGE_BUILD_DIR)" && LLVM_PROFILE_FILE="$$PROFILE_DIR/%m-%p.profraw" ctest --output-on-failure -C Debug -R pandapi_engine_visited_list_payload ); \
	PROFILE_LIST="$(ENGINE_COVERAGE_DIR)/profiles.txt"; \
	find "$$PROFILE_DIR" -name '*.profraw' -print | sort > "$$PROFILE_LIST"; \
	if [ ! -s "$$PROFILE_LIST" ]; then \
	  printf '%b\n' "$(RED)coverage-engine: no .profraw files were produced by engine CTest$(RESET)" >&2; \
	  exit 1; \
	fi; \
	"$$LLVM_PROFDATA_BIN" merge -sparse --input-files="$$PROFILE_LIST" -o "$(ENGINE_COVERAGE_PROFDATA)"; \
	ignore_regex='(^|/)(build|dist|release)(/|$$)|(^|/)pandaPI/engine/src/symbolic_search/cudd-3.0.0(/|$$)|(^|/)pandaPI/engine/src/cmdline\.(c|h)$$'; \
	"$$LLVM_COV_BIN" report "$(ENGINE_COVERAGE_BUILD_DIR)/pandapi_engine_visited_list_payload_test" \
	  -instr-profile="$(ENGINE_COVERAGE_PROFDATA)" \
	  -ignore-filename-regex="$$ignore_regex" > "$(ENGINE_COVERAGE_SUMMARY)"; \
	"$$LLVM_COV_BIN" show "$(ENGINE_COVERAGE_BUILD_DIR)/pandapi_engine_visited_list_payload_test" \
	  -instr-profile="$(ENGINE_COVERAGE_PROFDATA)" \
	  -ignore-filename-regex="$$ignore_regex" \
	  -format=text > "$(ENGINE_COVERAGE_DETAIL)"; \
	{ \
	  printf '%s\n' "# coverage-engine evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: coverage-engine"; \
	  printf '%s\n' "Scope: engine first-party visited_list unit seam."; \
	  printf '%s\n' "Profile map: source-quality-profile-map maps copied build roots back to canonical source."; \
	  printf '%s\n' "Exclusions: generated cmdline output, CUDD, BUILD_OUTPUT, dist, and release roots."; \
	  printf '%s\n' "Summary: $(ENGINE_COVERAGE_SUMMARY)"; \
	  printf '%s\n' "Detail: $(ENGINE_COVERAGE_DETAIL)"; \
	  printf '%s\n' "Policy: measured source-quality baseline only; no release floor."; \
	} > "$(SOURCE_QUALITY_DIR)/coverage-engine.md"; \
	printf '%b\n' "$(GREEN)Engine coverage summary: $(ENGINE_COVERAGE_SUMMARY)$(RESET)"; \
	cat "$(ENGINE_COVERAGE_SUMMARY)"

.PHONY: warning-inventory-engine
warning-inventory-engine: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Capturing engine compiler warning inventory...$(RESET)"; \
	mkdir -p "$(WARNING_INVENTORY_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	rm -f "$(WARNING_INVENTORY_DIR)"/*.log "$(WARNING_INVENTORY_DIR)"/*.txt; \
	tmp_log="$(WARNING_INVENTORY_DIR)/engine-build.log.tmp"; \
	engine_log="$(WARNING_INVENTORY_DIR)/engine-build.log"; \
	engine_inventory="$(WARNING_INVENTORY_DIR)/engine-warning-inventory.txt"; \
	if $(MAKE) build-engine > "$$tmp_log" 2>&1; then \
	  build_status=0; \
	else \
	  build_status="$$?"; \
	fi; \
	mv "$$tmp_log" "$$engine_log"; \
	{ \
	  printf '%s\n' "chengdu engine compiler warning inventory"; \
	  printf '%s\n' "command: make build-engine"; \
	  printf '%s\n' "platform: $(PLATFORM)"; \
	  printf '%s\n' "commit: $(GIT_COMMIT)"; \
	  printf '%s\n' "log: $$engine_log"; \
	  printf '%s\n' ""; \
	  awk '/warning:|warnings generated|ld: warning/ { printf "%d:%s\n", NR, $$0 }' "$$engine_log"; \
	} > "$$engine_inventory"; \
	duplicate_status="not observed"; \
	if rg -n -- "ignoring duplicate libraries|heuristics/libheuristics|heuristics/rc_heuristics/libhrc" "$$engine_inventory" >/dev/null 2>&1; then \
	  duplicate_status="observed and classified as duplicate link-library re-entry for libheuristics/libhrc"; \
	fi; \
	visited_payload_status="closed"; \
	if rg -n -- "visited_list.*int-to-void-pointer|visited_list.cpp.*cast to 'void \\*'|visited_list.cpp.*-Wint-to-void-pointer-cast" "$$engine_log" "$$engine_inventory" >/dev/null 2>&1; then \
	  visited_payload_status="blocked"; \
	fi; \
	{ \
	  printf '%s\n' "# warning-inventory-engine evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: warning-inventory-engine"; \
	  printf '%s\n' "Scope: engine first-party warning inventory with copied-build path mapping."; \
	  printf '%s\n' "visited_list payload representation status: $$visited_payload_status"; \
	  printf '%s\n' "duplicate link-library status: $$duplicate_status"; \
	  printf '%s\n' "Input command: make build-engine"; \
	  printf '%s\n' "Inventory: $$engine_inventory"; \
	  printf '%s\n' "Policy: generated cmdline output, CUDD, dependency, copied build, and BUILD_OUTPUT paths remain separate source classes."; \
	  printf '%s\n' "Re-entry: if duplicate linker diagnostics return, classify libheuristics/libhrc ownership before strict warning policy."; \
	} > "$(ENGINE_WARNING_REPORT)"; \
	cat "$$engine_inventory"; \
	if [ "$$build_status" -ne 0 ]; then \
	  printf '%b\n' "$(RED)warning-inventory-engine: build failed; inspect $$engine_log$(RESET)" >&2; \
	  exit "$$build_status"; \
	fi; \
	if [ "$$visited_payload_status" = "blocked" ]; then \
	  printf '%b\n' "$(RED)warning-inventory-engine: visited_list payload warning remains$(RESET)" >&2; \
	  exit 1; \
	fi; \
	printf '%b\n' "$(GREEN)Engine warning inventory report: $(ENGINE_WARNING_REPORT)$(RESET)"

.PHONY: sanitize-engine
sanitize-engine: source-quality-profile-map source-quality-naming-check
	set -e; \
	printf '%b\n' "$(BLUE)Running engine sanitizer seam tests...$(RESET)"; \
	find_tool() { \
	  local name="$$1"; \
	  if command -v "$$name" >/dev/null 2>&1; then command -v "$$name"; return 0; fi; \
	  if command -v xcrun >/dev/null 2>&1; then xcrun --find "$$name" 2>/dev/null || true; fi; \
	}; \
	CXX_BIN="$${CXX:-$$(find_tool clang++)}"; \
	if [ -z "$$CXX_BIN" ]; then \
	  printf '%b\n' "$(RED)sanitize-engine: clang++ is required for ASan/UBSan$(RESET)" >&2; \
	  exit 1; \
	fi; \
	SANITIZER_FLAGS="$(BINARY_SANITIZER_FLAGS)"; \
	mkdir -p "$(ENGINE_SANITIZE_BUILD_DIR)" "$(SOURCE_QUALITY_DIR)"; \
	cmake -S "$(ENGINE_SOURCE_DIR)" -B "$(ENGINE_SANITIZE_BUILD_DIR)" \
	  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -DCMAKE_CXX_COMPILER="$$CXX_BIN" \
	  -DCMAKE_CXX_FLAGS="$$SANITIZER_FLAGS" \
	  -DCMAKE_EXE_LINKER_FLAGS="$(BINARY_SANITIZER_LINK_FLAGS)" \
	  -DCMAKE_SHARED_LINKER_FLAGS="$(BINARY_SANITIZER_LINK_FLAGS)" \
	  -DPANDAPI_RUNTIME_INCLUDE="$(CURDIR)/$(RUNTIME_INCLUDE_DIR)"; \
	cmake --build "$(ENGINE_SANITIZE_BUILD_DIR)" --target pandapi_engine_visited_list_payload_test --parallel; \
	( cd "$(ENGINE_SANITIZE_BUILD_DIR)" && ASAN_OPTIONS="$(BINARY_SANITIZER_ASAN_OPTIONS)" UBSAN_OPTIONS="$(BINARY_SANITIZER_UBSAN_OPTIONS)" ctest --output-on-failure -C RelWithDebInfo -R pandapi_engine_visited_list_payload ); \
	{ \
	  printf '%s\n' "# sanitize-engine evidence"; \
	  printf '%s\n' ""; \
	  printf '%s\n' "Target: sanitize-engine"; \
	  printf '%s\n' "Scope: engine first-party unit seam under ASan/UBSan."; \
	  printf '%s\n' "ASan: enabled through $(BINARY_SANITIZER_FLAGS)."; \
	  printf '%s\n' "UBSan: enabled through $(BINARY_SANITIZER_FLAGS)."; \
	  printf '%s\n' "LSan: $(BINARY_SANITIZER_LSAN_STATUS)"; \
	  printf '%s\n' "TSan: re-entry only; no accepted engine concurrency workload in this source-quality slice."; \
	  printf '%s\n' "Binary/source class policy: managed binary sanitizer findings still map copied paths back to first-party, generated, dependency, CUDD, copied, or BUILD_OUTPUT classes before ownership is assigned."; \
	} > "$(ENGINE_SANITIZE_REPORT)"; \
	printf '%b\n' "$(GREEN)Engine sanitizer report: $(ENGINE_SANITIZE_REPORT)$(RESET)"
