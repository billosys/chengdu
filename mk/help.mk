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
	printf '%b\n' "  $(YELLOW)make build-runtime$(RESET)      - Build and test chengdu-owned runtime"; \
	printf '%b\n' "  $(YELLOW)make build-parser$(RESET)       - Build inherited pandaPIparser into dist/"; \
	printf '%b\n' "  $(YELLOW)make build-grounder$(RESET)     - Build inherited pandaPIgrounder into dist/"; \
	printf '%b\n' "  $(YELLOW)make build-engine$(RESET)       - Build inherited pandaPIengine into dist/"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Quality:$(RESET)"; \
	printf '%b\n' "  $(YELLOW)make format-check$(RESET)       - Check owned C++ formatting"; \
	printf '%b\n' "  $(YELLOW)make format$(RESET)             - Format owned C++ source"; \
	printf '%b\n' "  $(YELLOW)make sanitize-runtime$(RESET)   - Run runtime ASan/UBSan gate"; \
	printf '%b\n' "  $(YELLOW)make contract-baseline$(RESET)  - Run baseline contract fixtures"; \
	printf '%b\n' "  $(YELLOW)make smoke$(RESET)              - Run positive smoke test"; \
	printf '%b\n' "  $(YELLOW)make smoke-negative$(RESET)     - Run negative smoke test"; \
	printf '%b\n' "  $(YELLOW)make provenance-check$(RESET)   - Verify dist provenance"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)CI entrypoints:$(RESET)"; \
	printf '%b\n' "  $(YELLOW)make ci-linux$(RESET)           - Local equivalent of Linux build leg"; \
	printf '%b\n' "  $(YELLOW)make ci-macos$(RESET)           - Local equivalent of macOS build leg"; \
	printf '%b\n' "  $(YELLOW)make readme-verbatim$(RESET)    - Build/smoke path documented in README"; \
	printf '\n'; \
	printf '%b\n' "$(GREEN)Utilities:$(RESET)"; \
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
