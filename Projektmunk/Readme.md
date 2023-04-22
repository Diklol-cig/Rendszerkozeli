A program két üzemmódban működik:

    Az adatok előállítása és továbbítása a másik folyamatnak: küldő mód.
    Az adatok fogadása és megjelenítése a fogadó folyamatban: fogadó mód.

Az adatok átvitele fájl vagy socket segítségével történik.
Használati utasítás

    A program futtatása előtt a következő függőségek szükségesek:
        gcc (C fordító)
        make (build eszköz)

A program forráskódját letölthetjük a Github tárolóból, majd a forráskód könyvtárában 
a következő parancsokkal fordíthatjuk és futtathatjuk a programot:

    make
    ./chart [-send|-receive] [-file|-socket]

Opcionális kapcsolók:

    --help
    --version

Program használata:

    2 terminál megnyitása
    A, Terminál
    ./chart -send [-file|-socket]
    B, Terminál
    ./chart -receive [-file|-socket] //Az A, Terminálban kiválasztott mód
    
Ezek után a számítások el lesznek végezve, a fájlok továbbítv lesznek. Ezek után a program 0-ás hibakóddal leáll
