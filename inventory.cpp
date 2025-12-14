#include "component.cpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <vector>
using namespace std;

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

class Inventory
{
private:
    Component *head;
    Component *tail;
    Component *searchResultsHead;
    Component *searchResultsTail;

    // Helper methods
    Component *findComponent(const string &name)
    {
        Component *current = head;
        string upperName = Component::toUpperCase(name);

        while (current != nullptr)
        {
            if (Component::toUpperCase(current->getName()) == upperName)
            {
                return current;
            }
            current = current->getNext();
        }
        return nullptr;
    }
    
    vector<Component*> findComponentsBySubstring(const string &substr) {
        vector<Component*> results;
        Component *current = head;
        string upperSubstr = Component::toUpperCase(substr);
        
        while (current != nullptr) {
            string upperName = Component::toUpperCase(current->getName());
            if (upperName.find(upperSubstr) != string::npos) {
                results.push_back(current);
            }
            current = current->getNext();
        }
        return results;
    }
    
    void createSearchResultsList(const vector<Component*>& results) {
        clearSearchResults();
        
        if (results.empty()) {
            searchResultsHead = searchResultsTail = nullptr;
            return;
        }
        
        for (Component* original : results) {
            Component* copy = new Component(original->getName(), 
                                           original->getQuantity(), 
                                           original->getPrice());
            
            if (searchResultsHead == nullptr) {
                searchResultsHead = searchResultsTail = copy;
            } else {
                searchResultsTail->setNext(copy);
                searchResultsTail = copy;
            }
        }
    }
    
    void clearSearchResults() {
        Component* current = searchResultsHead;
        while (current != nullptr) {
            Component* next = current->getNext();
            delete current;
            current = next;
        }
        searchResultsHead = searchResultsTail = nullptr;
    }
    
    void inputName(string &name, int option)
    {
        while (true)
        {
            cout << (option == 2 ? "Enter the name of the component want to buy: " : option == 4 ? "Enter the component want to delete: "
                                                                                                  : "Enter the name of the component: ");
            getline(cin, name);

            name = Component::toUpperCase(name);

            if (name.empty())
            {
                cout << "Please input name of the component!!\n";
                continue;
            }
            if (name.length() > 25)
            {
                cout << "Name can be at most 25 characters!\n";
                continue;
            }
            if (!Component::isValidName(name))
            {
                cout << "Please input only alphabetic characters and spaces!!\n";
                continue;
            }
            break;
        }
    }
    void inputQuantity(int &quantity, int option)
    {
        string input;
        while (true)
        {
            cout << (option == 2 ? "Enter the amount of quantity want to buy: " : option == 3 ? "Enter the new amount of quantity: "
                                                                                               : "Enter the quantity of component: ");

            getline(cin, input);

            try
            {
                quantity = stoi(input);
                if (Component::isValidQuantity(quantity))
                {
                    break;
                }
                else
                {
                    cout << "Quantity must be between 0 and 32767!\n";
                }
            }
            catch (const exception &e)
            {
                cout << "Please enter a valid number!\n";
            }
        }
    }
    void inputPrice(float &price)
    {
        string input;
        while (true)
        {
            cout << "Enter the price of component ($): ";
            getline(cin, input);

            try
            {
                price = stof(input);
                if (Component::isValidPrice(price))
                {
                    break;
                }
                else
                {
                    cout << "Price must be between 0 and 999999.99!\n";
                }
            }
            catch (const exception &e)
            {
                cout << "Please enter a valid price!\n";
            }
        }
    }
    
    // REMOVED: The prompts for sort options from getYesNoOption function
    char getYesNoOption(int caseType, char opt1, char opt2)
    {
        string input;

        const char *prompts[] = {
            "Enter Y/N to make change information this component: ",
            "Enter Y/N to make change name this component: ",
            "Enter Y/N to make change quantity this component: ",
            "Enter Y/N to make change price this component: ",
            "Do you want to (D)ecrease or (I)ncrease: ",
            "Do you want to change price (Y/N): ",
            "Enter the option (U)pdate, (V)iew: ",
            "Do you want to add this component (Y/N): ",
            "Are you sure want to delete this component (Y/N): ",
            "Do you want buy other component (C)ontinue, (S)top: ",
            "Do you want to make change the quantity you buy (Y/N): ",
            "Are you accept for this buy (Y/N): ",
            "Do you want to buy other component or take some component out [(A)dd or (D)elete]: ",
            "Do you want to delete other component from the list you buy (Y/N): ",
            // REMOVED sort prompts from here
            "Do you want to delete a found component (Y/N): "};

        while (true)
        {
            cout << prompts[caseType];
            getline(cin, input);

            if (input.length() == 1)
            {
                char choice = toupper(input[0]);
                if (choice == opt1 || choice == opt2)
                {
                    return choice;
                }
            }
            cout << "Please enter only '" << opt1 << "' or '" << opt2 << "'!\n";
        }
    }
    
