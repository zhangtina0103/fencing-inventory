#include "Item.h"

//static initialization
int Item::next_id = 1;

//here we put implementation
//initialize
Item::Item(std::string n, int q, double p): name(n), quantity(q), price(p) {
    // increment id
    id = next_id;
    next_id++;
    std::cout << quantity << " " << name << "(s) created ~" << std::endl;
}

void Item::display() {
    std::cout << name << ": " << quantity << " [id: " << id << "]" << std::endl;
}

// getter methods
std::string Item::get_name() { return name; }
int Item::get_quantity() { return quantity; }
int Item::get_id() { return id; }
double Item::get_price() { return price; }
