#ifndef BLOCK_H
#define BLOCK_H

#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <iostream>
#include "transactions.h"
#include "merkleRootHash.h"
#include "user.h"
#include <climits>

class Block {
private:
    std::string previousHash;
    std::string timestamp;
    std::string version;
    std::string merkleRootHash;
    int nonce;
    int difficultyTarget;
    std::vector<Transaction> transactions;

    std::string getCurrentTime() const {
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

public:
    Block(const std::string& prevHash, const std::vector<Transaction>& txs, const std::string& version, int difficulty)
        : previousHash(prevHash), transactions(txs), version(version), nonce(0), difficultyTarget(difficulty) {
        MerkleTree merkleTree(transactions);
        merkleRootHash = merkleTree.createMerkleRootHash();
        timestamp = getCurrentTime();
    }

std::string mineBlock() {
    std::string blockHash;
    bool found = false;
    #pragma omp parallel for
    for (int i = 0; i < INT_MAX && !found; ++i) {
        std::string tempHash = processHashInput(previousHash + timestamp + std::to_string(i) + merkleRootHash);
        if (tempHash.substr(0, difficultyTarget) == std::string(difficultyTarget, '0')) {
            #pragma omp critical
            {
                if (!found) {
                    found = true;
                    nonce = i;
                    blockHash = tempHash;
                }
            }
        }
    }
    return blockHash;
}


    const std::vector<Transaction>& getTransactions() const { return transactions; }
    std::string getMerkleRootHash() const { return merkleRootHash; }
    int getNonce() const { return nonce; }
    std::string getPreviousHash() const { return previousHash; }
    std::string getBlockHash() const { return processHashInput(previousHash + timestamp + std::to_string(nonce) + merkleRootHash); }
};

class Blockchain {
private:
    std::vector<Block> chain;
    std::vector<Transaction> transactionPool;
    std::vector<User> users;
    int difficulty;

    std::vector<Transaction> selectRandomTransactions() {
        std::vector<Transaction> selectedTransactions;
        if (transactionPool.size() <= 10) {
            return transactionPool;
        }

        std::vector<Transaction> shuffledPool = transactionPool;
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(shuffledPool.begin(), shuffledPool.end(), g);

        selectedTransactions.insert(selectedTransactions.end(), shuffledPool.begin(), shuffledPool.begin() + 100);
        return selectedTransactions;
    }

    void updateBalances(const std::vector<Transaction>& transactions) {
        for (const auto& tx : transactions) {
            for (auto& user : users) {
                if (user.getPublicKey() == tx.GetSenderPublicKey()) {
                    user.addUtxo(UTXO(-tx.GetSentAmount(), user.getPublicKey())); // Deduct balance
                }
                if (user.getPublicKey() == tx.GetReceiverPublicKey()) {
                    user.addUtxo(UTXO(tx.GetSentAmount(), user.getPublicKey())); // Add balance
                }
            }
        }
    }

    void removeProcessedTransactions(const std::vector<Transaction>& minedTransactions) {
        for (const auto& tx : minedTransactions) {
            auto it = std::remove_if(transactionPool.begin(), transactionPool.end(),
                                    [&tx](const Transaction& t) { return t.GetTransactionID() == tx.GetTransactionID(); });
            transactionPool.erase(it, transactionPool.end());
        }
    }

public:
    Blockchain(int diff) : difficulty(diff) {}

    void generateInitialData(int userCount, int transactionCount) {
        users = generateUsers(userCount);
        transactionPool = generateTransactions(users, transactionCount);
    }

void mineBlocks() {
    std::string previousBlockHash = "0000000000000000000"; // Placeholder for the first block
    int blockCount = 1;

    while (!transactionPool.empty()) {
        std::cout << "Mining Block #" << blockCount << "..." << std::endl;

        // Step 3: Select 100 transactions randomly
        std::vector<Transaction> selectedTransactions = selectRandomTransactions();
        std::cout << "Selected " << selectedTransactions.size() << " transactions." << std::endl;

        // Safety check: Ensure transactions were selected
        if (selectedTransactions.empty()) {
            std::cout << "Error: No transactions selected. Ending mining process." << std::endl;
            break;
        }

        // Check that each selected transaction is valid
        for (const auto& tx : selectedTransactions) {
            if (tx.GetTransactionID().empty()) {
                std::cout << "Warning: Transaction with empty ID detected." << std::endl;
            }
            if (tx.GetSenderPublicKey().empty() || tx.GetReceiverPublicKey().empty()) {
                std::cout << "Warning: Transaction with missing sender or receiver public key." << std::endl;
            }
        }

        // Step 4: Attempt to create a new block with the selected transactions
        std::cout << "Attempting to create a new block..." << std::endl;
        Block newBlock(previousBlockHash, selectedTransactions, "1.0.0", difficulty);

        // Verify Merkle Root creation
        std::string merkleRoot = newBlock.getMerkleRootHash();
        if (merkleRoot.empty()) {
            std::cout << "Error: Failed to generate Merkle root. Skipping block creation." << std::endl;
            continue; // Skip this block if there's an issue
        }
        std::cout << "Merkle Root generated: " << merkleRoot << std::endl;

        // Step 5: Mine the block
        std::cout << "Mining block, please wait..." << std::endl;
        std::string newBlockHash = newBlock.mineBlock();

        // Check if mining was successful
        if (newBlockHash.substr(0, difficulty) != std::string(difficulty, '0')) {
            std::cout << "Error: Mining failed to meet the difficulty target." << std::endl;
            continue;
        }
        std::cout << "Block mined successfully! Hash: " << newBlockHash << std::endl;

        // Print mined block details
        std::cout << "New Block Mined!" << std::endl;
        std::cout << "Block Hash: " << newBlockHash << std::endl;
        std::cout << "Merkle Root: " << merkleRoot << std::endl;

        // Step 6: Update balances and remove processed transactions
        std::cout << "Updating user balances..." << std::endl;
        updateBalances(selectedTransactions);

        std::cout << "Removing processed transactions from the pool..." << std::endl;
        size_t initialPoolSize = transactionPool.size();
        removeProcessedTransactions(selectedTransactions);
        size_t newPoolSize = transactionPool.size();
        std::cout << "Transactions removed: " << (initialPoolSize - newPoolSize) << std::endl;

        // Add the block to the blockchain
        std::cout << "Adding new block to the blockchain..." << std::endl;
        chain.push_back(newBlock);

        // Set this block's hash as the previous block's hash for the next block
        previousBlockHash = newBlockHash;

        std::cout << "Remaining transactions in pool: " << transactionPool.size() << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
        blockCount++;
    }

    // Summary at the end of mining
    if (transactionPool.empty()) {
        std::cout << "All transactions processed. Mining completed successfully." << std::endl;
    } else {
        std::cout << "Mining ended with " << transactionPool.size() << " transactions still in the pool." << std::endl;
    }
}


    const std::vector<Block>& getChain() const { return chain; }
};

#endif // BLOCK_H
