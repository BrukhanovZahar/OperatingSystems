CC = gcc
CFLAGS = -c -Wall -Werror -Wextra

SRC1 = src/main.c
SRC2 = src/child.c

OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)

BUILD = build

TASK01 = $(BUILD)/task01
TASK02 = $(BUILD)/child

.PHONY: all clean rebuild task child

all: task child

task: clean $(TASK01)
$(TASK01): $(OBJ1)
	$(CC) $^ -o $@

child: clean $(TASK02)
$(TASK02): $(OBJ2)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean_all:
	rm -rf *.o **/*.o
	find $(BUILD) -type f ! -name "*.txt" -exec rm {} \;

clean:
	rm -rf *.o **/*.o

rebuild: clean all