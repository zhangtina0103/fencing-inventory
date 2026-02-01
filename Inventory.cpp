#include "Inventory.h"

Inventory::Inventory(Database& database) : db(database) {
    // load items when inventory is created
    loadFromDatabase();
}

void Inventory::loadFromDatabase() {
    // clear existing items
    items.clear();

    //query weapons table
    std::string sql = "SELECT items.id, items.name, items.quantity, items.price, weapons.grip, weapons.weapon_type "
                      "FROM items JOIN weapons ON items.id = weapons.id;";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db.db, sql.c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);
        const char* grip = (const char*)sqlite3_column_text(stmt, 4);
        const char* weapon_type = (const char*)sqlite3_column_text(stmt, 5);

        // create Weapon object and add to vector
        Weapon* weapon = new Weapon(name, quantity, price, grip, weapon_type);
        weapon->id = id;  // set id from database
        items.push_back(weapon);
    }

    sqlite3_finalize(stmt);

    //query gear table
    sql = "SELECT items.id, items.name, items.quantity, items.price, gear.size, gear.brand "
                "FROM items JOIN gear ON items.id = gear.id;";

    sqlite3_prepare_v2(db.db, sql.c_str(), -1, &stmt, nullptr);  // Prepare gear query

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        int quantity = sqlite3_column_int(stmt, 2);
        double price = sqlite3_column_double(stmt, 3);
        const char* size = (const char*)sqlite3_column_text(stmt, 4);
        const char* brand = (const char*)sqlite3_column_text(stmt, 5);

        // create Gear object and add to vector
        Gear* gear = new Gear(name, quantity, price, size, brand);
        gear->id = id;  // set id from database
        items.push_back(gear);
    }
    sqlite3_finalize(stmt);
}

void Inventory::addWeapon(std::string name, int quantity, double price, std::string grip, std::string weapon_type) {
    // add to database
    int id = db.addWeapon(name, quantity, price, grip, weapon_type);

    // create c++ object and add to vector
    Weapon* weapon = new Weapon(name, quantity, price, grip, weapon_type);
    weapon->id = id; // set id from database
    items.push_back(weapon);
}


void Inventory::addGear(std::string name, int quantity, double price, std::string size, std::string brand) {
    // add to database
    int id = db.addGear(name, quantity, price, size, brand);

    // create c++ object and add to vector
    Gear* gear = new Gear(name, quantity, price, size, brand);
    gear->id = id; //set id from database
    items.push_back(gear);
}


//Inventory class
void Inventory::add(Item* item) {
    // add to back of items
    items.emplace_back(item);
    std::cout << item->quantity << " " << item->name << "(s) [id: " << item->id << "] added ~" << std::endl;
};

void Inventory::remove(int id) {
    // remove by id
    // first find index of id inside Item object
    for (auto it = items.begin(); it != items.end(); it++) {
        if ((*it)->id == id) {
            std::cout << (*it)->quantity << " " << (*it)->name << "(s) [id: " << id << "] removed ~" << std::endl;
            db.removeItem(id); //remove from database
            delete *it;
            items.erase(it);
            return;
        }
    }
    std::cout << "item with id " << id << " not found ~" << std::endl;
}

// display all inventory items
void Inventory::display_all() {
    // first check if inventory empty
    if (items.empty()) {
        std::cout << "Inventory is empty ~" << std::endl;
        return;
    }

    for (Item* item: items) {
        item->display();
    }
}

