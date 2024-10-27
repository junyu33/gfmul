# Default architecture and run mode
ARCH ?= x86_64
RUN ?= local

# Compiler and flags based on the architecture
ifeq ($(ARCH), x86_64)
    CXX = g++
    CXXFLAGS = -march=native
    TARGET = gfmul_x86_64
    RUN_CMD = ./$(TARGET)
else ifeq ($(ARCH), arm64)
    CXX = aarch64-linux-gnu-g++
    CXXFLAGS = -march=armv8.1-a+crypto -static
    TARGET = gfmul_arm64
    ifeq ($(RUN), qemu)
        RUN_CMD = qemu-aarch64 ./$(TARGET)
    else
        RUN_CMD = ./$(TARGET)
    endif
else ifeq ($(ARCH), riscv64)
    CXX = riscv64-linux-gnu-g++
    CXXFLAGS = -march=rv64gcv_zve64x_zvbc -mabi=lp64d -static
    TARGET = gfmul_riscv64
    ifeq ($(RUN), qemu)
        RUN_CMD = qemu-riscv64 ./$(TARGET)
    else
        RUN_CMD = ./$(TARGET)
    endif
else
    $(error Unsupported architecture $(ARCH))
endif

# Source files
SRC = gfmul.cpp

# Build the target executable
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run the executable based on RUN parameter
run: $(TARGET)
	$(RUN_CMD)

# Clean up generated files
clean:
	rm -f gfmul_x86_64 gfmul_arm64 gfmul_riscv64
