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
build-parser:
	printf '%b\n' "$(BLUE)Building pandaPIparser...$(RESET)"; \
	REPO_ROOT="$(CURDIR)"; \
	. scripts/lib-platform.sh; \
	PLATFORM="$$(detect_platform)"; \
	SRC_DIR="$$(prepare_build_source_copy pandaPIparser)"; \
	DIST_DIR="$$REPO_ROOT/dist/$$PLATFORM"; \
	. "$$REPO_ROOT/vendor.env"; \
	$(MAKE) -C "$$SRC_DIR"; \
	if [ ! -x "$$SRC_DIR/pandaPIparser" ]; then \
	  printf '%b\n' "$(RED)pandaPIparser build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$$DIST_DIR"; \
	if [ -f "$$REPO_ROOT/scripts/pandapi-parser-adapter.sh" ]; then \
	  cp "$$SRC_DIR/pandaPIparser" "$$DIST_DIR/pandaPIparser.legacy"; \
	  cp "$$REPO_ROOT/scripts/pandapi-parser-adapter.sh" "$$DIST_DIR/pandapi-parser"; \
	  cp "$$REPO_ROOT/scripts/pandapi-parser-adapter.sh" "$$DIST_DIR/pandaPIparser"; \
	  chmod +x "$$DIST_DIR/pandaPIparser.legacy" "$$DIST_DIR/pandapi-parser" "$$DIST_DIR/pandaPIparser"; \
	else \
	  cp "$$SRC_DIR/pandaPIparser" "$$DIST_DIR/pandaPIparser"; \
	  chmod +x "$$DIST_DIR/pandaPIparser"; \
	fi; \
	COMPILER="$$(resolve_compiler_id g++)"; \
	append_provenance "$$DIST_DIR" "pandaPIparser" "PARSER" "$$COMPILER"; \
	printf '%b\n' "$(GREEN)Built parser artifacts in $$DIST_DIR$(RESET)"

.PHONY: build-grounder
build-grounder:
	printf '%b\n' "$(BLUE)Building pandaPIgrounder...$(RESET)"; \
	REPO_ROOT="$(CURDIR)"; \
	. scripts/lib-platform.sh; \
	PLATFORM="$$(detect_platform)"; \
	SRC_ROOT="$$(prepare_build_source_copy pandaPIgrounder)"; \
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
	( cd "$$SRC_ROOT/src" && $(MAKE) -j CXX="$$GROUNDER_CXX" CC="$$GROUNDER_CC" ); \
	if [ ! -x "$$SRC_ROOT/pandaPIgrounder" ]; then \
	  printf '%b\n' "$(RED)pandaPIgrounder build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$$DIST_DIR"; \
	cp "$$SRC_ROOT/pandaPIgrounder" "$$DIST_DIR/pandaPIgrounder"; \
	COMPILER="$$(resolve_compiler_id "$$GROUNDER_CXX")"; \
	append_provenance "$$DIST_DIR" "pandaPIgrounder" "GROUNDER" "$$COMPILER"; \
	printf '%b\n' "$(GREEN)Built $$DIST_DIR/pandaPIgrounder$(RESET)"

.PHONY: build-engine
build-engine:
	printf '%b\n' "$(BLUE)Building pandaPIengine...$(RESET)"; \
	REPO_ROOT="$(CURDIR)"; \
	. scripts/lib-platform.sh; \
	PLATFORM="$$(detect_platform)"; \
	SRC_DIR="$$(prepare_build_source_copy pandaPIengine)"; \
	BUILD_DIR="$$SRC_DIR/build"; \
	DIST_DIR="$$REPO_ROOT/dist/$$PLATFORM"; \
	. "$$REPO_ROOT/vendor.env"; \
	mkdir -p "$$BUILD_DIR"; \
	( cd "$$BUILD_DIR" && cmake ../src -DCMAKE_BUILD_TYPE=Release ); \
	( cd "$$BUILD_DIR" && $(MAKE) -j ); \
	if [ ! -x "$$BUILD_DIR/pandaPIengine" ]; then \
	  printf '%b\n' "$(RED)pandaPIengine build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	mkdir -p "$$DIST_DIR"; \
	cp "$$BUILD_DIR/pandaPIengine" "$$DIST_DIR/pandaPIengine"; \
	COMPILER="$$(resolve_compiler_id c++)"; \
	append_provenance "$$DIST_DIR" "pandaPIengine" "ENGINE" "$$COMPILER"; \
	printf '%b\n' "$(GREEN)Built $$DIST_DIR/pandaPIengine$(RESET)"

.PHONY: build
build: reset-provenance build-runtime build-parser build-grounder build-engine
	printf '%b\n' "$(GREEN)Build complete: $(DIST_DIR)$(RESET)"
