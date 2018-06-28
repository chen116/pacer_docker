all: clean
	-git pull
	gcc consumer.c shared.c -o c  
	gcc producer.c shared.c -o p  -lpthread

clean:
	-rm -f c p


