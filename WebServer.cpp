#include "WebServer.h"

WebServer::WebServer(Inventory& inventory) : inv(inventory) {}

void WebServer::start(int port) {
    server.Get("/", [this](const httplib::Request&, httplib::Response& res) {
        res.set_content(generateHTML(), "text/html");
    });

    server.Get("/api/items", [this](const httplib::Request&, httplib::Response& res) {
        res.set_content(generateJSON(), "application/json");
    });

    server.Post("/api/weapon", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string name = req.get_param_value("name");
            int qty = std::stoi(req.get_param_value("quantity"));
            double price = std::stod(req.get_param_value("price"));
            std::string grip = req.get_param_value("grip");
            std::string wtype = req.get_param_value("weapon_type");
            std::cout << "Adding weapon: " << name << ", " << qty << ", " << price << ", " << grip << ", " << wtype << std::endl;
            inv.addWeapon(name, qty, price, grip, wtype);
            res.set_content("{\"status\":\"ok\"}", "application/json");
        } catch (const std::exception& e) {
            std::cerr << "Error adding weapon: " << e.what() << std::endl;
            res.status = 500;
            res.set_content("{\"error\":\"" + std::string(e.what()) + "\"}", "application/json");
        }
    });

    server.Post("/api/gear", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            std::string name = req.get_param_value("name");
            int qty = std::stoi(req.get_param_value("quantity"));
            double price = std::stod(req.get_param_value("price"));
            std::string size = req.get_param_value("size");
            std::string brand = req.get_param_value("brand");
            std::cout << "Adding gear: " << name << ", " << qty << ", " << price << ", " << size << ", " << brand << std::endl;
            inv.addGear(name, qty, price, size, brand);
            res.set_content("{\"status\":\"ok\"}", "application/json");
        } catch (const std::exception& e) {
            std::cerr << "Error adding gear: " << e.what() << std::endl;
            res.status = 500;
            res.set_content("{\"error\":\"" + std::string(e.what()) + "\"}", "application/json");
        }
    });

    server.Post("/api/remove", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            int id = std::stoi(req.get_param_value("id"));
            std::cout << "Removing item: " << id << std::endl;
            inv.remove(id);
            res.set_content("{\"status\":\"ok\"}", "application/json");
        } catch (const std::exception& e) {
            std::cerr << "Error removing: " << e.what() << std::endl;
            res.status = 500;
            res.set_content("{\"error\":\"" + std::string(e.what()) + "\"}", "application/json");
        }
    });

    server.Post("/api/update", [this](const httplib::Request& req, httplib::Response& res) {
        int id = std::stoi(req.get_param_value("id"));
        std::string field = req.get_param_value("field");
        std::string value = req.get_param_value("value");
        if (field == "quantity") inv.update_quantity(id, std::stoi(value));
        else if (field == "name") inv.update_name(id, value);
        else if (field == "price") inv.update_price(id, std::stod(value));
        else if (field == "grip") inv.update_grip(id, value);
        else if (field == "weapon_type") inv.update_weapon_type(id, value);
        else if (field == "size") inv.update_size(id, value);
        else if (field == "brand") inv.update_brand(id, value);
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    server.Get("/api/search", [this](const httplib::Request& req, httplib::Response& res) {
        std::string field = req.get_param_value("field");
        std::string value = req.get_param_value("value");
        std::string op = req.get_param_value("op");
        if (op.empty()) op = "eq";
        std::stringstream ss;
        ss << "{\"results\":[";
        bool first = true;
        for (Item* item : inv.items) {
            bool match = false;
            Weapon* w = dynamic_cast<Weapon*>(item);
            Gear* g = dynamic_cast<Gear*>(item);
            if (field == "name" && item->name.find(value) != std::string::npos) match = true;
            if (field == "grip" && w && w->grip.find(value) != std::string::npos) match = true;
            if (field == "weapon_type" && w && w->weapon_type.find(value) != std::string::npos) match = true;
            if (field == "size" && g && g->size.find(value) != std::string::npos) match = true;
            if (field == "brand" && g && g->brand.find(value) != std::string::npos) match = true;
            if (field == "quantity" || field == "price" || field == "id") {
                double itemVal = (field == "quantity") ? item->quantity : (field == "price") ? item->price : item->id;
                double searchVal = std::stod(value);
                if (op == "eq" && itemVal == searchVal) match = true;
                else if (op == "gt" && itemVal > searchVal) match = true;
                else if (op == "lt" && itemVal < searchVal) match = true;
                else if (op == "gte" && itemVal >= searchVal) match = true;
                else if (op == "lte" && itemVal <= searchVal) match = true;
            }
            if (match) {
                if (!first) ss << ",";
                ss << "{\"id\":" << item->id << ",\"name\":\"" << item->name << "\",\"quantity\":" << item->quantity << ",\"price\":" << item->price << ",\"value\":" << (item->price * item->quantity);
                if (w) ss << ",\"type\":\"weapon\",\"grip\":\"" << w->grip << "\",\"weapon_type\":\"" << w->weapon_type << "\"";
                else if (g) ss << ",\"type\":\"gear\",\"size\":\"" << g->size << "\",\"brand\":\"" << g->brand << "\"";
                ss << "}";
                first = false;
            }
        }
        ss << "]}";
        res.set_content(ss.str(), "application/json");
    });

    server.Get("/api/sorted", [this](const httplib::Request& req, httplib::Response& res) {
        std::string field = req.get_param_value("field");
        std::string order = req.get_param_value("order");
        std::string limitStr = req.get_param_value("limit");
        bool asc = (order != "desc");
        int limit = limitStr.empty() ? 9999 : std::stoi(limitStr);
        std::vector<Item*> sorted = inv.items;
        if (field == "name") std::sort(sorted.begin(), sorted.end(), [asc](Item* a, Item* b) { return asc ? a->name < b->name : a->name > b->name; });
        else if (field == "quantity") std::sort(sorted.begin(), sorted.end(), [asc](Item* a, Item* b) { return asc ? a->quantity < b->quantity : a->quantity > b->quantity; });
        else if (field == "price") std::sort(sorted.begin(), sorted.end(), [asc](Item* a, Item* b) { return asc ? a->price < b->price : a->price > b->price; });
        else if (field == "id") std::sort(sorted.begin(), sorted.end(), [asc](Item* a, Item* b) { return asc ? a->id < b->id : a->id > b->id; });
        else if (field == "value") std::sort(sorted.begin(), sorted.end(), [asc](Item* a, Item* b) { return asc ? (a->price*a->quantity) < (b->price*b->quantity) : (a->price*a->quantity) > (b->price*b->quantity); });
        std::stringstream ss;
        ss << "{\"items\":[";
        bool first = true;
        int count = 0;
        for (Item* item : sorted) {
            if (count >= limit) break;
            if (!first) ss << ",";
            Weapon* w = dynamic_cast<Weapon*>(item);
            Gear* g = dynamic_cast<Gear*>(item);
            ss << "{\"id\":" << item->id << ",\"name\":\"" << item->name << "\",\"quantity\":" << item->quantity << ",\"price\":" << item->price << ",\"value\":" << (item->price * item->quantity);
            if (w) ss << ",\"type\":\"weapon\",\"grip\":\"" << w->grip << "\",\"weapon_type\":\"" << w->weapon_type << "\"";
            else if (g) ss << ",\"type\":\"gear\",\"size\":\"" << g->size << "\",\"brand\":\"" << g->brand << "\"";
            ss << "}";
            first = false;
            count++;
        }
        ss << "]}";
        res.set_content(ss.str(), "application/json");
    });

    server.Get("/api/filter", [this](const httplib::Request& req, httplib::Response& res) {
        std::string type = req.get_param_value("type");
        std::string minPrice = req.get_param_value("minPrice");
        std::string maxPrice = req.get_param_value("maxPrice");
        std::string minQty = req.get_param_value("minQty");
        std::stringstream ss;
        ss << "{\"items\":[";
        bool first = true;
        for (Item* item : inv.items) {
            Weapon* w = dynamic_cast<Weapon*>(item);
            Gear* g = dynamic_cast<Gear*>(item);
            bool pass = true;
            if (!type.empty() && type == "weapon" && !w) pass = false;
            if (!type.empty() && type == "gear" && !g) pass = false;
            if (!minPrice.empty() && item->price < std::stod(minPrice)) pass = false;
            if (!maxPrice.empty() && item->price > std::stod(maxPrice)) pass = false;
            if (!minQty.empty() && item->quantity < std::stoi(minQty)) pass = false;
            if (pass) {
                if (!first) ss << ",";
                ss << "{\"id\":" << item->id << ",\"name\":\"" << item->name << "\",\"quantity\":" << item->quantity << ",\"price\":" << item->price << ",\"value\":" << (item->price * item->quantity);
                if (w) ss << ",\"type\":\"weapon\",\"grip\":\"" << w->grip << "\",\"weapon_type\":\"" << w->weapon_type << "\"";
                else if (g) ss << ",\"type\":\"gear\",\"size\":\"" << g->size << "\",\"brand\":\"" << g->brand << "\"";
                ss << "}";
                first = false;
            }
        }
        ss << "]}";
        res.set_content(ss.str(), "application/json");
    });

    std::cout << "\n  Server running at http://localhost:" << port << "\n";
    std::cout << "  Press Ctrl+C to stop\n\n";
    server.listen("0.0.0.0", port);
}

std::string WebServer::generateJSON() {
    std::stringstream ss;
    ss << "{\"weapons\":[";
    bool first = true;
    for (Item* item : inv.items) {
        Weapon* w = dynamic_cast<Weapon*>(item);
        if (w) {
            if (!first) ss << ",";
            ss << "{\"id\":" << w->id << ",\"name\":\"" << w->name << "\",\"quantity\":" << w->quantity << ",\"price\":" << w->price << ",\"value\":" << (w->price * w->quantity) << ",\"grip\":\"" << w->grip << "\",\"weapon_type\":\"" << w->weapon_type << "\"}";
            first = false;
        }
    }
    ss << "],\"gear\":[";
    first = true;
    for (Item* item : inv.items) {
        Gear* g = dynamic_cast<Gear*>(item);
        if (g) {
            if (!first) ss << ",";
            ss << "{\"id\":" << g->id << ",\"name\":\"" << g->name << "\",\"quantity\":" << g->quantity << ",\"price\":" << g->price << ",\"value\":" << (g->price * g->quantity) << ",\"size\":\"" << g->size << "\",\"brand\":\"" << g->brand << "\"}";
            first = false;
        }
    }
    ss << "]}";
    return ss.str();
}

std::string WebServer::generateHTML() {
    std::string h;
    h += "<!DOCTYPE html><html><head><title>FENCING INVENTORY</title><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">";
    h += "<link href=\"https://fonts.googleapis.com/css2?family=Oswald:wght@400;500;600;700&family=Roboto:wght@300;400;500&display=swap\" rel=\"stylesheet\">";
    h += "<style>";
    h += "*{margin:0;padding:0;box-sizing:border-box}";
    h += "body{font-family:'Roboto',sans-serif;background:#f5f5f5;color:#333;min-height:100vh}";
    h += ".header{background:linear-gradient(135deg,#2c2c2c 0%,#4a4a4a 50%,#2c2c2c 100%);padding:35px 30px;text-align:center;border-bottom:5px solid #c41e3a}";
    h += "h1{font-family:'Oswald',sans-serif;font-size:3.5em;font-weight:700;text-transform:uppercase;letter-spacing:8px;color:#fff;text-shadow:2px 2px 0 #c41e3a}";
    h += ".tagline{font-family:'Oswald',sans-serif;font-size:1.1em;text-transform:uppercase;letter-spacing:4px;color:#ccc;margin-top:8px}";
    h += ".container{max-width:1500px;margin:0 auto;padding:25px}";
    h += ".stats-bar{display:flex;justify-content:center;gap:25px;margin:-25px auto 25px;padding:20px 35px;background:#fff;border-radius:0 0 15px 15px;box-shadow:0 5px 25px rgba(0,0,0,0.1);flex-wrap:wrap;max-width:800px}";
    h += ".stat{text-align:center;padding:0 15px}";
    h += ".stat-value{font-family:'Oswald',sans-serif;font-size:2.5em;font-weight:700;color:#2c2c2c}";
    h += ".stat-value.highlight{color:#c41e3a}";
    h += ".stat-label{font-size:0.75em;color:#888;text-transform:uppercase;letter-spacing:2px;margin-top:3px}";
    h += ".tabs{display:flex;gap:8px;margin-bottom:25px;flex-wrap:wrap;justify-content:center}";
    h += ".tab{font-family:'Oswald',sans-serif;padding:14px 28px;background:#fff;border:2px solid #ddd;color:#666;cursor:pointer;font-size:1em;font-weight:500;text-transform:uppercase;letter-spacing:2px;transition:all 0.3s;border-radius:5px}";
    h += ".tab:hover{border-color:#c41e3a;color:#c41e3a}";
    h += ".tab.active{background:#c41e3a;border-color:#c41e3a;color:#fff}";
    h += ".panel{display:none}";
    h += ".panel.active{display:block}";
    h += ".section{background:#fff;border:1px solid #e0e0e0;border-radius:12px;padding:25px;margin-bottom:20px;box-shadow:0 3px 15px rgba(0,0,0,0.05)}";
    h += ".section h2{font-family:'Oswald',sans-serif;font-size:1.6em;font-weight:600;text-transform:uppercase;letter-spacing:3px;color:#2c2c2c;margin-bottom:20px;padding-bottom:12px;border-bottom:3px solid #c41e3a;display:inline-block}";
    h += "table{width:100%;border-collapse:collapse;margin-top:15px}";
    h += "th{font-family:'Oswald',sans-serif;padding:15px 12px;text-align:left;background:#c41e3a;color:#fff;font-weight:500;text-transform:uppercase;letter-spacing:1px;font-size:0.85em}";
    h += "th:first-child{border-radius:8px 0 0 8px}th:last-child{border-radius:0 8px 8px 0}";
    h += "td{padding:15px 12px;border-bottom:1px solid #eee;font-size:0.95em}";
    h += "tr:hover{background:#fafafa}";
    h += ".btn{font-family:'Oswald',sans-serif;padding:10px 20px;border:none;cursor:pointer;font-size:0.9em;font-weight:500;text-transform:uppercase;letter-spacing:1px;transition:all 0.3s;border-radius:5px}";
    h += ".btn-primary{background:#c41e3a;color:#fff}";
    h += ".btn-primary:hover{background:#a01830;transform:translateY(-2px);box-shadow:0 5px 15px rgba(196,30,58,0.3)}";
    h += ".btn-danger{background:#fff;color:#c41e3a;border:2px solid #c41e3a}";
    h += ".btn-danger:hover{background:#c41e3a;color:#fff}";
    h += ".btn-secondary{background:#f0f0f0;color:#555;border:1px solid #ddd}";
    h += ".btn-secondary:hover{background:#e0e0e0}";
    h += ".btn-edit{background:#fff;color:#4a90d9;border:1px solid #4a90d9;padding:8px 14px;font-size:0.8em}";
    h += ".btn-edit:hover{background:#4a90d9;color:#fff}";
    h += ".form-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:18px;margin-bottom:20px}";
    h += ".form-group{display:flex;flex-direction:column;gap:6px}";
    h += ".form-group label{font-family:'Oswald',sans-serif;font-size:0.85em;color:#666;text-transform:uppercase;letter-spacing:1px}";
    h += "input,select{padding:14px 16px;border:2px solid #ddd;background:#fff;color:#333;font-size:1em;border-radius:5px}";
    h += "input:focus,select:focus{outline:none;border-color:#c41e3a}";
    h += ".price{color:#2e8b57;font-weight:600;font-family:'Oswald',sans-serif;font-size:1.05em}";
    h += ".quantity{color:#c41e3a;font-weight:600;font-family:'Oswald',sans-serif;background:rgba(196,30,58,0.1);padding:4px 12px;border-radius:4px;display:inline-block}";
    h += ".item-value{color:#4a90d9;font-family:'Oswald',sans-serif;font-weight:500}";
    h += ".type-badge{font-family:'Oswald',sans-serif;padding:5px 12px;font-size:0.8em;font-weight:500;text-transform:uppercase;letter-spacing:1px;border-radius:4px}";
    h += ".type-weapon{background:rgba(196,30,58,0.15);color:#c41e3a}";
    h += ".type-gear{background:rgba(74,144,217,0.15);color:#4a90d9}";
    h += ".controls{display:flex;gap:8px;margin-bottom:18px;flex-wrap:wrap;align-items:center}";
    h += ".controls span{font-family:'Oswald',sans-serif;color:#666;text-transform:uppercase;letter-spacing:1px;font-size:0.85em;margin-right:5px}";
    h += ".search-bar{display:flex;gap:12px;margin-bottom:20px;flex-wrap:wrap}";
    h += ".search-bar input{flex:1;min-width:180px}";
    h += ".filter-row{display:flex;gap:12px;margin-bottom:18px;flex-wrap:wrap;align-items:flex-end}";
    h += ".toast{position:fixed;bottom:25px;right:25px;padding:16px 30px;background:#c41e3a;color:#fff;font-family:'Oswald',sans-serif;font-size:1em;text-transform:uppercase;letter-spacing:2px;display:none;z-index:1000;border-radius:5px;box-shadow:0 5px 25px rgba(196,30,58,0.4)}";
    h += ".modal{display:none;position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,0.6);z-index:1000;align-items:center;justify-content:center}";
    h += ".modal.active{display:flex}";
    h += ".modal-content{background:#fff;padding:35px;border-radius:12px;max-width:450px;width:90%;border-top:5px solid #c41e3a}";
    h += ".modal h3{font-family:'Oswald',sans-serif;font-size:1.4em;text-transform:uppercase;letter-spacing:2px;margin-bottom:20px;color:#2c2c2c}";
    h += ".modal-buttons{display:flex;gap:12px;margin-top:20px}";
    h += ".empty{text-align:center;padding:50px 20px;color:#999}";
    h += "</style></head><body>";

    h += "<div class=\"header\"><h1>Fencing Inventory</h1><p class=\"tagline\">Equipment Management System</p></div>";
    h += "<div class=\"stats-bar\">";
    h += "<div class=\"stat\"><div class=\"stat-value\" id=\"totalItems\">0</div><div class=\"stat-label\">Total Items</div></div>";
    h += "<div class=\"stat\"><div class=\"stat-value\" id=\"totalWeapons\">0</div><div class=\"stat-label\">Weapons</div></div>";
    h += "<div class=\"stat\"><div class=\"stat-value\" id=\"totalGear\">0</div><div class=\"stat-label\">Gear</div></div>";
    h += "<div class=\"stat\"><div class=\"stat-value highlight\" id=\"totalValue\">$0</div><div class=\"stat-label\">Total Value</div></div>";
    h += "</div>";
    h += "<div class=\"container\">";
    h += "<div class=\"tabs\">";
    h += "<button class=\"tab active\" onclick=\"showPanel('inventory',this)\">Inventory</button>";
    h += "<button class=\"tab\" onclick=\"showPanel('weapons',this)\">Weapons</button>";
    h += "<button class=\"tab\" onclick=\"showPanel('gear',this)\">Gear</button>";
    h += "<button class=\"tab\" onclick=\"showPanel('add',this)\">+ Add</button>";
    h += "<button class=\"tab\" onclick=\"showPanel('search',this)\">Search</button>";
    h += "<button class=\"tab\" onclick=\"showPanel('filter',this)\">Filter</button>";
    h += "</div>";

    // Inventory panel
    h += "<div id=\"inventory\" class=\"panel active\"><div class=\"section\"><h2>All Equipment</h2>";
    h += "<div class=\"controls\"><span>Sort:</span>";
    h += "<button class=\"btn btn-secondary\" onclick=\"sortBy('name','asc')\">Name A-Z</button>";
    h += "<button class=\"btn btn-secondary\" onclick=\"sortBy('name','desc')\">Name Z-A</button>";
    h += "<button class=\"btn btn-secondary\" onclick=\"sortBy('price','desc')\">Price High</button>";
    h += "<button class=\"btn btn-secondary\" onclick=\"sortBy('price','asc')\">Price Low</button>";
    h += "<button class=\"btn btn-secondary\" onclick=\"sortBy('quantity','desc')\">Qty High</button>";
    h += "<button class=\"btn btn-secondary\" onclick=\"sortBy('value','desc')\">Value High</button></div>";
    h += "<div class=\"controls\"><span>Top N:</span>";
    h += "<button class=\"btn btn-secondary\" onclick=\"topN(3)\">Top 3</button>";
    h += "<button class=\"btn btn-secondary\" onclick=\"topN(5)\">Top 5</button>";
    h += "<button class=\"btn btn-secondary\" onclick=\"topN(10)\">Top 10</button>";
    h += "<button class=\"btn btn-secondary\" onclick=\"loadData()\">Show All</button></div>";
    h += "<table><thead><tr><th>ID</th><th>Type</th><th>Name</th><th>Qty</th><th>Price</th><th>Value</th><th>Details</th><th>Actions</th></tr></thead><tbody id=\"allTable\"></tbody></table></div></div>";

    // Weapons panel
    h += "<div id=\"weapons\" class=\"panel\"><div class=\"section\"><h2>Weapons</h2>";
    h += "<table><thead><tr><th>ID</th><th>Name</th><th>Qty</th><th>Price</th><th>Value</th><th>Grip</th><th>Type</th><th>Actions</th></tr></thead><tbody id=\"weaponsTable\"></tbody></table></div></div>";

    // Gear panel
    h += "<div id=\"gear\" class=\"panel\"><div class=\"section\"><h2>Protective Gear</h2>";
    h += "<table><thead><tr><th>ID</th><th>Name</th><th>Qty</th><th>Price</th><th>Value</th><th>Size</th><th>Brand</th><th>Actions</th></tr></thead><tbody id=\"gearTable\"></tbody></table></div></div>";

    // Add panel - FIXED with simpler IDs
    h += "<div id=\"add\" class=\"panel\"><div class=\"section\"><h2>Add Equipment</h2>";
    h += "<div class=\"form-group\" style=\"max-width:280px;margin-bottom:20px\"><label>Equipment Type</label><select id=\"addType\"><option value=\"weapon\">Weapon</option><option value=\"gear\">Gear</option></select></div>";
    h += "<div class=\"form-grid\">";
    h += "<div class=\"form-group\"><label>Name</label><input type=\"text\" id=\"addName\" placeholder=\"e.g., Competition Epee\"></div>";
    h += "<div class=\"form-group\"><label>Quantity</label><input type=\"number\" id=\"addQty\" placeholder=\"0\" min=\"0\"></div>";
    h += "<div class=\"form-group\"><label>Price ($)</label><input type=\"number\" id=\"addPrice\" step=\"0.01\" placeholder=\"0.00\" min=\"0\"></div></div>";
    h += "<div id=\"weaponFields\" class=\"form-grid\">";
    h += "<div class=\"form-group\"><label>Grip Style</label><select id=\"addGrip\"><option value=\"french\">French</option><option value=\"pistol\">Pistol</option><option value=\"belgian\">Belgian</option></select></div>";
    h += "<div class=\"form-group\"><label>Weapon Type</label><select id=\"addWtype\"><option value=\"epee\">Epee</option><option value=\"foil\">Foil</option><option value=\"sabre\">Sabre</option></select></div></div>";
    h += "<div id=\"gearFields\" class=\"form-grid\" style=\"display:none\">";
    h += "<div class=\"form-group\"><label>Size</label><input type=\"text\" id=\"addSize\" placeholder=\"e.g., Medium, 40\"></div>";
    h += "<div class=\"form-group\"><label>Brand</label><input type=\"text\" id=\"addBrand\" placeholder=\"e.g., Uhlmann, LP\"></div></div>";
    h += "<button class=\"btn btn-primary\" onclick=\"addItem()\" style=\"margin-top:12px;padding:16px 45px;font-size:1.05em\">Add to Inventory</button></div></div>";

    // Search panel
    h += "<div id=\"search\" class=\"panel\"><div class=\"section\"><h2>Search Equipment</h2>";
    h += "<div class=\"search-bar\">";
    h += "<select id=\"sField\"><option value=\"name\">Name</option><option value=\"quantity\">Quantity</option><option value=\"price\">Price</option><option value=\"grip\">Grip</option><option value=\"weapon_type\">Weapon Type</option><option value=\"size\">Size</option><option value=\"brand\">Brand</option></select>";
    h += "<select id=\"sOp\"><option value=\"eq\">Equals/Contains</option><option value=\"gt\">Greater Than</option><option value=\"lt\">Less Than</option><option value=\"gte\">Greater or Equal</option><option value=\"lte\">Less or Equal</option></select>";
    h += "<input type=\"text\" id=\"sVal\" placeholder=\"Search value...\">";
    h += "<button class=\"btn btn-primary\" onclick=\"doSearch()\">Search</button></div>";
    h += "<table><thead><tr><th>ID</th><th>Type</th><th>Name</th><th>Qty</th><th>Price</th><th>Value</th><th>Details</th></tr></thead><tbody id=\"searchResults\"></tbody></table></div></div>";

    // Filter panel
    h += "<div id=\"filter\" class=\"panel\"><div class=\"section\"><h2>Filter Equipment</h2>";
    h += "<div class=\"filter-row\">";
    h += "<div class=\"form-group\"><label>Type</label><select id=\"fType\"><option value=\"\">All</option><option value=\"weapon\">Weapons Only</option><option value=\"gear\">Gear Only</option></select></div>";
    h += "<div class=\"form-group\"><label>Min Price</label><input type=\"number\" id=\"fMinP\" placeholder=\"0\"></div>";
    h += "<div class=\"form-group\"><label>Max Price</label><input type=\"number\" id=\"fMaxP\" placeholder=\"Any\"></div>";
    h += "<div class=\"form-group\"><label>Min Qty</label><input type=\"number\" id=\"fMinQ\" placeholder=\"0\"></div>";
    h += "<button class=\"btn btn-primary\" onclick=\"doFilter()\" style=\"height:50px;margin-top:auto\">Apply</button></div>";
    h += "<table><thead><tr><th>ID</th><th>Type</th><th>Name</th><th>Qty</th><th>Price</th><th>Value</th><th>Details</th></tr></thead><tbody id=\"filterResults\"></tbody></table></div></div>";
    h += "</div>";

    // Edit Modal
    h += "<div class=\"modal\" id=\"editModal\"><div class=\"modal-content\"><h3>Edit Item</h3>";
    h += "<input type=\"hidden\" id=\"editId\">";
    h += "<div class=\"form-group\"><label>Field</label><select id=\"editField\"><option value=\"name\">Name</option><option value=\"quantity\">Quantity</option><option value=\"price\">Price</option><option value=\"grip\">Grip</option><option value=\"weapon_type\">Weapon Type</option><option value=\"size\">Size</option><option value=\"brand\">Brand</option></select></div>";
    h += "<div class=\"form-group\" style=\"margin-top:12px\"><label>New Value</label><input type=\"text\" id=\"editValue\" placeholder=\"Enter new value\"></div>";
    h += "<div class=\"modal-buttons\"><button class=\"btn btn-primary\" onclick=\"saveEdit()\">Save</button><button class=\"btn btn-secondary\" onclick=\"closeModal()\">Cancel</button></div></div></div>";
    h += "<div class=\"toast\" id=\"toast\">Success!</div>";

    // JavaScript - completely rewritten for clarity
    h += "<script>\n";
    h += "function loadData() {\n";
    h += "  fetch('/api/items')\n";
    h += "    .then(function(response) { return response.json(); })\n";
    h += "    .then(function(data) {\n";
    h += "      renderTables(data);\n";
    h += "      updateStats(data);\n";
    h += "    })\n";
    h += "    .catch(function(err) { console.log('Error:', err); });\n";
    h += "}\n\n";

    h += "function renderTables(data) {\n";
    h += "  var allHtml = '';\n";
    h += "  var weaponsHtml = '';\n";
    h += "  var gearHtml = '';\n";
    h += "  for (var i = 0; i < data.weapons.length; i++) {\n";
    h += "    var w = data.weapons[i];\n";
    h += "    var val = w.value.toFixed(2);\n";
    h += "    allHtml += '<tr><td>' + w.id + '</td><td><span class=\"type-badge type-weapon\">Weapon</span></td><td>' + w.name + '</td><td><span class=\"quantity\">' + w.quantity + '</span></td><td class=\"price\">$' + w.price.toFixed(2) + '</td><td class=\"item-value\">$' + val + '</td><td>' + w.grip + ' / ' + w.weapon_type + '</td><td><button class=\"btn btn-edit\" onclick=\"openEdit(' + w.id + ')\">Edit</button> <button class=\"btn btn-danger\" onclick=\"delItem(' + w.id + ')\">Delete</button></td></tr>';\n";
    h += "    weaponsHtml += '<tr><td>' + w.id + '</td><td>' + w.name + '</td><td><span class=\"quantity\">' + w.quantity + '</span></td><td class=\"price\">$' + w.price.toFixed(2) + '</td><td class=\"item-value\">$' + val + '</td><td>' + w.grip + '</td><td>' + w.weapon_type + '</td><td><button class=\"btn btn-edit\" onclick=\"openEdit(' + w.id + ')\">Edit</button> <button class=\"btn btn-danger\" onclick=\"delItem(' + w.id + ')\">Delete</button></td></tr>';\n";
    h += "  }\n";
    h += "  for (var i = 0; i < data.gear.length; i++) {\n";
    h += "    var g = data.gear[i];\n";
    h += "    var val = g.value.toFixed(2);\n";
    h += "    allHtml += '<tr><td>' + g.id + '</td><td><span class=\"type-badge type-gear\">Gear</span></td><td>' + g.name + '</td><td><span class=\"quantity\">' + g.quantity + '</span></td><td class=\"price\">$' + g.price.toFixed(2) + '</td><td class=\"item-value\">$' + val + '</td><td>' + g.size + ' / ' + g.brand + '</td><td><button class=\"btn btn-edit\" onclick=\"openEdit(' + g.id + ')\">Edit</button> <button class=\"btn btn-danger\" onclick=\"delItem(' + g.id + ')\">Delete</button></td></tr>';\n";
    h += "    gearHtml += '<tr><td>' + g.id + '</td><td>' + g.name + '</td><td><span class=\"quantity\">' + g.quantity + '</span></td><td class=\"price\">$' + g.price.toFixed(2) + '</td><td class=\"item-value\">$' + val + '</td><td>' + g.size + '</td><td>' + g.brand + '</td><td><button class=\"btn btn-edit\" onclick=\"openEdit(' + g.id + ')\">Edit</button> <button class=\"btn btn-danger\" onclick=\"delItem(' + g.id + ')\">Delete</button></td></tr>';\n";
    h += "  }\n";
    h += "  document.getElementById('allTable').innerHTML = allHtml || '<tr><td colspan=\"8\" class=\"empty\">No items yet</td></tr>';\n";
    h += "  document.getElementById('weaponsTable').innerHTML = weaponsHtml || '<tr><td colspan=\"8\" class=\"empty\">No weapons</td></tr>';\n";
    h += "  document.getElementById('gearTable').innerHTML = gearHtml || '<tr><td colspan=\"8\" class=\"empty\">No gear</td></tr>';\n";
    h += "}\n\n";

    h += "function updateStats(data) {\n";
    h += "  document.getElementById('totalItems').textContent = data.weapons.length + data.gear.length;\n";
    h += "  document.getElementById('totalWeapons').textContent = data.weapons.length;\n";
    h += "  document.getElementById('totalGear').textContent = data.gear.length;\n";
    h += "  var total = 0;\n";
    h += "  for (var i = 0; i < data.weapons.length; i++) total += data.weapons[i].value;\n";
    h += "  for (var i = 0; i < data.gear.length; i++) total += data.gear[i].value;\n";
    h += "  document.getElementById('totalValue').textContent = '$' + total.toFixed(2);\n";
    h += "}\n\n";

    h += "function showPanel(panelId, btn) {\n";
    h += "  var tabs = document.querySelectorAll('.tab');\n";
    h += "  var panels = document.querySelectorAll('.panel');\n";
    h += "  for (var i = 0; i < tabs.length; i++) tabs[i].classList.remove('active');\n";
    h += "  for (var i = 0; i < panels.length; i++) panels[i].classList.remove('active');\n";
    h += "  btn.classList.add('active');\n";
    h += "  document.getElementById(panelId).classList.add('active');\n";
    h += "}\n\n";

    h += "function toggleFields() {\n";
    h += "  var type = document.getElementById('addType').value;\n";
    h += "  document.getElementById('weaponFields').style.display = (type === 'weapon') ? 'grid' : 'none';\n";
    h += "  document.getElementById('gearFields').style.display = (type === 'gear') ? 'grid' : 'none';\n";
    h += "}\n";
    h += "document.getElementById('addType').addEventListener('change', toggleFields);\n\n";

    h += "function addItem() {\n";
    h += "  var type = document.getElementById('addType').value;\n";
    h += "  var name = document.getElementById('addName').value;\n";
    h += "  var qty = document.getElementById('addQty').value;\n";
    h += "  var price = document.getElementById('addPrice').value;\n";
    h += "  console.log('Adding item:', type, name, qty, price);\n";
    h += "  if (!name || !qty || !price) {\n";
    h += "    alert('Please fill all required fields');\n";
    h += "    return;\n";
    h += "  }\n";
    h += "  var params = 'name=' + encodeURIComponent(name) + '&quantity=' + qty + '&price=' + price;\n";
    h += "  if (type === 'weapon') {\n";
    h += "    params += '&grip=' + encodeURIComponent(document.getElementById('addGrip').value);\n";
    h += "    params += '&weapon_type=' + encodeURIComponent(document.getElementById('addWtype').value);\n";
    h += "  } else {\n";
    h += "    params += '&size=' + encodeURIComponent(document.getElementById('addSize').value);\n";
    h += "    params += '&brand=' + encodeURIComponent(document.getElementById('addBrand').value);\n";
    h += "  }\n";
    h += "  fetch('/api/' + type, {\n";
    h += "    method: 'POST',\n";
    h += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },\n";
    h += "    body: params\n";
    h += "  })\n";
    h += "    .then(function(response) {\n";
    h += "      console.log('Response status:', response.status);\n";
    h += "      return response.text();\n";
    h += "    })\n";
    h += "    .then(function(text) {\n";
    h += "      console.log('Response text:', text);\n";
    h += "      document.getElementById('addName').value = '';\n";
    h += "      document.getElementById('addQty').value = '';\n";
    h += "      document.getElementById('addPrice').value = '';\n";
    h += "      loadData();\n";
    h += "      showToast('Item Added!');\n";
    h += "    })\n";
    h += "    .catch(function(err) { console.log('Add error:', err); });\n";
    h += "}\n\n";

    h += "function delItem(id) {\n";
    h += "  if (confirm('Delete this item?')) {\n";
    h += "    fetch('/api/remove', {\n";
    h += "      method: 'POST',\n";
    h += "      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },\n";
    h += "      body: 'id=' + id\n";
    h += "    })\n";
    h += "      .then(function(response) { return response.text(); })\n";
    h += "      .then(function(text) {\n";
    h += "        console.log('Delete result:', text);\n";
    h += "        loadData();\n";
    h += "        showToast('Deleted!');\n";
    h += "      })\n";
    h += "      .catch(function(err) { console.log('Delete error:', err); });\n";
    h += "  }\n";
    h += "}\n\n";

    h += "function sortBy(field, order) {\n";
    h += "  fetch('/api/sorted?field=' + field + '&order=' + order)\n";
    h += "    .then(function(response) { return response.json(); })\n";
    h += "    .then(function(data) { renderSortedItems(data.items); })\n";
    h += "    .catch(function(err) { console.log('Sort error:', err); });\n";
    h += "}\n\n";

    h += "function topN(n) {\n";
    h += "  fetch('/api/sorted?field=value&order=desc&limit=' + n)\n";
    h += "    .then(function(response) { return response.json(); })\n";
    h += "    .then(function(data) { renderSortedItems(data.items); })\n";
    h += "    .catch(function(err) { console.log('TopN error:', err); });\n";
    h += "}\n\n";

    h += "function renderSortedItems(items) {\n";
    h += "  var html = '';\n";
    h += "  for (var i = 0; i < items.length; i++) {\n";
    h += "    var it = items[i];\n";
    h += "    var typeClass = (it.type === 'weapon') ? 'type-weapon' : 'type-gear';\n";
    h += "    var details = (it.type === 'weapon') ? (it.grip + ' / ' + it.weapon_type) : (it.size + ' / ' + it.brand);\n";
    h += "    html += '<tr><td>' + it.id + '</td><td><span class=\"type-badge ' + typeClass + '\">' + it.type + '</span></td><td>' + it.name + '</td><td><span class=\"quantity\">' + it.quantity + '</span></td><td class=\"price\">$' + it.price.toFixed(2) + '</td><td class=\"item-value\">$' + it.value.toFixed(2) + '</td><td>' + details + '</td><td><button class=\"btn btn-edit\" onclick=\"openEdit(' + it.id + ')\">Edit</button> <button class=\"btn btn-danger\" onclick=\"delItem(' + it.id + ')\">Delete</button></td></tr>';\n";
    h += "  }\n";
    h += "  document.getElementById('allTable').innerHTML = html || '<tr><td colspan=\"8\" class=\"empty\">No results</td></tr>';\n";
    h += "}\n\n";

    h += "function doSearch() {\n";
    h += "  var field = document.getElementById('sField').value;\n";
    h += "  var op = document.getElementById('sOp').value;\n";
    h += "  var val = document.getElementById('sVal').value;\n";
    h += "  fetch('/api/search?field=' + field + '&op=' + op + '&value=' + encodeURIComponent(val))\n";
    h += "    .then(function(response) { return response.json(); })\n";
    h += "    .then(function(data) {\n";
    h += "      var html = '';\n";
    h += "      for (var i = 0; i < data.results.length; i++) {\n";
    h += "        var it = data.results[i];\n";
    h += "        var typeClass = (it.type === 'weapon') ? 'type-weapon' : 'type-gear';\n";
    h += "        var details = (it.type === 'weapon') ? (it.grip + ' / ' + it.weapon_type) : (it.size + ' / ' + it.brand);\n";
    h += "        html += '<tr><td>' + it.id + '</td><td><span class=\"type-badge ' + typeClass + '\">' + it.type + '</span></td><td>' + it.name + '</td><td><span class=\"quantity\">' + it.quantity + '</span></td><td class=\"price\">$' + it.price.toFixed(2) + '</td><td class=\"item-value\">$' + it.value.toFixed(2) + '</td><td>' + details + '</td></tr>';\n";
    h += "      }\n";
    h += "      document.getElementById('searchResults').innerHTML = html || '<tr><td colspan=\"7\" class=\"empty\">No results</td></tr>';\n";
    h += "    })\n";
    h += "    .catch(function(err) { console.log('Search error:', err); });\n";
    h += "}\n\n";

    h += "function doFilter() {\n";
    h += "  var type = document.getElementById('fType').value;\n";
    h += "  var minP = document.getElementById('fMinP').value;\n";
    h += "  var maxP = document.getElementById('fMaxP').value;\n";
    h += "  var minQ = document.getElementById('fMinQ').value;\n";
    h += "  var url = '/api/filter?';\n";
    h += "  if (type) url += 'type=' + type + '&';\n";
    h += "  if (minP) url += 'minPrice=' + minP + '&';\n";
    h += "  if (maxP) url += 'maxPrice=' + maxP + '&';\n";
    h += "  if (minQ) url += 'minQty=' + minQ;\n";
    h += "  fetch(url)\n";
    h += "    .then(function(response) { return response.json(); })\n";
    h += "    .then(function(data) {\n";
    h += "      var html = '';\n";
    h += "      for (var i = 0; i < data.items.length; i++) {\n";
    h += "        var it = data.items[i];\n";
    h += "        var typeClass = (it.type === 'weapon') ? 'type-weapon' : 'type-gear';\n";
    h += "        var details = (it.type === 'weapon') ? (it.grip + ' / ' + it.weapon_type) : (it.size + ' / ' + it.brand);\n";
    h += "        html += '<tr><td>' + it.id + '</td><td><span class=\"type-badge ' + typeClass + '\">' + it.type + '</span></td><td>' + it.name + '</td><td><span class=\"quantity\">' + it.quantity + '</span></td><td class=\"price\">$' + it.price.toFixed(2) + '</td><td class=\"item-value\">$' + it.value.toFixed(2) + '</td><td>' + details + '</td></tr>';\n";
    h += "      }\n";
    h += "      document.getElementById('filterResults').innerHTML = html || '<tr><td colspan=\"7\" class=\"empty\">No results</td></tr>';\n";
    h += "    })\n";
    h += "    .catch(function(err) { console.log('Filter error:', err); });\n";
    h += "}\n\n";

    h += "function openEdit(id) {\n";
    h += "  document.getElementById('editId').value = id;\n";
    h += "  document.getElementById('editModal').classList.add('active');\n";
    h += "}\n\n";

    h += "function closeModal() {\n";
    h += "  document.getElementById('editModal').classList.remove('active');\n";
    h += "}\n\n";

    h += "function saveEdit() {\n";
    h += "  var id = document.getElementById('editId').value;\n";
    h += "  var field = document.getElementById('editField').value;\n";
    h += "  var value = document.getElementById('editValue').value;\n";
    h += "  var params = 'id=' + id + '&field=' + encodeURIComponent(field) + '&value=' + encodeURIComponent(value);\n";
    h += "  fetch('/api/update', {\n";
    h += "    method: 'POST',\n";
    h += "    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },\n";
    h += "    body: params\n";
    h += "  })\n";
    h += "    .then(function(response) { return response.text(); })\n";
    h += "    .then(function(text) {\n";
    h += "      console.log('Update result:', text);\n";
    h += "      loadData();\n";
    h += "      closeModal();\n";
    h += "      showToast('Updated!');\n";
    h += "    })\n";
    h += "    .catch(function(err) { console.log('Update error:', err); });\n";
    h += "}\n\n";

    h += "function showToast(message) {\n";
    h += "  var toast = document.getElementById('toast');\n";
    h += "  toast.textContent = message;\n";
    h += "  toast.style.display = 'block';\n";
    h += "  setTimeout(function() { toast.style.display = 'none'; }, 3000);\n";
    h += "}\n\n";

    h += "loadData();\n";
    h += "</script></body></html>";

    return h;
}
