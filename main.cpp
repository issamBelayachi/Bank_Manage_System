#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cstdlib>
#include <chrono> // For std::chrono
#include <thread> // For std::this_thread::sleep_for

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

struct Client {
    string account_id;
    string name;
    string email;
    string password;
    double balance;
};

void saveClientsToCSV(const vector<Client>& clients, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open the file " << filename << endl;
        return;
    }

    for (const Client& client : clients) {
        file << client.account_id << "," << client.name << "," << client.email << "," << client.password << "," << client.balance << endl;
    }

    file.close();
}

Client getClientByAccountID(const string& account_id, const string& filename) {
    ifstream file(filename);
    Client client;
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        vector<string> tokens;
        string token;
        while (getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() >= 4 && tokens[0] == account_id) {
            client.account_id = tokens[0];
            client.name = tokens[1];
            client.email = tokens[2];
            client.password = tokens[3];
            client.balance = stod(tokens[4]);
            break;
        }
    }
    file.close();
    return client;
}

bool updateBalanceByAccountID(vector<Client>& clients, const string& account_id, double new_balance, const string& filename) {
    for (Client& client : clients) {
        if (client.account_id == account_id) {
            client.balance = new_balance;
            saveClientsToCSV(clients, filename);
            return true;
        }
    }
    return false; // Account ID not found
}

void loadClientsFromCSV(vector<Client>& clients, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open the file " << filename << endl;
        return;
    }

    clients.clear(); // Clear the existing data

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        vector<string> tokens;
        string token;
        while (getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() >= 5) {
            Client client;
            client.account_id = tokens[0];
            client.name = tokens[1];
            client.email = tokens[2];
            client.password = tokens[3];
            client.balance = stod(tokens[4]);
            clients.push_back(client);
        }
    }
    file.close();
}

bool isAccountIDValid(const string& account_id, const string& filename) {
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        vector<string> tokens;
        string token;

        while (getline(iss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() >= 1 && tokens[0] == account_id) {
            file.close();
            return true; // Account ID is valid
        }
    }
    file.close();
    return false; // Account ID is not valid
}

Client login(const string& account_id, const string& password, const string& filename) {
    if (isAccountIDValid(account_id, filename)) {
        Client client = getClientByAccountID(account_id, filename);
        if (client.password == password) {
            // Login successful
            cout << "Login successful!" << endl;
            this_thread::sleep_for(chrono::milliseconds(2000));
            clearScreen();
            return client;
        }
        else {
            // Wrong password
            cout << "Wrong password!" << endl;
            clearScreen();
            return Client(); // Return an empty client to indicate login failure
        }
    }
    else {
        // Account not found
        cout << "Account not found!" << endl;
        clearScreen();
        return Client(); // Return an empty client to indicate login failure
    }
}

void deposit(vector<Client>& clients, Client& client, double amount, const string& filename) {
    if (amount > 0) {
        client.balance += amount;
        updateBalanceByAccountID(clients, client.account_id, client.balance, filename);
        cout << "Deposit successful! New balance: " << client.balance << endl;
    }
    else {
        cout << "Invalid deposit amount." << endl;
    }
}

void withdraw(vector<Client>& clients, Client& client, double amount, const string& filename) {
    if (amount > 0 && amount <= client.balance) {
        client.balance -= amount;
        updateBalanceByAccountID(clients, client.account_id, client.balance, filename);
        cout << "Withdrawal successful! New balance: " << client.balance << endl;
    }
    else {
        cout << "Invalid withdrawal amount." << endl;
    }
}

void checkDetails(const Client& client) {
    cout << "Account ID: " << client.account_id << endl;
    cout << "Name: " << client.name << endl;
    cout << "Email: " << client.email << endl;
    cout << "Balance: " << client.balance << endl;
}

string generateAccountID() {
    const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    const int length = 6;
    string account_id;
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < length; ++i) {
        account_id += characters[rand() % characters.length()];
    }
    return account_id;
}

