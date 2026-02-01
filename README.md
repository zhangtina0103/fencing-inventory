
## Fencing inventory management system

A C++ inventory management system for fencing equipment with SQLite database and web UI, inspired by my younger years of competitive fencing...
                        - Coming from a retired college varsity women's epee team fencer :0

![Screenshot](screenshot.png)

### Features
- Basic operations: add, remove, update, display items
- Two item types: Weapons (epee, foil, sabre) and Gear (masks, jackets, etc.)
- Sorting: by name, price, quantity, value (ascending/descending)
- Top N: View top 3, 5, or 10 items by value
- Search: by any field with operators (=, >, <, >=, <=)
- Filtering: By type, price range, quantity
- Persistent storage: SQLite database
- Two interfaces: CLI and Web UI
  
### Languages/tools used
- C++17
- SQLite3
- cpp-httplib (web server)
- HTML/CSS/JavaScript (frontend)

### Build & Run
```bash

# Compile
g++ -std=c++17 main.cpp Database.cpp Inventory.cpp Item.cpp Weapon.cpp Gear.cpp Menu.cpp WebServer.cpp -lsqlite3 -pthread -o main
# Run CLI
./main
# Run Web UI
./main --web
# Then open http://localhost:8080
```

### Project Structure
```
├── main.cpp          # entry point
├── Database.h/cpp    # SQLite operations
├── Inventory.h/cpp   # inventory management with C++ objects
├── Item.h/cpp        # base Item class
├── Weapon.h/cpp      # Weapon subclass
├── Gear.h/cpp        # Gear subclass
├── Menu.h/cpp        # cli interface
└── WebServer.h/cpp   # web server and ui
```

