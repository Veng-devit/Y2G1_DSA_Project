#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
using namespace std;

string username;
string password;

string encryptPassword(string pass) {
    for (char &c : pass) {
        c = c + 3;
    }
    return pass;
}

string getPasswordHidden() {
    string pass;
    char ch;

    while ((ch = _getch()) != 13) {  
        if (ch == 8 && !pass.empty()) {
            cout << "\b \b";
            pass.pop_back();
        } else if (ch != 8) {
            cout << '*';
            pass.push_back(ch);
        }
    }
    cout << endl;
    return pass;
}

void saveCredentials(string auth) {
    ofstream file(auth);   
    if (!file.is_open()) {
        cerr << "Error: Cannot create file!\n";
        return;
    }

    
    file << username << "," << password << "\n";

    file.close();
}

void initializeCredentials(string auth) {
    cout << "\n=== CREATE NEW ACCOUNT ===\n";

    do {
        cout << "Enter username: ";
        getline(cin, username);
    } while (username.empty());

    string p1, p2;
    do {
        cout << "Enter password: ";
        p1 = getPasswordHidden();

        cout << "Confirm password: ";
        p2 = getPasswordHidden();

        if (p1 != p2)
            cout << "Passwords do not match. Try again.\n";

    } while (p1 != p2);

    password = encryptPassword(p1);

    saveCredentials(auth);
    cout << "Account created successfully!\n";
}

void loadCredentials(string auth) {
    ifstream file(auth);   
    if (!file.is_open()) {
        cout << "No account found. Creating new one...\n";
        initializeCredentials(auth);
        return;
    }

    string line;
    if (getline(file, line)) {
        size_t commaPos = line.find(',');
        if (commaPos != string::npos) {
            username = line.substr(0, commaPos);
            password = line.substr(commaPos + 1);
        }
    }

    file.close();
}

bool login() {
    int attempts = 3;

    while (attempts > 0) {
        cout << "\n=== LOGIN ===\n";

        string userInput, passInput;

        cout << "Username: ";
        getline(cin, userInput);

        cout << "Password: ";
        passInput = getPasswordHidden();

        if (userInput == username &&
            encryptPassword(passInput) == password)
        {
            cout << "Login successful! Welcome " << username << "!\n";
            return true;
        }

        attempts--;
        cout << "Incorrect login. Attempts left: " << attempts << "\n";
    }
    return false;
}

  bool authentication() {
    string authFile = "auth.csv";

    loadCredentials(authFile);

    if (login()) {
        cout << "\nAccess granted. You may continue...\n";
         return true;
    } else {
        cout << "\nAccess denied. Exiting program...\n";
        return false;
    
    }
}
