
-- =========================================
-- Videobutiksdatabas
-- Skapad av: Peter Emad Younan Samy
-- =========================================
CREATE DATABASE videobutik;
USE videobutik;


CREATE TABLE Film (
    FilmID INT PRIMARY KEY,
    Titel VARCHAR(100),
    Filmlängd INT,
    Kategori VARCHAR(50)
);


CREATE TABLE Butik (
    ButiksID INT PRIMARY KEY,
    Butiksnamn VARCHAR(100) NOT NULL,
    Adress VARCHAR(150),
    Telefonnummer VARCHAR(20),
    Postnummer VARCHAR(10),
    Husnummer VARCHAR(10),
    Gata VARCHAR(100),
    Stad VARCHAR(50)
);


CREATE TABLE Anstalld (
    Anstallningsnr INT PRIMARY KEY,
    Lon DECIMAL(10,2),
    Roll VARCHAR(50),
    Utbildning VARCHAR(100),
    ButiksID INT,
    FOREIGN KEY (ButiksID) REFERENCES Butik(ButiksID)
);

CREATE TABLE film_personal (
    personal_ID INT PRIMARY KEY,
    personal_namn VARCHAR(100) NOT NULL
    
);



CREATE TABLE Jobbar (
	ID INT AUTO_INCREMENT PRIMARY KEY,
    FilmID INT,
    personal_ID INT,
    Roll VARCHAR(50),
    FOREIGN KEY (FilmID) REFERENCES Film(FilmID),
    FOREIGN KEY (personal_ID) REFERENCES film_personal(personal_ID)
);

CREATE TABLE Medlem (
    MedlemsID INT PRIMARY KEY,
    Medlemsnamn VARCHAR(100) NOT NULL,
    Start_datum_for_medlemskapet DATE,
    Telefonnummer VARCHAR(20)
);


CREATE TABLE Reservering (
    ReserveringsID INT PRIMARY KEY,
    Reserveringsdatum DATE,
    Reserveringsperiod INT,
    Onskade_datum DATE,
    MedlemsID INT,
    FilmID INT,
    FOREIGN KEY (MedlemsID) REFERENCES Medlem(MedlemsID),
    FOREIGN KEY (FilmID) REFERENCES Film(FilmID)
);

CREATE TABLE Finns (
    FilmID INT,
    ButiksID INT,
    PRIMARY KEY (FilmID, ButiksID),
    FOREIGN KEY (FilmID) REFERENCES Film(FilmID),
    FOREIGN KEY (ButiksID) REFERENCES Butik(ButiksID)
);
