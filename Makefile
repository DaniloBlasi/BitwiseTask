PROJ_DIR = .
OBJ_DIR = $(PROJ_DIR)/obj
OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(patsubst %.c,%.o,$(wildcard $(PROJ_DIR)/*.c))))
EXE = $(PROJ_DIR)/bwtask

CC = gcc
CFLAGS = -Wall -Werror

.PHONY : all release debug clean

all : release

release : $(EXE)

$(EXE): $(OBJS)
	@$(CC) $(CFLAGS) $(EXT_FLAGS) -o $@ $^

$(OBJ_DIR)/%.o : %.c
	@$(CC) -c $(CFLAGS) $(EXT_FLAGS) -o $@ $<

debug :
	@$(MAKE) all EXT_FLAGS=-DDEBUG

clean :
	@rm -f $(EXE) $(OBJS)
