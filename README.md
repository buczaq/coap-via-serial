# CoAP via Serial
### CoAP protocol implementation for constrained devices
------
### What is CoAP via Serial?
This project is an implementation of Constrained Application Protocol, which allows transporting CoAP packages through UART.

### How to run it?
CoAP server (under "server" directory) should be run as the first module. Other modukes (CoAP client under "client" directory and simple HTTP server from "examples") can be run in any order. Unit tests are placed in "test" directory.

Detailed info about building and running modules are written in corresponding README files for every module.

### What is implemented?
- Full support for single GET and POST commands
- Full support for raw devices (although it is higlhy recommended to adjust implementation details such as serial port parameters for a specific devices - this implementation was tested on virtual socat ports and proper working is guaranteed only in this enviroment!)

### What could be improved?
- Adding support for other messages types
- Adding support for ser2net (partially added, but for now is is not recommended to use it)
