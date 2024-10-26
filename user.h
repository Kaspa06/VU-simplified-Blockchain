#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include "hash.h"
#include "transactions.h"  // Ensure this is included

struct UTXO {
    double value;
    std::string id;
    std::string owner;

    UTXO() = default;
    UTXO(double value, const std::string& owner)
        : value(value), owner(owner) {
            id = processHashInput(owner + std::to_string(value));
        }
};

class User {
private:
    std::string name;
    std::string publicKey;
    std::vector<UTXO> utxoList;

public:
    User() = default;
    User(const std::string& name, const std::string& publicKey)
        : name(name), publicKey(publicKey) {}

    std::string getName() const { return name; }
    std::string getPublicKey() const { return publicKey; }
    const std::vector<UTXO>& getUtxoList() const { return utxoList; }

    void setName(const std::string& name) { this->name = name; }
    void setPublicKey(const std::string& publicKey) { this->publicKey = publicKey; }

    double calculateBalance() const {
        return std::accumulate(utxoList.begin(), utxoList.end(), 0.0, 
            [](double total, const UTXO& utxo) { return total + utxo.value; });
    }

    void addUtxo(const UTXO& utxo) {
        utxoList.push_back(utxo);
    }

    void removeUtxo(const std::string& utxoId) {
        utxoList.erase(std::remove_if(utxoList.begin(), utxoList.end(), 
            [&utxoId](const UTXO& u) { return u.id == utxoId; }), utxoList.end());
    }

    void display() const {
        std::cout << "User: " << name << "\n";
        std::cout << "Public Key: " << publicKey << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Balance: " << calculateBalance() << "\n";
        std::cout << "UTXOs:\n";
        for (const auto& utxo : utxoList) {
            std::cout << "  ID: " << utxo.id << ", Value: " << utxo.value << "\n";
        }
    }
};

// Declare utility functions for generating users and transactions
std::vector<User> generateUsers(int count);
std::vector<Transaction> generateTransactions(const std::vector<User>& users, int transactionCount);

#endif // USER_H
