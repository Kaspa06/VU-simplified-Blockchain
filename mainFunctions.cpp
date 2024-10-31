#include "mainFunctions.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <ctime>

std::vector<User> generateUsers(int userNumber) {
    std::vector<User> users;
    std::cout << "Generating " << userNumber << " users" << std::endl;
    for (int i = 1; i <= userNumber; ++i) {
        std::string name = "User" + std::to_string(i);
        std::string publicKey = HashUtils::processHashInput(name);
        int balance = rand() % 1000000 + 100; // Random initial balance
        users.emplace_back(name, publicKey, balance);
    }
    std::cout << "User generation completed" << std::endl;
    return users;
}

std::vector<Transaction> generateTransactions(int transactionNumber, std::vector<User>& users) {
    std::vector<Transaction> transactionPool;
    std::cout << "Generating " << transactionNumber << " transactions" << std::endl;
    for (int i = 0; i < transactionNumber; ++i) {
        int senderIndex, receiverIndex;
        do {
            senderIndex = rand() % users.size();
        } while (users[senderIndex].getBalance() < 100); // Ensure the sender has at least 100 units

        do {
            receiverIndex = rand() % users.size();
        } while (senderIndex == receiverIndex);

        int amount = rand() % (users[senderIndex].getBalance() - 1) + 1; // Ensure the transaction amount is valid
        std::string transactionID = HashUtils::processHashInput(users[senderIndex].getPublicKey() + users[receiverIndex].getPublicKey() + std::to_string(amount));
        transactionPool.emplace_back(transactionID, users[senderIndex].getPublicKey(), users[receiverIndex].getPublicKey(), amount);
    }
    std::cout << "Transactions generation completed" << std::endl;
    return transactionPool;
}

Block createBlock(const std::vector<Transaction>& transactions, const std::string& previousHash) {
    Block newBlock(previousHash, transactions, 0); // Set difficulty to 0 for faster mining
    newBlock.mineBlock();
    return newBlock;
}

std::vector<Block> mineBlockchain(std::vector<Transaction>& transactionPool, std::vector<User>& users) {
    std::vector<Block> blockchain;
    const int maxTransactionsPerBlock = 100;
    std::ofstream failedTransactionsFile("failedTransactions.txt");
    int minedBlockIndex = 1; // Move initialization outside the loop

    while (!transactionPool.empty()) {
        std::vector<Transaction> validTransactions;

        for (auto it = transactionPool.begin(); it != transactionPool.end() && validTransactions.size() < maxTransactionsPerBlock;) {
            const auto& transaction = *it;
            std::string senderPublicKey = transaction.getSenderPublicKey();
            std::string receiverPublicKey = transaction.getReceiverPublicKey();
            double amount = transaction.getAmount();

            int senderIndex = findUserIndex(users, senderPublicKey);
            int receiverIndex = findUserIndex(users, receiverPublicKey);

            if (senderIndex != -1 && receiverIndex != -1 && users[senderIndex].getBalance() >= amount) {
                validTransactions.push_back(transaction);
                users[senderIndex].updateBalance(-amount);
                users[receiverIndex].updateBalance(amount);
                it = transactionPool.erase(it); // Remove valid transaction from the pool
            } else {
                failedTransactionsFile << "Rejected Transaction due to insufficient balance or invalid user: " << transaction.getTransactionID() << "\n";
                it = transactionPool.erase(it); // Remove invalid transaction from the pool
            }
        }

        if (validTransactions.empty()) {
            break; // Exit the loop if no valid transactions are available
        }

        std::string previousHash = blockchain.empty()
            ? "0000000000000000000000000000000000000000000000000000000000000000"
            : blockchain.back().getBlockID();

        Block newBlock = createBlock(validTransactions, previousHash);
        blockchain.push_back(newBlock);

        std::cout << minedBlockIndex << " Mined new block: " << newBlock.getBlockID() << " | Transactions in pool: " << transactionPool.size() << std::endl;

        // Save updated user balances to file
        saveUsersToFile(users, "users.txt");
        minedBlockIndex++; // Increment the index correctly
    }

    failedTransactionsFile.close();
    return blockchain;
}

