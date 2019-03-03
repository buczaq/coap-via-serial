# CoAP via Serial
### Client
------
### What is a client?
Client is a CoAP client that is supposed to be run od devices which are under CoAP server control.

### How to run it?
Client is built with command
```
make
```
called from main folder of the client. The executable file ("client") would be placed in "build" directory. There is a possibility that this folder does not exist and the compliation would fail - in that case it is neccessary to make this folder manually.

The compiled module (the executable file) is ran with command
```
./build/client device-path
```
(assuming that user is under "client" directory).

in order to enable debugging information, it is necessary to add
```
debug
```
option at the end of running sequence.
