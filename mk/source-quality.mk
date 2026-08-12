# Source-quality selector and compile database surfaces for Arc07.

.PHONY: source-quality-surface
source-quality-surface:
	printf '%b\n' "$(BLUE)Writing source-quality selector surface for $(PLATFORM)...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" surface "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality surface report: $(SOURCE_QUALITY_DIR)/source-quality-surface.md$(RESET)"

.PHONY: source-quality-profile-map
source-quality-profile-map:
	printf '%b\n' "$(BLUE)Writing source-quality profile/source map for $(PLATFORM)...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" profile-map "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality profile map: $(SOURCE_QUALITY_DIR)/source-quality-profile-map.md$(RESET)"

.PHONY: source-quality-naming-report
source-quality-naming-report:
	printf '%b\n' "$(BLUE)Writing source-quality naming normalization report...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" naming-report "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality naming report: docs/design-v0.3.0/arc07-source-quality-expansion/source-naming-normalization.md$(RESET)"

.PHONY: source-quality-naming-check
source-quality-naming-check:
	printf '%b\n' "$(BLUE)Checking first-party source naming policy...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" naming-check "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Source-quality naming check passed$(RESET)"

.PHONY: compile-db-parser
compile-db-parser:
	printf '%b\n' "$(BLUE)Generating parser source-quality compile database...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" compile-db parser "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Parser compile database: $(SOURCE_QUALITY_COMPILE_DB_DIR)/parser/compile_commands.json$(RESET)"

.PHONY: compile-db-grounder
compile-db-grounder:
	printf '%b\n' "$(BLUE)Generating grounder source-quality compile database...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" compile-db grounder "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Grounder compile database: $(SOURCE_QUALITY_COMPILE_DB_DIR)/grounder/compile_commands.json$(RESET)"

.PHONY: compile-db-engine
compile-db-engine:
	printf '%b\n' "$(BLUE)Generating engine source-quality compile database...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" compile-db engine "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)Engine compile database: $(SOURCE_QUALITY_COMPILE_DB_DIR)/engine/compile_commands.json$(RESET)"

.PHONY: compile-db-first-party
compile-db-first-party: compile-db-parser compile-db-grounder compile-db-engine
	printf '%b\n' "$(BLUE)Writing first-party compile database summary...$(RESET)"
	"$(SOURCE_QUALITY_TOOL)" compile-db-summary "$(CURDIR)" "$(PLATFORM)"
	printf '%b\n' "$(GREEN)First-party compile database summary: $(SOURCE_QUALITY_COMPILE_DB_DIR)/first-party-selected.txt$(RESET)"
