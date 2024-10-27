#ifndef MERKLEROOTHASH_H
#define MERKLEROOTHASH_H

#include <vector>
#include <string>
#include "hash.h"
#include "transactions.h"

class MerkleTree {
private:
    std::vector<Transaction> transactions;

    // Combines and hashes two transaction hashes
    std::string hashCombinedTransactions(const std::string& left, const std::string& right) {
        return HashUtils::processHashInput(left + right);
    }

public:
    // Constructor to initialize with a list of transactions
    MerkleTree(const std::vector<Transaction>& txs) : transactions(txs) {}

    // Creates the Merkle root hash
    std::string createMerkleRootHash() {
        if (transactions.empty()) {
            return HashUtils::processHashInput("Empty Tree Placeholder Hash");
        }

        std::vector<std::string> tree;
        for (const auto& transaction : transactions) {
            tree.push_back(transaction.getTransactionID());  // Ensure correct function name
        }

        while (tree.size() > 1) {
            std::vector<std::string> newLevel;
            for (size_t i = 0; i < tree.size(); i += 2) {
                if (i + 1 < tree.size()) {
                    newLevel.push_back(hashCombinedTransactions(tree[i], tree[i + 1]));
                } else {
                    newLevel.push_back(hashCombinedTransactions(tree[i], tree[i]));
                }
            }
            tree = newLevel;
        }

        return tree.front();
    }
};

#endif // MERKLEROOTHASH_H
