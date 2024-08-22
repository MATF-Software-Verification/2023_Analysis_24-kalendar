# 2023_Analysis_24-kalendar
obsolete

# 2023_Analysis_19_UNDER_THE_C

Ovaj gitHub repozitorijum je nastao zarad izrade samostalnog praktičnog seminarskog rada za potrebe kursa Verifikacija softvera na master studijama Matematičkog fakulteta u Beogradu, smer Računarstvo i informatika.
Praktični seminarski rad podrazumeva primenu alata za statičku i dinamičku verifikaciju softvera na izabranom studentskom projektu.

**Autor: Jovana Adžić 1020/21**

## Opis projekta za koji je analiza rađena
**UNDER_THE_C** je studentski projekat rađen za potrebe kursa Razvoj softvera u školskoj 2020/2021 godini.
Under the C je 2D igrica koja prati 4 devojke u njihovoj avanturi da pronađu blago skriveno u zamku ispod mora.
Igrica ima ukupno četiri nivoa i u svakom nivou igrač upravlja različitim likom - drugom devojkom. Osim razlike u izgledu likova, njih razlikuju i drugačije logike nivoa kao i sredina izgleda scene. Potrebno je rešiti različite prepreke, tako da sama igra ima privid potpuno međusobno različitih igara.

Na kraju prva tri nivoa zadatak igrača jeste da na različite načine oslobodi sledećeg lika kojim će upravljati u narednom nivou. Prelaskom četvrtog nivoa dolazi se do kraja igrice gde su svi prethodni igrači zajedno kod blaga. Na putu do spašavanja, igrače vrebaju različiti neprijatelji koji im oduzimaju živote i otežavaju put ka cilju.

Projekat je rađen u programskom jeziku **C++** u `QtCreator` radnom okruženju.


## Osnovne informacije o analizi
[GitHub repozitorijum projekta](https://gitlab.com/matf-bg-ac-rs/course-rs/projects-2020-2021/19-under-the-c)
[Commit sha](691f17bbb4ceb32c120bd4e8d40e90d0c141a79d)

Sama analiza je vršena nad glavnom **master** granom.

U analizi su korišćeni sledeći alati:

- Valgrind alati:
    -Memcheck
    -Callgrind
- Alati za statičku analizu:
    - Clang-Tidy
    - Cppcheck
- Perf

Opisi svakog od alata, skripte za pokretanje, sami outputi analize se nalaze u dokumentu [dok](ProjectAnalysisReport.md)

## Uputstvo za reprodukciju analize
Alati su, većinski, pokretani iz terminala Ubuntu operativnog sistema koristeći unapred pripremljene skripte
Pojedini alati su se pokretali i direktno iz `QtCreator` okruženja jer u okviru okruženja postoji podrška za izvršavanje pojedinih alata za analizu.

## Zaključak
