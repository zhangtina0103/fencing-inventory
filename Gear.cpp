#include "Gear.h"

Gear::Gear(std::string n, int q, double p, std::string s, std::string b) : Item(n, q, p), size(s), brand(b) {}

// override
void Gear::display() {
    std::cout << name << ": " << quantity << " (size: " << size << ", " << "brand: " << brand << ")" << " [id: " << id << "]" << std::endl;
}

// getter methods
std::string Gear::get_size() { return size; }

std::string Gear::get_brand() { return brand; }
