# Databases – modelling and implementation

Labs on designing and implementing a relational database for a **video-rental chain**: stores, films, film staff and roles, members, copies per store, rentals and reservations.

| Lab | Content | Files |
|---|---|---|
| Lab 1–2 – ER model & SQL | ER diagram, translation to base relations, MySQL schema with primary/foreign keys, test data and queries (filtering, `LIKE`, `BETWEEN`, `GROUP BY`/`HAVING`) | [labb2-sql-videobutik](labb2-sql-videobutik) |
| Lab 3 – Normalisation | Step-by-step normalisation to 1NF, 2NF and 3NF, with resulting base relations | [labb3-normalisering](labb3-normalisering) |

## Run the SQL

```bash
mysql -u root -p < labb2-sql-videobutik/1_tables.sql
mysql -u root -p videobutik < labb2-sql-videobutik/2_insert.sql
mysql -u root -p videobutik < labb2-sql-videobutik/3_queries.sql
```

The `.drawio` files open in [diagrams.net](https://app.diagrams.net).