    // NEW: Function to get sort choice with multiple options
    char getSortChoice(const string& prompt, const vector<char>& validChoices) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            
            if (input.length() == 1) {
                char choice = toupper(input[0]);
                for (char valid : validChoices) {
                    if (choice == valid) {
                        return choice;
                    }
                }
            }
            
            cout << "Please enter a valid choice (";
            for (size_t i = 0; i < validChoices.size(); i++) {
                cout << validChoices[i];
                if (i < validChoices.size() - 1) cout << "/";
            }
            cout << "): ";
        }
    }
    
    // SIMPLE BUBBLE SORT for linked list
    void bubbleSort(int sortBy, bool ascending) {
        if (head == nullptr || head->getNext() == nullptr) return;
        
        bool swapped;
        int listSize = getListSize();
        
        // Bubble sort outer loop
        for (int i = 0; i < listSize - 1; i++) {
            swapped = false;
            Component* current = head;
            Component* prev = nullptr;
            
            // Inner loop - compare adjacent nodes
            for (int j = 0; j < listSize - i - 1; j++) {
                Component* nextNode = current->getNext();
                
                if (nextNode == nullptr) break;
                
                // Determine if we need to swap based on sort criteria
                bool shouldSwap = false;
                
                if (sortBy == 0) { // Sort by Name
                    string name1 = current->getName();
                    string name2 = nextNode->getName();
                    
                    if (ascending) {
                        shouldSwap = name1 > name2;
                    } else {
                        shouldSwap = name1 < name2;
                    }
                } 
                else if (sortBy == 1) { // Sort by Quantity
                    int qty1 = current->getQuantity();
                    int qty2 = nextNode->getQuantity();
                    
                    if (ascending) {
                        shouldSwap = qty1 > qty2;
                    } else {
                        shouldSwap = qty1 < qty2;
                    }
                } 
                else { // Sort by Price
                    float price1 = current->getPrice();
                    float price2 = nextNode->getPrice();
                    
                    if (ascending) {
                        shouldSwap = price1 > price2;
                    } else {
                        shouldSwap = price1 < price2;
                    }
                }
                
                // Swap nodes if needed
                if (shouldSwap) {
                    // Swap the nodes
                    current->setNext(nextNode->getNext());
                    nextNode->setNext(current);
                    
                    // Update previous pointer
                    if (prev == nullptr) {
                        head = nextNode;
                    } else {
                        prev->setNext(nextNode);
                    }
                    
                    // Update tail if necessary
                    if (current->getNext() == nullptr) {
                        tail = current;
                    }
                    
                    // Update pointers for next iteration
                    prev = nextNode;
                    swapped = true;
                } else {
                    // No swap, move pointers forward
                    prev = current;
                    current = current->getNext();
                }
            }
            
            // If no swaps occurred, list is sorted
            if (!swapped) break;
        }
        
        // Update tail pointer
        updateTail();
    }
    
    // Helper to get list size
    int getListSize() {
        int count = 0;
        Component* current = head;
        while (current != nullptr) {
            count++;
            current = current->getNext();
        }
        return count;
    }
    
    // Helper to update tail pointer
    void updateTail() {
        if (head == nullptr) {
            tail = nullptr;
            return;
        }
        
        Component* current = head;
        while (current->getNext() != nullptr) {
            current = current->getNext();
        }
        tail = current;
    }
    
    void displaySearchResults() {
        if (searchResultsHead == nullptr) {
            cout << "No components found!\n";
            return;
        }
        
        int count = 0;
        Component* current = searchResultsHead;
        
        cout << "\n=== SEARCH RESULTS ===" << endl;
        
        while (current != nullptr) {
            count++;
            current = current->getNext();
        }
        
        cout << "Found " << count << " item(s)" << endl;
        
        if (count > 0) {
            for (int i = 0; i < 59; i++) cout << "_";
            cout << "\n";
            cout << "|";
            print_space(30);
            cout << "|";
            print_space(10);
            cout << "|";
            print_space(15);
            cout << "|" << endl;
            cout << "|  Name Component";
            print_space(14);
            cout << "| ";
            cout << "Quantity ";
            cout << "|  Price ($)";
            print_space(4);
            cout << "|" << endl;
            cout << "|";
            print_underScore(30);
            cout << "|";
            print_underScore(10);
            cout << "|";
            print_underScore(15);
            cout << "|" << endl;

            current = searchResultsHead;
            int itemNumber = 1;
            
            while (current != nullptr) {
                string name = current->getName();
                int qty = current->getQuantity();
                float price = current->getPrice();

                cout << "|";
                print_space(30);
                cout << "|";

                print_space(10);
                cout << "|";
                print_space(15);
                cout << "|\n";
                
                string displayName = to_string(itemNumber) + ". " + name;
                cout << "|  " << displayName;
                print_space(30 - displayName.length() - 2);
                cout << "| ";
                cout << qty;
                
                if (qty < 10) print_space(8);
                else if (qty < 100) print_space(7);
                else if (qty < 1000) print_space(6);
                else if (qty < 10000) print_space(5);
                else print_space(4);
                
                cout << fixed << setprecision(2) << "| " << price;

                if (price < 10) print_space(9);
                else if (price < 100) print_space(8);
                else if (price < 1000) print_space(7);
                else if (price < 10000) print_space(6);
                else if (price < 100000) print_space(5);
                else print_space(4);
                
                cout << " |\n";
                cout << "|";
                print_underScore(30);
                cout << "|";
                print_underScore(10);
                cout << "|";
                print_underScore(15);
                cout << "|\n";

                current = current->getNext();
                itemNumber++;
            }
        }
        
        cout << "==============================\n";
    }
    
    void displaySingleComponent(Component* comp) {
        cout << "Name: " << comp->getName() << "\n";
        cout << "  Quantity: " << comp->getQuantity() << "\n";
        cout << "  Price: $" << fixed << setprecision(2) << comp->getPrice() << "\n";
    }
    
    Component* findComponentInMainList(Component* target) {
        Component* current = head;
        while (current != nullptr) {
            if (current == target) {
                return current;
            }
            current = current->getNext();
        }
        return nullptr;
    }
    
    bool deleteFromMainList(Component* target) {
        if (head == nullptr || target == nullptr) return false;
        
        Component* current = head;
        Component* prev = nullptr;
        
        while (current != nullptr) {
            if (current == target) {
                if (prev == nullptr) {
                    head = current->getNext();
                    if (head == nullptr) tail = nullptr;
                } else {
                    prev->setNext(current->getNext());
                    if (current == tail) {
                        tail = prev;
                    }
                }
                delete current;
                return true;
            }
            prev = current;
            current = current->getNext();
        }
        return false;
    }

