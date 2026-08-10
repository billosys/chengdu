# Formatting gates for chengdu-owned C++ source.

.PHONY: format-check
format-check:
	printf '%b\n' "$(BLUE)Checking owned C++ formatting...$(RESET)"; \
	$(resolve_clang_format); \
	files="$$( $(OWNED_CPP_FIND) )"; \
	if [ -z "$$files" ]; then \
	  printf '%b\n' "$(YELLOW)No owned C++ files found$(RESET)"; \
	  exit 0; \
	fi; \
	if "$$CLANG_FORMAT_BIN" --help 2>&1 | grep -q -- '--dry-run'; then \
	  if ! "$$CLANG_FORMAT_BIN" --dry-run --Werror $$files; then \
	    exit 1; \
	  fi; \
	else \
	  failed=0; \
	  for file in $$files; do \
	    if ! "$$CLANG_FORMAT_BIN" "$$file" | diff -u "$$file" - >/dev/null; then \
	      printf '%b\n' "$(RED)format drift: $${file}$(RESET)" >&2; \
	      failed=1; \
	    fi; \
	  done; \
	  if [ "$$failed" -ne 0 ]; then \
	    exit 1; \
	  fi; \
	fi; \
	count="$$(printf '%s\n' $$files | wc -l | tr -d ' ')"; \
	printf '%b\n' "$(GREEN)Format check passed for $$count files$(RESET)"

.PHONY: format
format:
	printf '%b\n' "$(BLUE)Formatting owned C++ source...$(RESET)"; \
	$(resolve_clang_format); \
	files="$$( $(OWNED_CPP_FIND) )"; \
	if [ -z "$$files" ]; then \
	  printf '%b\n' "$(YELLOW)No owned C++ files found$(RESET)"; \
	  exit 0; \
	fi; \
	"$$CLANG_FORMAT_BIN" -i $$files; \
	count="$$(printf '%s\n' $$files | wc -l | tr -d ' ')"; \
	printf '%b\n' "$(GREEN)Formatted $$count files$(RESET)"
