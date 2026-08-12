# Shared Make configuration for chengdu.

SHELL := /bin/bash
.SHELLFLAGS := -eu -o pipefail -c
.SILENT:
MAKEFLAGS += --no-print-directory

# Keep aggregate entrypoints deterministic even when invoked with `make -j`.
# GNU Make 3.81, still common on macOS, does not support target-scoped
# .NOTPARALLEL; component recipes invoke their own parallel sub-builds.
.NOTPARALLEL:

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
RUNTIME_SOURCE_DIR := pandaPI/runtime
RUNTIME_INCLUDE_DIR := $(RUNTIME_SOURCE_DIR)/include
RUNTIME_BUILD_DIR := build/runtime/$(PLATFORM)
RUNTIME_SANITIZE_BUILD_DIR := build/runtime-sanitize/$(PLATFORM)
RUNTIME_COVERAGE_DIR := build/coverage/runtime/$(PLATFORM)
RUNTIME_COVERAGE_BUILD_DIR := $(RUNTIME_COVERAGE_DIR)/build
RUNTIME_COVERAGE_REPORT_DIR := $(RUNTIME_COVERAGE_DIR)/report
RUNTIME_COVERAGE_PROFDATA := $(RUNTIME_COVERAGE_DIR)/runtime.profdata
RUNTIME_COVERAGE_SUMMARY := $(RUNTIME_COVERAGE_REPORT_DIR)/runtime-coverage-summary.txt
RUNTIME_COVERAGE_DETAIL := $(RUNTIME_COVERAGE_REPORT_DIR)/runtime-coverage.txt
RUNTIME_STATIC_ANALYSIS_DIR := build/static-analysis/runtime/$(PLATFORM)
RUNTIME_STATIC_ANALYSIS_BUILD_DIR := $(RUNTIME_STATIC_ANALYSIS_DIR)/build
WARNING_INVENTORY_DIR := build/warnings/$(PLATFORM)
WARNING_INVENTORY_LOG := $(WARNING_INVENTORY_DIR)/build.log
WARNING_INVENTORY_REPORT := $(WARNING_INVENTORY_DIR)/warning-inventory.txt
CLANG_TIDY_CHECKS ?= -*,clang-analyzer-*
CLANG_TIDY_HEADER_FILTER ?= (^|.*/)pandaPI/runtime/(include|src|tests)/.*
OWNED_CPP_DIRS := $(RUNTIME_INCLUDE_DIR) $(RUNTIME_SOURCE_DIR)/src $(RUNTIME_SOURCE_DIR)/tests
OWNED_CPP_FIND := find $(OWNED_CPP_DIRS) -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \) -print
SHELL_SCRIPTS := $(shell find scripts -type f -name '*.sh' -print) tests/contract/run tests/smoke/run tools/release/package tools/provenance/check tools/shared/platform

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