// display top n highest quantity items' specified by name, quantity, etc.
// pass in string "name", "quantity", etc.
void Inventory::display_top_n(std::string attribute, int n, bool ascending) {
    // mapping string attribute to which function to call
    std::map<std::string, std::function<bool(Item*, Item*)>> comparisons = {
        {"name", [](Item* a, Item* b) { return a->name < b->name; }},
        {"quantity", [](Item* a, Item* b) { return a->quantity < b->quantity; }},
        {"id", [](Item* a, Item* b) { return a->id < b->id; }},
        {"price", [](Item* a, Item* b) { return a->price < b->price; }},
    };

    //check attribute exists
    if (comparisons.find(attribute) == comparisons.end()) {
        std::cout << "Invalid attribute ~" << std::endl;
        return;
    };

    //make copy
    std::vector<Item*> sorted_items = items;
    //sort
    if (ascending) {
    std::sort(sorted_items.begin(), sorted_items.end(), comparisons[attribute]);
    } else {
    std::sort(sorted_items.begin(), sorted_items.end(), [&](Item* a, Item* b) {
        return comparisons[attribute](b, a);
    });
}
    // display top n (or less if not enough items)
    int count = std::min(n, (int)sorted_items.size());
    for (int i = 0; i < count; i++) {
        sorted_items[i]->display();
    };
}

// display sorted items alphabetical by item name default ascending
void Inventory::display_alphabetical(bool ascending) {
    display_top_n("name", items.size(), ascending);
}

// display sorted items by quantity
void Inventory::display_quantity(bool ascending) {
    display_top_n("quantity", items.size(), ascending);
}

//display sorted items by price
void Inventory::display_price(bool ascending) {
    display_top_n("price", items.size(), ascending);
}

// display sorted items by id
void Inventory::display_id(bool ascending) {
    display_top_n("id", items.size(), ascending);
}

// helper function to display specific things like quantity, etc
// 1. search for specific name, quantity, price, id (with * added then everything after letter), quantity, etc.
// 2. display all with quantity greater than 2, etc.
// display items with filters eg: specific blade
// able to take in multiple filters
void Inventory::display_filters(std::vector<std::function<bool(Item*)>> conditions) {
    for (Item* item: items) {
        bool passes_all = true;

        for (auto& condition: conditions) {
            if (!condition(item)) {
                passes_all = false;
                break;
            }
        }

        if (passes_all) {
            item->display();
        }
    }
}

//update - modify existing item based on id
void Inventory::update_quantity(int id, int new_quantity) {
    // find Item object with id
    for (Item* item : items) {
        if (item->id == id) {
            item->quantity = new_quantity;
            db.update("quantity", std::to_string(new_quantity), "id", std::to_string(id));
            std::cout << item->name << " quantity updated to " << new_quantity << " ~" << std::endl;
            return;
        }
    }
    std::cout << "item not found ~" << std::endl;
}
// update name
void Inventory::update_name(int id, std::string new_name) {
    // find Item object with id
    // placeholder for old name
    std::string old_name;
    for (Item* item : items) {
        if (item->id == id) {
            old_name = item->name;
            item->name = new_name;
            db.update("name", new_name, "id", std::to_string(id));
            std::cout << old_name << " name updated to " << new_name << " ~" << std::endl;
            return;
        }
    }
    std::cout << "item not found ~" << std::endl;
}

// update price
void Inventory::update_price(int id, double new_price) {
    // find Item object with id
    for (Item* item : items) {
        if (item->id == id) {
            item->price = new_price;
            db.update("price", std::to_string(new_price), "id", std::to_string(id));
            std::cout << item->name << " price updated to " << new_price << " ~" << std::endl;
            return;
        }
    }
    std::cout << "item not found ~" << std::endl;
}

//update child class attributes by dynamically casting types
void Inventory::update_grip(int id, std::string new_grip) {
    for (Item* item : items) {
        if (item->id == id) {
            // cast to child type
            Weapon* weapon = dynamic_cast<Weapon*>(item);
            if (weapon) { // if cast succeeded
                weapon->grip = new_grip;
                db.update("grip", new_grip, "id", std::to_string(id));
                std::cout << item->name << " grip updated to " << new_grip << " ~" << std::endl;
            } else {
                std::cout << "item is not a weapon ~" << std::endl;
            }
            return;
        }
    }
    std::cout << "item not found ~" << std::endl;
}

void Inventory::update_weapon_type(int id, std::string new_weapon_type) {
    for (Item* item : items) {
        if (item->id == id) {
            // cast to child type
            Weapon* weapon = dynamic_cast<Weapon*>(item);
            if (weapon) { // if cast succeeded
                weapon->weapon_type = new_weapon_type;
                db.update("weapon_type", new_weapon_type, "id", std::to_string(id));
                std::cout << item->name << " weapon type updated to " << new_weapon_type << " ~" << std::endl;
            } else {
                std::cout << "item is not a weapon ~" << std::endl;
            }
            return;
        }
    }
    std::cout << "item not found ~" << std::endl;
}

