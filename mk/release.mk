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
	  for bin in pandapi-parser pandaPIgrounder pandaPIengine; do \
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
	./tools/release/package "$(TAG)"
	printf '%b\n' "$(GREEN)Release package ready: release/$(RESET)"

.PHONY: publish-release
publish-release:
	if [ -z "$(TAG)" ]; then \
	  printf '%b\n' "$(RED)Set TAG, for example: make publish-release TAG=v0.3.0$(RESET)" >&2; \
	  exit 1; \
	fi
	printf '%b\n' "$(BLUE)Publishing release $(TAG) from $(RELEASE_DIR)...$(RESET)"
	release_dir="$(RELEASE_DIR)"; \
	if [ ! -f "$$release_dir/PRERELEASE" ]; then \
	  printf '%b\n' "$(RED)publish-release: $$release_dir/PRERELEASE not found; run make package-release first$(RESET)" >&2; \
	  exit 1; \
	fi; \
	for asset in SHA256SUMS release-manifest.txt THIRD-PARTY-LICENSES notes.md; do \
	  if [ ! -f "$$release_dir/$$asset" ]; then \
	    printf '%b\n' "$(RED)publish-release: $$release_dir/$$asset missing; run make package-release first$(RESET)" >&2; \
	    exit 1; \
	  fi; \
	done; \
	tarball_count="$$(find "$$release_dir" -maxdepth 1 -name 'pandapi-*.tar.gz' | wc -l | tr -d ' ')"; \
	if [ "$$tarball_count" -eq 0 ]; then \
	  printf '%b\n' "$(RED)publish-release: no pandapi-*.tar.gz found in $$release_dir$(RESET)" >&2; \
	  exit 1; \
	fi; \
	prerelease_args=(); \
	if [ "$$(cat "$$release_dir/PRERELEASE")" = "true" ]; then \
	  prerelease_args+=(--prerelease); \
	fi; \
	gh release create "$(TAG)" \
	  --title "$(TAG)" \
	  --notes-file "$$release_dir/notes.md" \
	  "$${prerelease_args[@]}" \
	  "$$release_dir"/pandapi-*.tar.gz "$$release_dir/SHA256SUMS" \
	  "$$release_dir/release-manifest.txt" "$$release_dir/THIRD-PARTY-LICENSES"; \
	printf '%b\n' "$(GREEN)publish-release: published $(TAG) ($$tarball_count tarball(s) + SHA256SUMS + release-manifest.txt + THIRD-PARTY-LICENSES)$(RESET)"
	printf '%b\n' "$(GREEN)Release published: $(TAG)$(RESET)"
