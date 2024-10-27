#ifndef HASHUTILS_H
#define HASHUTILS_H

#include <string>

class HashUtils {
public:
    static std::string processHashInput(const std::string& input);
    static std::string modifyInput(std::string& input);
    static std::string inputToBits(std::string& input);
    static char incrementHexChar(char hexChar);
    static std::string bitsToHex(std::string& bits, const std::string& originalInput);
    static int computeWordSum(const std::string& input);
    static std::string multiplyBitsByWordSum(std::string bits, int wordSum);
    static std::string applySaltTransformation(std::string& bits, const std::string& input);
};

#endif // HASHUTILS_H
