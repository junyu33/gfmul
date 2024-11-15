# Default architecture and run mode
ARCH ?= x86_64
RUN ?= local
BITS ?= 128

# Header file selection
HEADER ?= GFMUL
HEADER_OPTIONS = WANGXIAO INTEL CLASSIC GFMUL KARATSUBA

# Directory for binaries
BIN_DIR = ./
# filename
BENCH = bench

# Compiler and flags based on the architecture
ifeq ($(ARCH), x86_64)
    CXX = g++
    CXXFLAGS = -march=native
    TARGET = $(BIN_DIR)/$(BENCH)_x86_64
    RUN_CMD = ./$(TARGET)
else ifeq ($(ARCH), arm64)
    CXX = aarch64-linux-gnu-g++
    CXXFLAGS = -march=armv8.1-a+crypto -static
    TARGET = $(BIN_DIR)/$(BENCH)_arm64
    ifeq ($(RUN), qemu)
        RUN_CMD = qemu-aarch64 ./$(TARGET)
    else
        RUN_CMD = ./$(TARGET)
    endif
else ifeq ($(ARCH), riscv64)
    CXX = riscv64-linux-gnu-g++
    CXXFLAGS = -march=rv64gv_zbb_zbc -mabi=lp64d -static
    TARGET = $(BIN_DIR)/$(BENCH)_riscv64
    ifeq ($(RUN), qemu)
        RUN_CMD = qemu-riscv64 ./$(TARGET)
    else
        RUN_CMD = ./$(TARGET)
    endif
else
    $(error Unsupported architecture $(ARCH))
endif

# Ensure valid HEADER value
ifneq (,$(filter $(HEADER),$(HEADER_OPTIONS)))
    CXXFLAGS += -DHEADER_$(HEADER)
else
    $(error Unsupported HEADER $(HEADER). Supported options: $(HEADER_OPTIONS))
endif

# Source files - separate source files and headers
SOURCES = bench.cpp
GEN_SOURCE = gen.cpp

# Build the target executable
all: $(BIN_DIR) $(TARGET)

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

gen: 
	g++ -o $(BIN_DIR)/gen $(GEN_SOURCE)	
	$(BIN_DIR)/gen $(BITS)

# Build the target executable
$(TARGET): $(SOURCES) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -DUSE_$(HEADER) -o $(TARGET) $(SOURCES)

# Run the executable based on RUN parameter
run: $(TARGET)
	$(RUN_CMD)

# Clean up generated files
clean:
	rm -f $(BIN_DIR)/$(BENCH)_x86_64 $(BIN_DIR)/$(BENCH)_arm64 $(BIN_DIR)/$(BENCH)_riscv64
