# CoAP via Serial
### Server
------
### What is a server?
Server is a CoAP server that manages devices with client module running on them.

### How to run it?
Server is built with command
```
make
```
called from main folder of server. The executable file ("server") is placed in folder "build" after compilation. There is a possibility that this folder does not exist and the compliation would fail - in that case it is neccessary to make this folder manually.

The compiled module (the executable file) is ran with command
```
./build/server mode port1 port2 host-address

mode - raw or ser2net (ONLY RAW MODE IS FULLY SUPPORTED AT THIS MOMENT!)
port1 - a port which HTTP requests should be read from
port2 - a mapped ser2net port (CURRENTLY NOT SUPPORTED, SHOULD BE OMMITED WHEN RAW MODE IS SELECTED)
host-address - typically 0.0.0.0, it's just an information that will be filled in created CoAP message, with no impact on the flow
```
(assuming that user is under "server" directory).

