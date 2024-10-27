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

        int maxAmount = users[senderIndex].getBalance();
        int amount = rand() % (maxAmount - 1) + 1; // Ensure the transaction amount is between 1 and maxAmount - 1

        std::string transactionID = HashUtils::processHashInput(
            users[senderIndex].getPublicKey() + users[receiverIndex].getPublicKey() + std::to_string(amount)
        );

        Transaction newTransaction(transactionID, users[senderIndex].getPublicKey(), users[receiverIndex].getPublicKey(), amount);

        // Verify the transaction and add to the pool only if valid
        if (verifyTransaction(newTransaction, users)) {
            transactionPool.push_back(newTransaction);
        }
    }
    std::cout << "Transactions generation completed" << std::endl;
    return transactionPool;
}

std::vector<Block> mineBlockchain(std::vector<Transaction>& transactionPool, std::vector<User>& users) {
    std::vector<Block> blockchain;

    while (!transactionPool.empty()) {
        std::vector<Transaction> validTransactions;
        for (const auto& transaction : transactionPool) {
            // Verify transaction to ensure it's still valid
            bool isBalanceValid = verifyTransaction(transaction, users);
            bool isHashValid = verifyTransactionHash(transaction);

            if (isBalanceValid && isHashValid) {
                // Final check on the balance before adding
                int senderIndex = findUserIndex(users, transaction.getSenderPublicKey());
                if (senderIndex != -1 && users[senderIndex].getBalance() >= transaction.getAmount()) {
                    validTransactions.push_back(transaction);
                } else {
                    std::cout << "Rejected Transaction due to insufficient balance: " << transaction.getTransactionID() << "\n";
                }
            } else {
                std::cout << "Rejected Transaction: " << transaction.getTransactionID() << "\n";
            }
        }

        // Select up to 100 valid transactions for the block
        auto selectedTransactions = selectRandomTransactions(validTransactions, 100);
        selectedTransactions.erase(
            std::remove_if(selectedTransactions.begin(), selectedTransactions.end(), [&](const Transaction& tx) {
                return !verifyTransaction(tx, users);  // Re-check validity
            }), 
            selectedTransactions.end()
        );

        if (selectedTransactions.empty()) {
            break; // Exit the loop if no valid transactions are available
        }

        std::string previousHash = blockchain.empty()
            ? "0000000000000000000000000000000000000000000000000000000000000000"
            : blockchain.back().getBlockID();

        Block newBlock(previousHash, selectedTransactions, 0); // Set difficulty to 0 for faster mining
        newBlock.mineBlock();

        // Update balances after confirming the block's transactions are valid
        updateBalances(newBlock.getTransactions(), users);

        // Remove processed transactions from the pool
        for (const auto& tx : newBlock.getTransactions()) {
            transactionPool.erase(
                std::remove_if(transactionPool.begin(), transactionPool.end(), 
                               [&tx](const Transaction& t) { return t.getTransactionID() == tx.getTransactionID(); }),
                transactionPool.end());
        }

        blockchain.push_back(newBlock);
        std::cout << "Mined new block: " << newBlock.getBlockID() << " | Transactions in pool: " << transactionPool.size() << std::endl;
    }

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
    for (const auto& tx : transactions) {
        int senderIndex = findUserIndex(users, tx.getSenderPublicKey());
        int receiverIndex = findUserIndex(users, tx.getReceiverPublicKey());

        // Ensure the sender and receiver exist and the sender has sufficient funds before processing
        if (senderIndex != -1 && receiverIndex != -1) {
            int senderBalance = users[senderIndex].getBalance();
            int transactionAmount = tx.getAmount();

            if (senderBalance >= transactionAmount) {
                // Deduct from sender and add to receiver
                users[senderIndex].updateBalance(senderBalance - transactionAmount);
                users[receiverIndex].updateBalance(users[receiverIndex].getBalance() + transactionAmount);
            } else {
                std::cout << "Failed to process transaction due to insufficient balance. Transaction ID: " 
                          << tx.getTransactionID() << ", Sender: " << users[senderIndex].getName() 
                          << ", Balance: " << senderBalance 
                          << ", Amount: " << transactionAmount << "\n";
            }
        } else {
            std::cout << "Failed to process transaction due to invalid sender or receiver. Transaction ID: " 
                      << tx.getTransactionID() << "\n";
        }
    }
}

int findUserIndex(const std::vector<User>& users, const std::string& publicKey) {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].getPublicKey() == publicKey) {
            return i;
        }
    }
    return -1;
}

void saveUsersToFile(const std::vector<User>& users, const std::string& filename) {
    std::ofstream file(filename);
    for (const auto& user : users) {
        file << "Name: " << user.getName() << ", Public Key: " << user.getPublicKey() << ", Balance: " << user.getBalance() << "\n";
    }
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

void findBlock(const std::string& blockID, const std::vector<Block>& blockchain) {
    for (const auto& block : blockchain) {
        if (block.getBlockID() == blockID) {
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

void findUser(const std::string& userPublicKey, const std::vector<User>& users) {
    for (const auto& user : users) {
        if (user.getPublicKey() == userPublicKey) {
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
    int receiverIndex = findUserIndex(users, transaction.getReceiverPublicKey());

    if (senderIndex == -1) {
        std::cout << "Invalid Transaction: Sender not found. Transaction ID: " << transaction.getTransactionID() << "\n";
        return false;
    }
    if (receiverIndex == -1) {
        std::cout << "Invalid Transaction: Receiver not found. Transaction ID: " << transaction.getTransactionID() << "\n";
        return false;
    }
    if (users[senderIndex].getBalance() < transaction.getAmount()) {
        std::cout << "Invalid Transaction: Insufficient balance. Transaction ID: " << transaction.getTransactionID() 
                  << ", Sender: " << users[senderIndex].getName() 
                  << ", Balance: " << users[senderIndex].getBalance() 
                  << ", Amount: " << transaction.getAmount() << "\n";
        return false;
    }
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
