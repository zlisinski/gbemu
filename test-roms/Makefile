ASM := wla-gb
LINK := wlalink
ASM_FLAGS :=
LINK_FLAGS := -s

BUILD_DIR := build
SRC := $(shell find . -type f -name '*.s')
INCLUDES := $(shell find include -type f -name '*.inc')
ROMS := $(SRC:%.s=%.gb)

.PHONY: all clean

# Don't delete intermediate files.
.PRECIOUS: $(BUILD_DIR)/%.o $(BUILD_DIR)/%.link

# Build all ROM files.
all: $(ROMS)

# Create build dir.
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Assemble object files. Depends on single source file, and all includes.
# Build dir must exist, but last modified time is not considered.
$(BUILD_DIR)/%.o: %.s $(INCLUDES) | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) -o $@ $<

# Create link files. Depends on object file.
$(BUILD_DIR)/%.link: $(BUILD_DIR)/%.o
	@echo "[objects]\n$<" > $@

# Link object files into ROM files. Depends on link file.
%.gb: $(BUILD_DIR)/%.link
	$(LINK) $(LINK_FLAGS) $< $@

clean:
	rm -rf $(BUILD_DIR) *.gb *.sym