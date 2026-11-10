-- =========================================
-- Videobutiksdatabas
-- Skapad av: Peter Emad Younan Samy
-- =========================================

USE videobutik;


INSERT INTO Film (FilmID, Titel, Filmlängd, Kategori) VALUES
(1, 'The Matrix', 136, 'Sci-Fi'),
(2, 'Inception', 148, 'Action'),
(3, 'Titanic', 195, 'Drama'),
(4, 'The Dark Knight', 152, 'Action'),
(5, 'Interstellar', 169, 'Sci-Fi');


INSERT INTO film_personal (personal_ID, personal_namn) VALUES
(1, 'Keanu Reeves'),
(2, 'Carrie-Anne Moss'),
(3, 'Lana Wachowski'),
(4, 'Lilly Wachowski'),
(5, 'Leonardo DiCaprio'),
(6, 'Joseph Gordon-Levitt'),
(7, 'Christopher Nolan'),
(8, 'Kate Winslet'),
(9, 'James Cameron'),
(10, 'Christian Bale'),
(11, 'Heath Ledger'),
(12, 'Matthew McConaughey'),
(13, 'Anne Hathaway');


-- The Matrix
INSERT INTO Jobbar (FilmID, personal_ID, Roll) VALUES
(1, 1, 'Skådespelare'),
(1, 2, 'Skådespelare'),
(1, 3, 'Regissör'),
(1, 4, 'Regissör');

-- Inception
INSERT INTO Jobbar (FilmID, personal_ID, Roll) VALUES
(2, 5, 'Skådespelare'),
(2, 6, 'Skådespelare'),
(2, 7, 'Regissör');

-- Titanic
INSERT INTO Jobbar (FilmID, personal_ID, Roll) VALUES
(3, 5, 'Skådespelare'),
(3, 8, 'Skådespelare'),
(3, 9, 'Regissör');

-- The Dark Knight
INSERT INTO Jobbar (FilmID, personal_ID, Roll) VALUES
(4, 10, 'Skådespelare'),
(4, 11, 'Skådespelare'),
(4, 7, 'Regissör');

-- Interstellar
INSERT INTO Jobbar (FilmID, personal_ID, Roll) VALUES
(5, 12, 'Skådespelare'),
(5, 13, 'Skådespelare'),
(5, 7, 'Regissör');


INSERT INTO Butik (ButiksID, Butiksnamn, Adress, Telefonnummer, Postnummer, Husnummer, Gata, Stad) VALUES
(1, 'Videobutiken City', 'Storgatan 12', '070-1234567', '85230', '12', 'Storgatan', 'Sundsvall'),
(2, 'Videobutiken Centrum', 'Kungsgatan 5', '070-7654321', '11122', '5', 'Kungsgatan', 'Stockholm');


INSERT INTO Anstalld (Anstallningsnr, Lon, Roll, Utbildning, ButiksID) VALUES
(101, 28000.00, 'Kassör', 'Gymnasieekonomi', 1),
(102, 35000.00, 'Butikschef', 'Ekonomprogrammet', 1),
(103, 29000.00, 'Kundtjänst', 'Gymnasiehandel', 2),
(104, 36000.00, 'Butikschef', 'Handelshögskolan', 2);


INSERT INTO Medlem (MedlemsID, Medlemsnamn, Start_datum_for_medlemskapet, Telefonnummer) VALUES
(201, 'Anna Svensson', '2022-05-10', '073-1112223'),
(202, 'Erik Karlsson', '2023-01-15', '073-3334445'),
(203, 'Maria Andersson', '2024-02-20', '073-5556667');


INSERT INTO Reservering (ReserveringsID, Reserveringsdatum, Reserveringsperiod, Onskade_datum, MedlemsID, FilmID) VALUES
(301, '2024-10-01', 7, '2024-10-03', 201, 1),
(302, '2024-10-05', 5, '2024-10-07', 202, 3),
(303, '2024-10-06', 3, '2024-10-08', 203, 2);


INSERT INTO Finns (FilmID, ButiksID) VALUES
(1, 1),
(2, 1),
(3, 2),
(4, 1),
(5, 2);
