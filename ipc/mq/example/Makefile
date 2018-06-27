all: clean
	-git pull
	gcc srv.c -o s  -lrt 
	gcc cli.c -o c  -lrt 

clean:
	-rm -f c s
