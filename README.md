# MySocketServer

## Overview

- This is an object-oriented socket server program. It creates a socket server object that can establish a connection with a socket client. Once connection is established, the client will send XML data on that connection at which point the server will display the XML request, process the request, display the XML response, and then send the XML response back to the client.

## Important Notes

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
<!-- Response (server will not send this back as a single line) -->
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
| Yes | Happy | Valid card number, valid PIN | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
| Yes | Unhappy | Valid card number, invalid PIN | Command returned as GetPlayerInfo, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid PIN) | Command returned as GetPlayerInfo, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid PIN) |
| Yes | Unhappy | Invalid card number | Command returned as GetPlayerInfo, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Card Number) | Command returned as GetPlayerInfo, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Card Number) |
| Yes | Unhappy | Missing Request Tags | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Missing Command Tags | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Missing Data Tags | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Missing Row Tags | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Missing Row Attributes | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Incorrect Row Attributes | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Extra siblings to Request | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Extra siblings to Command | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Extra siblings to Data | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Extra siblings to Row | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Extra children to Request | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Extra children to Command | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Extra children to Data | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| Yes | Unhappy | Extra children to Row | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) |
| No | Unhappy | Extra attributes to Request | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
| No | Unhappy | Extra attributes to Command | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
| No | Unhappy | Extra attributes to Data | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
| No | Unhappy | Extra attributes to Row | Command returned as empty, Status returned as Failed, ErrorMessage returned under Data as Row (Invalid Request Format) | Command returned as GetPlayerInfo, Status returned as Success, demographics returned under Data as Rows (CardNumber, FirstName, LastName, Address, City, State, ZipCode) |
