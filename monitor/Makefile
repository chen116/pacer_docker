all: clean
	-git pull
	gcc monitor.cpp -o mcpp -lhb-shared -lhrm-shared -lrt
	gcc monitor.c -o m -lhb-shared -lhrm-shared -lrt


clean:
	-rm -f m
