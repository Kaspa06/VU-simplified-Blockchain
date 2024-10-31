#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Transaction {
private:
    std::string transactionID;
    std::string senderPublicKey;
    std::string receiverPublicKey;
    int amount;

public:
    Transaction(const std::string& transactionID, const std::string& senderPublicKey,
                const std::string& receiverPublicKey, int amount);
    std::string getTransactionID() const;
    std::string getSenderPublicKey() const;
    std::string getReceiverPublicKey() const;
     bool operator==(const Transaction& other) const {
        return this->getTransactionID() == other.getTransactionID();
    }
    int getAmount() const;
};

#endif // TRANSACTION_H
