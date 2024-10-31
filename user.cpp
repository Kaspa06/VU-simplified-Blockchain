#include "user.h"

User::User(const std::string& name, const std::string& publicKey, int balance)
    : name(name), publicKey(publicKey), balance(balance) {}

std::string User::getName() const { return name; }
std::string User::getPublicKey() const { return publicKey; }
int User::getBalance() const { return balance; }
void User::updateBalance(int amount) { balance += amount; }