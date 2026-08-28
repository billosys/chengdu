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
BIN_DIR ?= bin
DEV_STRICT_CONFIG := build/make/dev-strict.d
CHENGDU_DEV_STRICT ?= 0
PANDAPI_REQUIRE_CATCH2 = $(if $(filter 1 true TRUE yes YES on ON,$(CHENGDU_DEV_STRICT)),ON,OFF)
RUNTIME_SOURCE_DIR := pandaPI/runtime
RUNTIME_INCLUDE_DIR := $(RUNTIME_SOURCE_DIR)/include
RUNTIME_BUILD_DIR := build/runtime/$(PLATFORM)
RUNTIME_SANITIZE_BUILD_DIR := build/runtime-sanitize/$(PLATFORM)
GROUNDER_SOURCE_DIR := pandaPI/grounder/src
GROUNDER_TEST_DIR := pandaPI/grounder/tests
GROUNDER_UNIT_BUILD_DIR := build/grounder-unit/$(PLATFORM)
GROUNDER_SANITIZE_BUILD_DIR := build/grounder-sanitize/$(PLATFORM)
ENGINE_SOURCE_DIR := pandaPI/engine/src
ENGINE_UNIT_BUILD_DIR := build/engine-unit/$(PLATFORM)
ENGINE_SANITIZE_BUILD_DIR := build/engine-sanitize/$(PLATFORM)
BINARY_SANITIZE_DIR := build/binary-sanitize/$(PLATFORM)
BINARY_SANITIZE_DIST_DIR := $(BINARY_SANITIZE_DIR)/dist
BINARY_SANITIZER_FLAGS ?= -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
BINARY_SANITIZER_LINK_FLAGS ?= -fsanitize=address,undefined
ifeq ($(UNAME_S),Darwin)
BINARY_SANITIZER_ASAN_OPTIONS ?= detect_leaks=0:halt_on_error=1:abort_on_error=0
BINARY_SANITIZER_LSAN_STATUS := SKIP: macOS ASan leak detection is disabled for this gate; Re-entry: run on Linux or another LeakSanitizer-capable toolchain with detect_leaks=1.
else
BINARY_SANITIZER_ASAN_OPTIONS ?= detect_leaks=1:halt_on_error=1:abort_on_error=0
BINARY_SANITIZER_LSAN_STATUS := enabled: LeakSanitizer runs through ASan with detect_leaks=1 on this platform/toolchain.
endif
BINARY_SANITIZER_UBSAN_OPTIONS ?= halt_on_error=1:print_stacktrace=1
RUNTIME_COVERAGE_DIR := build/coverage/runtime/$(PLATFORM)
RUNTIME_COVERAGE_BUILD_DIR := $(RUNTIME_COVERAGE_DIR)/build
RUNTIME_COVERAGE_REPORT_DIR := $(RUNTIME_COVERAGE_DIR)/report
RUNTIME_COVERAGE_PROFDATA := $(RUNTIME_COVERAGE_DIR)/runtime.profdata
RUNTIME_COVERAGE_SUMMARY := $(RUNTIME_COVERAGE_REPORT_DIR)/runtime-coverage-summary.txt
RUNTIME_COVERAGE_DETAIL := $(RUNTIME_COVERAGE_REPORT_DIR)/runtime-coverage.txt
ENGINE_COVERAGE_DIR := build/coverage/engine/$(PLATFORM)
ENGINE_COVERAGE_BUILD_DIR := $(ENGINE_COVERAGE_DIR)/build
ENGINE_COVERAGE_REPORT_DIR := $(ENGINE_COVERAGE_DIR)/report
ENGINE_COVERAGE_PROFDATA := $(ENGINE_COVERAGE_DIR)/engine.profdata
ENGINE_COVERAGE_SUMMARY := $(ENGINE_COVERAGE_REPORT_DIR)/engine-coverage-summary.txt
ENGINE_COVERAGE_DETAIL := $(ENGINE_COVERAGE_REPORT_DIR)/engine-coverage.txt
GROUNDER_COVERAGE_DIR := build/coverage/grounder/$(PLATFORM)
GROUNDER_COVERAGE_BUILD_DIR := $(GROUNDER_COVERAGE_DIR)/build
GROUNDER_COVERAGE_REPORT_DIR := $(GROUNDER_COVERAGE_DIR)/report
GROUNDER_COVERAGE_PROFDATA := $(GROUNDER_COVERAGE_DIR)/grounder.profdata
GROUNDER_COVERAGE_SUMMARY := $(GROUNDER_COVERAGE_REPORT_DIR)/grounder-coverage-summary.txt
GROUNDER_COVERAGE_DETAIL := $(GROUNDER_COVERAGE_REPORT_DIR)/grounder-coverage.txt
RUNTIME_STATIC_ANALYSIS_DIR := build/static-analysis/runtime/$(PLATFORM)
RUNTIME_STATIC_ANALYSIS_BUILD_DIR := $(RUNTIME_STATIC_ANALYSIS_DIR)/build
GROUNDER_STATIC_ANALYSIS_DIR := build/static-analysis/grounder/$(PLATFORM)
GROUNDER_STATIC_ANALYSIS_REPORT := $(GROUNDER_STATIC_ANALYSIS_DIR)/clang-tidy.txt
ENGINE_STATIC_ANALYSIS_DIR := build/static-analysis/engine/$(PLATFORM)
ENGINE_STATIC_ANALYSIS_REPORT := $(ENGINE_STATIC_ANALYSIS_DIR)/clang-tidy.txt
GROUNDER_FORMAT_REPORT = $(SOURCE_QUALITY_DIR)/format-check-grounder.md
GROUNDER_TEST_UNIT_REPORT = $(SOURCE_QUALITY_DIR)/test-unit-grounder.md
GROUNDER_WARNING_REPORT = $(SOURCE_QUALITY_DIR)/warning-inventory-grounder.md
GROUNDER_SANITIZE_REPORT = $(SOURCE_QUALITY_DIR)/sanitize-grounder.md
ENGINE_FORMAT_REPORT = $(SOURCE_QUALITY_DIR)/format-check-engine.md
ENGINE_TEST_UNIT_REPORT = $(SOURCE_QUALITY_DIR)/test-unit-engine.md
ENGINE_WARNING_REPORT = $(SOURCE_QUALITY_DIR)/warning-inventory-engine.md
ENGINE_SANITIZE_REPORT = $(SOURCE_QUALITY_DIR)/sanitize-engine.md
WARNING_INVENTORY_DIR := build/warnings/$(PLATFORM)
WARNING_INVENTORY_LOG := $(WARNING_INVENTORY_DIR)/build.log
WARNING_INVENTORY_REPORT := $(WARNING_INVENTORY_DIR)/warning-inventory.txt
SOURCE_QUALITY_DIR := build/source-quality/$(PLATFORM)
SOURCE_QUALITY_COMPILE_DB_DIR := $(SOURCE_QUALITY_DIR)/compile-db
SOURCE_QUALITY_TOOL := tools/source-quality/surface
SOURCE_QUALITY_COPIED_BUILD_ROOT_PATTERN := build/$(PLATFORM)/source
SOURCE_QUALITY_BUILD_OUTPUT_DIRS := build dist release
SOURCE_QUALITY_FIRST_PARTY_ROOTS := pandaPI/parser pandaPI/grounder pandaPI/engine pandaPI/runtime
SOURCE_QUALITY_GENERATOR_PATTERNS := hddl.y hddl-token.l options.ggo
SOURCE_QUALITY_GENERATED_PATTERNS := cmdline.c cmdline.h hddl.cpp hddl.hpp hddl-token.cpp
SOURCE_QUALITY_THIRD_PARTY_ROOTS := pandaPI/grounder/cpddl pandaPI/grounder/h2-fd-preprocessor pandaPI/engine/src/symbolic_search/cudd-3.0.0
SOURCE_QUALITY_DEPENDENCY_ROOTS := pandaPI/grounder/cpddl pandaPI/grounder/h2-fd-preprocessor
SOURCE_QUALITY_COPIED_BUILD_ROOTS := $(SOURCE_QUALITY_COPIED_BUILD_ROOT_PATTERN)
CLANG_TIDY_CHECKS ?= -*,clang-analyzer-*
CLANG_TIDY_HEADER_FILTER ?= (^|.*/)pandaPI/runtime/(include|src|tests)/.*
OWNED_CPP_DIRS := $(RUNTIME_INCLUDE_DIR) $(RUNTIME_SOURCE_DIR)/src $(RUNTIME_SOURCE_DIR)/tests
OWNED_CPP_FIND := find $(OWNED_CPP_DIRS) -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \) -print
SHELL_SCRIPTS := $(shell if [ -d scripts ]; then find scripts -type f -name '*.sh' -print; fi) tests/contract/run tests/smoke/run tools/release/package tools/provenance/check tools/shared/platform $(SOURCE_QUALITY_TOOL)

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
