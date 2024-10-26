#include "user.h"
#include "transactions.h"
#include <random>
#include <iostream>
#include <fstream>

// Generate random users
std::vector<User> generateUsers(int count) {
    std::vector<User> users;
    for (int i = 0; i < count; ++i) {
        std::string name = "User" + std::to_string(i + 1);
        std::string publicKey = "public_key_" + std::to_string(i + 1);
        User user(name, publicKey);

        // Randomly generate UTXOs for the user
        int utxoCount = rand() % 10 + 1; // Each user gets 1 to 10 UTXOs
        for (int j = 0; j < utxoCount; ++j) {
            double value = static_cast<double>(rand() % 1000 + 1); // Random value between 1 and 1000
            user.addUtxo(UTXO(value, publicKey));
        }

        users.push_back(user);
    }
    return users;
}

std::vector<Transaction> generateTransactions(const std::vector<User>& users, int transactionCount) {
    std::vector<Transaction> transactions;
    std::default_random_engine generator;
    std::uniform_int_distribution<int> userDist(0, users.size() - 1);
    std::uniform_real_distribution<double> amountDist(1.0, 100.0);

    std::ofstream file("transactions.txt"); // Open file for writing
    if (!file) {
        std::cerr << "Error: Could not open transactions.txt for writing." << std::endl;
        return transactions;
    }

    for (int i = 0; i < transactionCount; ++i) {
        int senderIndex = userDist(generator);
        int receiverIndex = userDist(generator);

        // Ensure sender and receiver are not the same
        while (receiverIndex == senderIndex) {
            receiverIndex = userDist(generator);
        }

        double amount = std::min(users[senderIndex].calculateBalance(), amountDist(generator));
        if (amount > 0) {
            Transaction tx(users[senderIndex].getPublicKey(), users[receiverIndex].getPublicKey(), amount);
            transactions.push_back(tx);

            // Write transaction details to the file
            file << "Transaction ID: " << tx.GetTransactionID() << "\n";
            file << "Sender: " << tx.GetSenderPublicKey() << "\n";
            file << "Receiver: " << tx.GetReceiverPublicKey() << "\n";
            file << "Amount: " << tx.GetSentAmount() << "\n";
            file << "Time: " << tx.GetTimeStamp() << "\n";
            file << "\n"; // Add a blank line between transactions
        }
    }

    file.close(); // Close the file
    return transactions;
}
