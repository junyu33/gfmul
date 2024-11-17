# Architecture and run mode
ARCH ?= x86_64
RUN ?= local

# Directory for binaries
BIN_DIR ?= bin

# Header options
HEADER_OPTIONS = WANGXIAO INTEL CLASSIC GFMUL NTL

# Ensure valid HEADER value
ifneq (,$(filter $(HEADER),$(HEADER_OPTIONS)))
    CXXFLAGS += -DUSE_$(HEADER)
    ifeq ($(HEADER), NTL)
        LDFLAGS += -lntl -lgmp
    endif
else
    $(error Unsupported HEADER $(HEADER). Supported options: $(HEADER_OPTIONS))
endif

# Compiler and flags based on the architecture
ifeq ($(ARCH), x86_64)
    CXX = g++
    CXXFLAGS += -march=native
    TARGET = $(BIN_DIR)/$(BENCH)_x86_64
    RUN_CMD = ./$(TARGET)
else ifeq ($(ARCH), arm64)
    CXX = aarch64-linux-gnu-g++
    CXXFLAGS += -march=armv8.1-a+crypto -static
    TARGET = $(BIN_DIR)/$(BENCH)_arm64
    ifeq ($(RUN), qemu)
        RUN_CMD = qemu-aarch64 ./$(TARGET)
    else
        RUN_CMD = ./$(TARGET)
    endif
else ifeq ($(ARCH), riscv64)
    CXX = riscv64-linux-gnu-g++
    CXXFLAGS += -march=rv64gv_zbb_zbc -mabi=lp64d -static
    TARGET = $(BIN_DIR)/$(BENCH)_riscv64
    ifeq ($(RUN), qemu)
        RUN_CMD = qemu-riscv64 ./$(TARGET)
    else
        RUN_CMD = ./$(TARGET)
    endif
else
    $(error Unsupported architecture $(ARCH))
endif

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

