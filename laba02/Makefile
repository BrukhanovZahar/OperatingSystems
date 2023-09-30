CC = gcc
CFLAGS = -c -Wall -Werror -Wextra -pthread

SRC1 = src/main.c

OBJ1 = $(SRC1:.c=.o)

BUILD = build

TASK = $(BUILD)/task02

.PHONY: all clean rebuild task

all: task

task: clean $(TASK)
$(TASK): $(OBJ1)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean_all:
	rm -rf *.o **/*.o
	find $(BUILD) -type f ! -name "*.txt" ! -name ".gitignore" -exec rm {} \;

clean:
	rm -rf *.o **/*.o

rebuild: clean all
