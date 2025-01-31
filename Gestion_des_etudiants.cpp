#include <iostream>
#include <mysql/mysql.h>
#include <limits>
#include <fstream>
#include <string>
#include <iomanip> 
#include <sstream> 

using namespace std;

class Database {
    MYSQL *conn;

public:
    Database() {
        conn = mysql_init(0);
        conn = mysql_real_connect(conn, "localhost", "poo", "poo", "school", 3306, NULL, 0);
        if (!conn) {
            cout << "Database connection failed!" << endl;
            exit(1); 
        }
    }

    MYSQL* getConn() { return conn; }

    bool query(const string &sql) {
        if (mysql_query(conn, sql.c_str()) == 0) {
            cout << "Query executed successfully!" << endl;
            return true;
        }
        cout << "Query failed: " << mysql_error(conn) << endl;
        return false;
    }

    void fetch(const string &sql) {
        if (mysql_query(conn, sql.c_str()) == 0) {
            MYSQL_RES *res = mysql_store_result(conn);
            MYSQL_ROW row;
            // Print header
            unsigned int num_fields = mysql_num_fields(res);
            MYSQL_FIELD *fields = mysql_fetch_fields(res);
            for (unsigned int i = 0; i < num_fields; i++) {
                cout << setw(15) << left << fields[i].name; 
            }
            cout << endl;

            
            while ((row = mysql_fetch_row(res))) {
                for (unsigned int i = 0; i < num_fields; i++) {
                    cout << setw(15) << left << (row[i] ? row[i] : "NULL"); 
                }
                cout << endl;
            }
            mysql_free_result(res);
        } else {
            cout << "Query failed: " << mysql_error(conn) << endl;
        }
    }

    bool delete_record(const string &sql) {
        if (mysql_query(conn, sql.c_str()) == 0) {
            cout << "Record deleted successfully!" << endl;
            return true;
        }
        cout << "Delete failed: " << mysql_error(conn) << endl;
        return false;
    }

    bool update_record(const string &sql) {
        if (mysql_query(conn, sql.c_str()) == 0) {
            cout << "Record updated successfully!" << endl;
            return true;
        }
        cout << "Update failed: " << mysql_error(conn) << endl;
        return false;
    }

    void backup(const string &tableName) {
        ofstream file(tableName + ".csv");
        if (file.is_open()) {
            file << tableName << endl; 
            MYSQL_RES *res;
            string sql = "SELECT * FROM " + tableName;
            if (mysql_query(conn, sql.c_str()) == 0) {
                res = mysql_store_result(conn);
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res))) {
                    for (unsigned int i = 0; i < mysql_num_fields(res); i++) {
                        file << (row[i] ? row[i] : "NULL") << (i < mysql_num_fields(res) - 1 ? "," : ""); // Writing each row
                    }
                    file << endl;
                }
                mysql_free_result(res);
                cout << "Backup of table " << tableName << " created successfully." << endl;
            } else {
                cout << "Backup failed: " << mysql_error(conn) << endl;
            }
            file.close();
        } else {
            cout << "Error opening file." << endl;
        }
    }

    ~Database() {
        if (conn) mysql_close(conn);
    }
};

class Personne {
protected:
    string nom, prenom;

public:
    Personne(string n, string p) : nom(n), prenom(p) {}

    virtual void saisir() = 0; 
};

class Enseignant : public Personne {
    string matiere;

public:
    Enseignant(string n, string p, string m) : Personne(n, p), matiere(m) {}

    void saisir() override {
        cout << "Nom: "; cin >> nom;
        cout << "Prénom: "; cin >> prenom;
        cout << "Matière: "; cin >> matiere;
    }

    void enregistrer(Database &db) {
        string sql = "INSERT INTO enseignants (nom, prenom, matiere) VALUES ('" + nom + "', '" + prenom + "', '" + matiere + "')";
        db.query(sql);
    }

    static void afficher(Database &db, const string &orderBy = "") {
        string sql = "SELECT * FROM enseignants";
        if (!orderBy.empty()) {
            sql += " ORDER BY " + orderBy; // Sort by the specified column
        }
        db.fetch(sql);
    }

    static void rechercher(Database &db) {
        string search;
        cout << "Entrez le nom de l'enseignant à rechercher: ";
        cin >> search;
        db.fetch("SELECT * FROM enseignants WHERE nom LIKE '%" + search + "%'");
    }

    static void afficher_par_id(Database &db, int id) {
        db.fetch("SELECT * FROM enseignants WHERE id = " + to_string(id));
    }

