#include <iostream>
#include <hdf5.h>
#include <string>
#include <vector>
#include <cstring>
#include <random>

#define FILENAME "compound_example_gpt.h5"
#define DATASETNAME "CompoundData"
#define NUM_RECORDS 1000

struct Record {
    uint64_t recordId;
    char fixedStr[10];
    hvl_t varStr; // Variable-length string
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
    hid_t file_id = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    hid_t compound_type = H5Tcreate(H5T_COMPOUND, sizeof(Record));
    H5Tinsert(compound_type, "recordId", HOFFSET(Record, recordId), H5T_NATIVE_UINT64);
    hid_t fixed_str_type = H5Tcopy(H5T_C_S1);
    H5Tset_size(fixed_str_type, 10);
    H5Tset_strpad(fixed_str_type, H5T_STR_NULLTERM);
    H5Tinsert(compound_type, "fixedStr", HOFFSET(Record, fixedStr), fixed_str_type);
    hid_t var_str_type = H5Tvlen_create(H5T_C_S1); // Proper variable-length type
    H5Tinsert(compound_type, "varStr", HOFFSET(Record, varStr), var_str_type);
    H5Tinsert(compound_type, "floatVal", HOFFSET(Record, floatVal), H5T_NATIVE_FLOAT);
    H5Tinsert(compound_type, "doubleVal", HOFFSET(Record, doubleVal), H5T_NATIVE_DOUBLE);
    H5Tinsert(compound_type, "int8_Val", HOFFSET(Record, int8_Val), H5T_NATIVE_INT8);
    H5Tinsert(compound_type, "uint8_Val", HOFFSET(Record, uint8_Val), H5T_NATIVE_UINT8);
    H5Tinsert(compound_type, "int16_Val", HOFFSET(Record, int16_Val), H5T_NATIVE_INT16);
    H5Tinsert(compound_type, "uint16_Val", HOFFSET(Record, uint16_Val), H5T_NATIVE_UINT16);
    H5Tinsert(compound_type, "int32_Val", HOFFSET(Record, int32_Val), H5T_NATIVE_INT32);
    H5Tinsert(compound_type, "uint32_Val", HOFFSET(Record, uint32_Val), H5T_NATIVE_UINT32);
    H5Tinsert(compound_type, "int64_Val", HOFFSET(Record, int64_Val), H5T_NATIVE_INT64);
    H5Tinsert(compound_type, "uint64_Val", HOFFSET(Record, uint64_Val), H5T_NATIVE_UINT64);
    hid_t bitfield_type = H5Tcopy(H5T_NATIVE_UINT64);
    H5Tset_precision(bitfield_type, 57);
    H5Tset_offset(bitfield_type, 7);
    H5Tset_size(bitfield_type, 8);
    H5Tinsert(compound_type, "bitfieldVal", HOFFSET(Record, bitfieldVal), bitfield_type);

    hsize_t dims[1] = {NUM_RECORDS};
    hid_t dataspace_id = H5Screate_simple(1, dims, NULL);
    hid_t dataset_id = H5Dcreate2(file_id, DATASETNAME, compound_type, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    const char *attribute_name = "GIT root revision";
    const char *attribute_value = "Revision: , URL: ";
    hid_t attr_dataspace_id = H5Screate(H5S_SCALAR);
    hid_t attr_type_id = H5Tcopy(H5T_C_S1);
    H5Tset_size(attr_type_id, strlen(attribute_value));
    H5Tset_strpad(attr_type_id, H5T_STR_NULLTERM);
    hid_t attr_id = H5Acreate2(dataset_id, attribute_name, attr_type_id, attr_dataspace_id, H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attr_id, attr_type_id, attribute_value);
    H5Aclose(attr_id);
    H5Tclose(attr_type_id);
    H5Sclose(attr_dataspace_id);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1900);

    std::vector<Record> records(NUM_RECORDS);
    for (size_t i = 0; i < NUM_RECORDS; ++i) {
        records[i].recordId = 1000 + i;
        std::strcpy(records[i].fixedStr, "FixedData");

        std::string temp = "varData:" + std::to_string(dist(gen));
        records[i].varStr.len = temp.size();
        records[i].varStr.p = new char[temp.size() + 1]; // Use new instead of malloc
        std::strcpy((char*)records[i].varStr.p, temp.c_str());

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
        records[i].bitfieldVal = ((i + 1ULL) << 7) | ((i % 4) * 32);
    }

    H5Dwrite(dataset_id, compound_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, records.data());

    // Free memory with H5Tclose, not manual free/delete
    H5Tclose(compound_type);
    H5Tclose(fixed_str_type);
    H5Tclose(var_str_type); // This should reclaim varStr memory
    H5Tclose(bitfield_type);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
    H5Fclose(file_id);

    std::cout << "HDF5 file written successfully: " << FILENAME << std::endl;
    return 0;
}