void Inventory::update_size(int id, std::string new_size) {
    for (Item* item : items) {
        if (item->id == id) {
            // cast to child type
            Gear* gear = dynamic_cast<Gear*>(item);
            if (gear) { // if cast succeeded
                gear->size = new_size;
                db.update("size", new_size, "id", std::to_string(id));
                std::cout << item->name << " size updated to " << new_size << " ~" << std::endl;
            } else {
                std::cout << "item is not a gear ~" << std::endl;
            }
            return;
        }
    }
    std::cout << "item not found ~" << std::endl;
}

void Inventory::update_brand(int id, std::string new_brand) {
    for (Item* item : items) {
        if (item->id == id) {
            // cast to child type
            Gear* gear = dynamic_cast<Gear*>(item);
            if (gear) { // if cast succeeded
                gear->brand = new_brand;
                db.update("brand", new_brand, "id", std::to_string(id));
                std::cout << item->name << " brand updated to " << new_brand<< " ~" << std::endl;
            } else {
                std::cout << "item is not a gear ~" << std::endl;
            }
            return;
        }
    }
    std::cout << "item not found ~" << std::endl;
}

//get_total_value = sum of price*quantity
double Inventory::get_total_value() {
    double total = 0;
    for (Item* item: items) {
        total += item->price * item->quantity;
    }
    return total;
}

// get total value of specific id
void Inventory::get_value(int id) {
    // find Item with id
    for (auto it = items.begin(); it != items.end(); it++) {
        if ((*it)->id == id) {
            double total_value = ((*it)->quantity * (*it)->price);
            std::cout << (*it)->quantity << " " << (*it)->name << "(s) [id: " << id << "] total:" << total_value << " ~" << std::endl;
            return;
        }
    }
}

// search_name - find item by name or * using substr
void Inventory::search_name(std::string search_term) {
    display_filters({
        [search_term](Item* item) { return item->name.find(search_term) != std::string::npos; }
    });
};

// search_quantity - find item by specific quantity
void Inventory::search_quantity(int q) {
    display_filters({
        [q](Item* item) { return item->quantity == q; }
    });
}

// search_quantity -  find item >= and other inequality
void Inventory::search_quantity(std::string op, int q) {
    std::function<bool(Item*)> condition;
    // condition on inequality
    if (op == ">=") condition = [q](Item* item) {return item->quantity >= q; };
    else if (op == "<=") condition = [q](Item* item) {return item->quantity <= q; };
    else if (op == ">") condition = [q](Item* item) {return item->quantity > q; };
    else if (op == "<") condition = [q](Item* item) {return item->quantity < q; };
    else condition = [q](Item* item) {return item->quantity == q; };

    display_filters({condition});
}

// search_size
void Inventory::search_size(std::string s) {
    for (Item* item: items) {
        // dynamic recast
        Gear* gear = dynamic_cast<Gear*>(item);
        if (gear && gear->size == s) {
            item->display();
        }
    }
}

// search_brand
void Inventory::search_brand(std::string b) {
    for (Item* item: items) {
        // dynamic recast
        Gear* gear = dynamic_cast<Gear*>(item);
        if (gear && gear->brand == b) {
            item->display();
        }
    }
}

// search_weapon_type
void Inventory::search_weapon_type(std::string w) {
    for (Item* item: items) {
        // dynamic recast
        Weapon* weapon = dynamic_cast<Weapon*>(item);
        if (weapon && weapon->weapon_type == w) {
            item->display();
        }
    }
}

// search_grip
void Inventory::search_grip(std::string g) {
    for (Item* item: items) {
        // dynamic recast
        Weapon* weapon = dynamic_cast<Weapon*>(item);
        if (weapon && weapon->grip == g) {
            item->display();
        }
    }
};
