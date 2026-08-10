# Shared Make configuration for chengdu.

SHELL := /bin/bash
.SHELLFLAGS := -eu -o pipefail -c
.SILENT:
MAKEFLAGS += --no-print-directory

BLUE := \033[1;34m
GREEN := \033[1;32m
YELLOW := \033[1;33m
RED := \033[1;31m
CYAN := \033[1;36m
RESET := \033[0m

PROJECT_NAME := chengdu
GIT_COMMIT := $(shell git rev-parse --short HEAD 2>/dev/null || echo unknown)
GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)
BUILD_TIME := $(shell date -u '+%Y-%m-%dT%H:%M:%SZ')
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

ifeq ($(UNAME_S)-$(UNAME_M),Linux-x86_64)
PLATFORM := linux-x86_64
else ifeq ($(UNAME_S)-$(UNAME_M),Darwin-arm64)
PLATFORM := macos-arm64
else
PLATFORM := unsupported
endif

DIST_DIR := dist/$(PLATFORM)
RUNTIME_BUILD_DIR := build/runtime/$(PLATFORM)
RUNTIME_SANITIZE_BUILD_DIR := build/runtime-sanitize/$(PLATFORM)
OWNED_CPP_DIRS := pandapi-runtime/include pandapi-runtime/src pandapi-runtime/tests
OWNED_CPP_FIND := find $(OWNED_CPP_DIRS) -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \) -print

define banner
printf '\n'
printf '%b\n' "$(CYAN)============================================================$(RESET)"
printf '%b\n' "  $(BLUE)$(1)$(RESET)"
printf '%b\n' "$(CYAN)============================================================$(RESET)"
printf '\n'
endef

define resolve_clang_format
CLANG_FORMAT_BIN="$${CLANG_FORMAT:-}"; \
if [ -z "$$CLANG_FORMAT_BIN" ]; then \
  if command -v clang-format >/dev/null 2>&1; then \
    CLANG_FORMAT_BIN="$$(command -v clang-format)"; \
  elif command -v xcrun >/dev/null 2>&1; then \
    CLANG_FORMAT_BIN="$$(xcrun --find clang-format 2>/dev/null || true)"; \
  fi; \
fi; \
if [ -z "$$CLANG_FORMAT_BIN" ] || [ ! -x "$$CLANG_FORMAT_BIN" ]; then \
  printf '%b\n' "$(RED)clang-format not found on PATH or through xcrun$(RESET)" >&2; \
  exit 1; \
fi; \
CLANG_FORMAT_VERSION="$$("$$CLANG_FORMAT_BIN" --version)"; \
printf '%b\n' "$(CYAN)Using $$CLANG_FORMAT_BIN ($$CLANG_FORMAT_VERSION)$(RESET)"
endef
