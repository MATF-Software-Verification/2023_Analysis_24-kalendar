# Izveštaj sprovedene analize

Korišćeni alati:
- [Cppcheck](#cppcheck)
- [Clang-tidy](#clang-tidy)
- [Valgrind alati](#valgrind)
    - [Memcheck](#memcheck)
    - [Callgrind](#callgrind)
- [Perf](#perf)
- [Strace](#strace)

## Cppcheck

**Cppcheck** je alat koji se koristi za statičku analizu koda pisanog u C i C++ rogramskom jeziku. Njegova osnovna svrha je otkrivanje grešaka u kodu kao što su potencijalni problemi sa memorijom (npr. curenje memorije), dereferenciranje null pokazivača, buffer overflow-i, korišćenje neinicijalizovanih varijabli, i druge vrste grešaka koje mogu proći neopaženo tokom kompajliranja ili izvršavanja programa. Alat je za statičku analizu - analizira kod bez njegovog izvršavanja.
Alat se pokreće nad source fajlovima.

Za instalaciju *Cppcheck* alata potrebno je u terminalu pokrenuti sledeću komandu:
```
sudo apt-get install cppcheck
```

Dodatne opcije koje su korišćene prilikom analize koda posmatranog projekta:
- *--inconclusive* : alat prijavljuje i neodređene ili potencijalne greške (greške koje nije mogao da kategorizuje kao greške ili upozorenja i bez ove opcije ih ne bi uključio u izveštaj)
- *--enable=all* : alat uključuje sve dostupne provere (analize) koje može da izvrši
- *--suppress=missingInclude* : alat ignoriše greske koje se dobijaju iz *header*-a (kako bi se izbegao problem sa proveravanjem eksternih biblioteka koje se uključuju u *header* fajlovima)
- *--output-file="cppcheck-output.txt"* : rezultat koji se dobija analizom biće upisan u fajl *cppckeck-output.txt*
- *-I <header_dir>"* : dodaje direktorijum za zaglavlja koja se uključuju prilikom analize

Komanda na kraju izgleda ovako:
```
cppcheck --inconclusive --enable=all --suppress=missingInclude --output-file="cppcheck-output.txt" -I ../19-under-the-c/UNDER_THE_C/Headers/ ../19-under-the-c/UNDER_THE_C/Sources/*
```

Pokreće se pomoću [skripte](cppcheck/cppcheck.sh):
```
./cppcheck.sh
```

Kompletan izveštaj dobijen primenom *cppcheck* alata nalazi se u fajlu [*cppcheck-output.txt*](cppcheck/cppcheck-output.txt).
Ono što je dobro jeste da nije pronađeno mnogo greški u svakom  od 61 fajla koliko je analizirano alatom.
Primera iz dobijenog izveštaja:

* > 19-under-the-c/UNDER_THE_C/Headers/Score.h:9:5: Class 'Score' has a constructor with 1 argument that is not explicit. [noExplicitConstructor]
        Score(int t);

   **Komentar:** Alat upozorava da konstruktor ima samo jedan parametar i nije označen sa `explicit` tako da C++ kompajler može koristiti taj konstruktor za implicitne konverzije. To je problem jer kompajler može automatski pretvoriti tipove podataka bez eksplicitne dozvole.


* > 19-under-the-c/UNDER_THE_C/Sources/Music.cpp:16:9: There is an unknown macro here somewhere. Configuration is required. If foreach is a macro then please configure it.
        foreach(auto music, musics)

   **Komentar:** Alat kaže da nije uspeo da prepozna il pravilno interpretira makro (`foreach`) koji se koristi u kodu

* > 19-under-the-c/UNDER_THE_C/Sources/MenuScene.cpp:17:0: The function 'play' is never used. [unusedFunction]

     **Komentar:** Alat kaže da u kodu postoji deklarisana funkcija koja se nigde ne koristi.

