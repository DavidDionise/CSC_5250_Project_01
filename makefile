ftp_client:
	gcc -w ./global_utils/util.h ./global_utils/util.c ./client/client_util/util.h ./client/client_util/util.c ./client/TCP_client.c -o ./client/ftp_client -pthread
ftp_server:
	gcc -w ./global_utils/util.h ./global_utils/util.c ./server/server_util/util.h ./server/server_util/util.c ./server/TCP_server.c -o ./server/ftp_server -pthread

