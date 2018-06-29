all: clean
	-git pull
	g++ srv.cpp -o srv -lrt -lpthread
	g++ cli.cpp -o cli -lrt -lpthread
clean:
	-rm -f cli srv


