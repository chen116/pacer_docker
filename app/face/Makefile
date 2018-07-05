


app: clean
	-git pull
	g++ face.cpp -o app `pkg-config --cflags --libs opencv` -std=c++11 -lhb-shared -lhrm-shared
	g++ fd.cpp -o fd `pkg-config --cflags --libs opencv` -std=c++11  -lhb-shared -lhrm-shared
	g++ ufd.cpp -o ufd `pkg-config --cflags --libs opencv` -std=c++11 -lhb-shared -lhrm-shared


clean:
	-rm -f app fd ufd


#./app /path/to/video.avi