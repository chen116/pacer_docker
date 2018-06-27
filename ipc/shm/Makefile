all: clean
	-git pull
	gcc srv.c -o s  -lrt -lpthread
	gcc cli.c -o c  -lrt -lpthread

clean:
	-rm -f w r


