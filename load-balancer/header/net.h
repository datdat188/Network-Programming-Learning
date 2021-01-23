#include "utils.h"

void make_socket_non_blocking(int socket_fd);
int connect_to_backend(char* backend_host,
                       char* backend_port_str);