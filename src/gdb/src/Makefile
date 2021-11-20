all:
	gcc -o hello hello.c	
	echo "1" > hello.debug
	objcopy --add-gnu-debuglink=hello.debug hello
	rm hello.debug