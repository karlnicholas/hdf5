#include <hdf5.h>
#include <iostream>

struct TestRecord {
    uint64_t shipmentId;
    char origCountry[2];
} __attribute__((packed));

int main() {
    hid_t file = H5Fopen("test.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file < 0) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
    hid_t dset = H5Dopen(file, "Test", H5P_DEFAULT);
    hid_t mem_type = H5Tcreate(H5T_COMPOUND, sizeof(TestRecord));
    H5Tinsert(mem_type, "shipmentId", HOFFSET(TestRecord, shipmentId), H5T_STD_U64LE);
    hid_t str2 = H5Tcopy(H5T_C_S1);
    H5Tset_size(str2, 2);
    H5Tset_strpad(str2, H5T_STR_NULLPAD); // Match writer
    H5Tinsert(mem_type, "origCountry", HOFFSET(TestRecord, origCountry), str2);
    TestRecord record;
    herr_t status = H5Dread(dset, mem_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, &record);
    if (status < 0) {
        std::cerr << "Read failed" << std::endl;
    } else {
        std::cout << "shipmentId: " << record.shipmentId << "\n"
                  << "origCountry: " << record.origCountry << std::endl;
    }
    H5Tclose(str2);
    H5Tclose(mem_type);
    H5Dclose(dset);
    H5Fclose(file);
    return 0;
}