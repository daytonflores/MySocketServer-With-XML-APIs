#ifndef _MYSOCKETSERVER_H_
#define _MYSOCKETSERVER_H_

class MySocketServer {

public:
	int set_address(std::string _address);
	int set_port(int _port);
	std::string get_address();
	int get_port();
	int get_file_descriptor();
	char *get_buffer();
	int get_bytes_received();
	int get_bytes_sent();
	int create_tcp_ipv4();
	int bind_tcp_ipv4();
	int mark_passive();
	int accept_client(MySocketClient *source);
	int close_file_descriptor();
	void receive_request_from_client(MySocketClient* source);
	int process_request();
	void send_response_to_client(MySocketClient* source);

private:
	friend class MySocketClient;

	static const int BUF_SIZE = 1024;

	std::string address;
	int port;
	int file_descriptor;
	sockaddr_in socket_address;
	char buf[BUF_SIZE];
	pugi::xml_parse_result xml;
	pugi::xml_document request;
	pugi::xml_document response;
	int bytes_received;
	int bytes_sent;
};

#endif