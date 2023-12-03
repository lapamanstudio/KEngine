# Variable definitions
CC = gcc
CFLAGS = -Wall -g
INC_DIR = core/include
SRC_DIR = core/src
OUT_DIR = out

# Function to recursively find .c files in a directory
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

# Find all .c files
SRCS := $(call rwildcard, $(SRC_DIR)/, *.c)

# Convert the .c files to their corresponding .o files in the OUT_DIR
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OUT_DIR)/%.o, $(SRCS))

# Function to create directories in Windows
define make-dir
	@cmd /C if not exist "$(subst /,\,$(strip $(1)))" mkdir "$(subst /,\,$(strip $(1)))"
endef

# Main rule
all: build

# Rule to build the project
build: $(OBJS)
	$(call make-dir, $(OUT_DIR))
	$(CC) $(CFLAGS) -o $(OUT_DIR)/Kengine $^

# Rule to generate .o files from .c files
$(OUT_DIR)/%.o: $(SRC_DIR)/%.c
	$(call make-dir, $(dir $@))
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OUT_DIR)

.PHONY: all build clean
