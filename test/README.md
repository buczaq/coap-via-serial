# CoAP via Serial
### Testy jednostkowe
------
### Czym są testy?
Testy sprawdzają poprawność implementacji za pomocą gtesta.

### Jak uruchomić?
Testy budowane są poleceniami
```
cmake .
make
```
wywoływanym z folderu głównego testów. Plik wykonywalny ("run_tests") po zbudowaniu znajduje się w miejscu, w którym wywołano cmake. Sugeruje się stworzenie osobnego katalogu, np. "build", i wołanie stamtąd cmake'a poceleniem
```
cmake ..
```

Testy uruchamiane są za pomocą polecenia
```
./run_tests
```

