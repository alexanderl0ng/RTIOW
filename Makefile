CXX 			:= g++
EMCC			:= emcc
SRC_DIR 		:= src
SRCS 			:= $(wildcard $(SRC_DIR)/*.cpp)

DEBUG_DIR 		:= build/debug
RELEASE_DIR 	:= build/release
WASM_DIR		:= build/wasm
DEBUG_OUT 		:= $(DEBUG_DIR)/rtiow
RELEASE_OUT 	:= $(RELEASE_DIR)/rtiow
WASM_OUT		:= $(WASM_DIR)/rtiow

DEBUG_FLAGS 	:= -std=c++20 -Wall -Wextra -g -O0 -DDEBUG
RELEASE_FLAGS 	:= -std=c++20 -Wall -Wextra -O2 -DNDEBUG
WASM_FLAGS		:= -std=c++20 -Wall -Wextra -O3 -DNDEBUG \
				   -s WASM=1 \
				   -s MODULARIZE=1 \
				   -s EXPORT_NAME="RTIOW" \
				   -s ALLOW_MEMORY_GROWTH=1 \
				   -s EXPORTED_FUNCTIONS='["_render", "_create_buffer", "_destroy_buffer"]' \
				   -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap", "HEAPU8"]'

.PHONY: all debug release wasm clean run-debug run-release

all: debug release wasm

debug: | $(DEBUG_DIR)/
	$(CXX) $(DEBUG_FLAGS) $(SRCS) -o $(DEBUG_OUT)

release: | $(RELEASE_DIR)/
	$(CXX) $(RELEASE_FLAGS) $(SRCS) -o $(RELEASE_OUT)

wasm: | $(WASM_DIR)/
	source ~/Developer/emsdk/emsdk_env.sh && $(EMCC) $(WASM_FLAGS) $(SRCS) -o $(WASM_OUT).js

$(DEBUG_DIR)/:
	mkdir -p $@

$(RELEASE_DIR)/:
	mkdir -p $@

$(WASM_DIR)/:
	mkdir -p $@

run-debug: debug
	./$(DEBUG_OUT)

run-release: release
	./$(RELEASE_OUT)

clean:
	rm -rf $(DEBUG_DIR) $(RELEASE_DIR) $(WASM_DIR)

