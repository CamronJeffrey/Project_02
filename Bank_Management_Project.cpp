#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <cctype>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// Bank_Management structure to manage individual bank accounts
struct Bank_Management {
    string account;
    int IdPassword;
    double balance;
    vector<string> transactHist;

    // Constructor to initialize a new bank account
    Bank_Management(string acc, int id) : account(acc), IdPassword(id), balance(0.0) {}

    // Deposit money into the account if amount is within allowed limit
    void deposit(double amount) {
        if (amount > 0 && amount <= 10000) {
            balance += amount;
            transactHist.push_back("Deposited: $" + to_string(amount));
            cout << "Deposit successful!\n";
        }
        else {
            cout << "Invalid deposit amount.\nMaximum per transaction is $10,000.\n";
        }
    }

    // Withdraw money from the account if conditions are met
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance && amount <= 20000) {
            balance -= amount;
            transactHist.push_back("Withdrew: $" + to_string(amount));
            cout << "Withdrawal successful!\n";
        }
        else {
            cout << "Invalid withdrawal. Max $20,000 or insufficient funds.\n";
        }
    }

    // Display the details of the account (name, ID, balance)
    void displayAccDetails() const {
        cout << "\n===== Account Details =====\n";
        cout << "Account Holder: " << account << endl;
        cout << "Account ID: " << IdPassword << endl;
        cout << "Balance: $" << balance << endl;
        cout << "===========================\n";
    }

    // Display all past transactions along with total balance
    void displayTransactHist() const {
        cout << "\n===== Transaction History =====\n";
        for (const auto& transaction : transactHist) {
            cout << transaction << endl;
        }
        cout << "Total Balance: $" << balance << endl;
    }
};

// Global data structures for managing accounts
unordered_map<string, unique_ptr<Bank_Management>> accounts;
shared_ptr<Bank_Management> currentAccount = nullptr;

// Clears invalid input to avoid infinite loops
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Safely handles and validates integer input from user
int safeIntInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInput();
            return value;
        }
        else {
            cout << "Invalid input. Please enter a number.\n";
            clearInput();
        }
    }
}

// Safely handles and validates double input from user
double safeDoubleInput(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            clearInput();
            return value;
        }
        else {
            cout << "Invalid input. Please enter a number.\n";
            clearInput();
        }
    }
}

// Save all accounts to both text and binary files
void saveToFiles() {
    if (!fs::exists("BankData")) {
        fs::create_directory("BankData");
    }

    ofstream txtFile("BankData/accounts.txt");
    ofstream binFile("BankData/accounts.bin", ios::binary);

    if (!txtFile || !binFile) {
        cout << "Error opening files for saving.\n";
        return;
    }

    for (const auto& [username, accountPtr] : accounts) {
        txtFile << username << "," << accountPtr->IdPassword << "," << accountPtr->balance << "\n";
        for (const auto& tx : accountPtr->transactHist) {
            txtFile << tx << "\n";
        }
        txtFile << "---\n";

        size_t nameLen = username.size();
        binFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        binFile.write(username.c_str(), nameLen);
        binFile.write(reinterpret_cast<const char*>(&accountPtr->IdPassword), sizeof(accountPtr->IdPassword));
        binFile.write(reinterpret_cast<const char*>(&accountPtr->balance), sizeof(accountPtr->balance));
    }

    cout << "Accounts saved to BankData/accounts.txt and BankData/accounts.bin successfully.\n";
}

// Enum for user menu choices to improve readability
enum UserChoice {
    DEPOSIT = 1,
    WITHDRAW,
    DISPLAY_DETAILS,
    DISPLAY_HISTORY,
    LOGOUT
};

// Handles user login and new account creation
bool loginScreen() {
    string name;
    int password;
    char choice;

    cout << "Do you have an account? (y/n): ";
    while (!(cin >> choice) || !(choice == 'y' || choice == 'n' || choice == 'Y' || choice == 'N')) {
        cout << "Invalid input. Please enter 'y' or 'n': ";
        clearInput();
    }
    clearInput();

    if (tolower(choice) == 'n') {
        cout << "Enter new account holder name: ";
        getline(cin, name);
        if (name == "admin") {
            cout << "Cannot create account with reserved username 'admin'.\n";
            return false;
        }
        password = safeIntInput("Set a numeric password: ");
        accounts[name] = make_unique<Bank_Management>(name, password);
        cout << "Account created successfully!\n";
    }

    cout << "\nLogin to your account\n";
    cout << "Username: ";
    getline(cin, name);
    password = safeIntInput("Password: ");

    if (name == "admin" && password == 9999) {
        currentAccount = nullptr;
        cout << "Admin login successful!\n";
        return true;
    }

    auto it = accounts.find(name);
    if (it != accounts.end() && it->second->IdPassword == password) {
        currentAccount = shared_ptr<Bank_Management>(it->second.get(), [](Bank_Management*) {});
        cout << "Login successful!\n";
        return true;
    }

    cout << "Login failed. Invalid username or password.\n";
    return false;
}

// Displays options and processes input for normal users
void userMenu() {
    int choice = 0;
    do {
        cout << "\nBank Account Management System - Logged in as: " << currentAccount->account;
        cout << "\n1. Deposit";
        cout << "\n2. Withdraw";
        cout << "\n3. Display Account Details";
        cout << "\n4. Display Transaction History";
        cout << "\n5. Logout";
        choice = safeIntInput("\nEnter your choice: ");

        system("cls");

        switch (choice) {
        case DEPOSIT: {
            double amount = safeDoubleInput("Enter deposit amount: ");
            currentAccount->deposit(amount);
            break;
        }
        case WITHDRAW: {
            double amount = safeDoubleInput("Enter withdrawal amount: ");
            currentAccount->withdraw(amount);
            break;
        }
        case DISPLAY_DETAILS:
            currentAccount->displayAccDetails();
            break;
        case DISPLAY_HISTORY:
            currentAccount->displayTransactHist();
            break;
        case LOGOUT:
            cout << "Logging out...\n";
            currentAccount = nullptr;
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != LOGOUT);
}

// Admin panel for viewing all account data
void adminMenu() {
    int choice;
    do {
        cout << "\n===== Admin Panel =====";
        cout << "\n1. View All Accounts";
        cout << "\n2. Logout";
        choice = safeIntInput("\nEnter your choice: ");

        system("cls");

        switch (choice) {
        case 1:
            cout << "\n--- All Account Details ---\n";
            for (const auto& [username, accountPtr] : accounts) {
                cout << "\nUser: " << username << "\n";
                cout << "ID/Password: " << accountPtr->IdPassword << "\n";
                cout << "Balance: $" << accountPtr->balance << "\n";
                cout << "Transactions:\n";
                for (const string& tx : accountPtr->transactHist) {
                    cout << "  - " << tx << "\n";
                }
                cout << "-------------------------\n";
            }
            break;
        case 2:
            cout << "Logging out of Admin Panel...\n";
            break;
        default:
            cout << "Invalid option.\n";
        }
    } while (choice != 2);
}
