# Dvoprolazni-asembler
## Opis Problema

Zadatak je napraviti dvoprolazni asembler, ulaz je tekstualni fajl, koji se prevodi do linije .end, izlaz asemblera je tekstualni fajl baziran na skolskoj varijanti elf formata sa tabelom simbola, relokacionim tabelama i sadržajem sekcija.

## Opis Resenja

Program prima 3 argumenta preko komadne linije: nastavak -o,naziv izlaznog objektnog fajla(zavrsava se sa .o nastavkom) I naziv asemblerskog fajla(zavrsava se sa .s nastavkom). 
	Sastoji se fajlova: main.cpp (u kome se nalazi main funkcija), i types.h ( pomoćne strukture podataka ulaz u tabeli simbola, ulaz u relokacionoj tabeli,struktura koja opisuje adresiranje operanda i struktura za opis sekcije). 
	U main-u prvo se desava prvi prolaz kojem se šalju referenca na ulazni fajl gde se nalazi asemblerski kod. Čitanjem red po red i tokenizaciom se indentifikuju labele, popunjava se tabela simbola, određuju se početne adrese i veličine korišćenih sekcija. 
	U main-u se zatim desava drugi prolaz. Čitanjem red po red(istog onog fajla kao iz prvog prolaza) i tokenizacijom se vrši definisanje globalnih simbola kao takvih, popunjavanje sadržaja sekcija heksadecimalnim ciframa u little-endian formatu, kao i indentifikovanje nepoznatih simbola i pravljenje relokacionih zapisa gde god je korišćeno pc relativno adresiranje sa nerazrešenim simbolom ili apsolutna adresa.
	Posle drugog prolaza pravi se predmetni fajl u modifikovanom elf formatu i upisuju se redom podaci dobijeni asembliranjem: tabela simbola ( naziv, izvorna sekcija, pomeraj od početka sekcije, lokalitet i indentifikacioni broj),  relokacione tabele simbola za svaku korišćenu sekciju (pomeraj od početka sekcije, vrsta relokacije, indentifikacioni broj iz tabele simbola) i sadržaji sekcija u heksadecimalnom zapisu.
	Kada se program uspesno izvrsi asembliranje na konzoli se ispisuje program zavrsen bez gresaka.

