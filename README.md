# battleship1. Functionalitatea programului

In urma pornirii executabilului, se intra in meniu care are 3 optiuni precizate in enunt.

Note: Optiunea Resume functioneaza ca New Game daca este abia pornit programul. Altfel, foloseste ultima configuratie primita.

Din aceasta fereastra si din joc se poate iesi fie apasand tasta 'q' fie cu enter pe optiunea Quit.

Apasand New Game va fii citita RANDOM una dintre configuratiile date de catre jucator. In acelasi timp, va fii creata tot random mapa calculatorului. Pentru inceperea jocului, din motive tehnice, trebuie dublu click pe orice tasta. Mapa calculatorului va fii ascunsa iar cea a jucatorului va avea afisa '~' pentru apa, 'O' pentru barca. Alegerea calculatorului va fii random, cea a jucatorului prin click cu mouse-ul. 'X' marcheaza o casuta nimerita, 'M' una ratata.

Note: Daca apare "Please wait", just wait. 

Pentru a iesi din fereasta de new game sau de resume, apasati de doua ori tasta 'q'.

2. Implementare

Bibliotecile folosite principal au fost ncurses si curses. Pe langa am folosit alte biblioteci ajutatoare. Constantele folosite sunt doar pentru a face codul mai usor de citit. Am implementat 23 functii pe langa main. Pentru crearea fiecarei ferestre de joc am cate o functie. Meniul se intersecteaza cu "beginning_window". 

Functia play creeaza jocul in sine utilizand pc_Plays si player_Plays. pc_Plays creeaza o lovitura noua apeland functia kick care creeaza niste coordonate noi pentru lovitura cu get_random_kick_coordinates si verifica si returneaza daca aceasta este valabila. 

Functia player_Plays creeaza event-urile pentru mouse si returneaza daca lovitura e valabila sau nu. 

Ambele functii modifica hartile.

Revenind la functia play, pana la apasarea caracterului 'q', un counter va tine cont randul cui este. Par pentru player, impar pentru calculator. folosesc un ok pentru a tine cont daca jucatorul a nimerit sau nu. Daca a nimerit, folosesc continue pentru a pastra randul. Daca se ajunge la maximul de 1 de pe tabla, se iese din loop, se verifica cine a castigat si se afiseaza pe ecran punctele si castigatorul. Punctajul este afisat pe tot parcursul jocului si pastrat si in Resume. 

Pentru punctaj am scris 2 functii care numara cati de -1 se afla pe harta (am codat nimerit == -1).

Mapa jucatorului este citita din fisier. Mai multe mape sunt date ca argument in linia de comanda si calculatorul alege random una dintre ele.

Nota: CONFIGURATIA ESTE CODATA CU 0 APA 1 BARCA

Mapa calculatorului este randomizata. Sunt create niste coordonate random pentru inceputul barcii. Se considera random si asezarea (orizonatala sau verticala) Se genereaza n barci de lungime l (n si l date -- nu sunt notatiile din cod). Dupa generare se verifica daca barca este valida in functie de asezare si regula data in cerinta. Daca barca are locatie valida, se adauga pe tabla.

Destroy in advance: Apasand pe tasta d in timpul jocului, se aleg cate 10 pozitii random pe ambele mape.
Nota: For some reason, are un delay la afisarea scorului corect dupa apasarea d.

3. Afisare

Folosesc o singura paleta de culori, "magenta" si cyan. Am redefinit magenta deoarece era prea stridenta culoarea.
