#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string name;
    std::string publicKey;
    int balance;

public:
    User(const std::string& name, const std::string& publicKey, int balance);

    std::string getName() const;
    std::string getPublicKey() const;
    int getBalance() const;
    void updateBalance(int amount); // This method should exist
};

#endif // USER_H
