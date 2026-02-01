#include "Menu.h"

Menu::Menu(Inventory& inventory) : inv(inventory) {}

void Menu::clearScreen() {
    std::cout << "\033[2J\033[1;1H";  // ANSI escape code to clear
}

void Menu::run() {
    int choice = 0;

    while (choice != 8) {
        showMenu();
        std::cin >> choice;

        switch (choice) {
            case 1: handleAdd(); break;
            case 2: handleRemove(); break;
            case 3: handleDisplayAll(); break;
            case 4: handleDisplayWeapons(); break;
            case 5: handleDisplayGear(); break;
            case 6: handleSearch(); break;
            case 7: handleUpdate(); break;
            case 8: std::cout << "\n  👋 goodbye ~\n\n"; break;
            default: std::cout << "\n  ❌ invalid choice ~\n"; break;
        }

        if (choice != 8) {
            std::cout << "\n  Press Enter to continue...";
            std::cin.ignore();
            std::cin.get();
        }
    }
}

void Menu::showMenu() {
    clearScreen();
    std::cout << "\n";
    std::cout << "  ╔═══════════════════════════════════╗\n";
    std::cout << "  ║     🤺 FENCING INVENTORY 🤺        ║\n";
    std::cout << "  ╠═══════════════════════════════════╣\n";
    std::cout << "  ║  1. ➕ Add item                   ║\n";
    std::cout << "  ║  2. ➖ Remove item                ║\n";
    std::cout << "  ║  3. 📋 Display all                ║\n";
    std::cout << "  ║  4. ⚔️  Display weapons            ║\n";
    std::cout << "  ║  5. 🛡️  Display gear               ║\n";
    std::cout << "  ║  6. 🔍 Search                     ║\n";
    std::cout << "  ║  7. ✏️  Update item                ║\n";
    std::cout << "  ║  8. 🚪 Exit                       ║\n";
    std::cout << "  ╚═══════════════════════════════════╝\n";
    std::cout << "  Choose: ";
}

void Menu::handleAdd() {
    std::cout << "\n  ╔═══════════════════════════════════╗\n";
    std::cout << "  ║           ADD NEW ITEM            ║\n";
    std::cout << "  ╠═══════════════════════════════════╣\n";
    std::cout << "  ║  1. ⚔️  Weapon                     ║\n";
    std::cout << "  ║  2. 🛡️  Gear                       ║\n";
    std::cout << "  ╚═══════════════════════════════════╝\n";
    std::cout << "  Choose: ";

    int type;
    std::cin >> type;

    std::string name;
    int quantity;
    double price;

    std::cout << "\n  Name: ";
    std::cin >> name;
    std::cout << "  Quantity: ";
    std::cin >> quantity;
    std::cout << "  Price: $";
    std::cin >> price;

    if (type == 1) {
        std::string grip, weapon_type;
        std::cout << "  Grip (french/pistol): ";
        std::cin >> grip;
        std::cout << "  Type (epee/foil/sabre): ";
        std::cin >> weapon_type;
        inv.addWeapon(name, quantity, price, grip, weapon_type);
        std::cout << "\n  ✅ Weapon added!\n";
    } else {
        std::string size, brand;
        std::cout << "  Size: ";
        std::cin >> size;
        std::cout << "  Brand: ";
        std::cin >> brand;
        inv.addGear(name, quantity, price, size, brand);
        std::cout << "\n  ✅ Gear added!\n";
    }
}

void Menu::handleRemove() {
    std::cout << "\n  Enter ID to remove: ";
    int id;
    std::cin >> id;
    inv.remove(id);
}

void Menu::handleDisplayAll() {
    std::cout << "\n  ╔═══════════════════════════════════╗\n";
    std::cout << "  ║          ALL INVENTORY            ║\n";
    std::cout << "  ╚═══════════════════════════════════╝\n\n";
    inv.display_all();
}

void Menu::handleDisplayWeapons() {
    std::cout << "\n  ╔═══════════════════════════════════╗\n";
    std::cout << "  ║            WEAPONS ⚔️              ║\n";
    std::cout << "  ╚═══════════════════════════════════╝\n\n";
    inv.search_weapon_type("epee");
    inv.search_weapon_type("foil");
    inv.search_weapon_type("sabre");
}

void Menu::handleDisplayGear() {
    std::cout << "\n  ╔═══════════════════════════════════╗\n";
    std::cout << "  ║             GEAR 🛡️                ║\n";
    std::cout << "  ╚═══════════════════════════════════╝\n\n";
    inv.display_filters({
        [](Item* item) {
            return dynamic_cast<Gear*>(item) != nullptr;
        }
    });
}

void Menu::handleSearch() {
    std::cout << "\n  ╔═══════════════════════════════════╗\n";
    std::cout << "  ║             SEARCH 🔍             ║\n";
    std::cout << "  ╠═══════════════════════════════════╣\n";
    std::cout << "  ║  1. By name                       ║\n";
    std::cout << "  ║  2. By quantity                   ║\n";
    std::cout << "  ║  3. By price                      ║\n";
    std::cout << "  ╚═══════════════════════════════════╝\n";
    std::cout << "  Choose: ";

    int type;
    std::cin >> type;

    if (type == 1) {
        std::string name;
        std::cout << "  Name contains: ";
        std::cin >> name;
        inv.search_name(name);
    } else if (type == 2) {
        int q;
        std::cout << "  Quantity: ";
        std::cin >> q;
        inv.search_quantity(q);
    } else {
        std::cout << "  (Use display_filters for price)\n";
    }
}

void Menu::handleUpdate() {
    std::cout << "\n  ╔═══════════════════════════════════╗\n";
    std::cout << "  ║           UPDATE ITEM ✏️           ║\n";
    std::cout << "  ╠═══════════════════════════════════╣\n";
    std::cout << "  ║  1. Quantity                      ║\n";
    std::cout << "  ║  2. Name                          ║\n";
    std::cout << "  ║  3. Price                         ║\n";
    std::cout << "  ╚═══════════════════════════════════╝\n";
    std::cout << "  Choose: ";

    int type;
    std::cin >> type;

    std::cout << "  Item ID: ";
    int id;
    std::cin >> id;

    if (type == 1) {
        int q;
        std::cout << "  New quantity: ";
        std::cin >> q;
        inv.update_quantity(id, q);
    } else if (type == 2) {
        std::string name;
        std::cout << "  New name: ";
        std::cin >> name;
        inv.update_name(id, name);
    } else if (type == 3) {
        double p;
        std::cout << "  New price: $";
        std::cin >> p;
        inv.update_price(id, p);
    }
    std::cout << "\n  ✅ Updated!\n";
}
