# Top-level entrypoint for chengdu local development and CI.

.DEFAULT_GOAL := help

include mk/config.mk
include mk/help.mk
include mk/format.mk
include mk/build.mk
include mk/tests.mk
include mk/checks.mk
include mk/ci.mk
include mk/release.mk

-include build/make/*.d
