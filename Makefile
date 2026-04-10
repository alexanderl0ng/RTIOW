CXX 			:= g++
SRC_DIR 		:= src
SRCS 			:= $(wildcard $(SRC_DIR)/*.cpp)

DEBUG_DIR 		:= debug
RELEASE_DIR 	:= release
DEBUG_OUT 		:= $(DEBUG_DIR)/RTIOW
RELEASE_OUT 	:= $(RELEASE_DIR)/RTIOW

DEBUG_FLAGS 	:= -std=c++20 -Wall -Wextra -g -O0 -DDEBUG
RELEASE_FLAGS 	:= -std=c++20 -Wall -Wextra -O2 -DNDEBUG

.PHONY: all debug release clean run-debug run-release

all: debug release

debug: | $(DEBUG_DIR)/
	$(CXX) $(DEBUG_FLAGS) $(SRCS) -o $(DEBUG_OUT)

release: | $(RELEASE_DIR)/
	$(CXX) $(RELEASE_FLAGS) $(SRCS) -o $(RELEASE_OUT)

$(DEBUG_DIR)/:
	mkdir -p $@

$(RELEASE_DIR)/:
	mkdir -p $@

run-debug: debug
	./$(DEBUG_OUT)

run-release: release
	./$(RELEASE_OUT)

clean:
	rm -rf $(DEBUG_DIR) $(RELEASE_DIR)

