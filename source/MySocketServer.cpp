#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/pugiconfig.hpp"
#include "../include/pugixml.hpp"
#include "../include/MySocketClient.h"
#include "../include/MySocketServer.h"

/**
 * \def		TCP_PROTOCOL
 * \brief	The value to send to socket for TCP protocol
 */
#define TCP_PROTOCOL			(0)

/**
 * \brief	Test player attributes to verify against (due to lack
 *			of database connection). Make sure your XML requests
 *			for testing use this info to validate against
 */
#define TEST_CARD_NUMBER		("123456789")
#define TEST_CARD_PIN			("1234")
#define TEST_CARD_FIRST_NAME	("Dayton")
#define TEST_CARD_LAST_NAME		("Flores")
#define TEST_CARD_ADDRESS		("123 Las Vegas Blvd")
#define TEST_CARD_CITY			("Las Vegas")
#define TEST_CARD_STATE			("NV")
#define TEST_CARD_ZIP_CODE		("55555")

/**
 * \struct	xml_string_writer
 * \brief	Used for printing XML trees. Referenced from
 *			pugixml's manual
 */
struct xml_string_writer : pugi::xml_writer
{
	std::string result;

	virtual void write(const void* data, size_t size)
	{
		result.append(static_cast<const char*>(data), size);
	}
};

std::string MySocketServer::get_address() {
	return address;
}

int MySocketServer::get_port() {
	return port;
}

int MySocketServer::set_address(std::string _address) {
	address = _address;

	return EXIT_SUCCESS;
}

int MySocketServer::set_port(int _port) {
	port = _port;

	return EXIT_SUCCESS;
}

int MySocketServer::get_bytes_received() {
	return bytes_received;
}

int MySocketServer::get_bytes_sent() {
	return bytes_sent;
}

