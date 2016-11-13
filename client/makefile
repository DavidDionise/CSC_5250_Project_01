all:
	gcc -w ../global_utils/util.h ../global_utils/util.c client_util/util.h client_util/util.c TCP_client.c -pthread -lsocket -lnsl -lresolv

run:
	./a.out localhost 3000 3001
