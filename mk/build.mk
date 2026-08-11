# Build targets for the runtime and inherited pandaPI binaries.

.PHONY: reset-provenance
reset-provenance:
	rm -f "$(DIST_DIR)/provenance.txt"

.PHONY: build-runtime
build-runtime:
	printf '%b\n' "$(BLUE)Building pandapi-runtime for $(PLATFORM)...$(RESET)"; \
	mkdir -p "$(RUNTIME_BUILD_DIR)"; \
	cmake -S pandapi-runtime -B "$(RUNTIME_BUILD_DIR)" \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; \
	cmake --build "$(RUNTIME_BUILD_DIR)" --parallel; \
	printf '%b\n' "$(GREEN)Runtime build complete: $(RUNTIME_BUILD_DIR)$(RESET)"

.PHONY: sanitize-runtime
sanitize-runtime:
	printf '%b\n' "$(BLUE)Running pandapi-runtime sanitizer gate for $(PLATFORM)...$(RESET)"; \
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
	if [ -z "$$CXX_BIN" ]; then \
	  printf '%b\n' "$(RED)clang++ is required for ASan/UBSan$(RESET)" >&2; \
	  exit 1; \
	fi; \
	SANITIZER_FLAGS="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined"; \
	mkdir -p "$(RUNTIME_SANITIZE_BUILD_DIR)"; \
	cmake -S pandapi-runtime -B "$(RUNTIME_SANITIZE_BUILD_DIR)" \
	  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -DCMAKE_CXX_COMPILER="$$CXX_BIN" \
	  -DCMAKE_CXX_FLAGS="$$SANITIZER_FLAGS" \
	  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
	  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address,undefined"; \
	cmake --build "$(RUNTIME_SANITIZE_BUILD_DIR)" --parallel; \
	( cd "$(RUNTIME_SANITIZE_BUILD_DIR)" && ctest --output-on-failure -C RelWithDebInfo ); \
	printf '%b\n' "$(GREEN)Sanitizer gate passed: $(RUNTIME_SANITIZE_BUILD_DIR)$(RESET)"

.PHONY: build-parser
build-parser: build-runtime
	printf '%b\n' "$(BLUE)Building pandapi-parser...$(RESET)"; \
	REPO_ROOT="$(CURDIR)"; \
	. tools/shared/platform; \
	PLATFORM="$$(detect_platform)"; \
	SRC_DIR="$$(prepare_build_source_copy parser)"; \
	DIST_DIR="$$REPO_ROOT/dist/$$PLATFORM"; \
	. "$$REPO_ROOT/vendor.env"; \
	$(MAKE) -C "$$SRC_DIR" \
	  PANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/pandapi-runtime/include" \
	  PANDAPI_RUNTIME_LIB="$$REPO_ROOT/$(RUNTIME_BUILD_DIR)/libpandapi_runtime.a"; \
	if [ ! -x "$$SRC_DIR/pandapi-parser" ]; then \
	  printf '%b\n' "$(RED)pandapi-parser build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$$DIST_DIR"; \
	OLD_PARSER="pandaPI""parser"; \
	rm -f "$$DIST_DIR/$$OLD_PARSER" "$$DIST_DIR/$$OLD_PARSER.legacy"; \
	cp "$$SRC_DIR/pandapi-parser" "$$DIST_DIR/pandapi-parser"; \
	chmod +x "$$DIST_DIR/pandapi-parser"; \
	COMPILER="$$(resolve_compiler_id g++)"; \
	append_provenance "$$DIST_DIR" "pandapi-parser" "PARSER" "$$COMPILER"; \
	printf '%b\n' "$(GREEN)Built parser artifacts in $$DIST_DIR$(RESET)"

