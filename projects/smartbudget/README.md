# SmartBudget

A full-stack web application for **personal budgeting**. Users register incomes, fixed costs and variable expenses and get an overview of their finances, with statistics compared against national data from public APIs.

Course project in *Web Programming* (DT058G/DT100G), Mid Sweden University, spring 2026.

## Features

- **Authentication** – registration and login with PHP sessions and bcrypt (`password_hash`)
- **Budget items** – create, edit and delete incomes, fixed and variable expenses (AJAX delete without page reload)
- **Statistics** – interactive charts with Chart.js
- **External APIs**
  - *World Bank Open Data* – Swedish average income, fetched client-side
  - *Sveriges Riksbank Open API* – fetched server-side with PHP/cURL to work around CORS
- **Admin panel** – user overview, IP tracking, suspend and delete accounts
- **Security** – PDO with prepared statements against SQL injection, role-based access control

## Tech stack

PHP 8 · MySQL / MariaDB (PDO) · JavaScript · Chart.js · HTML5 / CSS3 (animations)

## Structure

```
index.php, login.php, register.php, logout.php   Auth & landing page
dashboard.php, add_item.php, edit_item.php,
delete_item.php                                  Budget CRUD
statistics.php                                   Charts + external APIs
admin_dashboard.php, admin_action.php            Admin panel
includes/   header, footer, auth guard
config/     database connection
assets/     CSS and JavaScript
database/   schema.sql
docs/       project report (Swedish)
```

## Run locally

1. Install a local PHP + MySQL stack (e.g. XAMPP).
2. Create the database: run `database/schema.sql` in MySQL.
3. Adjust credentials in `config/db.php` if needed (defaults: `root`, no password, `localhost`).
4. Place the folder in your web root (e.g. `htdocs/smartbudget`) and open `http://localhost/smartbudget`.
5. To get an admin account, register a user and set its role:
   ```sql
   UPDATE users SET role = 'admin' WHERE username = 'your_username';
   ```

## Report

The full project report (in Swedish), covering design, database model, API integration and problems solved, is in [docs/rapport.txt](docs/rapport.txt).
