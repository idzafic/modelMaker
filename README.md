# Binaries

https://github.com/idzafic/modelMaker/releases/tag/v1

# Upute korištenja
Model Maker je program koji olakšava kreiranje matematičkih modela, vrši njihovu simulaciju putem NatID frameworka i zatim prikazuje njihove rezultate.

Trenutno je podržano kreiranje modela tekstualno i grafički.

## Unos jednačina tekstualno (.modl)
Na alatnoj traci programa pritisnite tipku "Predlošci" da biste otvorili osnovne modele koji se mogu kreirati. Za kompleksnije primjere otvorite modele iz foldera "Examples".

![image](https://github.com/user-attachments/assets/92369d3f-e77d-45c9-b78e-c78a43afc72b)


Unos modela na ovaj način sastoji se od deklarisanja više blokova i unosa njihovog sadržaja. Blok se deklariše na ovaj način:
"ImeBloka \[atributi\]:"

Na samom početku mora se deklarisati blok "Model:" a zatim se deklarišu blokovi "Vars:" i "Params:" za navođenje varijabli i parametara modela. Jednačine se unose u svojim blokovima u zavisnosti od njihovog tipa (pogledati primjere i predloške).

Moguće je deklarisati blokove "PreProc:" i "PostProc:" koji se izvršavaju, respektivno, prije i poslije glavnog modela. Unutar ovih blokova moguće je definisati podmodele deklarisanjem bloka "Model:" unutar njih.

## Unos modela grafički (.tfstate)
Modeliranje grafički zasniva se na kreiranju više različitih tipova blokova i zatim njihovom međusobnom spajanju. Primjer PID modela (Examples/PID.tfstate) unesen na ovaj način prikazan je na sljedećoj slici:

![image](https://github.com/user-attachments/assets/07b8210d-a0ad-4eeb-a02c-abbaee078c7a)


Desnim pritiskom miša na prazan prostor dobija se meni putem kojeg se mogu kreirati svi tipovi blokova. Osnovni blok je blok prijenosne funkcije unutar kojeg se navodi jednačina u Laplaceovom domenu koristeći specijalnu varijablu "s".

Pritiskom lijeve tipke miša na blok dobijaju se sve njegove postavke. Pomjeranjem miša pomjera se i označeni blok. Oni se spajaju tako što se mišem povuče linija od izlaza jednog bloka do ulaza drugog. Program automatski crta spojeve između blokova.

## Podešavanje postavki simulacije
U donjem dijelu programa moguće je postaviti osnovne postavke simulacije kao što su vrijeme početka i kraja.

![image](https://github.com/user-attachments/assets/18474955-91d7-4709-bc01-58e751cfc806)


U dijelu "Pretprocesorske komande" definiše se koje grafove da program generiše nakon simulacije kao i modele koji se trebaju spojiti (import).

Primjer svih komandi mogu se pogledati otvaranjem modela "Examples/sin_gen_withNF.modl".

Dvije osnovne komande su:

1. import \<relativni put do xml fajla\>

Svaki model (grafički i tekstualni) može se izvesti u ".xml" formatu. Pored toga što se modeli u xml formatu mogu ponovo uvesti u program (kreira se tekstualni model), također se mogu navesti i kao "import" za neki model što znači da će se sve jednačine iz tog modela prenijeti u željeni model prije simulacije.


2. line \<ime varijable ili parametra\> \[atributi\], \<ime varijable ili parametra\> \[atributi\] ... versus \<ime varijable za X osu\>

Ovim definišemo kreiranje novog grafika koji prikazuje sve navedene varijable na Y osi u odnosu na posljednju zadanu varijablu nakon ključne riječi "versus" ili "wth" (with respect to). Parametri se prikazuju kao vertikalne ili horizontalne linije.

Primjeri:
- line amplituda \[inverse\] , output \[pattern = circle\], wc_time versus t
- line output \[width = 0.5\], wc \[inverse\], amplituda wrt w

## Prikaz rezultata
Tabelarni rezultati se mogu vidjeti u "Results" tab-u u donjem dijelu programa.

![image](https://github.com/user-attachments/assets/804437e3-05a1-43a4-9d54-09d2d15960c8)


Generisanje grafova opisano je u prethodnom poglavlju. Bez explicitnog navođenja bilo kojeg grafa generiše se automatski graf kada je to moguće. Pritiskom na tipku "F1" unutar grafa prikazuju se sve komande za kontrolu. Primjer upotrebe opisan u klipu ispod

Iz grafa moguće je podatke spasiti u slikovnom formatu ili u tekstualnom formatu kao niz tačaka u stilu jezika "C".




https://github.com/user-attachments/assets/a20cfbbd-02c6-43be-98ac-b448a0e020d6





# Kompajliranje
Za kompajliranje potreban je CMake i NatID framework:
https://github.com/idzafic/natID

Pokrenuti CMakeLists.txt fajl koji se nalazi na ovoj lokaciji:
SymbolicEditor/ModelMaker/CMakeLists.txt
