#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

class Component
{
private:
    string name;
    int quantity;
    float price;
    Component *next;

public:
    Component(const string &name = "", int quantity = 0, float price = 0.0f)
    {
        this->name = name;
        this->quantity = quantity;
        this->price = price;
        next = nullptr;
    }

    // Getters and Setters
    string getName()
    {
        return name;
    }
    void setName(const string &newName)
    {
        name = newName;
    }
    int getQuantity()
    {
        return quantity;
    }
    void setQuantity(int newQuantity)
    {
        quantity = newQuantity;
    }
    float getPrice()
    {
        return price;
    }
    void setPrice(float newPrice)
    {
        price = newPrice;
    }
    Component *getNext()
    {
        return next;
    }
    void setNext(Component *nextNode)
    {
        next = nextNode;
    }

    // Validation methods
    static bool isValidName(const string &name)
    {
        if (name.empty() || name.length() > 25)
            return false;
        return all_of(name.begin(), name.end(), [](char c)
                      { return isalpha(c) || isspace(c); });
    }
    static bool isValidQuantity(int quantity)
    {
        return quantity >= 0 && quantity <= 32767;
    }
    static bool isValidPrice(float price)
    {
        return price >= 0.0f && price < 1000000.0f;
    }

    // Utility
    static string toUpperCase(const string &str)
    {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
};