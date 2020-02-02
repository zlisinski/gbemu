.PHONY: all clean test

BUILD_DIR = build
BINS = zlgb test_zlgb

all:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && make -j$(nproc)

test:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && make -j$(nproc)
	./test_zlgb

clean:
	rm -rf $(BUILD_DIR) $(BINS)
