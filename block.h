#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include "transactions.h"
#include "merkleRootHash.h"

class Block {
private:
    std::string blockID;
    std::vector<Transaction> transactions;
    std::string currentHash;
    std::string previousHash;
    std::string merkleRootHash;
    int nonce;
    int difficultyTarget;
    std::string timestamp; // Stores the timestamp
    std::string version; // Stores the version of the blockchain

public:
    Block(const std::string& previousHash, const std::vector<Transaction>& transactions, int difficultyTarget);

    std::string calculateBlockHash() const;
    std::string getBlockID() const;
    int getDifficulty() const;
    int getNonce() const;
    int getNumTransactions() const;
    std::string getMerkleRootHash() const;
    const std::vector<Transaction>& getTransactions() const;
    std::string getTimestamp() const; // Getter for timestamp
    std::string getVersion() const; // Getter for version
    void mineBlock();
};

#endif // BLOCK_H
