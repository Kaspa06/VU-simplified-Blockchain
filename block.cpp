#include "block.h"
#include "hash.h" // Include for `processHashInput`
#include <iostream>
#include <ctime>

Block::Block(const std::string& previousHash, const std::vector<Transaction>& transactions, int difficultyTarget)
    : previousHash(previousHash), transactions(transactions), difficultyTarget(difficultyTarget) {
    this->timestamp = std::to_string(std::time(0)); // Initialize timestamp with current Unix time
    this->nonce = 0;
    MerkleTree merkleTree(transactions);
    this->merkleRootHash = merkleTree.createMerkleRootHash(); // Use existing MerkleTree class
    this->version = "1.0"; // Example version, change as needed
}

std::string Block::calculateBlockHash() const {
    std::string toHash = previousHash + merkleRootHash + std::to_string(nonce);
    return HashUtils::processHashInput(toHash);
}

void Block::mineBlock() {
    nonce = 0;
    std::string target(difficultyTarget, '0'); // Set target to a string with 'difficulty' number of zeros

    // Adjust loop to incrementally find a valid hash
    while (true) {
        currentHash = calculateBlockHash(); // Calculate hash using the current nonce
        if (currentHash.substr(0, difficultyTarget) == target) {
            std::cout << "Block mined! Nonce: " << nonce << ", Hash: " << currentHash << std::endl;
            break;
        } else {
            nonce++;
            if (nonce % 100000 == 0) {
                std::cout << "Still mining... Current Nonce: " << nonce << ", Current Hash: " << currentHash << std::endl;
            }
        }
    }
}

std::string Block::getBlockID() const {
    return blockID;
}

std::string Block::getTimestamp() const {
    return timestamp;
}

int Block::getDifficulty() const {
    return difficultyTarget;
}

 std::string Block::getMerkleRootHash() const { 
    return merkleRootHash; 
    }

int Block::getNonce() const {
    return nonce;
}

int Block::getNumTransactions() const {
    return transactions.size();
}

const std::vector<Transaction>& Block::getTransactions() const {
    return transactions;
}
