all: clean
	-git pull
	gcc consumer.c shared.c -o c  -lpthread
	gcc producer.c shared.c -o p  -lpthread

clean:
	-rm -f c p


