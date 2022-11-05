#ifndef _MYSOCKETCLIENT_H_
#define _MYSOCKETCLIENT_H_

class MySocketClient {
public:
	MySocketClient();
	int get_file_descriptor();
	socklen_t get_socket_address_length();
	char* get_host_name();
	char* get_service();
	unsigned short get_sin_port();
	int set_name_info();
	int set_ipv4_info();
	int close_file_descriptor();

private:
	friend class MySocketServer;
	socklen_t socket_address_length;
	sockaddr_in socket_address;
	int file_descriptor;
	char host_name[NI_MAXHOST];
	char service[NI_MAXSERV];
};

#endif