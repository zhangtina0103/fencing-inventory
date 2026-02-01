#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include <iostream>
#include "Item.h"

//Weapon - inherit from Item
class Weapon : public Item {
public:
    // additional characteristics - grip, weapon type
    // eg: french, pistol
    std::string grip;
    // eg: epee, foil, saber
    std::string weapon_type;

    Weapon(std::string n, int q, double p, std::string g, std::string w);

    // override
    void display() override;

    // getter methods
    std::string get_grip();
    std::string get_weapon_type();
};

#endif
