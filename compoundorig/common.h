#ifndef COMMON_H
#define COMMON_H

#include <H5Cpp.h>
#include <vector>
#include <string>

using namespace H5;

// Constants
extern const H5std_string FILE_NAME;
extern const H5std_string DATASET_NAME;
extern const H5std_string ATTRIBUTE_NAME;

// Define the structure for the compound datatype
struct Record {
    uint64_t recordId;
    char fixedStr[10];
    hvl_t varStr;           // Variable-length string (hvl_t for compatibility)
    float floatVal;
    double doubleVal;
    int8_t int8_Val;
    uint8_t uint8_Val;
    int16_t int16_Val;
    uint16_t uint16_Val;
    int32_t int32_Val;
    uint32_t uint32_Val;
    int64_t int64_Val;
    uint64_t uint64_Val;
    uint64_t bitfieldVal;   // Bitfield with bit offset = 7, precision 57
};

// Function to create the compound type
CompType createCompoundType();

#endif // COMMON_H