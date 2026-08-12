# Build targets for the runtime and inherited pandaPI binaries.

.PHONY: reset-provenance
reset-provenance:
	rm -f "$(DIST_DIR)/provenance.txt"

.PHONY: build-runtime
build-runtime:
	set -e; \
	printf '%b\n' "$(BLUE)Building pandapi-runtime for $(PLATFORM)...$(RESET)"; \
	if [ -f "$(RUNTIME_BUILD_DIR)/CMakeCache.txt" ] && ! grep -F "CMAKE_HOME_DIRECTORY:INTERNAL=$(CURDIR)/$(RUNTIME_SOURCE_DIR)" "$(RUNTIME_BUILD_DIR)/CMakeCache.txt" >/dev/null 2>&1; then \
	  printf '%b\n' "$(YELLOW)Resetting stale runtime CMake cache for $(RUNTIME_SOURCE_DIR)...$(RESET)"; \
	  rm -rf "$(RUNTIME_BUILD_DIR)"; \
	fi; \
	mkdir -p "$(RUNTIME_BUILD_DIR)"; \
	cmake -S "$(RUNTIME_SOURCE_DIR)" -B "$(RUNTIME_BUILD_DIR)" \
	  -DCMAKE_BUILD_TYPE=Release \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON; \
	cmake --build "$(RUNTIME_BUILD_DIR)" --parallel; \
	printf '%b\n' "$(GREEN)Runtime build complete: $(RUNTIME_BUILD_DIR)$(RESET)"

.PHONY: sanitize-runtime
sanitize-runtime:
	set -e; \
	printf '%b\n' "$(BLUE)Running pandapi-runtime sanitizer gate for $(PLATFORM)...$(RESET)"; \
	if [ -f "$(RUNTIME_SANITIZE_BUILD_DIR)/CMakeCache.txt" ] && ! grep -F "CMAKE_HOME_DIRECTORY:INTERNAL=$(CURDIR)/$(RUNTIME_SOURCE_DIR)" "$(RUNTIME_SANITIZE_BUILD_DIR)/CMakeCache.txt" >/dev/null 2>&1; then \
	  printf '%b\n' "$(YELLOW)Resetting stale runtime sanitizer CMake cache for $(RUNTIME_SOURCE_DIR)...$(RESET)"; \
	  rm -rf "$(RUNTIME_SANITIZE_BUILD_DIR)"; \
	fi; \
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
	cmake -S "$(RUNTIME_SOURCE_DIR)" -B "$(RUNTIME_SANITIZE_BUILD_DIR)" \
	  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
	  -DCMAKE_CXX_COMPILER="$$CXX_BIN" \
	  -DCMAKE_CXX_FLAGS="$$SANITIZER_FLAGS" \
	  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
	  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address,undefined"; \
	cmake --build "$(RUNTIME_SANITIZE_BUILD_DIR)" --parallel; \
	( cd "$(RUNTIME_SANITIZE_BUILD_DIR)" && ctest --output-on-failure -C RelWithDebInfo ); \
	printf '%b\n' "$(GREEN)Sanitizer gate passed: $(RUNTIME_SANITIZE_BUILD_DIR)$(RESET)"

