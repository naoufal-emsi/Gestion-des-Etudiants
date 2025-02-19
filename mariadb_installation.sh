#!/bin/bash

# Update package list and install MariaDB
sudo apt-get update
sudo apt-get install -y mariadb-server

# Start and enable MariaDB service
sudo service mariadb start

# Run mysql_secure_installation
sudo mysql_secure_installation <<EOF

y
poo
poo
y
y
y
y
EOF

# Log in to MariaDB as root and create user 'poo' with password 'poo'
sudo mysql -u root <<EOF
CREATE USER 'poo'@'localhost' IDENTIFIED BY 'poo';
GRANT ALL PRIVILEGES ON *.* TO 'poo'@'localhost' WITH GRANT OPTION;
FLUSH PRIVILEGES;
EXIT;
EOF

# Log in as 'poo' and create the 'school' database and tables
mysql -u poo -ppoo <<EOF
CREATE DATABASE school;
USE school;

CREATE TABLE enseignants (
    id INT AUTO_INCREMENT PRIMARY KEY,
    nom VARCHAR(255) NOT NULL,
    prenom VARCHAR(255) NOT NULL,
    matiere VARCHAR(255) NOT NULL
);

CREATE TABLE etudiants (
    id INT PRIMARY KEY,
    nom VARCHAR(255) NOT NULL,
    prenom VARCHAR(255) NOT NULL,
    id_groupe INT NOT NULL
);

CREATE TABLE examens (
    id INT PRIMARY KEY,
    matiere VARCHAR(255) NOT NULL,
    date DATE NOT NULL
);

CREATE TABLE notes (
    id_etudiant INT NOT NULL,
    id_examen INT NOT NULL,
    note FLOAT NOT NULL,
    PRIMARY KEY (id_etudiant, id_examen)
);

-- List all tables and their columns
SHOW TABLES;
SELECT TABLE_NAME, COLUMN_NAME, DATA_TYPE, IS_NULLABLE, COLUMN_DEFAULT, COLUMN_KEY, EXTRA
FROM information_schema.COLUMNS
WHERE TABLE_SCHEMA = 'school';
EOF