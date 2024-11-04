#include "block.h"
#include "hash.h" // Include for `processHashInput`
#include <iostream>
#include <sstream>
#include <ctime>
#include <omp.h>

Block::Block(const std::string& previousHash, const std::vector<Transaction>& transactions, int difficultyTarget)
    : previousHash(previousHash), transactions(transactions), difficultyTarget(difficultyTarget) {
    this->timestamp = std::to_string(std::time(0)); // Initialize timestamp with current Unix time
    this->nonce = 0;
    MerkleTree merkleTree(transactions);
    this->merkleRootHash = merkleTree.createMerkleRootHash(); // Use existing MerkleTree class
    this->version = "1.0"; // Example version, change as needed
}

std::string Block::calculateBlockHash() const {
    std::stringstream ss;
    ss << previousHash << timestamp << merkleRootHash << nonce << difficultyTarget;
    return HashUtils::processHashInput(ss.str());
}

void Block::mineBlock() {
    std::string target(difficultyTarget, '0'); // Create a string with 'difficultyTarget' number of leading zeros

    bool found = false;
    #pragma omp parallel num_threads(4)
    {
        while (!found) {
            #pragma omp critical
            {
                nonce++;
                currentHash = calculateBlockHash();
                if (currentHash.substr(0, difficultyTarget) == target) {
                    blockID = currentHash;
                    found = true;
                }
                if (nonce % 100000 == 0) {
                    std::cout << "Still mining... Current Nonce: " << nonce << ", Current Hash: " << currentHash << std::endl;
                }
            }
        }
    }
}


Block Block::createGenesisBlock() {
    std::vector<Transaction> emptyTransactions;
    std::string genesisPreviousHash = "0000000000000000000000000000000000000000000000000000000000000000";
    Block genesisBlock(genesisPreviousHash, emptyTransactions, 1); // Set minimal difficulty for the genesis block

    genesisBlock.mineBlock(); // Mine the genesis block

    // Print details
    std::cout << "Genesis Block created: " << genesisBlock.getBlockID() << std::endl;
    std::cout << "Timestamp: " << genesisBlock.getTimestamp() << std::endl;
    std::cout << "Version: 2.0" << std::endl;
    std::cout << "Difficulty Target: " << genesisBlock.getDifficulty() << std::endl;
    std::cout << "_______________________________________________________________________________________" << std::endl;

    return genesisBlock;
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