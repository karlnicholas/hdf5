#include <iostream>
#include <hdf5.h>
#include <string>
#include <vector>
#include <cstring>  // For strcpy
#include <random>   // For random number generation

#define FILENAME "compound_varlen.h5"
#define DATASETNAME "CompoundData"
#define NUM_RECORDS 1000

// Define a struct for the compound datatype
struct Record {
    uint8_t uint8_Val;
    std::string varStr;     // Variable-length ASCII string
    int8_t int8_Val;
};
// Function to generate cycling values
template <typename T>
T getCycledValue(int index, T minValue, T maxValue) {
    constexpr int cycleLength = 10;
    if constexpr (std::is_signed<T>::value) {
        // Convert to double to avoid truncation and overflow
        double minD = static_cast<double>(minValue);
        double maxD = static_cast<double>(maxValue);
        double step = (maxD - minD) / (cycleLength - 1);

        // Force last row to be exactly maxValue
        return (index % cycleLength == cycleLength - 1) 
            ? maxValue 
            : static_cast<T>(minD + (index % cycleLength) * step);
    } else {
        // Use integer math for unsigned types
        T range = maxValue - minValue;
        T step = range / (cycleLength - 1);

        // Force last row to be exactly maxValue
        return (index % cycleLength == cycleLength - 1) 
            ? maxValue 
            : minValue + (index % cycleLength) * step;
    }
}

int main() {
    // Create an HDF5 file
    hid_t file_id = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    // Create the compound datatype
    hid_t compound_type = H5Tcreate(H5T_COMPOUND, sizeof(Record));

    // Add fields to the compound type
    H5Tinsert(compound_type, "uint8_Val", HOFFSET(Record, uint8_Val), H5T_NATIVE_UINT8);

    // Variable-length string
    hid_t var_str_type = H5Tcopy(H5T_C_S1);
    H5Tset_size(var_str_type, H5T_VARIABLE);
    H5Tinsert(compound_type, "varStr", HOFFSET(Record, varStr), var_str_type);
    
    // Fixed-point values (signed and unsigned, 8, 16, 32, 64 bits)
    H5Tinsert(compound_type, "int8_Val", HOFFSET(Record, int8_Val), H5T_NATIVE_INT8);

    // Create a dataspace for the dataset
    hsize_t dims[1] = {NUM_RECORDS};
    hid_t dataspace_id = H5Screate_simple(1, dims, NULL);

    // Create the dataset
    hid_t dataset_id = H5Dcreate2(file_id, DATASETNAME, compound_type, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    // Setup random number generator for 1 to 9999
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 1900);

    // Generate data
    std::vector<Record> records(NUM_RECORDS);
    for (size_t i = 0; i < NUM_RECORDS; ++i) {
        records[i].uint8_Val  = getCycledValue<uint8_t>(i, std::numeric_limits<uint8_t>::min(), std::numeric_limits<uint8_t>::max());
        // Generate a random number between 1 and 9999 and append to "varData"
        records[i].varStr = "varData:" + std::to_string(dist(gen));
        // Cycle values every 10 rows
        records[i].int8_Val   = getCycledValue<int8_t>(i, std::numeric_limits<int8_t>::min(), std::numeric_limits<int8_t>::max());

    }

    // Write the data
    H5Dwrite(dataset_id, compound_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, records.data());

    // Close resources
    H5Tclose(compound_type);
    H5Tclose(var_str_type);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
    H5Fclose(file_id);

    std::cout << "HDF5 file written successfully: " << FILENAME << std::endl;
    return 0;
}
