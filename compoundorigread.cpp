#include <iostream>
#include <H5Cpp.h>
#include <vector>
#include <iomanip>

using namespace H5;

const H5std_string FILE_NAME("compound_example_ex.h5");
const H5std_string DATASET_NAME("CompoundData");

// Define the structure matching the written data
struct Record {
    uint64_t recordId;
    char fixedStr[10];
    hvl_t varStr;           // Variable-length string
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

int main() {
    try {
        // Open the existing HDF5 file in read-only mode
        H5File file(FILE_NAME, H5F_ACC_RDONLY);

        // Open the dataset
        DataSet dataset = file.openDataSet(DATASET_NAME);

        // Get the dataset's dataspace
        DataSpace dataspace = dataset.getSpace();
        hsize_t dims[1];
        dataspace.getSimpleExtentDims(dims);
        hsize_t numRecords = dims[0];
        std::cout << "Total number of records in file: " << numRecords << std::endl;

        // Define the compound data type for reading
        CompType compoundType(sizeof(Record));
        compoundType.insertMember("recordId", HOFFSET(Record, recordId), PredType::NATIVE_UINT64);
        compoundType.insertMember("fixedStr", HOFFSET(Record, fixedStr), StrType(PredType::C_S1, 10));
        VarLenType var_str_type(PredType::C_S1);
        compoundType.insertMember("varStr", HOFFSET(Record, varStr), var_str_type);
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
        IntType bitfield_type(PredType::NATIVE_UINT64);
        bitfield_type.setPrecision(57);
        bitfield_type.setOffset(7);
        compoundType.insertMember("bitfieldVal", HOFFSET(Record, bitfieldVal), bitfield_type);

        // Define the number of records to read (first 10 or less if file has fewer)
        const hsize_t recordsToRead = std::min<hsize_t>(10, numRecords);
        std::vector<Record> records(recordsToRead);

        // Define the hyperslab to read only the first 10 records
        hsize_t offset[1] = {0};
        hsize_t count[1] = {recordsToRead};
        dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

        // Define memory dataspace for the records to read
        DataSpace memspace(1, count);

        // Read the first 10 records
        dataset.read(records.data(), compoundType, memspace, dataspace);

        // Print the first 10 records
        std::cout << "\nFirst " << recordsToRead << " records:\n";
        std::cout << std::fixed << std::setprecision(7); // 7 decimal places for all floats
        for (hsize_t i = 0; i < recordsToRead; ++i) {
            // Convert bitfieldVal to fixed-point number
            double bitfieldValue = (records[i].bitfieldVal >> 7) + (records[i].bitfieldVal & 0x7F) / 128.0;

            std::cout << "Record " << i << ":\n";
            std::cout << "  recordId: " << records[i].recordId << "\n";
            std::cout << "  fixedStr: " << records[i].fixedStr << "\n";
            std::cout << "  varStr: ";
            if (records[i].varStr.p != nullptr && records[i].varStr.len > 0) {
                std::cout.write(static_cast<char*>(records[i].varStr.p), records[i].varStr.len);
            } else {
                std::cout << "(empty)";
            }
            std::cout << "\n";
            std::cout << "  floatVal: " << records[i].floatVal << "\n";
            std::cout << "  doubleVal: " << records[i].doubleVal << "\n";
            std::cout << "  int8_Val: " << (int)records[i].int8_Val << "\n";
            std::cout << "  uint8_Val: " << (unsigned)records[i].uint8_Val << "\n";
            std::cout << "  int16_Val: " << records[i].int16_Val << "\n";
            std::cout << "  uint16_Val: " << records[i].uint16_Val << "\n";
            std::cout << "  int32_Val: " << records[i].int32_Val << "\n";
            std::cout << "  uint32_Val: " << records[i].uint32_Val << "\n";
            std::cout << "  int64_Val: " << records[i].int64_Val << "\n";
            std::cout << "  uint64_Val: " << records[i].uint64_Val << "\n";
            std::cout << "  bitfieldVal: " << bitfieldValue << "\n\n";
        }

        // Reclaim variable-length string memory
        H5Dvlen_reclaim(compoundType.getId(), dataspace.getId(), H5P_DEFAULT, records.data());

        // Close resources
        dataset.close();
        file.close();

        std::cout << "Successfully read and printed the first " << recordsToRead << " records.\n";
    } catch (Exception &e) {
        std::cerr << "HDF5 Exception: " << e.getCDetailMsg() << std::endl;
        return 1;
    }

    return 0;
}