#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "httplib.h"
#include "Inventory.h"
#include <sstream>

class WebServer {
public:
    Inventory& inv;
    httplib::Server server;

    WebServer(Inventory& inventory);
    void start(int port);
    std::string generateHTML();
    std::string generateJSON();
};

#endif
