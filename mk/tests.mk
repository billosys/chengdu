# Test targets for runtime seams and black-box pandaPI process fixtures.

.PHONY: test-runtime
test-runtime: build-runtime
	printf '%b\n' "$(BLUE)Running pandapi-runtime CTest suite...$(RESET)"
	( cd "$(RUNTIME_BUILD_DIR)" && ctest --output-on-failure -C Release )
	printf '%b\n' "$(GREEN)Runtime tests passed$(RESET)"

.PHONY: test-runtime-sanitize
test-runtime-sanitize: sanitize-runtime
	printf '%b\n' "$(GREEN)Runtime sanitizer tests passed$(RESET)"

.PHONY: test-contract-list
test-contract-list:
	printf '%b\n' "$(BLUE)Listing baseline contract fixtures...$(RESET)"
	./tests/contract/run --list

.PHONY: test-contract
test-contract: build
	printf '%b\n' "$(BLUE)Running baseline contract fixtures...$(RESET)"
	./tests/contract/run --baseline
	printf '%b\n' "$(GREEN)Baseline contract fixtures passed$(RESET)"

.PHONY: test-contract-parser
test-contract-parser: build
	printf '%b\n' "$(BLUE)Running parser contract fixtures...$(RESET)"
	./tests/contract/run --baseline --component parser
	printf '%b\n' "$(GREEN)Parser contract fixtures passed$(RESET)"

.PHONY: test-contract-parser-managed
test-contract-parser-managed: build-parser
	printf '%b\n' "$(BLUE)Running managed parser contract fixtures...$(RESET)"
	./tests/contract/run --contract --component parser
	printf '%b\n' "$(GREEN)Managed parser contract fixtures passed$(RESET)"

.PHONY: test-contract-grounder
test-contract-grounder: build
	printf '%b\n' "$(BLUE)Running grounder contract fixtures...$(RESET)"
	./tests/contract/run --baseline --component grounder
	printf '%b\n' "$(GREEN)Grounder contract fixtures passed$(RESET)"

.PHONY: test-contract-grounder-managed
test-contract-grounder-managed: build-grounder
	printf '%b\n' "$(BLUE)Running managed grounder contract fixtures...$(RESET)"
	./scripts/install-grounder-adapter.sh "$(DIST_DIR)"
	./tests/contract/run --contract --component grounder
	printf '%b\n' "$(GREEN)Managed grounder contract fixtures passed$(RESET)"

.PHONY: test-contract-engine
test-contract-engine: build
	printf '%b\n' "$(BLUE)Running engine contract fixtures...$(RESET)"
	./tests/contract/run --baseline --component engine
	printf '%b\n' "$(GREEN)Engine contract fixtures passed$(RESET)"

.PHONY: test-contract-pipeline
test-contract-pipeline: build
	printf '%b\n' "$(BLUE)Running pipeline contract fixtures...$(RESET)"
	./tests/contract/run --baseline --component pipeline
	printf '%b\n' "$(GREEN)Pipeline contract fixtures passed$(RESET)"

.PHONY: smoke
smoke: build
	printf '%b\n' "$(BLUE)Running positive smoke test...$(RESET)"
	./tests/smoke/run
	printf '%b\n' "$(GREEN)Positive smoke test passed$(RESET)"

.PHONY: smoke-negative
smoke-negative: build
	printf '%b\n' "$(BLUE)Running negative smoke test...$(RESET)"
	./tests/smoke/run --negative
	printf '%b\n' "$(GREEN)Negative smoke test passed$(RESET)"

.PHONY: test-smoke
test-smoke: smoke
	printf '%b\n' "$(GREEN)Smoke test passed$(RESET)"

.PHONY: test-smoke-negative
test-smoke-negative: smoke-negative
	printf '%b\n' "$(GREEN)Negative smoke test passed$(RESET)"

.PHONY: test-corpus
test-corpus: build
	if [ -z "$(CORPUS_DIR)" ]; then \
	  printf '%b\n' "$(RED)Set CORPUS_DIR to an ipc2023-domains checkout$(RESET)" >&2; \
	  exit 1; \
	fi
	printf '%b\n' "$(BLUE)Running optional corpus smoke test: $(CORPUS_DIR)$(RESET)"
	./tests/smoke/run --corpus "$(CORPUS_DIR)"
	printf '%b\n' "$(GREEN)Corpus smoke test passed$(RESET)"

.PHONY: contract-baseline
contract-baseline: test-contract
	printf '%b\n' "$(GREEN)contract-baseline alias passed$(RESET)"

.PHONY: test
test: build test-runtime test-contract test-contract-parser-managed test-contract-grounder-managed smoke smoke-negative
	printf '%b\n' "$(GREEN)Test suite passed$(RESET)"
