all: treasury-cli

.PHONY: clean
clean:
	rm -f treasury-cli

treasury-cli:
	gcc -static -o treasury-cli main.c /usr/local/lib/libdotenv-s.a -ljansson
