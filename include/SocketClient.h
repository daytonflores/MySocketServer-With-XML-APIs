#ifndef _SOCKETCLIENT_H_
#define _SOCKETCLIENT_H_

/**
 * \class	SocketClient
 * \brief	Used to abstract data about the client
 */
class SocketClient {



public:

	/**
	 * \fn		Constructor
	 * \param	N/A
	 * \return	N/A
	 * \brief	No parameters passed
	 */
	SocketClient();

	/**
	 * \fn		char* get_host_name
	 * \param	N/A
	 * \return	Returns the client host name
	 * \brief	Getter for client host name
	 */
	char* get_host_name();

	/**
	 * \fn		char* get_service
	 * \param	N/A
	 * \return	Returns the client port
	 * \brief	Getter for client port
	 */
	char* get_service();

	/**
	 * \fn		unsigned short get_sin_port
	 * \param	N/A
	 * \return	Returns the client socket port
	 * \brief	Getter for client socket port
	 */
	unsigned short get_sin_port();

	/**
	 * \fn		int set_name_info
	 * \param	N/A
	 * \return	Returns EXIT_FAILURE upon any failures encountered,
	 *		and EXIT_SUCCESS otherwise
	 * \brief	Setter for client host name
	 */
	int set_name_info();

	/**
	 * \fn		int set_ipv4_info
	 * \param	N/A
	 * \return	Returns EXIT_FAILURE upon any failures encountered,
	 *		and EXIT_SUCCESS otherwise
	 * \brief	Setter for client IP address
	 */
	int set_ipv4_info();

	/**
	 * \fn		int close_file_descriptor
	 * \param	N/A
	 * \return	Returns EXIT_FAILURE upon any failures encountered,
	 *		and EXIT_SUCCESS otherwise
	 * \brief	Closes the client file descriptor when socket connection
	 *		to client has been lost
	 */
	int close_file_descriptor();



private:

	/**
	 * \brief	SocketServer needs to access some private members of SocketClient
	 */
	friend class SocketServer;

	/**
	 * \var		socklen_t socket_address_length
	 * \brief	Size of the client's socket address
	 */
	socklen_t socket_address_length;

	/**
	 * \var		sockaddr_in socket_address
	 * \brief	Socket address of the client
	 */
	sockaddr_in socket_address;

	/**
	 * \var		int file_descriptor
	 * \brief	Identifier for the client
	 */
	int file_descriptor;

	/**
	 * \var		char host_name[NI_MAXHOST]
	 * \brief	Host name of the client (if host name cannot be resolved
	 *		then this will hold the IP address)
	 */
	char host_name[NI_MAXHOST];

	/**
	 * \var		char service[NI_MAXSERV]
	 * \brief	Port of the client's socket connection
	 */
	char service[NI_MAXSERV];
};

#endif
