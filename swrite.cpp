#include <hdf5.h>
#include <cstring>

struct TestRecord {
    uint64_t shipmentId;
    char origCountry[2];
} __attribute__((packed));

int main() {
    hid_t file = H5Fcreate("test.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    hid_t dtype = H5Tcreate(H5T_COMPOUND, sizeof(TestRecord));
    H5Tinsert(dtype, "shipmentId", HOFFSET(TestRecord, shipmentId), H5T_STD_U64LE);
    hid_t str2 = H5Tcopy(H5T_C_S1);
    H5Tset_size(str2, 2);
    H5Tset_strpad(str2, H5T_STR_NULLPAD); // Switch to null-pad
    H5Tinsert(dtype, "origCountry", HOFFSET(TestRecord, origCountry), str2);
    hsize_t dims[1] = {1};
    hid_t space = H5Screate_simple(1, dims, NULL);
    hid_t dset = H5Dcreate(file, "Test", dtype, space, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    TestRecord record;
    record.shipmentId = 1000;
    strncpy(record.origCountry, "US", 2); // "US" (2 bytes, no null)
    H5Dwrite(dset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, &record);
    H5Dclose(dset);
    H5Sclose(space);
    H5Tclose(str2);
    H5Tclose(dtype);
    H5Fclose(file);
    return 0;
}