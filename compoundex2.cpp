#include <H5Cpp.h>
#include <H5Tpublic.h>  // Needed for low-level bitfield settings
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cstring>

using namespace H5;

const std::string FILENAME = "compound_example_ex2.h5";
const std::string DATASETNAME = "CompoundData";
const std::string ATTRIBUTE_NAME = "GIT root revision";
const size_t NUM_RECORDS = 1000;

// Define your struct
struct Record {
    uint64_t recordId;
    char fixedStr[10];
    std::string varStr;
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
    uint64_t bitfieldVal;
};

// Utility
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
        H5File file(FILENAME, H5F_ACC_TRUNC);

        // Bitfield type (low-level, needed for offset + precision)
        hid_t raw_bitfield_type = H5Tcopy(H5T_NATIVE_UINT64);
        H5Tset_precision(raw_bitfield_type, 57);
        H5Tset_offset(raw_bitfield_type, 7);
        H5Tset_size(raw_bitfield_type, 8);
        // Wrap into H5::DataType
        DataType bitfieldType(raw_bitfield_type);

        // Compound type
        CompType compoundType(sizeof(Record));
        compoundType.insertMember("recordId", HOFFSET(Record, recordId), PredType::NATIVE_UINT64);

        StrType fixedStrType(PredType::C_S1, 10);
        fixedStrType.setStrpad(H5T_STR_NULLTERM);
        compoundType.insertMember("fixedStr", HOFFSET(Record, fixedStr), fixedStrType);

        VarLenType varStrType(PredType::C_S1);
        compoundType.insertMember("varStr", HOFFSET(Record, varStr), varStrType);

        compoundType.insertMember("floatVal", HOFFSET(Record, floatVal), PredType::NATIVE_FLOAT);
        compoundType.insertMember("doubleVal", HOFFSET(Record, doubleVal), PredType::NATIVE_DOUBLE);

        compoundType.insertMember("int8_Val", HOFFSET(Record, int8_Val), PredType::NATIVE_INT8);
        compoundType.insertMember("uint8_Val", HOFFSET(Record, uint8_Val), PredType::NATIVE_UINT8);
        compoundType.insertMember("int16_Val", HOFFSET(Record, int16_Val), PredType::NATIVE_INT16);
        compoundType.insertMember("uint16_Val", HOFFSET(Record, uint16_Val), PredType::NATIVE_UINT16);
        compoundType.insertMember("int32_Val", HOFFSET(Record, int32_Val), PredType::NATIVE_INT32);
        compoundType.insertMember("uint32_Val", HOFFSET(Record, uint32_Val), PredType::NATIVE_UINT32);
        compoundType.insertMember("int64_Val", HOFFSET(Record, int64_Val), PredType::NATIVE_INT64);
        compoundType.insertMember("uint64_Val", HOFFSET(Record, uint64_Val), PredType::NATIVE_UINT64);

        compoundType.insertMember("bitfieldVal", HOFFSET(Record, bitfieldVal), bitfieldType);

        // Dataspace
        hsize_t dims[1] = { NUM_RECORDS };
        DataSpace dataspace(1, dims);

        // Dataset
        DataSet dataset = file.createDataSet(DATASETNAME, compoundType, dataspace);

        // Attribute
        std::string attr_value = "Revision: , URL: ";
        StrType attr_type(PredType::C_S1, attr_value.size());
        attr_type.setStrpad(H5T_STR_NULLTERM);
        DataSpace attr_space(H5S_SCALAR);
        Attribute attr = dataset.createAttribute(ATTRIBUTE_NAME, attr_type, attr_space);
        attr.write(attr_type, attr_value);

        // Data generation
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(1, 1900);
        std::vector<Record> records(NUM_RECORDS);

        for (size_t i = 0; i < NUM_RECORDS; ++i) {
            records[i].recordId = 1000 + i;
            std::strcpy(records[i].fixedStr, "FixedData");
            records[i].varStr = "varData:" + std::to_string(dist(gen));
            records[i].floatVal = 3.14f;
            records[i].doubleVal = 2.718;

            records[i].int8_Val = getCycledValue<int8_t>(i, INT8_MIN, INT8_MAX);
            records[i].uint8_Val = getCycledValue<uint8_t>(i, 0, UINT8_MAX);
            records[i].int16_Val = getCycledValue<int16_t>(i, INT16_MIN, INT16_MAX);
            records[i].uint16_Val = getCycledValue<uint16_t>(i, 0, UINT16_MAX);
            records[i].int32_Val = getCycledValue<int32_t>(i, INT32_MIN, INT32_MAX);
            records[i].uint32_Val = getCycledValue<uint32_t>(i, 0, UINT32_MAX);
            records[i].int64_Val = getCycledValue<int64_t>(i, INT64_MIN, INT64_MAX);
            records[i].uint64_Val = getCycledValue<uint64_t>(i, 0, UINT64_MAX);
            records[i].bitfieldVal = ((i + 1ULL) << 7) | ((i % 4) * 32);
        }

        dataset.write(records.data(), compoundType);
        std::cout << "✅ HDF5 C++ file created and written successfully.\n";

        // Cleanup
        bitfieldType.close();
        compoundType.close();
        dataset.close();
        file.close();
    } catch (Exception& e) {
        std::cerr << "❌ HDF5 Exception: " << e.getDetailMsg() << "\n";
        return 1;
    }

    return 0;
}
