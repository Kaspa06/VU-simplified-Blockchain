#include <iostream>
#include <vector>
#include "hash.h"
#include "transactions.h"
#include "user.h"
#include "block.h"

using namespace std;

int main() {
    vector<User> users;
    vector<Transaction> transactions;
    Blockchain blockchain(2); // Set difficulty for mining

    // Initialize menu
    int choice = 0;

    while (choice != 7) {
        cout << "1. Generate Users" << endl;
        cout << "2. Generate Transactions" << endl;
        cout << "3. Mine New Blocks" << endl;
        cout << "4. Find Block" << endl;
        cout << "5. Find Transaction" << endl;
        cout << "6. Find User" << endl;
        cout << "7. Exit" << endl;
        cin >> choice;

        // Validate input
        if (cin.fail() || choice < 1 || choice > 7) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice! Please enter a valid number (1-7)." << endl;
            continue;
        }

        switch (choice) {
            case 1: {
                size_t userCount;
                cout << "Enter number of users to generate: ";
                cin >> userCount;
                users = generateUsers(userCount);
                cout << userCount << " users generated successfully." << endl;
                break;
            }
            case 2: {
                if (users.empty()) {
                    cout << "Generate users first!" << endl;
                    break;
                }
                size_t transactionCount;
                cout << "Enter number of transactions to generate: ";
                cin >> transactionCount;
                transactions = generateTransactions(users, transactionCount);
                blockchain.generateInitialData(users.size(), transactionCount);
                cout << transactionCount << " transactions generated successfully." << endl;
                break;
            }
            case 3: {
                if (transactions.empty()) {
                    cout << "Generate transactions first!" << endl;
                    break;
                }
                blockchain.mineBlocks();
                cout << "Mining process completed." << endl;
                break;
            }
            case 4: {
                cout << "Enter block hash: ";
                string hash;
                cin >> hash;
                // Find and display block details
                bool found = false;
                for (const auto& block : blockchain.getChain()) {
                    if (block.getBlockHash() == hash) {
                        cout << "Block found!" << endl;
                        cout << "Previous Hash: " << block.getPreviousHash() << endl;
                        cout << "Nonce: " << block.getNonce() << endl;
                        cout << "Merkle Root: " << block.getMerkleRootHash() << endl;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    cout << "Block not found." << endl;
                }
                break;
            }
            case 5: {
                cout << "Enter transaction ID: ";
                string id;
                cin >> id;
                // Search and display transaction details
                bool found = false;
                for (const auto& tx : transactions) {
                    if (tx.GetTransactionID() == id) {
                        cout << "Transaction found!" << endl;
                        cout << "Sender: " << tx.GetSenderPublicKey() << endl;
                        cout << "Receiver: " << tx.GetReceiverPublicKey() << endl;
                        cout << "Amount: " << tx.GetSentAmount() << endl;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    cout << "Transaction not found." << endl;
                }
                break;
            }
            case 6: {
                cout << "Enter user public key: ";
                string publicKey;
                cin >> publicKey;
                // Search and display user information
                bool found = false;
                for (const auto& user : users) {
                    if (user.getPublicKey() == publicKey) {
                        cout << "User found!" << endl;
                        cout << "Public Key: " << user.getPublicKey() << endl;
                        cout << "Balance: " << user.calculateBalance() << endl;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    cout << "User not found." << endl;
                }
                break;
            }
            case 7:
                cout << "Program is closing." << endl;
                break;
        }
    }

    return 0;
}
