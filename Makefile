CC := gcc-14

CONSUMER_EXEC := screen
PRODUCER_EXEC := stream

BUILD_DIR := ./build
SRCS_DIR := ./src
TESTS_DIR := ./test

SRCS := $(shell find $(SRCS_DIR) -name '*.c')
TESTS := $(shell find $(TESTS_DIR) -name '*.c')

SRC_OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
TEST_OBJS := $(TESTS:%=$(BUILD_DIR)/%.o)

SRC_DEPS := $(SRC_OBJS:.o=.d)
TEST_DEPS := $(TEST_OBJS:.o=.d)

INC_DIRS := $(shell find $(SRCS_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP

.PHONY: all clean

all: $(BUILD_DIR)/$(CONSUMER_EXEC) $(BUILD_DIR)/$(PRODUCER_EXEC)

$(BUILD_DIR)/$(CONSUMER_EXEC): $(SRC_OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/$(PRODUCER_EXEC): $(TEST_OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) -std=gnu23 -Wall -O3 -c $< -o $@

clean:
	rm -r $(BUILD_DIR)

-include $(SRC_DEPS)
-include $(TEST_DEPS)
