#include "transactions.h"

Transaction::Transaction(const std::string& transactionID, const std::string& senderPublicKey,
                         const std::string& receiverPublicKey, int amount)
    : transactionID(transactionID), senderPublicKey(senderPublicKey), receiverPublicKey(receiverPublicKey), amount(amount) {}

std::string Transaction::getTransactionID() const { return transactionID; }
std::string Transaction::getSenderPublicKey() const { return senderPublicKey; }
std::string Transaction::getReceiverPublicKey() const { return receiverPublicKey; }
int Transaction::getAmount() const { return amount; }
