# Binaries

# Upute korištenja
Model maker je program olakšava kreiranje matematičkih modela, vrši njihovu simulaciju putem NatID framework-a i zatim prikazuje rezultate simulacije.

Trenutno podržano je kreiranje modela tekstualno i grafički.

##Unos jednačina tekstualno (.modl)
Na alatnoj traci programa pritisnuti tipku "Predlošci" da bi otvorili osnovne modele koji se mogu kreirati. Za kompleksnije primjere otvoriti modele iz foldera "Examples".

slika

Unos modela na ovaj način sastoji se u deklarisanju više blokova i unosom njihovog sadržaja. Blok se deklariše na ovaj način:
"ImeBloka [atributi]:"

Na samom početku mora se deklarisati blok "Model:" a zatim se deklarišu blokovi "Vars:" i "Params:" za navođenje varijabli i parametara modela. Jednačine se unose u svojim blokovima u zavisnosti od njihovog tipa (pogledati primjere i predloške).

Moguće je deklarisati blokove "PreProc:" i "PostProc:" koji se izvršavaju, respektivno, prije i poslije glavnog modela. Unutar ovih blokova moguće je definasti podmodele deklarisanjem bloka "Model:" unutar njih.

##Unos modela grafički (.tfstate)
Modeliranje grafički zasniva se na kreiranju više različitih tipova blokova i zatim njihovom međusobnom spajanju. Primjer PID modela (Examples/PID.tfstate) unešen na ovaj nacin prikazan je na sljedećoj slici:

slika

Desnim pritiskom miša na prazan prostor dobija se meni putem kojeg se mogu kreirati svi tipovi blokova. Osnovni blok je blok prijenosne funkcije unutar kojeg se navodi jednačina u laplasovom domenu koristeći specijalnu varijablu "s".

Pritiskom lijeve tipke miša na blok dobijaju se sve njegove postavke. Pomjeranjem miša pomjera se i zabilježeni blok. Oni se spajaju tako što se mišem povuče linija od izlaza jednog bloka do ulaza drugog. Program automatski crta spojeve između blokova.

##Podešavanje postavki simulacije
U donjem dijelu programa moguče je postaviti osnovne postavke simulacije kao što su vrijeme početka i kraja.

slika

U dijelu "Pretprocesorske komande" definiše se koje plotov-e da program generiše nakon simulacije kao i modele koji se trebaju spojiti (import).

Primjer svih konadi mogu se pogledati otvaranjem modela "Examples/sin_gen_withNF.modl".

Dvije osnovne komande su:

-import <relativni put do xml fajla>

Svaki model (grafički i tekstualni) može se izvesti u ".xml" formatu. Pored toga što se modeli u xml formatu mogu ponovo uvesti u program (kreira se teksutalni model), također se mogu navesti i kao "import" za neki model što znači da će se sve jednačine iz zog modela prenijeti u željeni model prije simulacije.

line amplituda [ inverse ] , output vs t
-line <ime varijable ili parametra> [atributi], <ime varijable ili parametra> [atributi] ... versus <ime varijable za X osu>

Ovim definišemo kreiranje novog grafika koji prikazuje sve navedene varijable na Y osi u odnosnu na posljednju zadanu varijablu nakon ključne riječi "versus" ili "wth" (with respect to). Parameri se prikazuju kao vertikalne ili horizontalne linije.

Primjeri:
line amplituda [inverse] , output [pattern = circle], wc_time versus t
line output [width = 0.5], wc [inverse], amplituda wrt w

##Prikaz rezultata
Tabelarne rezultati se mogu vidjeti u "Results" tab-u u donjem dijelu programa.

Generisanje grafova opisano je u prethodnom poglavlju. Bez explicitnog navođenja bilo kojeg grafa generiše se autoamtski graf kada je to moguće. Pritiskom na tipku "F1" unutar grafa prikazuju se sve komande za kontrolu. Primjer upotrebe opisan u klipu ispod


#Kompajliranje
Za Kompajliranje potreban je CMake i NatID framework:
https://github.com/idzafic/natID

Pokrenuti CMakeLists.txt fajl koji se nalazi na ovoj lokaciji:
SymbolicEditor/ModelMaker/modelMaker.cmake
