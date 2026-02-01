#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <map>
#include <sqlite3.h>
#include "Item.h"
#include "Weapon.h"
#include "Gear.h"
#include "Database.h"


//Inventory class
class Inventory {
private:
    void add(Item* item);
public:
    //hold pointers to Items
    std::vector<Item*> items;
    // reference to database
    Database& db;

    // constructor takes database
    Inventory(Database& database);

    // load items from database into vector
    void loadFromDatabase();

    void addWeapon(std::string name, int quantity, double price, std::string grip, std::string weapon_type);
    void addGear(std::string name, int quantity, double price, std::string size, std::string brand);
    void remove(int id);
    // display all inventory items
    void display_all();

    // display top n highest quantity items' specified by name, quantity, etc.
    // pass in string "name", "quantity", etc.
    void display_top_n(std::string attribute, int n, bool ascending = true);

    // display sorted items alphabetical by item name default ascending
    void display_alphabetical(bool ascending = true);
    // display sorted items by quantity
    void display_quantity(bool ascending = true);
    //display sorted items by price
    void display_price(bool ascending = true);
    // display sorted items by id
    void display_id(bool ascending = true);
    void display_filters(std::vector<std::function<bool(Item*)>> conditions);

    //update - modify existing item based on id
    void update_quantity(int id, int new_quantity);
    // update name
    void update_name(int id, std::string new_name);
    // update price
    void update_price(int id, double new_price);
    //update child class attributes by dynamically casting types
    void update_grip(int id, std::string new_grip);
    void update_weapon_type(int id, std::string new_weapon_type);
    void update_size(int id, std::string new_size);
    void update_brand(int id, std::string new_brand);

    //get_total_value = sum of price*quantity
    double get_total_value();

    // get total value of specific id
    void get_value(int id);

    // search_name - find item by name or * using substr
    void search_name(std::string search_term);
    // search_quantity - find item by specific quantity
    void search_quantity(int q);
    // search_quantity -  find item >= and other inequality
    void search_quantity(std::string op, int q);
    // search_size
    void search_size(std::string s);
    // search_brand
    void search_brand(std::string b);
    // search_weapon_type
    void search_weapon_type(std::string w);
    // search_grip
    void search_grip(std::string g);
};

#endif