    static void supprimer(Database &db, int id) {
        string sql = "DELETE FROM enseignants WHERE id = " + to_string(id);
        db.delete_record(sql);
    }

    static void modifier(Database &db, int id) {
        string nom, prenom, matiere;
        cout << "Nom: "; cin >> nom;
        cout << "Prénom: "; cin >> prenom;
        cout << "Matière: "; cin >> matiere;
        string sql = "UPDATE enseignants SET nom = '" + nom + "', prenom = '" + prenom + "', matiere = '" + matiere + "' WHERE id = " + to_string(id);
        db.update_record(sql);
    }
};

class Etudiant : public Personne {
    int id_groupe;

public:
    int id;

    Etudiant(int i, string n, string p, int g) : Personne(n, p), id_groupe(g), id(i) {}

    void saisir() override {
        cout << "ID: "; cin >> id;
        cout << "Nom: "; cin >> nom;
        cout << "Prénom: "; cin >> prenom;
        cout << "ID Groupe: "; cin >> id_groupe;
    }

    void enregistrer(Database &db) {
        string sql = "INSERT INTO etudiants (id, nom, prenom, id_groupe) VALUES (" + to_string(id) + ", '" + nom + "', '" + prenom + "', " + to_string(id_groupe) + ")";
        db.query(sql);
    }

    static void afficher(Database &db, const string &orderBy = "") {
        string sql = "SELECT * FROM etudiants";
        if (!orderBy.empty()) {
            sql += " ORDER BY " + orderBy; // Sort by the specified column
        }
        db.fetch(sql);
    }

    static void rechercher(Database &db) {
        string search;
        cout << "Entrez le nom de l'étudiant à rechercher: ";
        cin >> search;
        db.fetch("SELECT * FROM etudiants WHERE nom LIKE '%" + search + "%'");
    }

    static void afficher_par_id(Database &db, int id) {
        db.fetch("SELECT * FROM etudiants WHERE id = " + to_string(id));
    }

    static void supprimer(Database &db, int id) {
        string sql = "DELETE FROM etudiants WHERE id = " + to_string(id);
        db.delete_record(sql);
    }

    static void modifier(Database &db, int id) {
        string nom, prenom;
        int id_groupe;
        cout << "Nom: "; cin >> nom;
        cout << "Prénom: "; cin >> prenom;
        cout << "ID Groupe: "; cin >> id_groupe;
        string sql = "UPDATE etudiants SET nom = '" + nom + "', prenom = '" + prenom + "', id_groupe = " + to_string(id_groupe) + " WHERE id = " + to_string(id);
        db.update_record(sql);
    }
};

class Examen {
public:
    int id;
    string matiere, date;

    Examen(int i, string m, string d) : id(i), matiere(m), date(d) {}

    void saisir() {
        cout << "ID: "; cin >> id;
        cout << "Matière: "; cin >> matiere;
        cout << "Date: "; cin >> date;
    }

    void enregistrer(Database &db) {
        string sql = "INSERT INTO examens (id, matiere, date) VALUES (" + to_string(id) + ", '" + matiere + "', '" + date + "')";
        db.query(sql);
    }

    static void afficher(Database &db, const string &orderBy = "") {
        string sql = "SELECT * FROM examens";
        if (!orderBy.empty()) {
            sql += " ORDER BY " + orderBy; 
        }
        db.fetch(sql);
    }

    static void supprimer(Database &db, int id) {
        string sql = "DELETE FROM examens WHERE id = " + to_string(id);
        db.delete_record(sql);
    }

    static void modifier(Database &db, int id) {
        string matiere, date;
        cout << "Matière: "; cin >> matiere;
        cout << "Date: "; cin >> date;
        string sql = "UPDATE examens SET matiere = '" + matiere + "', date = '" + date + "' WHERE id = " + to_string(id);
        db.update_record(sql);
    }
};

class Note {
    int id_etudiant, id_examen;
    float note;

public:
    Note(int etu, int exa, float n) : id_etudiant(etu), id_examen(exa), note(n) {}

    void enregistrer(Database &db) {
        string sql = "INSERT INTO notes (id_etudiant, id_examen, note) VALUES (" + to_string(id_etudiant) + ", " + to_string(id_examen) + ", " + to_string(note) + ")";
        db.query(sql);
    }

    static void afficher(Database &db, const string &orderBy = "") {
        string sql = "SELECT * FROM notes";
        if (!orderBy.empty()) {
            sql += " ORDER BY " + orderBy;
        }
        db.fetch(sql);
    }

