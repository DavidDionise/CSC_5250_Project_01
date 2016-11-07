#include "util.h"

char* getIP(char* domain_name) {
	struct addrinfo res;
	struct addrinfo *res_ptr = &res;
	int error_val;

	memset(&res, 0, sizeof(res));
	
	if((error_val = getaddrinfo(domain_name, 0, 0, &res_ptr)) == 0) {
		struct sockaddr_in *sa = (struct sockaddr_in *) res_ptr->ai_addr;
		socklen_t sa_len = (socklen_t) sizeof(*sa);

		return inet_ntoa(sa->sin_addr);
	}
	else {
		printf("%s", gai_strerror(error_val));
		exit(1);
	}
}

