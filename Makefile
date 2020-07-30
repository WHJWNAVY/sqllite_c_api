# tool marcros
CC := gcc
LD := gcc
CFLAGS := -Wall
LDFLAGS :=
LIBS := -lsqlite3
DFLAGS := $(CFLAGS) -g

# path marcros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

# compile marcros
TARGET_NAME := sqlc_test
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)

# src files & obj files
SRCS := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJS := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRCS)))))
OBJS_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRCS)))))

# clean files list
DISTCLEAN_LIST := $(OBJS) $(OBJS_DEBUG)
CLEAN_LIST := $(TARGET) $(TARGET_DEBUG) $(DISTCLEAN_LIST)

# default rule
default: all

# non-phony targets
$(TARGET): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $< -o $@ $(DFLAGS)

# phony rules
.PHONY: all
all: $(TARGET)

.PHONY: debug
debug: $(TARGET_DEBUG)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)

lint:
	find ${SRC_PATH} -iname "*.[ch]" | xargs clang-format-6.0 -i