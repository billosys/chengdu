# CI-equivalent aggregate targets.

.PHONY: ci-linux
ci-linux: reset-provenance format-check sanitize-runtime test provenance-check
	printf '%b\n' "$(GREEN)Linux CI target passed$(RESET)"

.PHONY: ci-macos
ci-macos: reset-provenance format-check test record-min-os provenance-check
	printf '%b\n' "$(GREEN)macOS CI target passed$(RESET)"

.PHONY: readme-verbatim
readme-verbatim: reset-provenance build smoke
	printf '%b\n' "$(GREEN)README verbatim target passed$(RESET)"
