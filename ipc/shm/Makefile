all: clean
	-git pull
	gcc writer.c -o w  -lrt -pthread
	gcc reader.c -o r  -lrt -pthread

clean:
	-rm -f w r


