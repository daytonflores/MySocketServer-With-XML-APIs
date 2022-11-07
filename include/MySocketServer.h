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
	void validate_request();
	void process_request();
	void send_response_to_client(MySocketClient* source);
	std::string get_printable_xml(pugi::xml_document* xml);
	void command_getplayerinfo();
	void command_unknown();
	void request_not_valid();

private:
	friend class MySocketClient;
	friend class pugi::xml_document;

	static const int BUF_SIZE = 1024;

	std::string address;
	int port;
	int file_descriptor;
	sockaddr_in socket_address;
	char buf[BUF_SIZE];
	pugi::xml_parse_result xml;
	pugi::xml_document request;
	pugi::xml_document response;
	bool request_validated;
	int bytes_received;
	int bytes_sent;
};

#endif