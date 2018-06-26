


app: clean
	g++ face.cpp -o app `pkg-config --cflags --libs opencv` -std=c++11 
	g++ fd.cpp -o fd `pkg-config --cflags --libs opencv` -std=c++11 
	g++ ufd.cpp -o ufd `pkg-config --cflags --libs opencv` -std=c++11 


clean:
	-rm -f app fd ufd


#./app /path/to/video.avi