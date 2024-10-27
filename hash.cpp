#include "hash.h"
#include <bitset>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

// Function to apply transformations to make inputs look more different
std::string HashUtils::modifyInput(std::string& input) {
    for (size_t i = 0; i < input.size(); ++i) {
        if (isupper(input[i]) || input[i] == '!') {
            input[i] ^= (i % 10);  // XOR with position modulo 10
            input[i] += 5;         // Shift ASCII value up by 5
        } else {
            input[i] ^= (i + 3);  // XOR with position plus 3
            input[i] -= 2;        // Shift ASCII value down by 2
        }
    }

    reverse(input.begin(), input.end());

    return input;
}

// Function to convert input string to 256-bit binary
std::string HashUtils::inputToBits(std::string& input) {
    std::string bits;

    while (input.length() < 32) {
        input += "blockchain"; // might even call it SALT in case the input is too short
    }

    // Convert each character to its 8-bit binary representation
    for (char c : input) {
        bits += std::bitset<8>(static_cast<unsigned char>(c)).to_string();
    }

    return bits;
}

// Function to increment hexadecimal characters
char HashUtils::incrementHexChar(char hexChar) {
    if (hexChar >= '0' && hexChar <= '9') {
        return (hexChar == '9') ? '0' : (hexChar + 1);
    } else if (hexChar >= 'a' && hexChar <= 'f') {
        return (hexChar == 'f') ? 'a' : (hexChar + 1);
    }
    return hexChar;
}

// Function to convert bits to hexadecimal hash
std::string HashUtils::bitsToHex(std::string& bits, const std::string& originalInput) {
    // Pad the binary string to be a multiple of 4
    while (bits.size() % 4 != 0) {
        bits = '0' + bits;
    }

    std::stringstream hexStream;
    bool containsUpper = std::any_of(originalInput.begin(), originalInput.end(), ::isupper);
    bool containsExclamation = originalInput.find('!') != std::string::npos;

    // Determine multiplier base
    int multiplierBase = 1;
    if (containsUpper) {
        multiplierBase = originalInput.length(); // Random logic to radically change the output for UpperCase
    } else if (containsExclamation) {
        multiplierBase = originalInput.length() / 2; // Random logic to radically change the output for !
    }

    int rollingValue = 1;
    size_t originalInputLength = originalInput.length();  // Added safeguard to avoid out-of-bounds access

    // Convert 4-bit chunks to hexadecimal
    for (size_t i = 0; i < bits.size(); i += 4) {
        std::string chunk = bits.substr(i, 4);
        int value = std::stoi(chunk, nullptr, 2);

        // Apply transformations
        if (i / 4 < originalInputLength) {  // Ensure no out-of-bounds access here
            int charInfluence = originalInput[i / 4] % 16; // Safeguard: ensure i/4 doesn't exceed the length
            rollingValue += (charInfluence + 1);
        } else {
            rollingValue += 1;  // Default increment if out-of-bounds
        }

        // make the value less predictable and more complex
        value *= rollingValue;
        value *= multiplierBase + (i / 4); // ensuring that longer inputs and higher indices lead to greater modifications
        value *= abs(sin(originalInput.length() + i / 4)) * 100; // Adding non-linearity to the transformation

        hexStream << std::hex << value;
    }

    std::string hexResult = hexStream.str();

    // Modify each hexadecimal character
    for (char& c : hexResult) {
        c = incrementHexChar(c);
    }

    // Ensure the result is exactly 64 characters
    if (hexResult.length() > 64) {
        hexResult = hexResult.substr(0, 64);
    } else {
        while (hexResult.length() < 64) {
            hexResult += '0';
        }
    }

    return hexResult;
}

// Function to compute word sum
int HashUtils::computeWordSum(const std::string& input) {
    int sum = 0;

    // Calculate the sum of alphabetical positions for each character (ignoring non-alphabetical characters)
    for (char c : input) {
        if (isalpha(c)) {
            // Convert to lowercase for consistent calculation
            sum += tolower(c) - 'a' + 1;
        }
    }

    return sum;
}

// Function to multiply bits by word sum
std::string HashUtils::multiplyBitsByWordSum(std::string bits, int wordSum) {
    // Iterate over each bit and multiply its numeric value by wordSum
    for (size_t i = 0; i < bits.size(); ++i) {
        if (bits[i] == '1') {
            bits[i] = (wordSum % 2) ? '1' : '0';
        } else {
            bits[i] = (wordSum % 2) ? '0' : '1';
        }
        wordSum /= 2; // Update wordSum for the next bit, scaling down
        if (wordSum == 0) break; // If wordSum is exhausted, stop modifying the bits
    }

    return bits;
}

// Function to apply a salt-based transformation to diversify bits
std::string HashUtils::applySaltTransformation(std::string& bits, const std::string& input) {
    std::string saltedBits = bits;
    size_t inputLength = input.length();
    unsigned long long salt = 0;

    // Generate a salt value based on the input length
    for (size_t i = 0; i < inputLength; ++i) {
        salt += input[i] * (i + 1);  // Use character position to modify the salt
    }

    // XOR each bit with the salt, modifying the salt value based on the index
    for (size_t i = 0; i < bits.size(); ++i) {
        int saltBit = (salt >> (i % 64)) & 1;  // Get one bit from the salt
        saltedBits[i] = (saltBit ^ (bits[i] - '0')) + '0';  // XOR with the current bit and store as char
    }

    return saltedBits;
}

// Function to process input and generate the hash
std::string HashUtils::processHashInput(const std::string& input) {
    std::string modifiedInput = input + std::to_string(input.length());
    modifyInput(modifiedInput);
    std::string binaryResult = inputToBits(modifiedInput);
    int wordSum = computeWordSum(modifiedInput);
    std::string modifiedBits = multiplyBitsByWordSum(binaryResult, wordSum);
    std::string saltedBits = applySaltTransformation(modifiedBits, modifiedInput);
    std::string hashResult = bitsToHex(saltedBits, modifiedInput);
    // std::cout << "Hashing input: " << input << " -> " << hashResult << std::endl;
    return hashResult;
}