std::vector<Transaction> selectRandomTransactions(const std::vector<Transaction>& transactions, int count) {
    std::vector<Transaction> selectedTransactions;
    if (transactions.size() <= count) {
        return transactions; // Return all if fewer than required
    }

    std::vector<int> indices(transactions.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::random_shuffle(indices.begin(), indices.end());

    for (int i = 0; i < count; ++i) {
        selectedTransactions.push_back(transactions[indices[i]]);
    }

    return selectedTransactions;
}

void updateBalances(const std::vector<Transaction>& transactions, std::vector<User>& users) {
    for (const auto& transaction : transactions) {
        int senderIndex = findUserIndex(users, transaction.getSenderPublicKey());
        int receiverIndex = findUserIndex(users, transaction.getReceiverPublicKey());

        if (senderIndex != -1) {
            users[senderIndex].updateBalance(-transaction.getAmount());
        }

        if (receiverIndex != -1) {
            users[receiverIndex].updateBalance(transaction.getAmount());
        }
    }
}

int findUserIndex(const std::vector<User>& users, const std::string& publicKey) {
    auto it = std::find_if(users.begin(), users.end(), [&publicKey](const User& user) {
        return user.getPublicKey() == publicKey;
    });
    return (it != users.end()) ? std::distance(users.begin(), it) : -1;
}

void saveUsersToFile(const std::vector<User>& users, const std::string& filename) {
    std::ofstream file(filename);
    double totalBalance = 0.0;
    for (const auto& user : users) {
        file << "Name: " << user.getName() << ", Public Key: " << user.getPublicKey() << ", Balance: " << user.getBalance() << "\n";
        totalBalance += user.getBalance();
    }
    file << "Total Balance: " << totalBalance << "\n";
    file.close();
}

void saveTransactionsToFile(const std::vector<Transaction>& transactions, const std::string& filename) {
    std::ofstream file(filename);
    for (const auto& tx : transactions) {
        file << "\nTransaction ID: " << tx.getTransactionID() 
             << "\nSender: " << tx.getSenderPublicKey() 
             << "\nReceiver: " << tx.getReceiverPublicKey() 
             << "\nAmount: " << tx.getAmount() << "\n";
    }
    file.close();
}

void saveBlocksToFile(const std::vector<Block>& blockchain, const std::string& filename) {
    std::ofstream file(filename);
    for (const auto& block : blockchain) {
        file << "Block ID: " << block.getBlockID() 
             << "\nBlock Hash: " << block.calculateBlockHash() 
             << "\nMerkle Root Hash: " << block.getMerkleRootHash()
             << "\nTimestamp: " << block.getTimestamp() 
             << "\nDifficulty Target: " << block.getDifficulty()
             << "\nNonce: " << block.getNonce()
             << "\nNumber of Transactions: " << block.getNumTransactions()
             << "\nTransactions:\n";

        int totalAmount = 0;
        for (const auto& tx : block.getTransactions()) {
            file << "Transaction ID: " << tx.getTransactionID() << "\n";
            totalAmount += tx.getAmount();
        }

        file << "Total Amount: " << totalAmount << "\n";
        file << "-- End of Block --\n";
    }
    file.close();
}

void findBlock(const std::string& searchHash, const std::vector<Block>& blockchain) {
    for (const auto& block : blockchain) {
        if (block.getBlockID() == searchHash || block.getMerkleRootHash() == searchHash) {
            std::cout << "Block found: " << block.getBlockID() << "\n";
            std::cout << "Block Hash: " << block.calculateBlockHash() << "\n";
            std::cout << "Merkle Root Hash: " << block.getMerkleRootHash() << "\n";
            std::cout << "Timestamp: " << block.getTimestamp() << "\n";
            std::cout << "Difficulty Target: " << block.getDifficulty() << "\n";
            std::cout << "Nonce: " << block.getNonce() << "\n";
            std::cout << "Number of Transactions: " << block.getNumTransactions() << "\n";
            std::cout << "Transactions:\n";
            for (const auto& tx : block.getTransactions()) {
                std::cout << "Transaction ID: " << tx.getTransactionID() << "\n";
            }
            return;
        }
    }
    std::cout << "Block not found.\n";
}

void findTransaction(const std::string& transactionID, const std::vector<Block>& blockchain) {
    for (const auto& block : blockchain) {
        for (const auto& transaction : block.getTransactions()) {
            if (transaction.getTransactionID() == transactionID) {
                std::cout << "Transaction found:\n";
                std::cout << "Transaction ID: " << transaction.getTransactionID() << "\n";
                std::cout << "Sender: " << transaction.getSenderPublicKey() << "\n";
                std::cout << "Receiver: " << transaction.getReceiverPublicKey() << "\n";
                std::cout << "Amount: " << transaction.getAmount() << "\n";
                return;
            }
        }
    }
    std::cout << "Transaction not found.\n";
}

void findUser(const std::string& userName, const std::vector<User>& users) {
    for (const auto& user : users) {
        if (user.getName() == userName) {
            std::cout << "User found:\n";
            std::cout << "Name: " << user.getName() << "\n";
            std::cout << "Public Key: " << user.getPublicKey() << "\n";
            std::cout << "Balance: " << user.getBalance() << "\n";
            return;
        }
    }
    std::cout << "User not found.\n";
}

bool verifyTransaction(const Transaction& transaction, const std::vector<User>& users) {
    int senderIndex = findUserIndex(users, transaction.getSenderPublicKey());
    if (senderIndex == -1) {
        return false; // Sender not found
    }

    // Check if the sender's balance is sufficient
    if (users[senderIndex].getBalance() < transaction.getAmount()) {
        return false; // Insufficient balance
    }

    // Additional verification logic (e.g., signature verification) can be added here

    return true;
}

bool verifyTransactionHash(const Transaction& transaction) {
    std::string expectedHash = HashUtils::processHashInput(
        transaction.getSenderPublicKey() + transaction.getReceiverPublicKey() + std::to_string(transaction.getAmount())
    );
    if (transaction.getTransactionID() != expectedHash) {
        std::cout << "Invalid Transaction: Hash mismatch. Transaction ID: " << transaction.getTransactionID() 
                  << ", Expected Hash: " << expectedHash << "\n";
        return false;
    }
    return true;
}
