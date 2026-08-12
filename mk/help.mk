# User-facing help and environment introspection.

.PHONY: help
help:
	printf '\n'; \
	printf '%b\n' "$(CYAN)============================================================$(RESET)"; \
	printf '%b\n' "  $(BLUE)$(PROJECT_NAME) Build System$(RESET)"; \
	printf '%b\n' "$(CYAN)============================================================$(RESET)"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Build:$(RESET)"; \
	printf '%b\n' "  $(YELLOW)make build$(RESET)              - Build runtime, parser, grounder, and engine"; \
	printf '%b\n' "  $(YELLOW)make build-runtime$(RESET)      - Build chengdu-owned runtime"; \
	printf '%b\n' "  $(YELLOW)make build-parser$(RESET)       - Build pandapi-parser into dist/"; \
	printf '%b\n' "  $(YELLOW)make build-grounder$(RESET)     - Build pandapi-grounder into dist/"; \
	printf '%b\n' "  $(YELLOW)make build-engine$(RESET)       - Build pandapi-engine into dist/"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Tests:$(RESET)"; \
	printf '%b\n' "  $(YELLOW)make test$(RESET)               - Build and run runtime, contract, and smoke tests"; \
	printf '%b\n' "  $(YELLOW)make test-runtime$(RESET)       - Run chengdu-owned runtime CTest suite"; \
	printf '%b\n' "  $(YELLOW)make test-runtime-sanitize$(RESET) - Run runtime ASan/UBSan tests"; \
	printf '%b\n' "  $(YELLOW)make test-binary-sanitize$(RESET) - Run pandapi-* ASan/UBSan managed fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-list$(RESET) - List baseline contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-list-managed$(RESET) - List managed contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract$(RESET)      - Run all baseline contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-parser$(RESET) - Run parser contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-parser-managed$(RESET) - Run managed parser contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-grounder$(RESET) - Run grounder contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-grounder-managed$(RESET) - Run managed grounder contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-engine$(RESET) - Run engine contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-engine-managed$(RESET) - Run managed engine contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-pipeline$(RESET) - Run pipeline contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make test-contract-pipeline-managed$(RESET) - Run managed pipeline contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make smoke$(RESET)              - Run positive smoke test"; \
	printf '%b\n' "  $(YELLOW)make smoke-negative$(RESET)     - Run negative smoke test"; \
	printf '%b\n' "  $(YELLOW)make test-corpus CORPUS_DIR=...$(RESET) - Run optional IPC corpus smoke"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Quality:$(RESET)"; \
	printf '%b\n' "  $(YELLOW)make check$(RESET)               - Run pre-commit lint, static, safety, test, and provenance gates"; \
	printf '%b\n' "  $(YELLOW)make actionlint$(RESET)          - Run downloaded actionlint"; \
	printf '%b\n' "  $(YELLOW)make format-check$(RESET)       - Check owned C++ formatting"; \
	printf '%b\n' "  $(YELLOW)make static-analysis$(RESET)    - Run shell and owned C++ static-analysis gates"; \
	printf '%b\n' "  $(YELLOW)make static-analysis-cpp$(RESET) - Run clang-tidy over owned runtime C++"; \
	printf '%b\n' "  $(YELLOW)make warning-inventory$(RESET)  - Capture compiler-warning inventory from make build"; \
	printf '%b\n' "  $(YELLOW)make safety-checks$(RESET)      - Run repository safety checks"; \
	printf '%b\n' "  $(YELLOW)make format$(RESET)             - Format owned C++ source"; \
	printf '%b\n' "  $(YELLOW)make coverage$(RESET)           - Run runtime Clang source coverage"; \
	printf '%b\n' "  $(YELLOW)make source-quality-surface$(RESET) - Write Arc07 source-class selector report"; \
	printf '%b\n' "  $(YELLOW)make source-quality-profile-map$(RESET) - Write Arc07 source/profile mapping report"; \
	printf '%b\n' "  $(YELLOW)make source-quality-naming-report$(RESET) - Write Arc07 source naming normalization report"; \
	printf '%b\n' "  $(YELLOW)make source-quality-naming-check$(RESET) - Check first-party source naming policy"; \
	printf '%b\n' "  $(YELLOW)make compile-db-parser$(RESET)   - Generate parser source-quality compile database"; \
	printf '%b\n' "  $(YELLOW)make compile-db-grounder$(RESET) - Generate grounder source-quality compile database"; \
	printf '%b\n' "  $(YELLOW)make compile-db-engine$(RESET)   - Generate engine source-quality compile database"; \
	printf '%b\n' "  $(YELLOW)make compile-db-first-party$(RESET) - Generate parser, grounder, and engine compile databases"; \
	printf '%b\n' "  $(YELLOW)make source-quality-gate-report$(RESET) - Write first-party quality gate scaffold report"; \
	printf '%b\n' "  $(YELLOW)make format-check-first-party$(RESET) - Run/report first-party format scaffold"; \
	printf '%b\n' "  $(YELLOW)make static-analysis-first-party$(RESET) - Run/report first-party static-analysis scaffold"; \
	printf '%b\n' "  $(YELLOW)make coverage-first-party$(RESET) - Run/report first-party coverage scaffold"; \
	printf '%b\n' "  $(YELLOW)make warning-inventory-first-party$(RESET) - Run/report first-party warning scaffold"; \
	printf '%b\n' "  $(YELLOW)make generated-warning-triage$(RESET) - Write generated-warning root-cause triage"; \
	printf '%b\n' "  $(YELLOW)make test-unit$(RESET)           - Run/report unit and seam test scaffold"; \
	printf '%b\n' "  $(YELLOW)make sanitize-first-party$(RESET) - Run/report first-party sanitizer scaffold"; \
	printf '%b\n' "  $(YELLOW)make sanitize-runtime$(RESET)   - Build and run runtime ASan/UBSan gate"; \
	printf '%b\n' "  $(YELLOW)make sanitize-binaries$(RESET)  - Build pandapi-* ASan/UBSan binaries and run managed fixtures"; \
	printf '%b\n' "  $(YELLOW)make provenance-check$(RESET)   - Verify dist provenance"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)CI entrypoints:$(RESET)"; \
	printf '%b\n' "  $(YELLOW)make ci-linux$(RESET)           - Local equivalent of Linux build leg"; \
	printf '%b\n' "  $(YELLOW)make ci-macos$(RESET)           - Local equivalent of macOS build leg"; \
	printf '%b\n' "  $(YELLOW)make readme-verbatim$(RESET)    - Build/smoke path documented in README"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Release:$(RESET)"; \
	printf '%b\n' "  $(YELLOW)make restore-release-executable-bits$(RESET) - Restore executable bits on release artifacts"; \
	printf '%b\n' "  $(YELLOW)make package-release TAG=vX.Y.Z$(RESET) - Package release assets"; \
	printf '%b\n' "  $(YELLOW)make publish-release TAG=vX.Y.Z$(RESET) - Publish packaged release"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Utilities:$(RESET)"; \
	printf '%b\n' "  $(YELLOW)make install-actionlint$(RESET) - Download actionlint for workflow linting"; \
	printf '%b\n' "  $(YELLOW)make check-tools$(RESET)        - Verify required tools"; \
	printf '%b\n' "  $(YELLOW)make record-min-os$(RESET)      - Append macOS min-OS observation"; \
	printf '%b\n' "  $(YELLOW)make info$(RESET)               - Show build information"; \
	printf '\n'; \
	printf '%b\n' "$(CYAN)Status:$(RESET) Branch: $(GIT_BRANCH) | Commit: $(GIT_COMMIT) | Platform: $(PLATFORM)"; \
	printf '\n'