.PHONY: build-grounder
build-grounder: build-runtime
	printf '%b\n' "$(BLUE)Building pandapi-grounder...$(RESET)"; \
	REPO_ROOT="$(CURDIR)"; \
	. tools/shared/platform; \
	PLATFORM="$$(detect_platform)"; \
	SRC_ROOT="$$(prepare_build_source_copy grounder)"; \
	CPDDL_DIR="$$SRC_ROOT/cpddl"; \
	DIST_DIR="$$REPO_ROOT/dist/$$PLATFORM"; \
	. "$$REPO_ROOT/vendor.env"; \
	if [ "$$PLATFORM" = "macos-arm64" ]; then \
	  : "$${GROUNDER_CC:=cc}"; \
	  : "$${GROUNDER_CXX:=c++}"; \
	else \
	  : "$${GROUNDER_CC:=gcc}"; \
	  : "$${GROUNDER_CXX:=g++}"; \
	fi; \
	printf '%b\n' "$(CYAN)Using CC=$$GROUNDER_CC CXX=$$GROUNDER_CXX$(RESET)"; \
	( cd "$$CPDDL_DIR" && CC="$$GROUNDER_CC" CXX="$$GROUNDER_CXX" $(MAKE) boruvka opts bliss lpsolve ); \
	( cd "$$CPDDL_DIR" && CC="$$GROUNDER_CC" CXX="$$GROUNDER_CXX" $(MAKE) ); \
	( cd "$$SRC_ROOT/src" && $(MAKE) -j CXX="$$GROUNDER_CXX" CC="$$GROUNDER_CC" \
	  PANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/pandapi-runtime/include" \
	  PANDAPI_RUNTIME_LIB="$$REPO_ROOT/$(RUNTIME_BUILD_DIR)/libpandapi_runtime.a" ); \
	if [ ! -x "$$SRC_ROOT/pandapi-grounder" ]; then \
	  printf '%b\n' "$(RED)pandapi-grounder build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$$DIST_DIR"; \
	OLD_GROUNDER="pandaPI""grounder"; \
	rm -f "$$DIST_DIR/$$OLD_GROUNDER" "$$DIST_DIR/$$OLD_GROUNDER.legacy"; \
	cp "$$SRC_ROOT/pandapi-grounder" "$$DIST_DIR/pandapi-grounder"; \
	chmod +x "$$DIST_DIR/pandapi-grounder"; \
	COMPILER="$$(resolve_compiler_id "$$GROUNDER_CXX")"; \
	append_provenance "$$DIST_DIR" "pandapi-grounder" "GROUNDER" "$$COMPILER"; \
	printf '%b\n' "$(GREEN)Built $$DIST_DIR/pandapi-grounder$(RESET)"

.PHONY: build-engine
build-engine: build-runtime
	printf '%b\n' "$(BLUE)Building pandapi-engine...$(RESET)"; \
	REPO_ROOT="$(CURDIR)"; \
	. tools/shared/platform; \
	PLATFORM="$$(detect_platform)"; \
	SRC_DIR="$$(prepare_build_source_copy engine)"; \
	BUILD_DIR="$$SRC_DIR/build"; \
	DIST_DIR="$$REPO_ROOT/dist/$$PLATFORM"; \
	. "$$REPO_ROOT/vendor.env"; \
	mkdir -p "$$BUILD_DIR"; \
	( cd "$$BUILD_DIR" && cmake ../src -DCMAKE_BUILD_TYPE=Release \
	  -DPANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/pandapi-runtime/include" \
	  -DPANDAPI_RUNTIME_LIB="$$REPO_ROOT/$(RUNTIME_BUILD_DIR)/libpandapi_runtime.a" ); \
	( cd "$$BUILD_DIR" && $(MAKE) -j ); \
	if [ ! -x "$$BUILD_DIR/pandapi-engine" ]; then \
	  printf '%b\n' "$(RED)pandapi-engine build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$$DIST_DIR"; \
	OLD_ENGINE="pandaPI""engine"; \
	rm -f "$$DIST_DIR/$$OLD_ENGINE"; \
	cp "$$BUILD_DIR/pandapi-engine" "$$DIST_DIR/pandapi-engine"; \
	chmod +x "$$DIST_DIR/pandapi-engine"; \
	COMPILER="$$(resolve_compiler_id c++)"; \
	append_provenance "$$DIST_DIR" "pandapi-engine" "ENGINE" "$$COMPILER"; \
	printf '%b\n' "$(GREEN)Built $$DIST_DIR/pandapi-engine$(RESET)"

.PHONY: build
build: reset-provenance build-runtime build-parser build-grounder build-engine
	printf '%b\n' "$(GREEN)Build complete: $(DIST_DIR)$(RESET)"
