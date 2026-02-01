#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>

//keep track of items - weapon, gear
// keep track of name, how many, price, id
class Item {
public:
    std::string name;
    int quantity;
    double price;
    // shared across all items
    static int next_id;
    int id;

    //just signatures, no body
    Item(std::string n, int q, double p);
    virtual void display();
    std::string get_name();
    int get_quantity();
    int get_id();
    double get_price();
};

#endif
