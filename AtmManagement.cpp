#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <limits>
#include <cstring>

using namespace std;

// ===================================================
//  Class Account
// ===================================================
class Account {
private:
    string username;
    string password;
    double balance;

public:
    Account(string name, string pass, double bal = 0.0)
        : username(name), password(pass), balance(bal) {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    double getBalance() const { return balance; }

    bool checkPassword(const string &pass) const { return pass == password; }

    void deposit(double amount) {
        if (amount <= 0) {
            cout << " Invalid deposit amount.\n";
            return;
        }
        balance += amount;
        cout << " Deposit successful. New balance: $" << balance << endl;
    }

    void withdraw(double amount) {
        if (amount <= 0) {
            cout << " Invalid withdrawal amount.\n";
        } else if (amount > balance) {
            cout << " Insufficient funds.\n";
        } else {
            balance -= amount;
            cout << " Withdrawal successful. Remaining balance: $" << balance << endl;
        }
    }

    void showBalance() const {
        cout << " Your current balance: $" << balance << endl;
    }

    void setBalance(double newBalance) { balance = newBalance; }
};//end account class

// ===================================================
//  Class ATM
// ===================================================
class ATM {
private:
    vector<Account> accounts;
    Account* currentAccount = nullptr;

    template<typename T>
    bool safeInput(T &out) {
        cin >> out;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return false;
        }
        return true;
    }

public:
    ATM() {
        loadAccounts();
    }

    ~ATM() {
        saveAccounts();
    }

    void run() {
        cout << "=============================\n";
        cout << "      Welcome to ATM!      \n";
        cout << "=============================\n";

        if (!login()) {
            cout << " Too many failed attempts. Exiting program.\n";
            return;
        }

        int choice = 0;
        do {
            showMenu();
            cout << "Enter your choice: ";
            if (!safeInput(choice)) {
                cout << " Invalid input. Please enter a number.\n";
                continue;
            }

            switch (choice) {
                case 1:
                    currentAccount->showBalance();
                    break;
                case 2:
                    handleWithdraw();
                    break;
                case 3:
                    handleDeposit();
                    break;
                case 4:
                    cout << " Thank you for using our ATM.\n";
                    break;
                default:
                    cout << " Invalid choice. Try again.\n";
            }
        } while (choice != 4);
    }

private:
    //  function to upload account
    void loadAccounts() {
        ifstream file("accounts.txt");
        if (!file.is_open()) {
            cout << " accounts.txt not found — creating default users.\n";
            accounts.emplace_back("khalil", "1234", 1000.0);
            accounts.emplace_back("ahmad", "1111", 750.0);
            accounts.emplace_back("sara", "0000", 1200.0);
            saveAccounts(); // إنشاء الملف الجديد
            return;
        }

        string name, pass;
        double bal;
        while (file >> name >> pass >> bal) {
            accounts.emplace_back(name, pass, bal);
        }

        file.close();
    }

    //  function to save accounts
    void saveAccounts() {
        ofstream file("accounts.txt");
        for (auto &acc : accounts) {
            file << acc.getUsername() << " " << acc.getPassword()
                 << " " << acc.getBalance() << endl;
        }
        file.close();
    }

    //  function to register process in file
    void logTransaction(const string &type, double amount, double newBalance) {
        ofstream log("transactions.txt", ios::app);
        if (!log.is_open()) return;

        // الحصول على الوقت الحالي
        time_t now = time(0);
        char* dt = ctime(&now);
        dt[strcspn(dt, "\n")] = 0; // إزالة السطر الجديد

        log << currentAccount->getUsername() << " "
            << type << " $" << amount
            << " NewBalance=" << newBalance
            << " on " << dt << endl;

        log.close();
    }

    bool login() {
        string name, pass;
        int attempts = 0;

        while (attempts < 3) {
            cout << "\nEnter username: ";
            cin >> name;
            cout << "Enter password: ";
            cin >> pass;

            for (auto &acc : accounts) {
                if (acc.getUsername() == name && acc.checkPassword(pass)) {
                    currentAccount = &acc;
                    cout << " Login successful! Welcome, " << name << ".\n";
                    return true;
                }
            }

            attempts++;
            cout << " Incorrect username or password. Attempts left: " << (3 - attempts) << endl;
        }

        return false;
    }

    void showMenu() const {
        cout << "\n===== Main Menu =====\n";
        cout << "1. Check Balance\n";
        cout << "2. Withdraw Money\n";
        cout << "3. Deposit Money\n";
        cout << "4. Exit\n";
        cout << "=====================\n";
    }

    void handleWithdraw() {
        double amount;
        cout << "Enter amount to withdraw: ";
        if (!safeInput(amount)) {
            cout << " Invalid input.\n";
            return;
        }

        double oldBalance = currentAccount->getBalance();
        currentAccount->withdraw(amount);
        if (currentAccount->getBalance() != oldBalance)
            logTransaction("Withdraw", amount, currentAccount->getBalance());
    }

    void handleDeposit() {
        double amount;
        cout << "Enter amount to deposit: ";
        if (!safeInput(amount)) {
            cout << " Invalid input.\n";
            return;
        }

        double oldBalance = currentAccount->getBalance();
        currentAccount->deposit(amount);
        if (currentAccount->getBalance() != oldBalance)
            logTransaction("Deposit", amount, currentAccount->getBalance());
    }
};

// ===================================================
//  Main
// ===================================================
int main() {
    ATM atm;
    atm.run();
    return 0;
}
