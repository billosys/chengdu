# Non-test validation and release-support checks.

.PHONY: install-actionlint
install-actionlint:
	printf '%b\n' "$(BLUE)Installing actionlint 1.7.12...$(RESET)"
	tmp="$$(mktemp)"; \
	if ! curl -fsSL -o "$$tmp" https://raw.githubusercontent.com/rhysd/actionlint/v1.7.12/scripts/download-actionlint.bash; then \
	  rm -f "$$tmp"; \
	  exit 1; \
	fi; \
	if ! bash "$$tmp" 1.7.12; then \
	  rm -f "$$tmp"; \
	  exit 1; \
	fi; \
	rm -f "$$tmp"
	printf '%b\n' "$(GREEN)actionlint installed$(RESET)"

.PHONY: actionlint
actionlint:
	printf '%b\n' "$(BLUE)Running actionlint...$(RESET)"
	if [ -x ./actionlint ]; then \
	  ./actionlint -color; \
	elif command -v actionlint >/dev/null 2>&1; then \
	  actionlint -color; \
	else \
	  printf '%b\n' "$(RED)actionlint not found; run make install-actionlint$(RESET)" >&2; \
	  exit 1; \
	fi
	printf '%b\n' "$(GREEN)actionlint passed$(RESET)"

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
