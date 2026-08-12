# Coverage targets for chengdu-owned runtime source.

.PHONY: coverage
coverage:
	set -e; \
	printf '%b\n' "$(BLUE)Running pandapi-runtime Clang source coverage for $(PLATFORM)...$(RESET)"; \
	find_tool() { \
	  local name="$$1"; \
	  if command -v "$$name" >/dev/null 2>&1; then \
	    command -v "$$name"; \
	    return 0; \
	  fi; \
	  if command -v xcrun >/dev/null 2>&1; then \
	    xcrun --find "$$name" 2>/dev/null || true; \
	  fi; \
	}; \
	CXX_BIN="$${CXX:-$$(find_tool clang++)}"; \
	LLVM_PROFDATA_BIN="$${LLVM_PROFDATA:-$$(find_tool llvm-profdata)}"; \
	LLVM_COV_BIN="$${LLVM_COV:-$$(find_tool llvm-cov)}"; \
	if [ -z "$$CXX_BIN" ]; then \
	  printf '%b\n' "$(RED)coverage: clang++ is required for -fprofile-instr-generate -fcoverage-mapping$(RESET)" >&2; \
	  exit 1; \
	fi; \
	if [ -z "$$LLVM_PROFDATA_BIN" ]; then \
	  printf '%b\n' "$(RED)coverage: llvm-profdata not found on PATH or through xcrun$(RESET)" >&2; \
	  exit 1; \
	fi; \
	if [ -z "$$LLVM_COV_BIN" ]; then \
	  printf '%b\n' "$(RED)coverage: llvm-cov not found on PATH or through xcrun$(RESET)" >&2; \
	  exit 1; \
	fi; \
	printf '%b\n' "$(CYAN)Using $$CXX_BIN$(RESET)"; \
	printf '%b\n' "$(CYAN)Using $$LLVM_PROFDATA_BIN$(RESET)"; \
	printf '%b\n' "$(CYAN)Using $$LLVM_COV_BIN$(RESET)"; \
	mkdir -p "$(RUNTIME_COVERAGE_DIR)" "$(RUNTIME_COVERAGE_REPORT_DIR)"; \
	if [ -f "$(RUNTIME_COVERAGE_BUILD_DIR)/CMakeCache.txt" ] && ! grep -F "CMAKE_HOME_DIRECTORY:INTERNAL=$(CURDIR)/$(RUNTIME_SOURCE_DIR)" "$(RUNTIME_COVERAGE_BUILD_DIR)/CMakeCache.txt" >/dev/null 2>&1; then \
	  printf '%b\n' "$(YELLOW)Resetting stale runtime coverage CMake cache for $(RUNTIME_SOURCE_DIR)...$(RESET)"; \
	  rm -rf "$(RUNTIME_COVERAGE_BUILD_DIR)"; \
	fi; \
	COVERAGE_FLAGS="-O0 -g -fprofile-instr-generate -fcoverage-mapping"; \
	cmake -S "$(RUNTIME_SOURCE_DIR)" -B "$(RUNTIME_COVERAGE_BUILD_DIR)" \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -DCMAKE_CXX_COMPILER="$$CXX_BIN" \
	  -DCMAKE_CXX_FLAGS="$$COVERAGE_FLAGS" \
	  -DCMAKE_EXE_LINKER_FLAGS="-fprofile-instr-generate -fcoverage-mapping" \
	  -DCMAKE_SHARED_LINKER_FLAGS="-fprofile-instr-generate -fcoverage-mapping"; \
	cmake --build "$(RUNTIME_COVERAGE_BUILD_DIR)" --parallel; \
	PROFILE_DIR="$$(mktemp -d "$(CURDIR)/$(RUNTIME_COVERAGE_DIR)/profiles.XXXXXX")"; \
	( cd "$(RUNTIME_COVERAGE_BUILD_DIR)" && LLVM_PROFILE_FILE="$$PROFILE_DIR/%m-%p.profraw" ctest --output-on-failure -C Debug ); \
	PROFILE_LIST="$(RUNTIME_COVERAGE_DIR)/profiles.txt"; \
	find "$$PROFILE_DIR" -name '*.profraw' -print | sort > "$$PROFILE_LIST"; \
	if [ ! -s "$$PROFILE_LIST" ]; then \
	  printf '%b\n' "$(RED)coverage: no .profraw files were produced by runtime CTest$(RESET)" >&2; \
	  exit 1; \
	fi; \
	"$$LLVM_PROFDATA_BIN" merge -sparse --input-files="$$PROFILE_LIST" -o "$(RUNTIME_COVERAGE_PROFDATA)"; \
	primary=""; \
	extra_objects=(); \
	for name in \
	  pandapi_runtime_smoke \
	  pandapi_status_result_smoke \
	  pandapi_status_io_smoke \
	  pandapi_cli_tty_provenance_smoke \
	  pandapi_fixture_substrate_smoke \
	  pandapi_fixture_catch2_smoke; do \
	  path="$(RUNTIME_COVERAGE_BUILD_DIR)/$$name"; \
	  if [ -x "$$path" ]; then \
	    if [ -z "$$primary" ]; then \
	      primary="$$path"; \
	    else \
	      extra_objects+=( "-object" "$$path" ); \
	    fi; \
	  fi; \
	done; \
	if [ -z "$$primary" ]; then \
	  printf '%b\n' "$(RED)coverage: no runtime test executables found in $(RUNTIME_COVERAGE_BUILD_DIR)$(RESET)" >&2; \
	  exit 1; \
	fi; \
	ignore_regex='(^|/)(build|dist|release|upstream|workbench)(/|$$)|(^|/)pandaPI/(parser|grounder|engine)(/|$$)|(^|/)pandaPI/[^/]+/(cpddl|cudd|bliss|lpsolve|generated)(/|$$)'; \
	"$$LLVM_COV_BIN" report "$$primary" "$${extra_objects[@]}" \
	  -instr-profile="$(RUNTIME_COVERAGE_PROFDATA)" \
	  -ignore-filename-regex="$$ignore_regex" > "$(RUNTIME_COVERAGE_SUMMARY)"; \
	"$$LLVM_COV_BIN" show "$$primary" "$${extra_objects[@]}" \
	  -instr-profile="$(RUNTIME_COVERAGE_PROFDATA)" \
	  -ignore-filename-regex="$$ignore_regex" \
	  -format=text > "$(RUNTIME_COVERAGE_DETAIL)"; \
	printf '%b\n' "$(GREEN)Coverage summary: $(RUNTIME_COVERAGE_SUMMARY)$(RESET)"; \
	printf '%b\n' "$(GREEN)Coverage detail:  $(RUNTIME_COVERAGE_DETAIL)$(RESET)"; \
	for summary in "$(RUNTIME_COVERAGE_REPORT_DIR)"/*summary*.txt; do \
	  if [ ! -f "$$summary" ]; then \
	    continue; \
	  fi; \
	  printf '\n%b\n' "$(CYAN)Coverage report summary: $$summary$(RESET)"; \
	  cat "$$summary"; \
	done
