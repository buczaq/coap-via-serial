# CoAP via Serial
### Serwer
------
### Czym jest serwer?
Serwer jest serwerem CoAP, zarządzającym urządzeniami, na których uruchomiony jest moduł klienta.

### Jak uruchomić?
Serwer budowany jest poleceniem
```
make
```
wywoływanym z folderu głównego serwera. Plik wykonywalny ("server") po zbudowaniu znajduje się w podfolderze "build". Może się zdarzyć, że podczas kompilacji folder ten nie zostanie stworzony -- należy wtedy utworzyć go ręcznie.

Moduł uruchamiany jest za pomocą polecenia
```
./build/server port1 port2

port1 - port, na który przychodzą zapytania z serwera HTTP
port2 - port zmapowany pod urządzenie za pomocą ser2net
```
(zakładając, że użytkownik znajduje się w folderze server). Jeśli wartości port1 i port2 nie zostaną podane, program przyjmie domyślnie kolejno 8001 i 9001.

