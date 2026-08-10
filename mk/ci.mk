# CI-equivalent aggregate targets.

.PHONY: ci-linux
ci-linux: reset-provenance format-check build-runtime sanitize-runtime build-parser build-grounder build-engine contract-baseline provenance-check smoke smoke-negative
	printf '%b\n' "$(GREEN)Linux CI target passed$(RESET)"

.PHONY: ci-macos
ci-macos: reset-provenance format-check build-runtime build-parser build-grounder build-engine contract-baseline record-min-os provenance-check smoke smoke-negative
	printf '%b\n' "$(GREEN)macOS CI target passed$(RESET)"

.PHONY: readme-verbatim
readme-verbatim: reset-provenance build-parser build-grounder build-engine smoke
	printf '%b\n' "$(GREEN)README verbatim target passed$(RESET)"