    static void supprimer(Database &db, int id_etudiant, int id_examen) {
        string sql = "DELETE FROM notes WHERE id_etudiant = " + to_string(id_etudiant) + " AND id_examen = " + to_string(id_examen);
        db.delete_record(sql);
    }

    static void modifier(Database &db, int id_etudiant, int id_examen) {
        float note;
        cout << "Note: "; cin >> note;
        string sql = "UPDATE notes SET note = " + to_string(note) + " WHERE id_etudiant = " + to_string(id_etudiant) + " AND id_examen = " + to_string(id_examen);
        db.update_record(sql);
    }

    static void calculerMoyenne(Database &db, int id_etudiant) {
        string sql = "SELECT AVG(note) FROM notes WHERE id_etudiant = " + to_string(id_etudiant);
        MYSQL_RES *res;
        if (mysql_query(db.getConn(), sql.c_str()) == 0) {
            res = mysql_store_result(db.getConn());
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row[0]) {
                cout << "La moyenne de l'étudiant (ID " << id_etudiant << ") est: " << row[0] << endl;
            } else {
                cout << "Aucune note trouvée pour cet étudiant." << endl;
            }
            mysql_free_result(res);
        } else {
            cout << "Échec du calcul de la moyenne: " << mysql_error(db.getConn()) << endl;
        }
    }
};

// Function to clear input buffer
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to validate int input
bool validateIntInput(int &input) {
    while (true) {
        cin >> input;
        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input. Please enter an integer: ";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            return true;
        }
    }
}

bool validateFloatInput(float &input) {
    while (true) {
        cin >> input;
        if (cin.fail()) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a float: ";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            return true;
        }
    }
}


void exporterEtudiants(Database &db) {
    ofstream file("etudiants.csv");
    if (file.is_open()) {
        file << "ID,Nom,Prénom,ID Groupe\n"; 
        MYSQL_RES *res;
        if (mysql_query(db.getConn(), "SELECT * FROM etudiants") == 0) {
            res = mysql_store_result(db.getConn());
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                file << row[0] << "," << row[1] << "," << row[2] << "," << row[3] << "\n"; 
            }
            mysql_free_result(res);
            cout << "Les données des étudiants ont été exportées vers etudiants.csv" << endl;
        } else {
            cout << "Échec de l'exportation : " << mysql_error(db.getConn()) << endl;
        }
        file.close();
    } else {
        cout << "Erreur lors de l'ouverture du fichier." << endl;
    }
}

void importerEtudiants(Database &db) {
    ifstream file("etudiants.csv");
    if (file.is_open()) {
        string line, id, nom, prenom, id_groupe;
        getline(file, line); 
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, id, ',');
            getline(ss, nom, ',');
            getline(ss, prenom, ',');
            getline(ss, id_groupe, ',');
            string sql = "INSERT INTO etudiants (id, nom, prenom, id_groupe) VALUES (" + id + ", '" + nom + "', '" + prenom + "', " + id_groupe + ")";
            db.query(sql);
        }
        cout << "Les données des étudiants ont été importées." << endl;
        file.close();
    } else {
        cout << "Erreur lors de l'ouverture du fichier." << endl;
    }
}

void displayMainMenu() {
    cout << "\n==============================" << endl;
    cout << "           MENU               " << endl;
    cout << "==============================" << endl;
    cout << "1. Gérer les Étudiants" << endl;
    cout << "2. Gérer les Enseignants" << endl;
    cout << "3. Gérer les Examens" << endl;
    cout << "4. Gérer les Notes" << endl;
    cout << "5. Exporter/Importer" << endl;
    cout << "6. Backup" << endl;
    cout << "0. Quitter" << endl;
    cout << "==============================" << endl;
    cout << "Veuillez entrer votre choix: ";
}

void displayEtudiantMenu() {
    cout << "\n=== Menu Étudiants ===" << endl;
    cout << "1. Ajouter un étudiant" << endl;
    cout << "2. Afficher les étudiants" << endl;
    cout << "3. Rechercher un étudiant" << endl;
    cout << "4. Afficher un étudiant par ID" << endl;
    cout << "12. Supprimer un étudiant" << endl;
    cout << "16. Modifier un étudiant" << endl;
    cout << "20. Calculer la moyenne d'un étudiant" << endl;
    cout << "Retourner au menu principal (0)" << endl;
    cout << "==============================" << endl;
    cout << "Veuillez entrer votre choix: ";
}

