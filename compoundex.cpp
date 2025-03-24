#include <iostream>
#include <H5Cpp.h>
#include <vector>
#include <cstring>  // For strcpy
#include <random>   // For random number generation

using namespace H5;

const H5std_string FILE_NAME("compound_example_ex.h5");
const H5std_string DATASET_NAME("CompoundData");
const H5std_string ATTRIBUTE_NAME("GIT root revision");
const int NUM_RECORDS = 1000;

// Define a struct for the compound datatype
struct Record {
    uint64_t recordId;
    char fixedStr[10];      // Fixed-length ASCII string
    std::string varStr;     // Variable-length ASCII string
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

// Function to generate cycling values
template <typename T>
T getCycledValue(int index, T minValue, T maxValue) {
    constexpr int cycleLength = 10;
    if constexpr (std::is_signed<T>::value) {
        double minD = static_cast<double>(minValue);
        double maxD = static_cast<double>(maxValue);
        double step = (maxD - minD) / (cycleLength - 1);
        return (index % cycleLength == cycleLength - 1) 
            ? maxValue 
            : static_cast<T>(minD + (index % cycleLength) * step);
    } else {
        T range = maxValue - minValue;
        T step = range / (cycleLength - 1);
        return (index % cycleLength == cycleLength - 1) 
            ? maxValue 
            : minValue + (index % cycleLength) * step;
    }
}

int main() {
    try {
        // Create a new HDF5 file
        H5File file(FILE_NAME, H5F_ACC_TRUNC);

        // Create the compound datatype
        CompType compound_type(sizeof(Record));
        
        // Add fields to the compound type
        compound_type.insertMember("recordId", HOFFSET(Record, recordId), PredType::NATIVE_UINT64);
        
        // Fixed-length string
        StrType fixed_str_type(PredType::C_S1, 10);
        compound_type.insertMember("fixedStr", HOFFSET(Record, fixedStr), fixed_str_type);
        
        // Variable-length string
        VarLenType var_str_type(PredType::C_S1);
        compound_type.insertMember("varStr", HOFFSET(Record, varStr), var_str_type);
        
        // Floating-point values
        compound_type.insertMember("floatVal", HOFFSET(Record, floatVal), PredType::NATIVE_FLOAT);
        compound_type.insertMember("doubleVal", HOFFSET(Record, doubleVal), PredType::NATIVE_DOUBLE);
        
        // Fixed-point values
        compound_type.insertMember("int8_Val", HOFFSET(Record, int8_Val), PredType::NATIVE_INT8);
        compound_type.insertMember("uint8_Val", HOFFSET(Record, uint8_Val), PredType::NATIVE_UINT8);
        compound_type.insertMember("int16_Val", HOFFSET(Record, int16_Val), PredType::NATIVE_INT16);
        compound_type.insertMember("uint16_Val", HOFFSET(Record, uint16_Val), PredType::NATIVE_UINT16);
        compound_type.insertMember("int32_Val", HOFFSET(Record, int32_Val), PredType::NATIVE_INT32);
        compound_type.insertMember("uint32_Val", HOFFSET(Record, uint32_Val), PredType::NATIVE_UINT32);
        compound_type.insertMember("int64_Val", HOFFSET(Record, int64_Val), PredType::NATIVE_INT64);
        compound_type.insertMember("uint64_Val", HOFFSET(Record, uint64_Val), PredType::NATIVE_UINT64);

        // Create bitfield type (7-bit offset, 57-bit precision)
        IntType bitfield_type(PredType::NATIVE_UINT64);
        bitfield_type.setPrecision(57);
        bitfield_type.setOffset(7);
        compound_type.insertMember("bitfieldVal", HOFFSET(Record, bitfieldVal), bitfield_type);

        // Create a dataspace for the dataset
        hsize_t dims[1] = {NUM_RECORDS};
        DataSpace dataspace(1, dims);

        // Create the dataset
        DataSet dataset = file.createDataSet(DATASET_NAME, compound_type, dataspace);

        // ✅ ADD ATTRIBUTE: "GIT root revision"
        H5std_string attribute_value = "Revision: , URL: ";
        StrType attr_type(PredType::C_S1, attribute_value.size());
        DataSpace attr_space(H5S_SCALAR);
        Attribute attribute = dataset.createAttribute(ATTRIBUTE_NAME, attr_type, attr_space);
        attribute.write(attr_type, attribute_value);
        attribute.close();

        // Setup random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(1, 1900);

        // Generate data
        std::vector<Record> records(NUM_RECORDS);
        for (size_t i = 0; i < NUM_RECORDS; ++i) {
            records[i].recordId = 1000 + i;
            std::strcpy(records[i].fixedStr, "FixedData");
            records[i].varStr = "varData:" + std::to_string(dist(gen));
            records[i].floatVal = 3.14f;
            records[i].doubleVal = 2.718;

            records[i].int8_Val   = getCycledValue<int8_t>(i, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());
            records[i].uint8_Val  = getCycledValue<uint8_t>(i, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
            records[i].int16_Val  = getCycledValue<int16_t>(i, std::numeric_limits<int16_t>::min(), std::numeric_limits<int16_t>::max());
            records[i].uint16_Val = getCycledValue<uint16_t>(i, std::numeric_limits<uint16_t>::min(), std::numeric_limits<uint16_t>::max());
            records[i].int32_Val  = getCycledValue<int32_t>(i, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());
            records[i].uint32_Val = getCycledValue<uint32_t>(i, std::numeric_limits<uint32_t>::min(), std::numeric_limits<uint32_t>::max());
            records[i].int64_Val  = getCycledValue<int64_t>(i, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());
            records[i].uint64_Val = getCycledValue<uint64_t>(i, std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
            
            // Set bitfieldVal with mask to ensure 57-bit precision
            uint64_t value = ((i + 1ULL) << 7) | ((i % 4) * 32);
            records[i].bitfieldVal = value & 0x01FFFFFFFFFFFFFFULL; // Mask to 57 bits
        }

        // Write the data
        dataset.write(records.data(), compound_type);

        // Resources are automatically closed when objects go out of scope
        std::cout << "HDF5 file written successfully: " << FILE_NAME << std::endl;
    }
    catch (const H5::Exception& e) {
        std::cerr << "HDF5 Exception: " << e.getDetailMsg() << std::endl;
        return 1;
    }

    return 0;
}