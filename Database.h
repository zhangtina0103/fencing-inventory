#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <iostream>

class Database {
public:
    //pointer to database connection
    sqlite3* db;

    //open database
    Database(std::string filename);
    //close database
    ~Database();

    //for running SQL commands
    void execute(std::string sql);
    void createTables();
    void displayAll();
    void addItem(std::string name, int quantity, double price, std::string item_type);
    void removeItem(int id);
    void update(std::string field, std::string new_value, std::string where_field, std::string where_value);
    void search(std::string field, std::string value);

    int addWeapon(std::string name, int quantity, double price, std::string grip, std::string weapon_type);
    int addGear(std::string name, int quantity, double price, std::string size, std::string brand);
    void displayWeapons();
    void displayGear();
};

#endif
