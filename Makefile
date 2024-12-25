all: treasury-cli

.PHONY: clean
clean:
	rm -f treasury-cli

treasury-cli:
	gcc -static -o treasury-cli main.c -ldotenv-s -ljansson -lwebsockets -lssl -lcrypto -lz
