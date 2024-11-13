# Default architecture and run mode
ARCH ?= x86_64
RUN ?= local

# Directory for binaries
BIN_DIR = bin

# Compiler and flags based on the architecture
ifeq ($(ARCH), x86_64)
    CXX = g++
    CXXFLAGS = -march=native
    TARGET = $(BIN_DIR)/gfmul_x86_64
    RUN_CMD = ./$(TARGET)
else ifeq ($(ARCH), arm64)
    CXX = aarch64-linux-gnu-g++
    CXXFLAGS = -march=armv8.1-a+crypto -static
    TARGET = $(BIN_DIR)/gfmul_arm64
    ifeq ($(RUN), qemu)
        RUN_CMD = qemu-aarch64 ./$(TARGET)
    else
        RUN_CMD = ./$(TARGET)
    endif
else ifeq ($(ARCH), riscv64)
    CXX = riscv64-linux-gnu-g++
    CXXFLAGS = -march=rv64gv_zbb_zbc -mabi=lp64d -static
    TARGET = $(BIN_DIR)/gfmul_riscv64
    ifeq ($(RUN), qemu)
        RUN_CMD = qemu-riscv64 ./$(TARGET)
    else
        RUN_CMD = ./$(TARGET)
    endif
else
    $(error Unsupported architecture $(ARCH))
endif

# Source files - separate source files and headers
SOURCES = $(wildcard src/*.c src/*.cpp)
HEADERS = $(wildcard src/include/*.h)

# Build the target executable
all: $(BIN_DIR) $(TARGET)

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build the target executable
$(TARGET): $(SOURCES) $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)

# Run the executable based on RUN parameter
run: $(TARGET)
	$(RUN_CMD)

# Clean up generated files
clean:
	rm -f $(BIN_DIR)/gfmul_x86_64 $(BIN_DIR)/gfmul_arm64 $(BIN_DIR)/gfmul_riscv64