int MySocketServer::create_tcp_ipv4() {
	int return_value;

	/**
	 *	- Domain of AF_INET for IPv4 Internet protocols
	 *	- Type of SOCK_STREAM for TCP (sequenced, reliable, two-way
	 *	  connection-based byte streams)
	 *	- If socket is successful, socket file descriptor will be returned
	 */
	return_value = socket(AF_INET, SOCK_STREAM, TCP_PROTOCOL);

	if (return_value < EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		file_descriptor = return_value;
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

int MySocketServer::bind_tcp_ipv4() {
	int return_value;

	/**
	 *	- Family of AF_INET for IPv4 Internet protocols
	 *	- htons used for endian conversion
	 *	- inet_pton used to load string IPv4 address to unsigned long
	 */
	socket_address.sin_family = AF_INET;
	socket_address.sin_port = htons(port);
	inet_pton(AF_INET, address.c_str(), &socket_address.sin_addr);
	return_value = bind(file_descriptor, (sockaddr*)(&socket_address), sizeof(socket_address));

	if (return_value < EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

/**
 * Set the socket to listen
 *		- SOMAXCONN defines the maximum length to which the queue of pending
 *		  connections for sockfd may grow. It can be viewed on Linux machines with:
 *			cat /proc/sys/net/core/somaxconn
 */
int MySocketServer::mark_passive() {
	int return_value;

	/**
	 *	- SOMAXCONN defines the maximum length to which the queue of pending
	 *	  connections for sockfd may grow. It can be viewed on Debian machines with:
	 * 
	 *		cat /proc/sys/net/core/somaxconn
	 */
	return_value = listen(file_descriptor, SOMAXCONN);

	if (return_value < EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

int MySocketServer::accept_client(MySocketClient* source) {
	int return_value;

	return_value = accept(file_descriptor, (sockaddr*)(&source->socket_address), &source->socket_address_length);

	if (return_value < EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		source->file_descriptor = return_value;
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

int MySocketServer::close_file_descriptor() {
	int return_value;

	return_value = close(file_descriptor);

	if (return_value != EXIT_SUCCESS) {
		return_value = EXIT_FAILURE;
	}
	else {
		return_value = EXIT_SUCCESS;
	}

	return return_value;
}

void MySocketServer::receive_request_from_client(MySocketClient* source) {
	/**
	 *	- Clear the buffer that holds the request 
	 *	- Waits in this function until data has been received from client
	 *	  via socket connection
	 *	- If valid number of bytes have been received, store and print the request
	 */
	memset(buf, 0, BUF_SIZE);
	bytes_received = recv(source->file_descriptor, buf, BUF_SIZE, 0);

	if (bytes_received > 0) {
		request.load_buffer(buf, bytes_received);
		std::cout << "Received XML Request: " << std::endl;
		std::cout << std::endl << get_printable_xml(&request) << std::endl << std::endl;
	}
}

void MySocketServer::validate_request() {
	int children;
	request_validated = true;

	/**
	 *	Validate that Request, Command, and Date nodes exist
	 */
	if (request.child("Request") == NULL
		|| request.child("Request").child("Command") == NULL
		|| request.child("Request").child("Data") == NULL) {
		request_validated = false;
		return;
	}

	/**
	 *	Validate that a Row node exists with attribute Type, value CardNumber
	 *	Validate that a Row node exists with attribute Type, value PIN
	 */
	if (request.child("Request").child("Data").find_child_by_attribute("Row", "Type", "CardNumber") == NULL
		|| request.child("Request").child("Data").find_child_by_attribute("Row", "Type", "PIN") == NULL) {
		request_validated = false;
		return;
	}
	
	/**
	 *	Validate Request is the only node at its level
	 */
	for (pugi::xml_node node = request.first_child(); node; node = node.next_sibling()) {
		if ((std::string)node.name() != "Request") {
			request_validated = false;
			return;
		}
	}
	
	/**
	 *	Validate Command + Data are the only nodes at their level
	 */
	for (pugi::xml_node node = request.child("Request").first_child(); node; node = node.next_sibling()) {
		if ((std::string)node.name() != "Command"
			&& (std::string)node.name() != "Data") {
			request_validated = false;
			return;
		}
	}
	
	/**
	 *	Validate Command node has exactly 1 text field and no child nodes
	 */
	children = 0;
	for (pugi::xml_node node = request.child("Request").child("Command").first_child(); node; node = node.next_sibling(), children++) {
		if (children > 0 || node.type() != pugi::node_pcdata) {
			request_validated = false;
			return;
		}
	}

	/**
	 *	Validate Data node has exactly 0 text fields and 2 child nodes
	 */
	children = 0;
	for (pugi::xml_node node = request.child("Request").child("Data").first_child(); node; node = node.next_sibling(), children++) {
		if (children > 1 || node.type() == pugi::node_pcdata) {
			request_validated = false;
			return;
		}
	}

	/**
	 *	Validate Row node with Type=CardNumber attribute has exactly 1 text field and no child nodes
	 */
	children = 0;
	for (pugi::xml_node node = request.child("Request").child("Data").find_child_by_attribute("Row", "Type", "CardNumber").first_child(); node; node = node.next_sibling(), children++) {
		if (children > 0 || node.type() != pugi::node_pcdata) {
			request_validated = false;
			return;
		}
	}

	/**
	 *	Validate Row node with Type=PIN attribute has exactly 1 text field and no child nodes
	 */
	children = 0;
	for (pugi::xml_node node = request.child("Request").child("Data").find_child_by_attribute("Row", "Type", "PIN").first_child(); node; node = node.next_sibling(), children++) {
		if (children > 0 || node.type() != pugi::node_pcdata) {
			request_validated = false;
			return;
		}
	}

}

void MySocketServer::process_request() {
	/**
	 *	- Clear the response XML tree
	 *	- If request is not validated then construct the response for bad XML format
	 *	- Otherwise, parse the command from the request and route to the
	 *	  respective method to construct the correct response
	 */
	response.reset();
	if (request_validated) {
		if ((std::string)request.child("Request").child("Command").child_value() == "GetPlayerInfo") {
			command_getplayerinfo();
		}
		else {
			command_unknown();
		}
	}
	else {
		request_not_valid();
	}
}

void MySocketServer::send_response_to_client(MySocketClient* source) {
	/**
	 *	- Send the response to the client and if successful, have the server
	 *	  print the response
	 */
	bytes_sent = send(source->file_descriptor, get_printable_xml(&response).c_str(), get_printable_xml(&response).length() + 1, 0);

	if (bytes_sent > 0) {
		std::cout << std::endl << "Sent XML Response:" << std::endl;
		std::cout << std::endl << get_printable_xml(&response) << std::endl << std::endl;
	}
}

std::string MySocketServer::get_printable_xml(pugi::xml_document* xml) {
	/**
	 *	- Use xml_string_writer struct for generating the XML tree
	 *	  as a string. Referenced from pugixml's manual
	 */
	xml_string_writer writer;

	for (pugi::xml_node child = xml->first_child(); child; child = child.next_sibling()) {
		child.print(writer, "");
	}
	
	return writer.result;
}

void MySocketServer::command_getplayerinfo() {
	/**
	 *	- Used to hold the Row node that was just appended to the response
	 */
	pugi::xml_node row;

	/**
	 *	- Clear the response XML tree
	 *	- Create root node as Response
	 *	- Build out Command node and set the text field to the method name
	 *	- Build out Status node but don't set the text field until data is verified
	 *	  against test player
	 */
	response.reset();
	response.append_child("Response");
	response.child("Response").append_child("Command");
	response.child("Response").child("Command").append_child(pugi::node_pcdata).set_value("GetPlayerInfo");
	response.child("Response").append_child("Status");

	/**
	 *	- Used to hold the Row node that was just appended to the response
	 */
	std::string card_number = request.child("Request").child("Data").find_child_by_attribute("Row", "Type", "CardNumber").child_value();
	std::string pin = request.child("Request").child("Data").find_child_by_attribute("Row", "Type", "PIN").child_value();

	/**
	 *	- Verify valid card number + valid PIN
	 *	- If valid, construct the response based on test player
	 */
	if (card_number == TEST_CARD_NUMBER) {
		if (pin == TEST_CARD_PIN) {
			response.child("Response").child("Status").append_child(pugi::node_pcdata).set_value("Success");
			response.child("Response").append_child("Data");
			row = response.child("Response").child("Data").append_child("Row");
			row.append_attribute("Type") = "CardNumber";
			row = response.child("Response").child("Data").append_child("Row");
			row.append_attribute("Type") = "FirstName";
			row = response.child("Response").child("Data").append_child("Row");
			row.append_attribute("Type") = "LastName";
			row = response.child("Response").child("Data").append_child("Row");
			row.append_attribute("Type") = "Address";
			row = response.child("Response").child("Data").append_child("Row");
			row.append_attribute("Type") = "City";
			row = response.child("Response").child("Data").append_child("Row");
			row.append_attribute("Type") = "State";
			row = response.child("Response").child("Data").append_child("Row");
			row.append_attribute("Type") = "ZipCode";
			response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "CardNumber").append_child(pugi::node_pcdata).set_value(TEST_CARD_NUMBER);
			response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "FirstName").append_child(pugi::node_pcdata).set_value(TEST_CARD_FIRST_NAME);
			response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "LastName").append_child(pugi::node_pcdata).set_value(TEST_CARD_LAST_NAME);
			response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "Address").append_child(pugi::node_pcdata).set_value(TEST_CARD_ADDRESS);
			response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "City").append_child(pugi::node_pcdata).set_value(TEST_CARD_CITY);
			response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "State").append_child(pugi::node_pcdata).set_value(TEST_CARD_STATE);
			response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "ZipCode").append_child(pugi::node_pcdata).set_value(TEST_CARD_ZIP_CODE);
		}

		/**
		 *	- Log error message if card number checks out but PIN is invalid
		 */
		else {
			response.child("Response").child("Status").append_child(pugi::node_pcdata).set_value("Fail");
			response.child("Response").append_child("Data");
			row = response.child("Response").child("Data").append_child("Row");
			row.append_attribute("Type") = "ErrorMessage";
			response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "ErrorMessage").append_child(pugi::node_pcdata).set_value("Invalid PIN");
		}
	}

	/**
	 *	- Log error message if card number does not check out
	 */
	else {
		response.child("Response").child("Status").append_child(pugi::node_pcdata).set_value("Fail");
		response.child("Response").append_child("Data");
		row = response.child("Response").child("Data").append_child("Row");
		row.append_attribute("Type") = "ErrorMessage";
		response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "ErrorMessage").append_child(pugi::node_pcdata).set_value("Invalid Card Number");

	}
}

