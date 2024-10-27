// mainFunctions.h
#ifndef MAINFUNCTIONS_H
#define MAINFUNCTIONS_H

#include "user.h"
#include "transactions.h"
#include "block.h"
#include <vector>
#include <string>
#include <algorithm>

std::vector<Transaction> selectRandomTransactions(const std::vector<Transaction>& transactions, int count);
std::vector<Block> mineBlockchain(std::vector<Transaction>& transactionPool, std::vector<User>& users);
void updateBalances(const std::vector<Transaction>& transactions, std::vector<User>& users);
int findUserIndex(const std::vector<User>& users, const std::string& publicKey);
void saveUsersToFile(const std::vector<User>& users, const std::string& filename);
void saveTransactionsToFile(const std::vector<Transaction>& transactions, const std::string& filename);
void saveBlocksToFile(const std::vector<Block>& blockchain, const std::string& filename);
std::vector<User> generateUsers(int userNumber);
std::vector<Transaction> generateTransactions(int transactionNumber, std::vector<User>& users);
void findBlock(const std::string& blockID, const std::vector<Block>& blockchain);
void findTransaction(const std::string& transactionID, const std::vector<Block>& blockchain);
void findUser(const std::string& userPublicKey, const std::vector<User>& users);
bool verifyTransaction(const Transaction& transaction, const std::vector<User>& users);
bool verifyTransactionHash(const Transaction& transaction);

#endif // MAINFUNCTIONS_H
