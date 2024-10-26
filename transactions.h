#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "hash.h"

class Transaction {
private:
    std::string transactionID;
    std::string senderPublicKey;
    std::string receiverPublicKey;
    double sentAmount;
    std::string timeStamp;

    std::string generateTransactionID() const {
        return processHashInput(senderPublicKey + receiverPublicKey + std::to_string(sentAmount));
    }

    std::string getCurrentTime() const {
        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

public:
    Transaction() = default;

    Transaction(const std::string& from, const std::string& to, double amount)
        : senderPublicKey(processHashInput(from)), 
          receiverPublicKey(processHashInput(to)), 
          sentAmount(amount) {
        transactionID = processHashInput(senderPublicKey + receiverPublicKey + std::to_string(sentAmount));
        timeStamp = getCurrentTime();
    }

    std::string GetSenderPublicKey() const { return senderPublicKey; }
    std::string GetReceiverPublicKey() const { return receiverPublicKey; }
    std::string GetTransactionID() const { return transactionID; }
    double GetSentAmount() const { return sentAmount; }
    std::string GetTimeStamp() const { return timeStamp; }

    void print(std::ostream& os = std::cout) const {
        os << "Transaction ID: " << transactionID << "\n"
           << "Sender: " << senderPublicKey << "\n"
           << "Receiver: " << receiverPublicKey << "\n"
           << "Amount: " << sentAmount << "\n"
           << "Time: " << timeStamp << "\n";
    }
};

#endif // TRANSACTION_H
