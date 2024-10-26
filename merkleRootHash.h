#ifndef MERKLEROOTHASH_H
#define MERKLEROOTHASH_H

#include <vector>
#include <string>
#include "hash.h"        // Ensure your hashing functions are included
#include "transactions.h" // Include the Transaction class

class MerkleTree {
private:
    std::vector<Transaction> transactions;

    // Combines and hashes two transaction hashes
    std::string hashCombinedTransactions(const std::string& left, const std::string& right) {
        return processHashInput(left + right); // Use your custom hash function
    }

public:
    // Constructor to initialize with a list of transactions
    MerkleTree(const std::vector<Transaction>& transactions) : transactions(transactions) {}

    // Creates the Merkle root hash
    std::string createMerkleRootHash() {
        if (transactions.empty()) {
            return processHashInput("Empty Tree Placeholder Hash");
        }

        // Convert transaction IDs to their hashes
        std::vector<std::string> tree;
        for (const auto& transaction : transactions) {
            tree.push_back(transaction.GetTransactionID());
        }

        // Generate the Merkle Tree
        while (tree.size() > 1) {
            std::vector<std::string> newLevel;
            for (size_t i = 0; i < tree.size(); i += 2) {
                if (i + 1 < tree.size()) {
                    // Hash pairs of transaction hashes
                    newLevel.push_back(hashCombinedTransactions(tree[i], tree[i + 1]));
                } else {
                    // Handle odd number of transactions by pairing the last hash with itself
                    newLevel.push_back(hashCombinedTransactions(tree[i], tree[i]));
                }
            }
            tree = newLevel; // Move up the tree level
        }

        // The root of the Merkle Tree
        return tree.front();
    }
};

#endif // MERKLEROOTHASH_H
