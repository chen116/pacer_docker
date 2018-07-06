all: clean
	-git pull
	g++ monitor.cpp -o mcpp -lhb-shared -lhrm-shared -lrt -lboost_thread -std=c++11
	gcc monitor.c -o m -lhb-shared -lhrm-shared -lrt 


clean:
	-rm -f m mcpp
