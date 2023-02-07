#ifndef _SOCKETSERVER_H_
#define _SOCKETSERVER_H_

/**
 * \class	SocketServer
 * \brief	Used to abstract data about the server along with XML handling
 */
class SocketServer {



public:

	/**
	 * \fn		int set_address
	 * \param	std::string _address
	 * \return	Returns EXIT_SUCCESS
	 * \brief	Setter for socket server IP address. Will be invoked by main
	 */
	int set_address(std::string _address);

	/**
	 * \fn		int set_port
	 * \param	int _port
	 * \return	Returns EXIT_SUCCESS
	 * \brief	Setter for socket server port. Will be invoked by main
	 */
	int set_port(int _port);

	/**
	 * \fn		std::string get_address
	 * \param	N/A
	 * \return	Returns the socket server IP address
	 * \brief	Getter for socket server IP address
	 */
	std::string get_address();

	/**
	 * \fn		int get_port
	 * \param	N/A
	 * \return	Returns the socket server port
	 * \brief	Getter for socket server port
	 */
	int get_port();

	/**
	 * \fn		int get_bytes_received
	 * \param	N/A
	 * \return	Returns the bytes received from client
	 * \brief	Getter for bytes received from client (for error checking)
	 */
	int get_bytes_received();

	/**
	 * \fn		int get_bytes_sent
	 * \param	N/A
	 * \return	Returns the bytes sent to client
	 * \brief	Getter for bytes sent to client (for error checking)
	 */
	int get_bytes_sent();

	/**
	 * \fn		int create_tcp_ipv4
	 * \param	N/A
	 * \return	Returns EXIT_FAILURE upon any failures encountered,
	 *		and EXIT_SUCCESS otherwise
	 * \brief	Creates a socket object for the server with TCP properties
	 *		in the IPv4 domain
	 */
	int create_tcp_ipv4();

	/**
	 * \fn		int bind_tcp_ipv4
	 * \param	N/A
	 * \return	Returns EXIT_FAILURE upon any failures encountered,
	 *		and EXIT_SUCCESS otherwise
	 * \brief	Binds the newly created socket server object to the server's
	 *		socket IP address + port
	 */
	int bind_tcp_ipv4();

	/**
	 * \fn		int mark_passive
	 * \param	N/A
	 * \return	Returns EXIT_FAILURE upon any failures encountered,
	 *		and EXIT_SUCCESS otherwise
	 * \brief	Marks the socket server object as passive (in order to
	 *		listen to incoming client socket connections)
	 */
	int mark_passive();

	/**
	 * \fn		int accept_client
	 * \param	SocketClient *source
	 * \return	Returns EXIT_FAILURE upon any failures encountered,
	 *		and EXIT_SUCCESS otherwise
	 * \brief	Accepts the socket client connection
	 */
	int accept_client(SocketClient *source);

	/**
	 * \fn		int close_file_descriptor
	 * \param	N/A
	 * \return	Returns EXIT_FAILURE upon any failures encountered,
	 *		and EXIT_SUCCESS otherwise
	 * \brief	Closes the server file descriptor when socket connection
	 *		to client has been established
	 */
	int close_file_descriptor();

	/**
	 * \fn		void receive_request_from_client
	 * \param	SocketClient *source
	 * \return	N/A
	 * \brief	Waits in this function until data has been received from client
	 *		via socket connection
	 */
	void receive_request_from_client(SocketClient *source);

	/**
	 * \fn		void validate_request
	 * \param	N/A
	 * \return	N/A
	 * \brief	Validates the XML format of the request (see README.md for
	 *		details of the expected XML format). If request is not
	 *		valid format, the request_validated flag will be set false
	 */
	void validate_request();

	/**
	 * \fn		void process_request
	 * \param	N/A
	 * \return	N/A
	 * \brief	Invoked directly after validate_request. This method will
	 *		route the program to the correct method based on the
	 *		request_validated flag combined with the command parsed
	 *		from the request
	 */
	void process_request();

	/**
	 * \fn		void send_response_to_client
	 * \param	SocketClient *source
	 * \return	N/A
	 * \brief	Sends the server's response to the client
	 */
	void send_response_to_client(SocketClient *source);

	/**
	 * \fn		std::string get_printable_xml
	 * \param	pugi::xml_document *xml
	 * \return	Returns XML tree as a printable string
	 * \brief	Used to convert XML tree document as a printable
	 *		string to be used in IO stream
	 */
	std::string get_printable_xml(pugi::xml_document *xml);

	/**
	 * \fn		void command_getplayerinfo
	 * \param	N/A
	 * \return	N/A
	 * \brief	Program gets routed here if the XML request is
	 *		validated and the parsed command from the request
	 *		is GetPlayerInfo. Server's response for this method
	 *		is constructed here
	 */
	void command_getplayerinfo();

	/**
	 * \fn		void command_unknown
	 * \param	N/A
	 * \return	N/A
	 * \brief	Program gets routed here if the XML request is
	 *		validated and the parsed command from the request
	 *		is not supported. Server's response for this scenario
	 *		is constructed here
	 */
	void command_unknown();

	/**
	 * \fn		void command_unknown
	 * \param	N/A
	 * \return	N/A
	 * \brief	Program gets routed here if the XML request is
	 *		not validated. Server's response for this scenario
	 *		is constructed here
	 */
	void request_not_valid();



private:

	/**
	 * \brief	SocketClient needs to access some private members of SocketServer
	 */
	friend class SocketClient;

	/**
	 * \brief	xml_document needs to access some private members of SocketServer
	 */
	friend class pugi::xml_document;

	/**
	 * \var		static const int BUF_SIZE
	 * \brief	Size of the buffer used to hold request from client
	 */
	static const int BUF_SIZE = 1024;

	/**
	 * \var		std::string address
	 * \brief	IP address of socket server. This is set by main
	 */
	std::string address;

	/**
	 * \var		std::string address
	 * \brief	IP address of socket server. This is set by main
	 */
	int port;

	/**
	 * \var		int file_descriptor
	 * \brief	Identifier for the server
	 */
	int file_descriptor;

	/**
	 * \var		sockaddr_in socket_address
	 * \brief	Socket address of the client
	 */
	sockaddr_in socket_address;

	/**
	 * \var		char buf[BUF_SIZE]
	 * \brief	The buffer used to hold request from client
	 */
	char buf[BUF_SIZE];

	/**
	 * \var		pugi::xml_document request
	 * \brief	Used to store request received from client
	 *		as XML document (for parsing)
	 */
	pugi::xml_document request;

	/**
	 * \var		pugi::xml_document request
	 * \brief	Used to store response to send to client
	 *		as XML document (for easy building)
	 */
	pugi::xml_document response;

	/**
	 * \var		bool request_validated
	 * \brief	Flag that is set false by validate_request if
	 *		XML request received from client is not
	 *		formatted correctly
	 */
	bool request_validated;

	/**
	 * \var		int bytes_received
	 * \brief	Stores the amount of bytes received from client
	 *		in receive_request_from_client
	 */
	int bytes_received;

	/**
	 * \var		int bytes_sent
	 * \brief	Stores the amount of bytes sent to client
	 *		in send_response_to_client
	 */
	int bytes_sent;
};

#endif
