#include <hdf5.h>
#include <iostream>
#include <cstdint>
#include <cstring>

#define FILE_NAME "demand_data_cpp.h5"
#define DATASET_NAME "Demand"
#define NUM_RECORDS 1750
#define ATTR_NAME "GIT root revision"
#define ATTR_VALUE "Revision: , URL: "

struct DemandRecord {
    uint64_t shipmentId;
    char origCountry[2];
    char origSlic[5];
    char origSort[1];
    char destCountry[2];
    char destSlic[5];
    char destIbi[1];
    char destPostalCode[9];
    char shipper[10];
    char service[1];
    char packageType[1];
    char accessorials[1];
    uint16_t pieces;
    uint16_t weight;
    uint32_t cube;
    char committedTnt[1];
    char committedDate[1];
} __attribute__((packed));

int main() {
    hid_t file = H5Fcreate(FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    hid_t dtype = H5Tcreate(H5T_COMPOUND, sizeof(DemandRecord));
    H5Tinsert(dtype, "shipmentId", HOFFSET(DemandRecord, shipmentId), H5T_STD_U64LE);

    hid_t str2 = H5Tcopy(H5T_C_S1);
    H5Tset_size(str2, 2);
    H5Tset_strpad(str2, H5T_STR_NULLPAD);
    H5Tinsert(dtype, "origCountry", HOFFSET(DemandRecord, origCountry), str2);

    hid_t str5 = H5Tcopy(H5T_C_S1);
    H5Tset_size(str5, 5);
    H5Tset_strpad(str5, H5T_STR_NULLPAD);
    H5Tinsert(dtype, "origSlic", HOFFSET(DemandRecord, origSlic), str5);

    hid_t str1 = H5Tcopy(H5T_C_S1);
    H5Tset_size(str1, 1);
    H5Tset_strpad(str1, H5T_STR_NULLPAD);
    H5Tinsert(dtype, "origSort", HOFFSET(DemandRecord, origSort), str1);
    H5Tinsert(dtype, "destCountry", HOFFSET(DemandRecord, destCountry), str2);
    H5Tinsert(dtype, "destSlic", HOFFSET(DemandRecord, destSlic), str5);
    H5Tinsert(dtype, "destIbi", HOFFSET(DemandRecord, destIbi), str1);

    hid_t str9 = H5Tcopy(H5T_C_S1);
    H5Tset_size(str9, 9);
    H5Tset_strpad(str9, H5T_STR_NULLPAD);
    H5Tinsert(dtype, "destPostalCode", HOFFSET(DemandRecord, destPostalCode), str9);

    hid_t str10 = H5Tcopy(H5T_C_S1);
    H5Tset_size(str10, 10);
    H5Tset_strpad(str10, H5T_STR_NULLPAD);
    H5Tinsert(dtype, "shipper", HOFFSET(DemandRecord, shipper), str10);

    H5Tinsert(dtype, "service", HOFFSET(DemandRecord, service), str1);
    H5Tinsert(dtype, "packageType", HOFFSET(DemandRecord, packageType), str1);
    H5Tinsert(dtype, "accessorials", HOFFSET(DemandRecord, accessorials), str1);

    H5Tinsert(dtype, "pieces", HOFFSET(DemandRecord, pieces), H5T_STD_U16LE);
    H5Tinsert(dtype, "weight", HOFFSET(DemandRecord, weight), H5T_STD_U16LE);
    H5Tinsert(dtype, "cube", HOFFSET(DemandRecord, cube), H5T_STD_U32LE);
    H5Tinsert(dtype, "committedTnt", HOFFSET(DemandRecord, committedTnt), str1);
    H5Tinsert(dtype, "committedDate", HOFFSET(DemandRecord, committedDate), str1);

    hsize_t dims[1] = {NUM_RECORDS};
    hid_t dataspace = H5Screate_simple(1, dims, NULL);
    hid_t dataset = H5Dcreate(file, DATASET_NAME, dtype, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    DemandRecord records[NUM_RECORDS];
    for (int i = 0; i < NUM_RECORDS; i++) {
        records[i].shipmentId = 1000 + i;
        strncpy(records[i].origCountry, "US", 2);
        strncpy(records[i].origSlic, "SLIC1", 5);
        strncpy(records[i].origSort, "A", 1);
        strncpy(records[i].destCountry, "CA", 2);
        strncpy(records[i].destSlic, "SLIC2", 5);
        strncpy(records[i].destIbi, "B", 1);
        strncpy(records[i].destPostalCode, "123456789", 9);
        strncpy(records[i].shipper, "SHIPPER123", 10);
        strncpy(records[i].service, "S", 1);
        strncpy(records[i].packageType, "P", 1);
        strncpy(records[i].accessorials, "Y", 1);
        records[i].pieces = 5;
        records[i].weight = 10;
        records[i].cube = 50;
        strncpy(records[i].committedTnt, "T", 1);
        strncpy(records[i].committedDate, "D", 1);
    }

    herr_t status = H5Dwrite(dataset, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, records);
    if (status < 0) {
        std::cerr << "Failed to write dataset" << std::endl;
    } else {
        std::cout << "Successfully wrote " << NUM_RECORDS << " records\n";
    }

    hid_t attr_space = H5Screate(H5S_SCALAR);
    hid_t attr_type = H5Tcopy(H5T_C_S1);
    H5Tset_size(attr_type, strlen(ATTR_VALUE) + 1);
    H5Tset_strpad(attr_type, H5T_STR_NULLPAD);
    hid_t attr = H5Acreate(dataset, ATTR_NAME, attr_type, attr_space, H5P_DEFAULT, H5P_DEFAULT);
    char attr_value[] = ATTR_VALUE;
    status = H5Awrite(attr, attr_type, attr_value);
    if (status < 0) {
        std::cerr << "Failed to write attribute" << std::endl;
    } else {
        std::cout << "Successfully wrote attribute '" << ATTR_NAME << "'\n";
    }

    H5Aclose(attr);
    H5Tclose(attr_type);
    H5Sclose(attr_space);
    H5Dclose(dataset);
    H5Sclose(dataspace);
    H5Tclose(str1);
    H5Tclose(str2);
    H5Tclose(str5);
    H5Tclose(str9);
    H5Tclose(str10);
    H5Tclose(dtype);
    H5Fclose(file);
    return 0;
}