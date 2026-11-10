BAS-RELATIONER:

Butik:
(ButiksID (PK), Butiksnamn, Stad, Telefonnummer, Gata, Postnummer, Husnummer)
ButiksID – PK

Anstalld:
(Anstallningsnr (PK), ButiksID (FK), Lon, Roll, Utbildning)
Anstallningsnr – PK
ButiksID – FK

Medlem:
(MedlemsID (PK), Medlemsnamn, Telefonnummer, Start_datum_for_medlemskapet)
MedlemsID – PK

Film:
(FilmID (PK), Titel, Filmlängd, Kategori)
FilmID – PK

Film_Personal:
(Personal_ID (PK), Personal_namn)
Personal_ID – PK

Jobbar:
(FilmID (FK, PK), Personal_ID (FK, PK), Roll)
FilmID – FK, PK
Personal_ID – FK, PK

Reservering:
(ReserveringsID (PK), MedlemsID (FK), FilmID (FK), Reserveringsdatum, Reserveringsperiod, Onskade_datum)
ReserveringsID – PK
MedlemsID – FK
FilmID – FK

Finns:
(FilmID (FK, PK), ButiksID (FK, PK))
FilmID – FK, PK
ButiksID – FK, PK