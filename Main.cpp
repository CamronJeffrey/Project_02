// Main program loop that manages login, admin, and user interfaces
int main() {
    while (true) {
        system("cls");
        if (loginScreen()) {
            if (currentAccount == nullptr) {
                adminMenu();
            }
            else {
                userMenu();
            }
            saveToFiles();
        }
        else {
            char retry;
            cout << "Try logging in again? (y/n): ";
            while (!(cin >> retry) || !(retry == 'y' || retry == 'n' || retry == 'Y' || retry == 'N')) {
                cout << "Invalid input. Please enter 'y' or 'n': ";
                clearInput();
            }
            clearInput();
            if (tolower(retry) != 'y') {
                cout << "Goodbye!\n";
                break;
            }
        }
    }
    return 0;
}
