# MySocketServer With XML APIs

## Overview

- This is an object-oriented socket server program. It creates a socket server object that can establish a connection with a socket client. Once connection is established, the client will send XML data on that connection at which point the server will display the XML request, process the request, display the XML response, and then send the XML response back to the client.

- Socket server data and methods are abstracted within MySocketServer class

- Socket client data and methods are abstracted within MySocketClient class

- XML parsing/handling was done using the open source C++ XML processing library [pugixml](https://github.com/zeux/pugixml)

## Important Notes

- This is only compatible with Linux systems

- Default IP address and port of the socket server are 127.0.0.1 and 5000, respectively. But these are configurable:
	- To configure IP address alone, send this as a command-line parameter:
	``` bash
	./main 127.0.0.222
	```
	
	- To configure IP address and port both, send these as command-line parameters:
	``` bash
	./main 127.0.0.222 6060
	```
	
	- NOTE: Configuring port alone is NOT supported
	
- Only 1 client can establish a connection with the server at a time

- Lack of database connection limits the "test player" functionality of the program. For testing purposes, there is exactly 1 test player whose attributes are defined as macros at the top of ```../source/MySocketServer.cpp```

- XML requests must be sent to the server as a single line (i.e. no newlines)

## User Guide

1. Download all files and keep relative file structure the same
	- Make sure ```include``` + ```source``` are sibling directories
2. Navigate to ```source``` directory and run ```make```
3. Once executable has been created, run ```./main```
	- You may also configure the IP address alone or the IP address + port with command-line parameters (see Important Notes above)
4. Launch a client and initiate a socket connection to the server
	- For my testing, I use ```netcat``` (installed on my Debian system with ```sudo apt-get install netcat```)
	- Once server is listening, initiate socket connection to the server ```netcat 127.0.0.1 5000```
5. Create a valid XML request (as a single line) using the Samples as reference (see Supported Commands below)
	- Make sure the data being used in the request matches the test player attributes defined as macros at the top of ```../source/MySocketServer.cpp```
6. Observe and validate the XML response (see Test Cases below)
7. To end the program, close the client by pressing ```CTRL``` + ```C``` in ```netcat```
	
## Supported Commands

### GetPlayerInfo

- The client will send the card number and PIN of a player to the server. The program will verify that the player's account exists and that the PIN is correct. Once verified, the server will display demographics about the player and send it back to the client

#### Sample

``` xml
<!-- Incorrect Request Format (only shown here for readability) -->
<?xml version='1.0' encoding='UTF-8'?>
<Request>
	<Command>GetPlayerInfo</Command>
	<Data>
		<Row Type="Card">123456789</Row>
		<Row Type="PIN">1234</Row>
	</Data>
</Request>

<!-- Correct Request Format -->
<?xml version='1.0' encoding='UTF-8'?><Request><Command>GetPlayerInfo</Command><Data><Row Type="CardNumber">123456789</Row><Row Type="PIN">1234</Row></Data></Request>
```

``` xml
<!-- Response (server will :x:t send this back as a single line) -->
<Response>
<Command>GetPlayerInfo</Command>
<Status>Success</Status>
<Data>
<Row Type="CardNumber">123456789</Row>
<Row Type="FirstName">Dayton</Row>
<Row Type="LastName">Flores</Row>
<Row Type="Address">123 Las Vegas Blvd</Row>
<Row Type="City">Las Vegas</Row>
<Row Type="State">NV</Row>
<Row Type="ZipCode">55555</Row>
</Data>
</Response>
```

#### Test Cases

| Passed | Path | Scenario | Expected | Results |
| ------ | ---- | -------- | -------- | ------- |
| :white_check_mark: | Happy | Valid card number, valid PIN | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
| :white_check_mark: | Unhappy | Valid card number, invalid PIN | Command returned as GetPlayerInfo, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid PIN) | Command returned as GetPlayerInfo, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid PIN) |
| :white_check_mark: | Unhappy | Invalid card number | Command returned as GetPlayerInfo, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Card Number) | Command returned as GetPlayerInfo, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Card Number) |
| :white_check_mark: | Unhappy | Missing Request Tags | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Missing Command Tags | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Missing Data Tags | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Missing Row Tags | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Missing Row Attributes | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Incorrect Row Attributes | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Extra siblings to Request | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Extra siblings to Command | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Extra siblings to Data | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Extra siblings to Row | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Extra children to Request | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Extra children to Command | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Extra children to Data | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :white_check_mark: | Unhappy | Extra children to Row | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) |
| :x: | Unhappy | Extra attributes to Request | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
| :x: | Unhappy | Extra attributes to Command | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
| :x: | Unhappy | Extra attributes to Data | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
| :x: | Unhappy | Extra attributes to Row | Command returned as empty, Status returned as Fail, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |

## References

- [Creating a TCP Server in C++ [Linux / Code Blocks]](https://www.youtube.com/watch?v=cNdlrbZSkyQ) by SloanKelly
- [pugixml 1.13 manual](https://pugixml.org/docs/manual.html)