int main() {
    const string filename = "client_data.csv";
    vector<Client> clients;
    int choice;
    int choicee;
    loadClientsFromCSV(clients, filename);


    while (true) {
        cout << "Choose an option:" << endl;
        cout << "1. Register as a new client" << endl;
        cout << "2. Login" << endl;
        cout << "3. Exit" << endl;
        cout << "Your Choice: ";
        cin >> choice;

        if (choice == 1) {
            clearScreen();
            Client newClient;
            cout << "Enter your full name: ";
            cin.ignore();
            getline(cin, newClient.name);
            cout << "Enter your email: ";
            cin >> newClient.email;
            newClient.account_id = generateAccountID();
            cout << "Your account ID: " << newClient.account_id << endl;
            cout << "Enter your password: ";
            cin >> newClient.password;
            newClient.balance = 0.0; // Initialize balance to zero
            clients.push_back(newClient);
            saveClientsToCSV(clients, filename);
            cout << "Registration successful!" << endl;
            cout << "---------------------------------" << endl;
            cout << "Go back: 1" << endl;
            cout << "Exit: 0" << endl;
            cout << "Your choice: ";
            cin >> choicee;
            if (choicee == 1) {
                continue;
            }
            else if (choicee == 0) {
                break;
            }
            else {
                cout << "Invalid option ";
                break;
            }
        }
        else if (choice == 2) {
            clearScreen();
            string accountToCheck, password;
            cout << "Enter your account ID: ";
            cin >> accountToCheck;
            cout << "Enter your password: ";
            cin >> password;
            Client loggedInClient = login(accountToCheck, password, filename);
            if (loggedInClient.account_id != "") {
                while (true) {
                    int innerChoice;
                    clearScreen();
                    cout << "Choose an option:" << endl;
                    cout << "1. Deposit" << endl;
                    cout << "2. Withdraw" << endl;
                    cout << "3. Check Account Details" << endl;
                    cout << "4. Log Out" << endl;
                    cout << "Your Choice: ";
                    cin >> innerChoice;
                    if (innerChoice == 1) {
                        clearScreen();
                        double amount;
                        cout << "Enter the amount to deposit: ";
                        cin >> amount;
                        deposit(clients, loggedInClient, amount, filename);
                        cout << "---------------------------------" << endl;
                        cout << "Go back: 1" << endl;
                        cout << "Exit: 0" << endl;
                        cout << "Your choice: ";
                        cin >> choicee;
                        if (choicee == 1) {
                            clearScreen();
                            continue;
                        }
                        else if (choicee == 0) {
                            clearScreen();
                            break;
                        }
                        else {
                            cout << "Invalid option ";
                            break;
                        }
                    }
                    else if (innerChoice == 2) {
                        clearScreen();
                        double amount;
                        cout << "Enter the amount to withdraw: ";
                        cin >> amount;
                        withdraw(clients, loggedInClient, amount, filename);
                        cout << "---------------------------------" << endl;
                        cout << "Go back: 1" << endl;
                        cout << "Exit: 0" << endl;
                        cout << "Your choice: ";
                        cin >> choicee;
                        if (choicee == 1) {
                            clearScreen();
                            continue;
                        }
                        else if (choicee == 0) {
                            clearScreen();
                            break;
                        }
                        else {
                            cout << "Invalid option ";
                            break;
                        }
                    }
                    else if (innerChoice == 3) {
                        clearScreen();
                        checkDetails(loggedInClient);
                        cout << "---------------------------------" << endl;
                        cout << "Go back: 1" << endl;
                        cout << "Exit: 0" << endl;
                        cout << "Your choice: ";
                        cin >> choicee;
                        if (choicee == 1) {
                            clearScreen();
                            continue;
                        }
                        else if (choicee == 0) {
                            clearScreen();
                            break;
                        }
                        else {
                            cout << "Invalid option ";
                            break;
                        }
                    }
                    else if (innerChoice == 4) {
                        clearScreen();
                        break; // Log out and return to the main menu
                    }
                    else {
                        cout << "Invalid choice." << endl;
                    }
                }
            }
        }
        else if (choice == 3) {
            break;
        }
        else {
            cout << "Invalid choice." << endl;
        }
    }

    return 0;
}