.PHONY: info
info:
	printf '\n'; \
	printf '%b\n' "$(CYAN)============================================================$(RESET)"; \
	printf '%b\n' "  $(BLUE)Build Information$(RESET)"; \
	printf '%b\n' "$(CYAN)============================================================$(RESET)"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Project:$(RESET)"; \
	printf '%s\n' "  Name:       $(PROJECT_NAME)"; \
	printf '%s\n' "  Platform:   $(PLATFORM)"; \
	printf '%s\n' "  Build Time: $(BUILD_TIME)"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Paths:$(RESET)"; \
	printf '%s\n' "  Dist Dir:   $(DIST_DIR)/"; \
	printf '%s\n' "  Runtime:    $(RUNTIME_BUILD_DIR)/"; \
	printf '%s\n' "  Workspace:  $$(pwd)"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Git:$(RESET)"; \
	printf '%s\n' "  Branch:     $(GIT_BRANCH)"; \
	printf '%s\n' "  Commit:     $(GIT_COMMIT)"; \
	printf '\n'

.PHONY: check-tools
check-tools:
	printf '%b\n' "$(BLUE)Checking required tools...$(RESET)"; \
	command -v git >/dev/null 2>&1 && printf '%b\n' "$(GREEN)git found: $$(git --version)$(RESET)" || { printf '%b\n' "$(RED)git not found$(RESET)"; exit 1; }; \
	command -v make >/dev/null 2>&1 && printf '%b\n' "$(GREEN)make found: $$(make --version | head -1)$(RESET)" || { printf '%b\n' "$(RED)make not found$(RESET)"; exit 1; }; \
	command -v cmake >/dev/null 2>&1 && printf '%b\n' "$(GREEN)cmake found: $$(cmake --version | head -1)$(RESET)" || { printf '%b\n' "$(RED)cmake not found$(RESET)"; exit 1; }; \
	command -v flex >/dev/null 2>&1 && printf '%b\n' "$(GREEN)flex found$(RESET)" || { printf '%b\n' "$(RED)flex not found$(RESET)"; exit 1; }; \
	command -v bison >/dev/null 2>&1 && printf '%b\n' "$(GREEN)bison found$(RESET)" || { printf '%b\n' "$(RED)bison not found$(RESET)"; exit 1; }; \
	command -v gengetopt >/dev/null 2>&1 && printf '%b\n' "$(GREEN)gengetopt found$(RESET)" || { printf '%b\n' "$(RED)gengetopt not found$(RESET)"; exit 1; }; \
	command -v zip >/dev/null 2>&1 && printf '%b\n' "$(GREEN)zip found$(RESET)" || { printf '%b\n' "$(RED)zip not found$(RESET)"; exit 1; }; \
	{ command -v clang-format >/dev/null 2>&1 || command -v xcrun >/dev/null 2>&1; } && printf '%b\n' "$(GREEN)clang-format path available$(RESET)" || { printf '%b\n' "$(RED)clang-format not found$(RESET)"; exit 1; }
