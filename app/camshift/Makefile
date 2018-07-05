


app: clean
	-git pull
	g++ camshift.cpp -o app `pkg-config --cflags --libs opencv` -std=c++11 -lhb-shared -lhrm-shared


clean:
	-rm -f app


#./app 