public:
    Inventory()
    {
        head = nullptr;
        tail = nullptr;
        searchResultsHead = nullptr;
        searchResultsTail = nullptr;
    }
    ~Inventory()
    {
        Component *current = head;
        while (current != nullptr)
        {
            Component *next = current->getNext();
            delete current;
            current = next;
        }
        clearSearchResults();
    }

    void addComponent()
    {
        string name;
        int quantity;
        float price;

        do
        {
            inputName(name, 1);
            if (findComponent(name) != nullptr)
            {
                cout << "Component already exists!\n";
                name.clear();
            }
        } while (name.empty());

        inputQuantity(quantity, 1);
        inputPrice(price);

        Component *newComponent = new Component(name, quantity, price);

        if (head == nullptr)
        {
            head = tail = newComponent;
        }
        else
        {
            tail->setNext(newComponent);
            tail = newComponent;
        }

        cout << "Component added successfully!\n";
    }
    
    void searchComponent()
    {
        cout << "\n=== SEARCH OPTIONS ===\n";
        cout << "1. Search by exact name\n";
        cout << "2. Search by keyword/substring\n";
        cout << "=====================\n";
        cout << "Enter your choice: ";
        
        string choiceStr;
        getline(cin, choiceStr);
        
        if (choiceStr == "1") {
            string searchName;
            inputName(searchName, 0);

            Component *found = findComponent(searchName);

            if (found != nullptr)
            {
                cout << "\nComponent found!\n";
                displaySingleComponent(found);

                char option = getYesNoOption(6, 'U', 'V');
                if (option == 'U')
                {
                    updateComponent(found);
                }
            }
            else
            {
                cout << "Component not found!\n";
                char option = getYesNoOption(7, 'Y', 'N');
                if (option == 'Y')
                {
                    addComponent();
                }
            }
        } 
        else if (choiceStr == "2") {
            cout << "Enter search keyword: ";
            string keyword;
            getline(cin, keyword);
            
            if (keyword.empty()) {
                cout << "Keyword cannot be empty!\n";
                return;
            }
            
            vector<Component*> results = findComponentsBySubstring(keyword);
            createSearchResultsList(results);
            displaySearchResults();
            
            if (searchResultsHead != nullptr) {
                cout << "\nOptions:\n";
                cout << "1. Update a component\n";
                cout << "2. Delete a component\n";
                cout << "3. View details of a component\n";
                cout << "Enter choice (1-3) or any other key to cancel: ";
                
                string optionStr;
                getline(cin, optionStr);
                
                if (optionStr == "1") {
                    cout << "Enter the number of component to update: ";
                    string numStr;
                    getline(cin, numStr);
                    
                    try {
                        int num = stoi(numStr);
                        if (num >= 1) {
                            Component* current = searchResultsHead;
                            int count = 1;
                            
                            while (current != nullptr && count < num) {
                                current = current->getNext();
                                count++;
                            }
                            
                            if (current != nullptr) {
                                Component* mainComp = findComponent(current->getName());
                                if (mainComp != nullptr) {
                                    updateComponent(mainComp);
                                    createSearchResultsList(findComponentsBySubstring(keyword));
                                }
                            } else {
                                cout << "Invalid number!\n";
                            }
                        } else {
                            cout << "Invalid number!\n";
                        }
                    } catch (...) {
                        cout << "Invalid input!\n";
                    }
                } 
                else if (optionStr == "2") {
                    cout << "Enter the number of component to delete: ";
                    string numStr;
                    getline(cin, numStr);
                    
                    try {
                        int num = stoi(numStr);
                        if (num >= 1) {
                            Component* current = searchResultsHead;
                            Component* prev = nullptr;
                            int count = 1;
                            
                            while (current != nullptr && count < num) {
                                prev = current;
                                current = current->getNext();
                                count++;
                            }
                            
                            if (current != nullptr) {
                                char confirm = getYesNoOption(14, 'Y', 'N');
                                if (confirm == 'Y') {
                                    Component* mainComp = findComponent(current->getName());
                                    if (mainComp != nullptr && deleteFromMainList(mainComp)) {
                                        cout << "Component deleted successfully!\n";
                                        
                                        if (prev == nullptr) {
                                            searchResultsHead = current->getNext();
                                            if (searchResultsHead == nullptr) {
                                                searchResultsTail = nullptr;
                                            }
                                        } else {
                                            prev->setNext(current->getNext());
                                            if (current == searchResultsTail) {
                                                searchResultsTail = prev;
                                            }
                                        }
                                        delete current;
                                    }
                                }
                            } else {
                                cout << "Invalid number!\n";
                            }
                        } else {
                            cout << "Invalid number!\n";
                        }
                    } catch (...) {
                        cout << "Invalid input!\n";
                    }
                }
                else if (optionStr == "3") {
                    cout << "Enter the number of component to view details: ";
                    string numStr;
                    getline(cin, numStr);
                    
                    try {
                        int num = stoi(numStr);
                        if (num >= 1) {
                            Component* current = searchResultsHead;
                            int count = 1;
                            
                            while (current != nullptr && count < num) {
                                current = current->getNext();
                                count++;
                            }
                            
                            if (current != nullptr) {
                                cout << "\n=== DETAILED VIEW ===\n";
                                displaySingleComponent(current);
                                cout << "===================\n";
                            } else {
                                cout << "Invalid number!\n";
                            }
                        } else {
                            cout << "Invalid number!\n";
                        }
                    } catch (...) {
                        cout << "Invalid input!\n";
                    }
                }
                
                clearSearchResults();
            }
        } 
        else {
            cout << "Invalid choice!\n";
        }
    }
    
    void updateComponent(Component *component)
    {
        if (component == nullptr)
            return;

        char confirm = getYesNoOption(0, 'Y', 'N');
        if (confirm != 'Y')
            return;

        string newName;
        int newQuantity;
        float newPrice;

        char updateName = getYesNoOption(1, 'Y', 'N');
        if (updateName == 'Y')
        {
            do
            {
                inputName(newName, 0);
                if (newName == component->getName())
                {
                    cout << "You entered the same name! Please change it!\n";
                }
                else
                {
                    component->setName(newName);
                    break;
                }
            } while (true);
        }

        char updateQuantity = getYesNoOption(2, 'Y', 'N');
        if (updateQuantity == 'Y')
        {
            char operation = getYesNoOption(4, 'D', 'I');
            inputQuantity(newQuantity, 1);

            int currentQty = component->getQuantity();
            if (operation == 'I')
            {
                if (currentQty + newQuantity <= 32767)
                {
                    component->setQuantity(currentQty + newQuantity);
                }
                else
                {
                    cout << "Quantity would exceed maximum limit!\n";
                }
            }
            else
            {
                if (currentQty - newQuantity >= 0)
                {
                    component->setQuantity(currentQty - newQuantity);
                }
                else
                {
                    cout << "Quantity cannot be negative!\n";
                }
            }
        }

        char updatePrice = getYesNoOption(3, 'Y', 'N');
        if (updatePrice == 'Y')
        {
            do
            {
                inputPrice(newPrice);
                if (newPrice == component->getPrice())
                {
                    cout << "You entered the same price! Please change it!\n";
                }
                else
                {
                    component->setPrice(newPrice);
                    break;
                }
            } while (true);
        }

        cout << "Component updated successfully!\n";
    }
    
    void deleteComponent()
    {
        if (isEmpty())
        {
            cout << "The system doesn't have any component!!\n";
            return;
        }

        string componentName;
        inputName(componentName, 0);

        Component *toDelete = findComponent(componentName);

        if (toDelete != nullptr)
        {
            cout << "Name Component: " << toDelete->getName() << "\n";
            cout << "Quantity Component: " << toDelete->getQuantity() << "\n";
            cout << "Price Component: $" << toDelete->getPrice() << "\n";

            char confirm = getYesNoOption(8, 'Y', 'N');

            if (confirm == 'Y')
            {
                if (toDelete == head && toDelete == tail)
                {
                    head = tail = nullptr;
                }
                else if (toDelete == head)
                {
                    head = toDelete->getNext();
                }
                else if (toDelete == tail)
                {
                    Component *current = head;
                    while (current->getNext() != tail)
                    {
                        current = current->getNext();
                    }
                    current->setNext(nullptr);
                    tail = current;
                }
                else
                {
                    Component *current = head;
                    while (current->getNext() != toDelete)
                    {
                        current = current->getNext();
                    }
                    current->setNext(toDelete->getNext());
                }

                delete toDelete;
                cout << "Component deleted successfully!\n";
            }
            else
            {
                cout << "Deletion cancelled. Component is safe!\n";
            }
        }
        else
        {
            cout << "Component not found in the system!\n";
        }
    }
    
    // UPDATED: Sort function with options asked inside
    void sortInventory() {
        if (isEmpty() || !head->getNext()) {
            cout << "Not enough components to sort!\n";
            return;
        }
        
        cout << "\n=== SORT INVENTORY ===\n";
        
        // Ask for sort by option (using new function)
        char sortBy = getSortChoice(
            "Sort by:\n"
            "N - Name\n"
            "Q - Quantity\n"
            "P - Price\n"
            "Enter your choice (N/Q/P): ",
            {'N', 'Q', 'P'}
        );
        
        // Ask for sort order option (using new function)
        char order = getSortChoice(
            "\nSort order:\n"
            "A - Ascending (A-Z, 0-9, low to high)\n"
            "D - Descending (Z-A, 9-0, high to low)\n"
            "Enter your choice (A/D): ",
            {'A', 'D'}
        );
        
        // Convert choices to parameters
        int sortOption;
        if (sortBy == 'N') sortOption = 0;      // Name
        else if (sortBy == 'Q') sortOption = 1; // Quantity
        else sortOption = 2;                    // Price
        
        bool ascending = (order == 'A');
        
        // Perform bubble sort
        bubbleSort(sortOption, ascending);
        
        cout << "\nInventory sorted successfully!\n";
        cout << "Sorted by: ";
        if (sortOption == 0) cout << "Name";
        else if (sortOption == 1) cout << "Quantity";
        else cout << "Price";
        cout << " (" << (ascending ? "Ascending" : "Descending") << ")\n";
    }
    
    void displayAll()
    {
        if (isEmpty())
        {
            cout << "Inventory is empty!\n";
            return;
        }

        for (int i = 0; i < 59; i++)
            cout << "_";
        cout << "\n";
        cout << "|";
        print_space(30);
        cout << "|";
        print_space(10);
        cout << "|";
        print_space(15);
        cout << "|" << endl;
        cout << "|  Name Component";
        print_space(14);
        cout << "| ";
        cout << "Quantity ";
        cout << "|  Price ($)";
        print_space(4);
        cout << "|" << endl;
        cout << "|";
        print_underScore(30);
        cout << "|";
        print_underScore(10);
        cout << "|";
        print_underScore(15);
        cout << "|" << endl;

        Component *current = head;
        while (current != nullptr)
        {
            string name = current->getName();
            int qty = current->getQuantity();
            float price = current->getPrice();

            cout << "|";
            print_space(30);
            cout << "|";

            print_space(10);
            cout << "|";
            print_space(15);
            cout << "|\n";
            cout << "|  " << name;
            print_space(30 - name.length() - 2);
            cout << "| ";
            cout << qty;
            if (qty < 10)
                print_space(8);
            else if (qty < 100)
                print_space(7);
            else if (qty < 1000)
                print_space(6);
            else if (qty < 10000)
                print_space(5);
            else
                print_space(4);
            cout << fixed << setprecision(2) << "| " << price;

            if (price < 10)
                print_space(9);
            else if (price < 100)
                print_space(8);
            else if (price < 1000)
                print_space(7);
            else if (price < 10000)
                print_space(6);
            else if (price < 100000)
                print_space(5);
            else
                print_space(4);
            cout << " |\n";
            cout << "|";
            print_underScore(30);
            cout << "|";
            print_underScore(10);
            cout << "|";
            print_underScore(15);
            cout << "|\n";

            current = current->getNext();
        }
    }

    void saveToFile(const string &filename)
    {
        ofstream file(filename);
        if (!file.is_open())
        {
            cout << "Error opening file for saving!\n";
            return;
        }

        Component *current = head;
        while (current != nullptr)
        {
            file << current->getName() << ","
                 << current->getQuantity() << ","
                 << current->getPrice() << "\n";
            current = current->getNext();
        }

        file.close();
    }
    void loadFromFile(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            return;
        }

        string line;
        while (getline(file, line))
        {
            stringstream ss(line);
            string name, qtyStr, priceStr;

            if (getline(ss, name, ',') &&
                getline(ss, qtyStr, ',') &&
                getline(ss, priceStr))
            {

                try
                {
                    int quantity = stoi(qtyStr);
                    float price = stof(priceStr);

                    Component *newComponent = new Component(name, quantity, price);

                    if (head == nullptr)
                    {
                        head = tail = newComponent;
                    }
                    else
                    {
                        tail->setNext(newComponent);
                        tail = newComponent;
                    }
                }
                catch (const exception &e)
                {
                    cout << "Error parsing line: " << line << "\n";
                }
            }
        }

        file.close();
    }

    Component *findComponentForSale(const string &name)
    {
        return findComponent(name);
    }
    void processSaleQuantity(const string &name, int soldQuantity)
    {
        Component *comp = findComponent(name);
        if (comp != nullptr)
        {
            comp->setQuantity(comp->getQuantity() - soldQuantity);
        }
    }

    bool isEmpty()
    {
        return head == nullptr;
    }
    void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }
    void pressAnyKey()
    {
        cout << "\nPress any key to continue...";
#ifdef _WIN32
        _getch();
#else
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    }
    void print_space(int size)
    {
        for (int i = 0; i < size; i++)
            cout << " ";
    }
    void print_underScore(int size)
    {
        for (int i = 0; i < size; i++)
            cout << "_";
    }

    Component *getHead() { return head; }
    Component* getSearchResultsHead() { return searchResultsHead; }
    void displaySearchResultsOnly() {
        displaySearchResults();
    }
};