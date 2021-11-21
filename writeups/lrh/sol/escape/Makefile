.PHONY: all
all: exploit

const.h: secret genoff.sh
	./genoff.sh ./secret > const.h

exploit: exploit.c const.h
	gcc -Wall -O3 -lpthread -o exploit exploit.c

# secret: secret.c
# 	gcc -Wall -O0 -o secret secret.c

.PHONY: localtest
localtest: exploit secret
	./exploit

.PHONY: remotetest
remotetest: exploit secret
	./exploit -s

.PHONY: clean
clean:
	rm -f ./exploit
