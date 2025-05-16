//
// Created by janezp on 15/05/25.
//

#pragma once
#include <WString.h>
#include <vector>

using namespace std;

class RLP {
public:
    static uint32_t        RlpEncodeWholeHeader(uint8_t *header_output, uint32_t total_len);
    static vector<uint8_t> RlpEncodeWholeHeaderWithVector(uint32_t total_len);
    static uint32_t        RlpEncodeItem(uint8_t* output, const uint8_t* input, uint32_t input_len);
    static vector<uint8_t> RlpEncodeItemWithVector(const vector<uint8_t> input);
    static vector<uint8_t> RlpEncodeString(const String* str);

    static vector<uint8_t> ConvertStringToVector(const String* str);
    static uint32_t        ConvertNumberToUintArray(uint8_t *str, uint32_t val);
    static vector<uint8_t> ConvertNumberToVector(uint32_t val);
    static vector<uint8_t> ConvertNumberToVector(unsigned long long val);
    static uint32_t        ConvertCharStrToUintArray(uint8_t *out, const uint8_t *in);
    static vector<uint8_t> ConvertHexToVector(const uint8_t *in);
    static vector<uint8_t> ConvertHexToVector(const String* str);
    static char *          ConvertToString(const uint8_t *in);

    static uint8_t HexToInt(uint8_t s);
    static String  VectorToString(const vector<uint8_t> *buf);
    static String  PlainVectorToString(const vector<uint8_t> *buf);
    static String  ConvertBytesToHex(const uint8_t *bytes, int length);
    static void    ConvertHexToBytes(uint8_t *_dst, const char *_src, int length);
    static String  ConvertString(const char* value);
    static String  ConvertHexToASCII(const char *result, size_t length);
    static String  InterpretStringResult(const char *result);

    static vector<String>* ConvertCharStrToVector32(const char *resultPtr, size_t resultSize, vector<String> *result);


    static String ConvertIntegerToBytes(const int32_t value);

private:
    static uint8_t ConvertCharToByte(const uint8_t* ptr);
};

