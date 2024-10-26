#ifndef HASH_H
#define HASH_H

#include <string>
#include <fstream>

std::string modifyInput(std::string& input);
std::string inputToBits(std::string& input);
char incrementHexChar(char hexChar);
std::string bitsToHex(std::string& bits, const std::string& originalInput);
int computeWordSum(const std::string& input);
std::string multiplyBitsByWordSum(std::string bits, int wordSum);
std::string applySaltTransformation(std::string& bits, const std::string& input);
std::string processHashInput(const std::string& input);

#endif // HASH_H
