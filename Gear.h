#ifndef GEAR_H
#define GEAR_H

#include <string>
#include <iostream>
#include "Item.h"

//Gear - inherit from item
class Gear : public Item {
public:
    // additional characteritic - size, brand
    std::string size;
    // eg: all-star
    std::string brand;

    Gear(std::string n, int q, double p, std::string s, std::string b);

    // override
    void display() override;

    // getter methods
    std::string get_size();
    std::string get_brand();
};


#endif
