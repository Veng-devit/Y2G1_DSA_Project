#include "component.cpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <iomanip>
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
    void inputName(string &name, int option)
    {
        while (true)
        {
            cout << (option == 2 ? "Enter the name of the component want to buy: " : option == 4 ? "Enter the component want to delete: "
                                                                                                  : "Enter the name of the component: ");
            getline(cin, name);

            // Convert to uppercase
            name = Component::toUpperCase(name);

            // Validation
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
            "Do you want to delete other component from the list you buy (Y/N): "};

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

public:
    Inventory()
    {
        head = nullptr;
        tail = nullptr;
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
    }

    // Core operations
    void addComponent()
    {
        string name;
        int quantity;
        float price;

        // Input name with validation
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
        string searchName;
        inputName(searchName, 0);

        Component *found = findComponent(searchName);

        if (found != nullptr)
        {
            cout << "Component found in system!\n";
            cout << "Name: " << found->getName() << "\n";
            cout << "Quantity: " << found->getQuantity() << "\n";
            cout << "Price: $" << found->getPrice() << "\n";

            char option = getYesNoOption(6, 'U', 'V');
            if (option == 'U')
            {
                // For update, we need non-const version
                updateComponent(found);
            }
        }
        else
        {
            cout << "Component not found in system!\n";
            char option = getYesNoOption(7, 'Y', 'N');
            if (option == 'Y')
            {
                addComponent();
            }
        }
    }
    void updateComponent(Component *component)
    {
        if (component == nullptr)
            return;

        char confirm = getYesNoOption(0, 'Y', 'N');
        if (confirm != 'Y')
            return;

        // Temporary storage for new values
        string newName;
        int newQuantity;
        float newPrice;

        // Update name
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

        // Update quantity
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
            { // Decrease
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

        // Update price
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
            // Display component info
            cout << "Name Component: " << toDelete->getName() << "\n";
            cout << "Quantity Component: " << toDelete->getQuantity() << "\n";
            cout << "Price Component: $" << toDelete->getPrice() << "\n";

            char confirm = getYesNoOption(8, 'Y', 'N');

            if (confirm == 'Y')
            {
                // Handle different deletion cases
                if (toDelete == head && toDelete == tail)
                {
                    // Only one component in list
                    head = tail = nullptr;
                }
                else if (toDelete == head)
                {
                    // Delete first component
                    head = toDelete->getNext();
                }
                else if (toDelete == tail)
                {
                    // Delete last component
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
                    // Delete middle component
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
    void displayAll()
    {
        if (isEmpty())
        {
            cout << "Inventory is empty!\n";
            return;
        }

        // Display header
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

            // Format the output to match your original table design
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

    // File operations
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
            return; // No file to load is not an error
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

    // Sales related
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

    // Utility
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

    // For sales class access
    Component *getHead() { return head; }
};