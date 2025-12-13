#include <iostream>
#include <string>
#include "sales.cpp"
#include "auth.cpp"

using namespace std;

void displayMenu() {
    cout << "\n=== INVENTORY MANAGEMENT SYSTEM ===" << endl;
    cout << "1. Add Component" << endl;
    cout << "2. Search Component" << endl;
    cout << "3. View All Components" << endl;
    cout << "4. Delete Component" << endl;
    cout << "5. Process Sale" << endl;
    cout << "0. Exit" << endl;
    cout << "Choose option: ";
}

int getMenuOption() {
    string input;
    while (true) {
        displayMenu();
        getline(cin, input);
        
        try {
            int choice = stoi(input);
            if (choice >= 0 && choice <= 5) {
                return choice;
            } else {
                cout << "Please enter 0-5!" << endl;
            }
        } catch (...) {
            cout << "Please enter a valid number!" << endl;
        }
    }
}

int main() {

    if (!authentication()){
        return 0;
    }


    Inventory inventory;
    Sales sales(&inventory);
    
    // Load existing data
    inventory.loadFromFile("component_list.csv");
    
    int choice;
    do {
        inventory.clearScreen();
        choice = getMenuOption();
        inventory.clearScreen();
        
        switch (choice) {
            case 1:
                cout << "[1] ADD COMPONENT\n\n";
                inventory.addComponent();
                break;
            case 2:
                cout << "[2] SEARCH COMPONENT\n\n";
                inventory.searchComponent();
                break;
            case 3:
                cout << "[3] VIEW ALL COMPONENTS\n\n";
                inventory.displayAll();
                break;
            case 4:
                cout << "[4] DELETE COMPONENT\n\n";
                inventory.deleteComponent();
                break;
            case 5:
                cout << "[5] PROCESS SALES\n\n";
                sales.processSale();
                break;
            case 0:
                inventory.saveToFile("component_list.csv");
                cout << "Data saved. Goodbye!" << endl;
                break;
        }
        
        if (choice != 0) {
            inventory.pressAnyKey();
        }
    } while (choice != 0);
    
    return 0;
}