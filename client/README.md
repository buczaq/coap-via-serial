# CoAP via Serial
### Klient
------
### Czym jest klient?
Klient jest klientem CoAP, uruchamianym na urządzeniach, którymi zarządza serwer CoAP.

### Jak uruchomić?
Klient budowany jest poleceniem
```
make
```
wywoływanym z folderu głównego klienta. Plik wykonywalny ("client") po zbudowaniu znajduje się w podfolderze "build". Może się zdarzyć, że podczas kompilacji folder ten nie zostanie stworzony -- należy wtedy utworzyć go ręcznie.

Moduł uruchamiany jest za pomocą polecenia
```
./build/client sciezka-do-urzadzenia
```
(zakładając, że użytkownik znajduje się w folderze client).

