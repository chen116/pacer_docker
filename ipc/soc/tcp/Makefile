app: gitpull clean
	gcc srv.c -o srv 
	gcc cli.c -o cli 
gitpull:
	-git pull

clean:
	-rm -f srv cli


#./app --model=bvlc_googlenet.caffemodel --config=bvlc_googlenet.prototxt --width=224 --height=224 --classes=classification_classes_ILSVRC2012.txt --input=space_shuttle.jpg --mean="104 117 123"
