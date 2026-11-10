-- =========================================
-- SQL-frågor till filmbutiksdatabas
-- Skapad av: Peter Emad Younan Samy
-- =========================================

USE videobutik;

-- Hämtar alla filmer som inte tillhör kategorin 'Action'
SELECT * 
FROM Film
WHERE Kategori != 'Action';

-- Hämtar alla medlemmar vars namn börjar med bokstaven 'A'
SELECT * 
FROM Medlem
WHERE Medlemsnamn LIKE 'A%';

-- Hämtar alla reservationer som gjorts mellan 1 och 6 oktober 2024
SELECT * 
FROM Reservering
WHERE Reserveringsdatum BETWEEN '2024-10-01' AND '2024-10-06';

-- Räknar hur många filmer det finns i varje kategori
SELECT Kategori, COUNT(*) AS AntalFilmer
FROM Film
GROUP BY Kategori;

-- Räknar hur många filmer det finns i varje kategori, men visar bara kategorier med fler än 1 film
SELECT Kategori, COUNT(*) AS AntalFilmer
FROM Film
GROUP BY Kategori
HAVING COUNT(*) > 1;

-- Visar alla medlemmar sorterade efter telefonnummer i fallande ordning
SELECT * 
FROM Medlem
ORDER BY telefonnummer DESC;

-- Uppdaterar lönen till 18 000 kr för den anställde med anställningsnummer 101
UPDATE Anstalld
SET Lon = 18000.00
WHERE Anstallningsnr = 101;

-- Uppdaterar telefonnumret för medlemmen med ID 202
UPDATE Medlem
SET Telefonnummer = '073-9998888'
WHERE MedlemsID = 202;

-- Tar bort kopplingen mellan film med ID 5 och butiker i tabellen 'Finns'
DELETE FROM Finns
WHERE FilmID = 5;

-- Tar bort själva filmen med ID 5 från tabellen 'Film'
DELETE FROM Film
WHERE FilmID = 5;

-- Hämtar namnen på medlemmar som reserverat filmer som finns i butik med ID 1
SELECT Medlemsnamn
FROM Medlem
WHERE MedlemsID IN (
    SELECT MedlemsID
    FROM Reservering
    WHERE FilmID IN (
        SELECT FilmID
        FROM Finns
        WHERE ButiksID = 1
    )
);

