# CI-equivalent aggregate targets.

.PHONY: ci-linux
ci-linux:
	$(MAKE) reset-provenance; \
	$(MAKE) format-check; \
	$(MAKE) build-runtime; \
	$(MAKE) sanitize-runtime; \
	$(MAKE) build-parser; \
	$(MAKE) build-grounder; \
	$(MAKE) build-engine; \
	$(MAKE) contract-baseline; \
	$(MAKE) provenance-check; \
	$(MAKE) smoke; \
	$(MAKE) smoke-negative; \
	printf '%b\n' "$(GREEN)Linux CI target passed$(RESET)"

.PHONY: ci-macos
ci-macos:
	$(MAKE) reset-provenance; \
	$(MAKE) format-check; \
	$(MAKE) build-runtime; \
	$(MAKE) build-parser; \
	$(MAKE) build-grounder; \
	$(MAKE) build-engine; \
	$(MAKE) contract-baseline; \
	$(MAKE) record-min-os; \
	$(MAKE) provenance-check; \
	$(MAKE) smoke; \
	$(MAKE) smoke-negative; \
	printf '%b\n' "$(GREEN)macOS CI target passed$(RESET)"

.PHONY: readme-verbatim
readme-verbatim:
	$(MAKE) reset-provenance; \
	$(MAKE) build-parser; \
	$(MAKE) build-grounder; \
	$(MAKE) build-engine; \
	$(MAKE) smoke; \
	printf '%b\n' "$(GREEN)README verbatim target passed$(RESET)"
