// main.cpp
#include <iostream>
#include "mainFunctions.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned int>(time(0)));
    int userNumber = 60, transactionNumber = 2000;

    std::vector<User> users = generateUsers(userNumber);
    saveUsersToFile(users, "users.txt");
    std::vector<Transaction> transactionPool = generateTransactions(transactionNumber, users);
    saveTransactionsToFile(transactionPool, "transactions.txt");

    std::vector<Block> blockchain = mineBlockchain(transactionPool, users);

    saveUsersToFile(users, "usersSort.txt");

    int choice;
    do {
        std::cout << "\nChoose what to do next:\n";
        std::cout << "1. Find Block\n";
        std::cout << "2. Find Transaction\n";
        std::cout << "3. Find User\n";
        std::cout << "4. Exit\n";
        std::cout << "Enter your choice (1-4): ";

        // Input validation
        while (!(std::cin >> choice) || choice < 1 || choice > 4) {
            std::cin.clear(); // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cout << "Invalid input. Please enter a number between 1 and 4: ";
        }

        switch (choice) {
            case 1: {
                std::string blockID;
                std::cout << "Enter Block ID: ";
                std::cin >> blockID;
                findBlock(blockID, blockchain);
                break;
            }
            case 2: {
                std::string transactionID;
                std::cout << "Enter Transaction ID: ";
                std::cin >> transactionID;
                findTransaction(transactionID, blockchain);
                break;
            }
            case 3: {
                std::string userName;
                std::cout << "Enter User Name: ";
                std::cin >> userName;
                findUser(userName, users);
                break;
            }
            case 4:
                std::cout << "Exiting the program...\n";
                break;
        }
    } while (choice != 4);

    return 0;
}
