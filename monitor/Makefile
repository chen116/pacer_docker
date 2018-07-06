all: clean
	-git pull
	gcc monitor.cpp -o mcpp -lhb-shared -lhrm-shared -lrt -lboost_thread
	gcc monitor.c -o m -lhb-shared -lhrm-shared -lrt -lboost_thread


clean:
	-rm -f m
