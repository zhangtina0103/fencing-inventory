#include "Weapon.h"


Weapon::Weapon(std::string n, int q, double p, std::string g, std::string w) : Item(n, q, p), grip(g), weapon_type(w) {}

// override
void Weapon::display() {
    std::cout << name << ": " << quantity << " (grip: " << grip << ", " << "weapon type: " << weapon_type << ")" << " [id: " << id << "]" << std::endl;
}

// getter methods
std::string Weapon::get_grip() { return grip; }
std::string Weapon::get_weapon_type() { return weapon_type; }
