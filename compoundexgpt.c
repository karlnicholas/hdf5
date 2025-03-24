#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FILENAME     "compound_example_gpt.h5"
#define DATASETNAME  "CompoundData"
#define NUM_RECORDS  1000

typedef struct {
    uint64_t recordId;
    char fixedStr[10];
    char *varStr;  // ✅ variable-length string (HDF5 type class 9)
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
} Record;

int main() {
    // Create file
    hid_t file_id = H5Fcreate(FILENAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    // Define compound datatype
    hid_t compound_type = H5Tcreate(H5T_COMPOUND, sizeof(Record));
    H5Tinsert(compound_type, "recordId", HOFFSET(Record, recordId), H5T_NATIVE_UINT64);

    // Fixed-length string (fixedStr)
    hid_t fixed_str_type = H5Tcopy(H5T_C_S1);
    H5Tset_size(fixed_str_type, 10);
    H5Tset_strpad(fixed_str_type, H5T_STR_NULLTERM);
    H5Tinsert(compound_type, "fixedStr", HOFFSET(Record, fixedStr), fixed_str_type);

    // ✅ Variable-length string (varStr)
    hid_t var_str_type = H5Tcopy(H5T_C_S1);
    H5Tset_size(var_str_type, H5T_VARIABLE);
    H5Tinsert(compound_type, "varStr", HOFFSET(Record, varStr), var_str_type);

    // Other fields
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

    // Bitfield (57-bit precision, offset 7)
    hid_t bitfield_type = H5Tcopy(H5T_NATIVE_UINT64);
    H5Tset_precision(bitfield_type, 57);
    H5Tset_offset(bitfield_type, 7);
    H5Tset_size(bitfield_type, 8);
    H5Tinsert(compound_type, "bitfieldVal", HOFFSET(Record, bitfieldVal), bitfield_type);

    // Create dataspace and dataset
    hsize_t dims[1] = {NUM_RECORDS};
    hid_t dataspace_id = H5Screate_simple(1, dims, NULL);
    hid_t dataset_id = H5Dcreate2(file_id, DATASETNAME, compound_type, dataspace_id,
                                  H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    // Add string attribute
    const char *attr_name = "GIT root revision";
    const char *attr_value = "Revision: , URL: ";
    hid_t attr_space = H5Screate(H5S_SCALAR);
    hid_t attr_type = H5Tcopy(H5T_C_S1);
    H5Tset_size(attr_type, strlen(attr_value));
    H5Tset_strpad(attr_type, H5T_STR_NULLTERM);
    hid_t attr_id = H5Acreate2(dataset_id, attr_name, attr_type, attr_space,
                               H5P_DEFAULT, H5P_DEFAULT);
    H5Awrite(attr_id, attr_type, attr_value);
    H5Aclose(attr_id);
    H5Tclose(attr_type);
    H5Sclose(attr_space);

    // Allocate and populate records
    Record *records = malloc(NUM_RECORDS * sizeof(Record));
    char **varStrings = malloc(NUM_RECORDS * sizeof(char*)); // store for cleanup

    for (size_t i = 0; i < NUM_RECORDS; ++i) {
        records[i].recordId = 1000 + i;
        strcpy(records[i].fixedStr, "FixedData");

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "varData:%lu", (unsigned long)(rand() % 1900 + 1));
        varStrings[i] = strdup(buffer);
        records[i].varStr = varStrings[i];

        records[i].floatVal = 3.14f;
        records[i].doubleVal = 2.718;
        records[i].int8_Val   = (int8_t)(i % 127);
        records[i].uint8_Val  = (uint8_t)(i % 255);
        records[i].int16_Val  = (int16_t)(i * 3);
        records[i].uint16_Val = (uint16_t)(i * 4);
        records[i].int32_Val  = (int32_t)(i * 5);
        records[i].uint32_Val = (uint32_t)(i * 6);
        records[i].int64_Val  = (int64_t)(i * 1000);
        records[i].uint64_Val = (uint64_t)(i * 2000);
        records[i].bitfieldVal = ((i + 1ULL) << 7) | ((i % 4) * 32);
    }

    // Write data
    herr_t status = H5Dwrite(dataset_id, compound_type, H5S_ALL, H5S_ALL,
                             H5P_DEFAULT, records);
    if (status < 0) {
        fprintf(stderr, "H5Dwrite failed\n");
        return 1;
    }

    // Cleanup
    for (size_t i = 0; i < NUM_RECORDS; ++i) {
        free(varStrings[i]);
    }
    free(varStrings);
    free(records);

    H5Tclose(compound_type);
    H5Tclose(fixed_str_type);
    H5Tclose(var_str_type);
    H5Tclose(bitfield_type);
    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
    H5Fclose(file_id);

    printf("HDF5 file written successfully: %s\n", FILENAME);
    return 0;
}
