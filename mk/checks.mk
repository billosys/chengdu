# Non-test validation and release-support checks.

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
