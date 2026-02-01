#include "Database.h"
#include "Inventory.h"
#include "Menu.h"
#include "WebServer.h"

int main(int argc, char* argv[]) {
    Database db("inventory.db");
    db.createTables();

    Inventory inv(db);

    // If --web flag, start web server
    if (argc > 1 && std::string(argv[1]) == "--web") {
        WebServer server(inv);
        server.start(8080);
    } else {
        Menu menu(inv);
        menu.run();
    }

    return 0;
}
