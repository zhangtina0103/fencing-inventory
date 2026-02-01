#include "Database.h"

Database::Database(std::string filename) {
    int result = sqlite3_open(filename.c_str(), &db);
    if (result) {
        std::cerr << "can't open database: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "database opened ~" << std::endl;
    }
}

Database::~Database() {
    sqlite3_close(db);
    std::cout << "database closed ~" << std::endl;
}

void Database::execute(std::string sql) {
    char* errMsg;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (result != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "success ~" << std::endl;
    }
}

void Database::createTables() {
    // only create if table doesn't exist yet
    std::string sql = R"(
        CREATE TABLE IF NOT EXISTS items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            quantity INTEGER,
            price REAL,
            item_type TEXT
        );

        CREATE TABLE IF NOT EXISTS weapons (
            id INTEGER PRIMARY KEY,
            grip TEXT,
            weapon_type TEXT,
            FOREIGN KEY (id) REFERENCES items(id)
        );

        CREATE TABLE IF NOT EXISTS gear (
            id INTEGER PRIMARY KEY,
            size TEXT,
            brand TEXT,
            FOREIGN KEY (id) REFERENCES items(id)
        );
    )";

    char* errMsg;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (result != SQLITE_OK) {
        std::cerr << "error creating tables: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "tables ready ~" << std::endl;
    }
}

void Database::displayAll() {
    std::string sql = "SELECT * FROM items;";

    sqlite3_stmt* stmt;  // prepared statement
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    std::cout << "\n=== all items ~ ====" << std::endl;

    // loop through each row
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);
        const char* item_type = (const char*)sqlite3_column_text(stmt, 4);

        std::cout << id << " | " << name << " | " << quantity << " | $" << price << " | " << item_type << std::endl;
    }
    // cleanup
    sqlite3_finalize(stmt);
}

void Database::addItem(std::string name, int quantity, double price, std::string item_type) {
    std::string sql = "INSERT INTO items (name, quantity, price, item_type) VALUES ('"
                      + name + "', "
                      + std::to_string(quantity) + ", "
                      + std::to_string(price) + ", '"
                      + item_type + "');";
    execute(sql);
}

void Database::removeItem(int id) {
    std::string sql = "DELETE FROM items WHERE id = "
                      + std::to_string(id) + ";";
    execute(sql);

}

void Database::update(std::string field, std::string new_value, std::string where_field, std::string where_value) {
    std::string sql = "UPDATE items SET " + field + " = '" + new_value + "' WHERE " + where_field + " = '" + where_value + "';";
    execute(sql);
}

void Database::search(std::string field, std::string value) {
    std::string sql = "SELECT * FROM items WHERE " + field + " = '" + value + "';";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);
        const char* item_type = (const char*)sqlite3_column_text(stmt, 4);

        std::cout << id << " | " << name << " | " << quantity << " | $" << price << " | " << item_type << std::endl;
    }
    sqlite3_finalize(stmt);
}

int Database::addWeapon(std::string name, int quantity, double price, std::string grip, std::string weapon_type) {
    // first insert into items table
    std::string sql = "INSERT INTO items (name, quantity, price, item_type) VALUES ('"
                      + name + "', "
                      + std::to_string(quantity) + ", "
                      + std::to_string(price) + ", 'weapon');";
    execute(sql);

    // get id
    int id = sqlite3_last_insert_rowid(db);

    // insert into weapons table
    sql = "INSERT into weapons (id, grip, weapon_type) VALUES ("
        + std::to_string(id) + ", '"
        + grip + "', '"
        + weapon_type + "');";
    execute(sql);

    return id;
}

int Database::addGear(std::string name, int quantity, double price, std::string size, std::string brand) {
    // first insert into items table
    std::string sql = "INSERT INTO items (name, quantity, price, item_type) VALUES ('"
                      + name + "', "
                      + std::to_string(quantity) + ", "
                      + std::to_string(price) + ", 'gear');";
    execute(sql);

    // get id
    int id = sqlite3_last_insert_rowid(db);

    // insert into weapons table
    sql = "INSERT into gear (id, size, brand) VALUES ("
        + std::to_string(id) + ", '"
        + size + "', '"
        + brand + "');";
    execute(sql);
    return id;
}

void Database::displayWeapons() {
    std::string sql = "SELECT items.id, items.name, items.quantity, items.price, weapons.grip, weapons.weapon_type "
                      "FROM items JOIN weapons ON items.id = weapons.id;";

    sqlite3_stmt* stmt;  // prepared statement
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    std::cout << "\n=== all weapons ~ ====" << std::endl;

    // loop through each row
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);
        const char* grip = (const char*)sqlite3_column_text(stmt, 4);
        const char* weapon_type = (const char*)sqlite3_column_text(stmt, 5);

        std::cout << id << " | " << name << " | " << quantity << " | $" << price << " | " << grip << " | " << weapon_type << std::endl;
    }
    // cleanup
    sqlite3_finalize(stmt);
}

void Database::displayGear(){
    std::string sql = "SELECT items.id, items.name, items.quantity, items.price, gear.size, gear.brand "
                      "FROM items JOIN gear ON items.id = gear.id;";

    sqlite3_stmt* stmt;  // prepared statement
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    std::cout << "\n=== all gear ~ ====" << std::endl;

    // loop through each row
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);
        const char* size = (const char*)sqlite3_column_text(stmt, 4);
        const char* brand= (const char*)sqlite3_column_text(stmt, 5);

        std::cout << id << " | " << name << " | " << quantity << " | $" << price << " | " << size << " | " << brand << std::endl;
    }
    // cleanup
    sqlite3_finalize(stmt);

}
