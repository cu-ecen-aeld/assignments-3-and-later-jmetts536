# finder-app MakeFile

# Compiler Path which is overridable from the command line
CROSS_COMPILE ?=

# Tool Paths 
CC := $(CROSS_COMPILE)gcc
LC := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar

# Name of the final binary
TARGET := writer

# Source Files
SRC := writer.c

# Object Files
OBJ := $(patsubst %.c, %.o, $(SRC))

# Build Flags
CFLAGS := -Wall -Og

# Default Build Target
all: $(TARGET)
		
# Link Target
$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile Source Files
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Build Target
clean:
	rm -rf $(TARGET) $(OBJ)

# Phony Targets
.PHONY: all clean
