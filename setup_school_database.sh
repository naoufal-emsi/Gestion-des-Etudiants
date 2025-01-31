#!/bin/bash

# Define variables
DB_NAME="school"
SQL_FILE="setup_school_database.sql"

# Create the SQL file
cat <<EOF > $SQL_FILE
CREATE DATABASE IF NOT EXISTS $DB_NAME;
USE $DB_NAME;

-- Create table for enseignants (teachers)
CREATE TABLE IF NOT EXISTS enseignants (
    id INT AUTO_INCREMENT PRIMARY KEY,
    nom VARCHAR(100) NOT NULL,
    prenom VARCHAR(100) NOT NULL,
    matiere VARCHAR(100) NOT NULL
);

-- Create table for etudiants (students)
CREATE TABLE IF NOT EXISTS etudiants (
    id INT PRIMARY KEY,
    nom VARCHAR(100) NOT NULL,
    prenom VARCHAR(100) NOT NULL,
    id_groupe INT NOT NULL
);

-- Create table for examens (exams)
CREATE TABLE IF NOT EXISTS examens (
    id INT PRIMARY KEY,
    matiere VARCHAR(100) NOT NULL,
    date DATE NOT NULL
);

-- Create table for notes (grades)
CREATE TABLE IF NOT EXISTS notes (
    id_etudiant INT,
    id_examen INT,
    note FLOAT NOT NULL,
    PRIMARY KEY (id_etudiant, id_examen),
    FOREIGN KEY (id_etudiant) REFERENCES etudiants(id),
    FOREIGN KEY (id_examen) REFERENCES examens(id)
);
EOF

# Log in to MySQL and execute the SQL file
mysql -u root -p < $SQL_FILE