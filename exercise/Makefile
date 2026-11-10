CFLAGS = -Wall -Wextra 

all: main

run: main
	@./main

main: rng.c

watch:
	@ls *.c | entr -c make run

clean:
	@rm -f ./main

.PHONY: all run watch clean