.PHONY: sanitize-binaries
sanitize-binaries: sanitize-runtime
	set -e; \
	printf '%b\n' "$(BLUE)Running binary sanitizer gate for $(PLATFORM)...$(RESET)"; \
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
	CC_BIN="$${CC:-$$(find_tool clang)}"; \
	CXX_BIN="$${CXX:-$$(find_tool clang++)}"; \
	if [ -z "$$CC_BIN" ] || [ -z "$$CXX_BIN" ]; then \
	  printf '%b\n' "$(RED)clang and clang++ are required for ASan/UBSan binary sanitizer builds$(RESET)" >&2; \
	  exit 1; \
	fi; \
	SANITIZER_FLAGS="$(BINARY_SANITIZER_FLAGS)"; \
	SANITIZER_LINK_FLAGS="$(BINARY_SANITIZER_LINK_FLAGS)"; \
	export ASAN_OPTIONS="$(BINARY_SANITIZER_ASAN_OPTIONS)"; \
	export UBSAN_OPTIONS="$(BINARY_SANITIZER_UBSAN_OPTIONS)"; \
	printf '%b\n' "$(CYAN)Using CC=$$CC_BIN CXX=$$CXX_BIN$(RESET)"; \
	printf '%b\n' "$(CYAN)ASan/UBSan flags: $$SANITIZER_FLAGS$(RESET)"; \
	printf '%b\n' "$(CYAN)LSan: $(BINARY_SANITIZER_LSAN_STATUS)$(RESET)"; \
	rm -rf "$(BINARY_SANITIZE_DIST_DIR)"; \
	mkdir -p "$(BINARY_SANITIZE_DIST_DIR)"; \
	REPO_ROOT="$(CURDIR)"; \
	. tools/shared/platform; \
	PLATFORM="$$(detect_platform)"; \
	. "$$REPO_ROOT/vendor.env"; \
	PARSER_SRC="$$(prepare_build_source_copy parser)"; \
	$(MAKE) -C "$$PARSER_SRC" \
	  CC="$$CC_BIN" \
	  CXX="$$CXX_BIN" \
	  CFLAGS="-O1 -g -fno-omit-frame-pointer -DNDEBUG -fsanitize=address,undefined" \
	  COMPILEFLAGS="-O1 -g -fno-omit-frame-pointer -Wall -Wextra -pedantic -std=c++17 -DNDEBUG -I$$REPO_ROOT/$(RUNTIME_INCLUDE_DIR) -fsanitize=address,undefined -Wno-unused-parameter" \
	  LINKERFLAG="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -lm" \
	  PANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/$(RUNTIME_INCLUDE_DIR)" \
	  PANDAPI_RUNTIME_LIB="$$REPO_ROOT/$(RUNTIME_SANITIZE_BUILD_DIR)/libpandapi_runtime.a"; \
	if [ ! -x "$$PARSER_SRC/pandapi-parser" ]; then \
	  printf '%b\n' "$(RED)pandapi-parser sanitizer build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	cp "$$PARSER_SRC/pandapi-parser" "$(BINARY_SANITIZE_DIST_DIR)/pandapi-parser"; \
	chmod +x "$(BINARY_SANITIZE_DIST_DIR)/pandapi-parser"; \
	append_provenance "$(BINARY_SANITIZE_DIST_DIR)" "pandapi-parser" "PARSER" "$$(resolve_compiler_id "$$CXX_BIN")"; \
	GROUNDER_SRC="$$(prepare_build_source_copy grounder)"; \
	CPDDL_DIR="$$GROUNDER_SRC/cpddl"; \
	( cd "$$CPDDL_DIR" && CC="$$CC_BIN" CXX="$$CXX_BIN" CFLAGS="$$SANITIZER_FLAGS" CXXFLAGS="$$SANITIZER_FLAGS" LDFLAGS="$$SANITIZER_LINK_FLAGS" $(MAKE) boruvka opts bliss lpsolve ); \
	( cd "$$CPDDL_DIR" && CC="$$CC_BIN" CXX="$$CXX_BIN" CFLAGS="$$SANITIZER_FLAGS" CXXFLAGS="$$SANITIZER_FLAGS" LDFLAGS="$$SANITIZER_LINK_FLAGS" $(MAKE) ); \
	( cd "$$GROUNDER_SRC/src" && $(MAKE) -j \
	  CXX="$$CXX_BIN" \
	  CC="$$CC_BIN" \
	  CXXFLAGS_LIBS="-fsanitize=address,undefined" \
	  CXXFLAGS_PROD="-O1 -g -fno-omit-frame-pointer -DNDEBUG" \
	  LDFLAGS_GENERAL="-fsanitize=address,undefined" \
	  LDFLAGS_PROD="-O1 -g -fno-omit-frame-pointer" \
	  PANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/$(RUNTIME_INCLUDE_DIR)" \
	  PANDAPI_RUNTIME_LIB="$$REPO_ROOT/$(RUNTIME_SANITIZE_BUILD_DIR)/libpandapi_runtime.a" ); \
	if [ ! -x "$$GROUNDER_SRC/pandapi-grounder" ]; then \
	  printf '%b\n' "$(RED)pandapi-grounder sanitizer build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	cp "$$GROUNDER_SRC/pandapi-grounder" "$(BINARY_SANITIZE_DIST_DIR)/pandapi-grounder"; \
	chmod +x "$(BINARY_SANITIZE_DIST_DIR)/pandapi-grounder"; \
	append_provenance "$(BINARY_SANITIZE_DIST_DIR)" "pandapi-grounder" "GROUNDER" "$$(resolve_compiler_id "$$CXX_BIN")"; \
	ENGINE_SRC="$$(prepare_build_source_copy engine)"; \
	ENGINE_BUILD="$$ENGINE_SRC/build-sanitize"; \
	mkdir -p "$$ENGINE_BUILD"; \
	( cd "$$ENGINE_BUILD" && cmake ../src \
	  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
	  -DCMAKE_C_COMPILER="$$CC_BIN" \
	  -DCMAKE_CXX_COMPILER="$$CXX_BIN" \
	  -DCMAKE_C_FLAGS="$$SANITIZER_FLAGS" \
	  -DCMAKE_CXX_FLAGS="$$SANITIZER_FLAGS" \
	  -DCMAKE_EXE_LINKER_FLAGS="$$SANITIZER_LINK_FLAGS" \
	  -DCMAKE_SHARED_LINKER_FLAGS="$$SANITIZER_LINK_FLAGS" \
	  -DPANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/$(RUNTIME_INCLUDE_DIR)" \
	  -DPANDAPI_RUNTIME_LIB="$$REPO_ROOT/$(RUNTIME_SANITIZE_BUILD_DIR)/libpandapi_runtime.a" ); \
	( cd "$$ENGINE_BUILD" && $(MAKE) -j ); \
	if [ ! -x "$$ENGINE_BUILD/pandapi-engine" ]; then \
	  printf '%b\n' "$(RED)pandapi-engine sanitizer build did not produce an executable$(RESET)" >&2; \
	  exit 1; \
	fi; \
	cp "$$ENGINE_BUILD/pandapi-engine" "$(BINARY_SANITIZE_DIST_DIR)/pandapi-engine"; \
	chmod +x "$(BINARY_SANITIZE_DIST_DIR)/pandapi-engine"; \
	append_provenance "$(BINARY_SANITIZE_DIST_DIR)" "pandapi-engine" "ENGINE" "$$(resolve_compiler_id "$$CXX_BIN")"; \
	CHENGDU_CONTRACT_DIST_DIR="$$REPO_ROOT/$(BINARY_SANITIZE_DIST_DIR)" ./tests/contract/run --contract; \
	printf '%b\n' "$(GREEN)Binary sanitizer gate passed: $(BINARY_SANITIZE_DIST_DIR)$(RESET)"

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
	  PANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/$(RUNTIME_INCLUDE_DIR)" \
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
	  PANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/$(RUNTIME_INCLUDE_DIR)" \
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
	  -DPANDAPI_RUNTIME_INCLUDE="$$REPO_ROOT/$(RUNTIME_INCLUDE_DIR)" \
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
