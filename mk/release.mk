# Release packaging and publication targets.

RELEASE_DIR ?= release
RELEASE_PLATFORMS ?= linux-x86_64 macos-arm64

.PHONY: restore-release-executable-bits
restore-release-executable-bits:
	printf '%b\n' "$(BLUE)Restoring release artifact executable bits...$(RESET)"
	for platform in $(RELEASE_PLATFORMS); do \
	  dist="dist/$$platform"; \
	  if [ ! -d "$$dist" ]; then \
	    printf '%b\n' "$(RED)Missing release artifact directory: $$dist$(RESET)" >&2; \
	    exit 1; \
	  fi; \
	  for bin in pandapi-parser pandaPIparser pandaPIparser.legacy pandaPIgrounder pandaPIengine; do \
	    if [ -f "$$dist/$$bin" ]; then \
	      chmod +x "$$dist/$$bin"; \
	    fi; \
	  done; \
	done
	printf '%b\n' "$(GREEN)Release artifact executable bits restored$(RESET)"

.PHONY: package-release
package-release:
	if [ -z "$(TAG)" ]; then \
	  printf '%b\n' "$(RED)Set TAG, for example: make package-release TAG=v0.3.0$(RESET)" >&2; \
	  exit 1; \
	fi
	printf '%b\n' "$(BLUE)Packaging release $(TAG)...$(RESET)"
	./scripts/package-release.sh "$(TAG)"
	printf '%b\n' "$(GREEN)Release package ready: release/$(RESET)"

.PHONY: publish-release
publish-release:
	if [ -z "$(TAG)" ]; then \
	  printf '%b\n' "$(RED)Set TAG, for example: make publish-release TAG=v0.3.0$(RESET)" >&2; \
	  exit 1; \
	fi
	printf '%b\n' "$(BLUE)Publishing release $(TAG) from $(RELEASE_DIR)...$(RESET)"
	./scripts/publish-release.sh "$(TAG)" "$(RELEASE_DIR)"
	printf '%b\n' "$(GREEN)Release published: $(TAG)$(RESET)"
