all: clean
	-git pull
	g++ write_read.c -o srv 
	g++ read_write.c -o cli 
clean:
	-rm -f cli srv

