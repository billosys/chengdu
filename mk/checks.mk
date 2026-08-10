# Validation targets. Large harnesses remain here as target entrypoints while
# their shell bodies are migrated out of scripts over time.

.PHONY: contract-baseline
contract-baseline:
	printf '%b\n' "$(BLUE)Running baseline contract fixtures...$(RESET)"
	./scripts/run-contract-fixtures.sh --baseline
	printf '%b\n' "$(GREEN)Baseline contract fixtures passed$(RESET)"

.PHONY: smoke
smoke:
	printf '%b\n' "$(BLUE)Running positive smoke test...$(RESET)"
	./scripts/smoke-test.sh
	printf '%b\n' "$(GREEN)Positive smoke test passed$(RESET)"

.PHONY: smoke-negative
smoke-negative:
	printf '%b\n' "$(BLUE)Running negative smoke test...$(RESET)"
	./scripts/smoke-test.sh --negative
	printf '%b\n' "$(GREEN)Negative smoke test passed$(RESET)"

.PHONY: provenance-check
provenance-check:
	printf '%b\n' "$(BLUE)Checking provenance...$(RESET)"
	./scripts/check-provenance.sh
	printf '%b\n' "$(GREEN)Provenance check passed$(RESET)"

.PHONY: record-min-os
record-min-os:
	printf '%b\n' "$(BLUE)Recording macOS minimum-OS observation...$(RESET)"
	./scripts/record-min-os.sh
	printf '%b\n' "$(GREEN)Minimum-OS observation recorded$(RESET)"