void displayEnseignantMenu() {
    cout << "\n=== Menu Enseignants ===" << endl;
    cout << "5. Ajouter un enseignant" << endl;
    cout << "6. Afficher les enseignants" << endl;
    cout << "7. Rechercher un enseignant" << endl;
    cout << "13. Supprimer un enseignant" << endl;
    cout << "17. Modifier un enseignant" << endl;
    cout << "Retourner au menu principal (0)" << endl;
    cout << "==============================" << endl;
    cout << "Veuillez entrer votre choix: ";
}

void displayExamenMenu() {
    cout << "\n=== Menu Examens ===" << endl;
    cout << "8. Ajouter un examen" << endl;
    cout << "9. Afficher les examens" << endl;
    cout << "14. Supprimer un examen" << endl;
    cout << "18. Modifier un examen" << endl;
    cout << "Retourner au menu principal (0)" << endl;
    cout << "==============================" << endl;
    cout << "Veuillez entrer votre choix: ";
}

void displayNoteMenu() {
    cout << "\n=== Menu Notes ===" << endl;
    cout << "10. Ajouter une note" << endl;
    cout << "11. Afficher les notes" << endl;
    cout << "15. Supprimer une note" << endl;
    cout << "19. Modifier une note" << endl;
    cout << "Retourner au menu principal (0)" << endl;
    cout << "==============================" << endl;
    cout << "Veuillez entrer votre choix: ";
}

void displayBackupMenu() {
    cout << "\n=== Menu Backup ===" << endl;
    cout << "23. Backup des enseignants" << endl;
    cout << "24. Backup des examens" << endl;
    cout << "25. Backup des notes" << endl;
    cout << "Retourner au menu principal (0)" << endl;
    cout << "==============================" << endl;
    cout << "Veuillez entrer votre choix: ";
}

void displayFileMenu() {
    cout << "\n=== Menu Importer/Exporter ===" << endl;
    cout << "21. Exporter les données des étudiants" << endl;
    cout << "22. Importer les données des étudiants" << endl;
    cout << "Retourner au menu principal (0)" << endl;
    cout << "==============================" << endl;
    cout << "Veuillez entrer votre choix: ";
}


