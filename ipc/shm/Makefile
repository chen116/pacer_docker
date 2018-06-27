all: clean
	-git pull
	gcc writer.c -o w  -lrt -lpthread
	gcc reader.c -o r  -lrt -lpthread

clean:
	-rm -f w r


