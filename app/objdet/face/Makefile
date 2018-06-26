


app: clean
	g++ face.cpp -o app `pkg-config --cflags --libs opencv` -std=c++11 
	g++ fd.cpp -o fd `pkg-config --cflags --libs opencv` -std=c++11 
	g++ ufd.cpp -o ufd `pkg-config --cflags --libs opencv` -std=c++11 


clean:
	-rm -f app fd ufd


#./app --model=bvlc_googlenet.caffemodel --config=bvlc_googlenet.prototxt --width=224 --height=224 --classes=classification_classes_ILSVRC2012.txt --input=space_shuttle.jpg --mean="104 117 123"