int main() {
    Database db;
    int choix;

    do {
        displayMainMenu();
        validateIntInput(choix); 

        switch (choix) {
            case 1: {
                int etuChoix;
                do {
                    displayEtudiantMenu();
                    validateIntInput(etuChoix);
                    switch (etuChoix) {
                        case 1: {
                            Etudiant e(0, "", "", 0);
                            e.saisir();
                            e.enregistrer(db);
                            break;
                        }
                        case 2: {
                            string order;
                            cout << "Trier les étudiants par (id, nom): "; 
                            cin >> order;
                            Etudiant::afficher(db, order);
                            break;
                        }
                        case 3: {
                            Etudiant::rechercher(db);
                            break;
                        }
                        case 4: {
                            int id;
                            cout << "Saisir l'ID de l'étudiant: "; validateIntInput(id);
                            Etudiant::afficher_par_id(db, id);
                            break;
                        }
                        case 12: {
                            int id;
                            cout << "Saisir l'ID de l'étudiant à supprimer: "; validateIntInput(id);
                            Etudiant::supprimer(db, id);
                            break;
                        }
                        case 16: {
                            int id;
                            cout << "Saisir l'ID de l'étudiant à modifier: "; validateIntInput(id);
                            Etudiant::modifier(db, id);
                            break;
                        }
                        case 20: {
                            int id_etudiant;
                            cout << "Saisir l'ID de l'étudiant pour calculer la moyenne: "; validateIntInput(id_etudiant);
                            Note::calculerMoyenne(db, id_etudiant);
                            break;
                        }
                        case 0: break;
                        default: {
                            cout << "Choix invalide! Veuillez réessayer." << endl; 
                        }
                    }
                } while (etuChoix != 0);
                break;
            }
            case 2: {
                int enseignantChoix;
                do {
                    displayEnseignantMenu();
                    validateIntInput(enseignantChoix);
                    switch (enseignantChoix) {
                        case 5: {
                            Enseignant e("", "", "");
                            e.saisir();
                            e.enregistrer(db);
                            break;
                        }
                        case 6: {
                            string order;
                            cout << "Trier les enseignants par (id, nom): ";
                            cin >> order;
                            Enseignant::afficher(db, order);
                            break;
                        }
                        case 7: {
                            Enseignant::rechercher(db);
                            break;
                        }
                        case 13: {
                            int id;
                            cout << "Saisir l'ID de l'enseignant à supprimer: "; validateIntInput(id);
                            Enseignant::supprimer(db, id);
                            break;
                        }
                        case 17: {
                            int id;
                            cout << "Saisir l'ID de l'enseignant à modifier: "; validateIntInput(id);
                            Enseignant::modifier(db, id);
                            break;
                        }
                        case 0: break;
                        default: {
                            cout << "Choix invalide! Veuillez réessayer." << endl;
                        }
                    }
                } while (enseignantChoix != 0);
                break;
            }
            case 3: {
                int examenChoix;
                do {
                    displayExamenMenu();
                    validateIntInput(examenChoix);
                    switch (examenChoix) {
                        case 8: {
                            Examen ex(0, "", "");
                            ex.saisir();
                            ex.enregistrer(db);
                            break;
                        }
                        case 9: {
                            string order;
                            cout << "Trier les examens par (id, matiere): ";
                            cin >> order;
                            Examen::afficher(db, order);
                            break;
                        }
                        case 14: {
                            int id;
                            cout << "Saisir l'ID de l'examen à supprimer: "; validateIntInput(id);
                            Examen::supprimer(db, id);
                            break;
                        }
                        case 18: {
                            int id;
                            cout << "Saisir l'ID de l'examen à modifier: "; validateIntInput(id);
                            Examen::modifier(db, id);
                            break;
                        }
                        case 0: break;
                        default: {
                            cout << "Choix invalide! Veuillez réessayer." << endl;
                        }
                    }
                } while (examenChoix != 0);
                break;
            }
            case 4: {
                int noteChoix;
                do {
                    displayNoteMenu();
                    validateIntInput(noteChoix);
                    switch (noteChoix) {
                        case 10: {
                            int etu, exa;
                            float note;
                            cout << "Saisir l'ID de l'étudiant: "; validateIntInput(etu);
                            cout << "Saisir l'ID de l'examen: "; validateIntInput(exa);
                            cout << "Saisir la note: "; validateFloatInput(note);
                            Note n(etu, exa, note);
                            n.enregistrer(db);
                            break;
                        }
                        case 11: {
                            string order;
                            cout << "Trier les notes par (id_etudiant, id_examen): ";
                            cin >> order;
                            Note::afficher(db, order);
                            break;
                        }
                        case 15: {
                            int id_etudiant, id_examen;
                            cout << "Saisir l'ID de l'étudiant: "; validateIntInput(id_etudiant);
                            cout << "Saisir l'ID de l'examen: "; validateIntInput(id_examen);
                            Note::supprimer(db, id_etudiant, id_examen);
                            break;
                        }
                        case 19: {
                            int id_etudiant, id_examen;
                            cout << "Saisir l'ID de l'étudiant: "; validateIntInput(id_etudiant);
                            cout << "Saisir l'ID de l'examen: "; validateIntInput(id_examen);
                            Note::modifier(db, id_etudiant, id_examen);
                            break;
                        }
                        case 0: break;
                        default: {
                            cout << "Choix invalide! Veuillez réessayer." << endl;
                        }
                    }
                } while (noteChoix != 0);
                break;
            }
            case 5: {
                int fileChoix;
                do {
                    displayFileMenu();
                    validateIntInput(fileChoix);
                    switch (fileChoix) {
                        case 21: {
                            exporterEtudiants(db);
                            break;
                        }
                        case 22: {
                            importerEtudiants(db);
                            break;
                        }
                        case 0: break;
                        default: {
                            cout << "Choix invalide! Veuillez réessayer." << endl;
                        }
                    }
                } while (fileChoix != 0);
                break;
            }
            case 6: {
                int backupChoix;
                do {
                    displayBackupMenu();
                    validateIntInput(backupChoix);
                    switch (backupChoix) {
                        case 23: {
                            db.backup("enseignants");
                            break;
                        }
                        case 24: {
                            db.backup("examens");
                            break;
                        }
                        case 25: {
                            db.backup("notes");
                            break;
                        }
                        case 0: break;
                        default: {
                            cout << "Choix invalide! Veuillez réessayer." << endl;
                        }
                    }
                } while (backupChoix != 0);
                break;
            }
            case 0: {
                cout << "Merci d'utiliser le programme. Au revoir!" << endl;
                break;
            }
            default: {
                cout << "Choix invalide! Veuillez réessayer." << endl;
            }
        }

    } while (choix != 0); 

    return 0;
}