# CoAP via Serial
### Unit Tests
------
### what is test?
These are UTs that checks the correct behaviour of functions and modules.

### How to run it?
Test are built with
```
cmake .
make
```
called from main test directory. The executable ("run_tests") would be placed in a directory where make has been called. Is is recommended to create a separate directory (eg. build) and store compilation result there by calling
```
cmake ..
```
from it.

Tests are ran with command
```
./run_tests
```

Please note that for now Unit Tests does not cover some crucial cases and should be improved.