void MySocketServer::command_unknown() {
	/**
	 *	- Used to hold the Row node that was just appended to the response
	 */
	pugi::xml_node row;

	/**
	 *	- Clear the response XML tree
	 *	- Create root node as Response
	 *	- Build out Command node and set the text field to Unknown
	 *	- Build out Status node and set the text field to Fail
	 *	- Log error message for Invalid Command
	 */
	response.reset();
	response.append_child("Response");
	response.child("Response").append_child("Command");
	response.child("Response").child("Response").append_child(pugi::node_pcdata).set_value("Unknown");
	response.child("Response").append_child("Status");
	response.child("Response").child("Status").append_child(pugi::node_pcdata).set_value("Fail");
	response.child("Response").append_child("Data");
	row = response.child("Response").child("Data").append_child("Row");
	row.append_attribute("Type") = "ErrorMessage";
	response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "ErrorMessage").append_child(pugi::node_pcdata).set_value("Invalid Command");

}

void MySocketServer::request_not_valid() {
	/**
	 *	- Used to hold the Row node that was just appended to the response
	 */
	pugi::xml_node row;

	/**
	 *	- Clear the response XML tree
	 *	- Create root node as Response
	 *	- Build out Command node and set the text field to Unknown
	 *	- Build out Status node and set the text field to Fail
	 *	- Log error message for Invalid Request Format
	 */
	response.reset();
	response.append_child("Response");
	response.child("Response").append_child("Command");
	response.child("Response").child("Response").append_child(pugi::node_pcdata).set_value("Unknown");
	response.child("Response").append_child("Status");
	response.child("Response").child("Status").append_child(pugi::node_pcdata).set_value("Fail");
	response.child("Response").append_child("Data");
	row = response.child("Response").child("Data").append_child("Row");
	row.append_attribute("Type") = "ErrorMessage";
	response.child("Response").child("Data").find_child_by_attribute("Row", "Type", "ErrorMessage").append_child(pugi::node_pcdata).set_value("Invalid Request Format");

}