#ifndef MENU_H
#define MENU_H

#include "Inventory.h"

class Menu {
public:
    Inventory& inv;

    Menu(Inventory& inventory);
    void run();
    void showMenu();
    void clearScreen();
    void handleAdd();
    void handleRemove();
    void handleDisplayAll();
    void handleDisplayWeapons();
    void handleDisplayGear();
    void handleSearch();
    void handleUpdate();
};

#endif
