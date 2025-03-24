#include <iostream>
#include <H5Cpp.h>
#include <vector>
#include <cstring>

using namespace H5;

const H5std_string FILE_NAME("compound_example_ex.h5");
const H5std_string DATASET_NAME("CompoundData");

// Match the structure used in the writer
struct Record {
    uint64_t recordId;
    char fixedStr[10];
    hvl_t varStr;           // Variable-length string using HDF5's hvl_t
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
        // Open the file and dataset
        H5File file(FILE_NAME, H5F_ACC_RDONLY);
        DataSet dataset = file.openDataSet(DATASET_NAME);

        // Create the compound datatype
        CompType compoundType(sizeof(Record));
        compoundType.insertMember("recordId", HOFFSET(Record, recordId), PredType::NATIVE_UINT64);
        compoundType.insertMember("fixedStr", HOFFSET(Record, fixedStr), StrType(PredType::C_S1, 10));
        compoundType.insertMember("varStr", HOFFSET(Record, varStr), VarLenType(PredType::C_S1));
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

        // Bitfield type (7-bit offset, 57-bit precision)
        IntType bitfield_type(PredType::NATIVE_UINT64);
        bitfield_type.setPrecision(57);
        bitfield_type.setOffset(7);
        compoundType.insertMember("bitfieldVal", HOFFSET(Record, bitfieldVal), bitfield_type);

        // Read first 10 records
        const size_t NUM_TO_READ = 10;
        std::vector<Record> records(NUM_TO_READ);
        DataSpace filespace = dataset.getSpace();

        hsize_t offset[1] = {0};
        hsize_t count[1] = {NUM_TO_READ};
        filespace.selectHyperslab(H5S_SELECT_SET, count, offset);

        DataSpace memspace(1, count);
        dataset.read(records.data(), compoundType, memspace, filespace);

        // Print first 10 records
        for (size_t i = 0; i < NUM_TO_READ; ++i) {
            const Record& rec = records[i];
            std::cout << "Record " << i << ":\n";
            std::cout << "  shipmentId: " << rec.recordId << "\n";
            std::cout << "  fixedStr: " << rec.fixedStr << "\n";
            std::cout << "  varStr: " << std::string(static_cast<char*>(rec.varStr.p), rec.varStr.len) << "\n";
            std::cout << "  floatVal: " << rec.floatVal << "\n";
            std::cout << "  doubleVal: " << rec.doubleVal << "\n";
            std::cout << "  int8_Val: " << static_cast<int>(rec.int8_Val) << "\n";
            std::cout << "  uint8_Val: " << static_cast<unsigned>(rec.uint8_Val) << "\n";
            std::cout << "  int16_Val: " << rec.int16_Val << "\n";
            std::cout << "  uint16_Val: " << rec.uint16_Val << "\n";
            std::cout << "  int32_Val: " << rec.int32_Val << "\n";
            std::cout << "  uint32_Val: " << rec.uint32_Val << "\n";
            std::cout << "  int64_Val: " << rec.int64_Val << "\n";
            std::cout << "  uint64_Val: " << rec.uint64_Val << "\n";

            uint64_t raw = rec.bitfieldVal & 0x01FFFFFFFFFFFFFFULL; // Mask 57 bits
            uint64_t integerPart = raw >> 7;
            uint64_t fractionalBits = raw & 0x7F; // 7 bits
            double fixedPointValue = static_cast<double>(integerPart) + (fractionalBits / 128.0);
            std::cout << "  bitfieldVal (fixed-point): " << fixedPointValue << "\n";

            std::cout << std::endl;
        }

        // Manually free varStr memory
        for (auto& rec : records) {
            if (rec.varStr.p) {
                free(rec.varStr.p); // Required by HDF5 for hvl_t
                rec.varStr.p = nullptr;
            }
        }

        dataset.close();
        file.close();

    } catch (Exception& e) {
        std::cerr << "HDF5 Exception: " << e.getCDetailMsg() << "\n";
        return 1;
    }

    return 0;
}
