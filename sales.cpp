#include "inventory.cpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <ctime>
using namespace std;

class Sales
{
private:
    Component *head;
    Component *tail;
    Inventory *inventory;

    string getCurrentDateTime(){
        time_t now = time(nullptr);
        tm *local = localtime(&now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
        return string(buffer);          
    }
    void saveOrderHistory() {
        string date = getCurrentDateTime();
        ofstream file("order_history.csv", ios::app);
        if (!file.is_open()){
            cerr << "Failed to open file!\n";
            return;
        }
        Component *current = head;
        float total = 0;
        file << date << " :\n";                   
        file << "Name,Quantity,Price,Subtotal\n";
        while (current != nullptr){
            float subtotal = current->getQuantity() * current->getPrice();
            file << current->getName() << ","
                 << current->getQuantity() << ","
                 << current->getPrice() << ","
                 << subtotal << endl;
            total += subtotal;
            current = current->getNext();
        }
        file << "Total Price:$" << total << "\n\n"; 
        file.close();
    }
    // Helper methods
    Component *findInCart(const string &name)
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
    void inputNameForSale(string &name, int option)
    {
        while (true)
        {
            cout << (option == 2 ? "Enter the name of the component want to buy: " : option == 4 ? "Enter the component want to delete from cart: "
                                                                                                  : "Enter component name: ");

            getline(cin, name);
            name = Component::toUpperCase(name);

            if (name.empty())
            {
                cout << "Please input a name!!\n";
                continue;
            }
            if (name.length() > 25)
            {
                cout << "Name can be at most 25 characters!\n";
                continue;
            }
            break;
        }
    }
    void inputQuantityForSale(int &quantity, int option)
    {
        string input;
        while (true)
        {
            cout << (option == 2 ? "Enter the amount of quantity want to buy: " : option == 3 ? "Enter the new quantity: "
                                                                                               : "Enter quantity: ");

            getline(cin, input);

            try
            {
                quantity = stoi(input);
                if (quantity > 0)
                {
                    break;
                }
                else
                {
                    cout << "Quantity must be positive!\n";
                }
            }
            catch (const exception &e)
            {
                cout << "Please enter a valid number!\n";
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
            if (caseType >= 0 && caseType <= 13)
            {
                cout << prompts[caseType];
            }
            else
            {
                cout << "Enter " << opt1 << "/" << opt2 << ": ";
            }

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
    float calculateTotal()
    {
        float total = 0.0f;
        Component *current = head;
        while (current != nullptr)
        {
            total += current->getQuantity() * current->getPrice();
            current = current->getNext();
        }
        return total;
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
public:
    Sales(Inventory *inv)
    {
        head = nullptr;
        tail = nullptr;
        inventory = inv;
    }
    ~Sales()
    {
        Component *current = head;
        while (current != nullptr)
        {
            Component *next = current->getNext();
            delete current;
            current = next;
        }
    }

    void processSale()
    {
        // Clear any existing cart items
        Component *current = head;
        while (current != nullptr)
        {
            Component *next = current->getNext();
            delete current;
            current = next;
        }
        head = tail = nullptr;

        cout << "=== PROCESS SALE ===\n";

        // Add items to cart
        char option;
        do
        {
            addToCart();
            option = getYesNoOption(9, 'C', 'S'); // Case 9: Continue or Stop
        } while (option == 'C');

        // Review and complete sale
        while (true)
        {
            if (head == nullptr)
            {
                cout << "You didn't buy anything!\n";
                break;
            }

            displayCart();
            option = getYesNoOption(11, 'Y', 'N'); // Case 11: Accept purchase?

            if (option == 'Y')
            {
                completeSale();
                break;
            }
            else
            {
                option = getYesNoOption(12, 'A', 'D'); // Case 12: Add or Delete
                if (option == 'A')
                {
                    addToCart();
                }
                else
                {
                    removeFromCart();
                }
            }
        }
    }
    void addToCart()
    {
        string name;
        inputNameForSale(name, 2);

        // Check if component exists in inventory
        Component *inventoryComp = inventory->findComponentForSale(name);
        if (inventoryComp == nullptr)
        {
            cout << "This component doesn't exist in inventory!\n";
            return;
        }

        // Check if already in cart
        Component *cartItem = findInCart(name);
        if (cartItem != nullptr)
        {
            cout << "This component is already in your cart!\n";
            char change = getYesNoOption(10, 'Y', 'N'); // Case 10: Change quantity?
            if (change == 'Y')
            {
                int newQty;
                inputQuantityForSale(newQty, 3);
                if (newQty <= inventoryComp->getQuantity())
                {
                    cartItem->setQuantity(newQty);
                    cout << "Quantity updated successfully!\n";
                }
                else
                {
                    cout << "Not enough stock! Available: " << inventoryComp->getQuantity() << "\n";
                }
            }
            return;
        }

        // Add new item to cart
        int quantity;
        inputQuantityForSale(quantity, 2);

        if (quantity > inventoryComp->getQuantity())
        {
            cout << "Not enough stock! Available: " << inventoryComp->getQuantity() << "\n";
            return;
        }

        // Create a new component for the cart
        Component *newItem = new Component(name, quantity, inventoryComp->getPrice());

        if (head == nullptr)
        {
            head = tail = newItem;
        }
        else
        {
            tail->setNext(newItem);
            tail = newItem;
        }

        cout << "Added to cart successfully!\n";
    }
    void removeFromCart()
    {
        if (head == nullptr)
        {
            cout << "Your cart is empty!\n";
            return;
        }

        string name;
        inputNameForSale(name, 4);

        Component *toDelete = findInCart(name);
        if (toDelete == nullptr)
        {
            cout << "This component is not in your cart!\n";
            return;
        }

        char confirm = getYesNoOption(8, 'Y', 'N'); // Case 8: Confirm delete
        if (confirm != 'Y')
        {
            cout << "Removal cancelled.\n";
            return;
        }

        // Remove from cart
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
        cout << "Removed from cart successfully!\n";
    }
    void displayCart()
    {
        if (head == nullptr)
        {
            cout << "Your cart is empty!\n";
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
        cout << "\nTOTAL PRICE: $" << fixed << setprecision(2) << calculateTotal() << "\n";
    }
    void completeSale()
    {
        // Update inventory quantities
        Component *current = head;
        while (current != nullptr)
        {
            inventory->processSaleQuantity(current->getName(), current->getQuantity());
            current = current->getNext();
        }

        cout << "\n"
             << string(50, '=') << "\n";
        cout << "         SALE COMPLETED SUCCESSFULLY!\n";
        cout << string(50, '=') << "\n";

        displayCart();
        cout << "\nThank you for your purchase!\n";
        saveOrderHistory();
        // Clear cart after sale
        current = head;
        while (current != nullptr)
        {
            Component *next = current->getNext();
            delete current;
            current = next;
        }
        head = tail = nullptr;
    }
};