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

.PHONY: shell-syntax
shell-syntax:
	printf '%b\n' "$(BLUE)Checking shell script syntax...$(RESET)"
	for script in $(SHELL_SCRIPTS); do \
	  bash -n "$$script"; \
	done
	printf '%b\n' "$(GREEN)Shell script syntax passed$(RESET)"

.PHONY: shellcheck
shellcheck:
	printf '%b\n' "$(BLUE)Running shellcheck...$(RESET)"
	if ! command -v shellcheck >/dev/null 2>&1; then \
	  printf '%b\n' "$(RED)shellcheck not found$(RESET)" >&2; \
	  exit 1; \
	fi
	shellcheck $(SHELL_SCRIPTS)
	printf '%b\n' "$(GREEN)shellcheck passed$(RESET)"

.PHONY: static-analysis
static-analysis: shell-syntax shellcheck
	printf '%b\n' "$(GREEN)Static analysis passed$(RESET)"

.PHONY: workflow-make-entrypoints-check
workflow-make-entrypoints-check:
	printf '%b\n' "$(BLUE)Checking workflow entrypoints use make...$(RESET)"
	matches="$$(grep -REn '(\./scripts/|scripts/[^[:space:]"]+\.sh|bash[[:space:]].*actionlint|curl[[:space:]].*actionlint|\./actionlint)' .github/workflows || true)"; \
	if [ -n "$$matches" ]; then \
	  printf '%s\n' "$$matches" >&2; \
	  printf '%b\n' "$(RED)Workflow steps must call make targets instead of project scripts$(RESET)" >&2; \
	  exit 1; \
	fi
	printf '%b\n' "$(GREEN)Workflow entrypoints use make$(RESET)"

.PHONY: safety-checks
safety-checks: workflow-make-entrypoints-check
	printf '%b\n' "$(BLUE)Running repository safety checks...$(RESET)"
	git diff --check
	nested_git="$$(find pandaPI -name .git -print)"; \
	if [ -n "$$nested_git" ]; then \
	  printf '%s\n' "$$nested_git" >&2; \
	  printf '%b\n' "$(RED)Nested Git metadata found under pandaPI$(RESET)" >&2; \
	  exit 1; \
	fi
	gitlinks="$$(git ls-files -s pandaPI | awk '$$1 == "160000" { print $$4 }')"; \
	if [ -n "$$gitlinks" ]; then \
	  printf '%s\n' "$$gitlinks" >&2; \
	  printf '%b\n' "$(RED)Gitlink entries found under pandaPI$(RESET)" >&2; \
	  exit 1; \
	fi
	printf '%b\n' "$(GREEN)Repository safety checks passed$(RESET)"

.PHONY: provenance-check
provenance-check:
	printf '%b\n' "$(BLUE)Checking provenance...$(RESET)"
	./tools/provenance/check
	printf '%b\n' "$(GREEN)Provenance check passed$(RESET)"

.PHONY: record-min-os
record-min-os:
	printf '%b\n' "$(BLUE)Recording macOS minimum-OS observation...$(RESET)"
	dist_dir="dist/macos-arm64"; \
	provenance="$$dist_dir/provenance.txt"; \
	if [ ! -d "$$dist_dir" ]; then \
	  printf '%b\n' "$(RED)record-min-os: $$dist_dir missing; run make build first$(RESET)" >&2; \
	  exit 1; \
	fi; \
	{ \
	  printf '%s\n' "min-os-macos-arm64:"; \
	  for bin in pandaPIparser pandaPIgrounder pandaPIengine; do \
	    path="$$dist_dir/$$bin"; \
	    if [ ! -x "$$path" ]; then \
	      printf '%b\n' "$(RED)record-min-os: $$path missing or not executable$(RESET)" >&2; \
	      exit 1; \
	    fi; \
	    minos="$$(otool -l "$$path" | awk '/LC_BUILD_VERSION/{f=1} f && /minos/{print $$2; exit}')"; \
	    printf '%s=%s\n' "$$bin" "$$minos"; \
	  done; \
	  printf '%s\n' "---"; \
	} >> "$$provenance"
	printf '%b\n' "$(GREEN)Minimum-OS observation recorded$(RESET)"

.PHONY: record-min-os-if-macos
record-min-os-if-macos:
	if [ "$(PLATFORM)" = "macos-arm64" ]; then \
	  $(MAKE) record-min-os; \
	else \
	  printf '%b\n' "$(YELLOW)Skipping macOS minimum-OS observation on $(PLATFORM)$(RESET)"; \
	fi

.PHONY: check
check: check-tools actionlint static-analysis format-check safety-checks test record-min-os-if-macos provenance-check
	printf '%b\n' "$(GREEN)All checks passed$(RESET)"
