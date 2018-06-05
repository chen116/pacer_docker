
# vic: clean shmlib

# 	gcc -c -o vic.o client.c
# 	gcc -o vic vic.o -Llib -lhb-shared -lhrm-shared -lpthread -lrt -lm

	# gcc -Wall server.c -o vicc

# for running python
# clilib: 
# 	gcc -shared -Wl,-soname,clilib -o clilib.so -fPIC clilib.c
heartbeat_python_lib: 
	gcc -shared -Wl,-soname,heartbeat_python_lib -o heartbeat_python_lib.so -fPIC heartbeat_python_lib.c

clean:
	-rm -f *lib.so