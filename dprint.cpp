#include <hdf5.h>
#include <iostream>
#include <cstdint>
#include <cstring>

#define FILE_NAME "demand_data_py.h5"
#define DATASET_NAME "Demand"

struct DemandRecord {
    uint64_t shipmentId;      // 8 bytes
    char origCountry[2];      // 2 bytes
    char origSlic[5];         // 5 bytes
    char origSort[1];         // 1 byte
    char destCountry[2];      // 2 bytes
    char destSlic[5];         // 5 bytes
    char destIbi[1];          // 1 byte
    char destPostalCode[9];   // 9 bytes
    char shipper[10];         // 10 bytes
    char service[1];          // 1 byte
    char packageType[1];      // 1 byte
    char accessorials[1];     // 1 byte
    uint16_t pieces;          // 2 bytes
    uint16_t weight;          // 2 bytes
    uint32_t cube;            // 4 bytes
    char committedTnt[1];     // 1 byte
    char committedDate[1];    // 1 byte
} __attribute__((packed));    // Total: 56 bytes

int main() {
    hid_t file = H5Fopen(FILE_NAME, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file < 0) {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    hid_t dataset = H5Dopen(file, DATASET_NAME, H5P_DEFAULT);
    if (dataset < 0) {
        std::cerr << "Failed to open dataset" << std::endl;
        H5Fclose(file);
        return 1;
    }

    hid_t file_type = H5Dget_type(dataset);
    hid_t dataspace = H5Dget_space(dataset);
    hsize_t start[1] = {0};
    hsize_t count[1] = {5};
    H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, start, NULL, count, NULL);

    DemandRecord records[5];
    memset(records, 0, sizeof(records)); // Clear buffer
    herr_t status = H5Dread(dataset, file_type, H5S_ALL, dataspace, H5P_DEFAULT, records);
    if (status < 0) {
        std::cerr << "Failed to read dataset" << std::endl;
    } else {
        std::cout << "First 5 records from " << DATASET_NAME << ":\n";
        for (int i = 0; i < 5; i++) {
            // Null-terminate strings in-place
            char origCountry[3], origSlic[6], origSort[2], destCountry[3], destSlic[6], destIbi[2],
                 destPostalCode[10], shipper[11], service[2], packageType[2], accessorials[2],
                 committedTnt[2], committedDate[2];
            strncpy(origCountry, records[i].origCountry, 2); origCountry[2] = '\0';
            strncpy(origSlic, records[i].origSlic, 5); origSlic[5] = '\0';
            strncpy(origSort, records[i].origSort, 1); origSort[1] = '\0';
            strncpy(destCountry, records[i].destCountry, 2); destCountry[2] = '\0';
            strncpy(destSlic, records[i].destSlic, 5); destSlic[5] = '\0';
            strncpy(destIbi, records[i].destIbi, 1); destIbi[1] = '\0';
            strncpy(destPostalCode, records[i].destPostalCode, 9); destPostalCode[9] = '\0';
            strncpy(shipper, records[i].shipper, 10); shipper[10] = '\0';
            strncpy(service, records[i].service, 1); service[1] = '\0';
            strncpy(packageType, records[i].packageType, 1); packageType[1] = '\0';
            strncpy(accessorials, records[i].accessorials, 1); accessorials[1] = '\0';
            strncpy(committedTnt, records[i].committedTnt, 1); committedTnt[1] = '\0';
            strncpy(committedDate, records[i].committedDate, 1); committedDate[1] = '\0';

            std::cout << "Record " << i << ":\n"
                      << "  shipmentId: " << records[i].shipmentId << "\n"
                      << "  origCountry: " << origCountry << "\n"
                      << "  origSlic: " << origSlic << "\n"
                      << "  origSort: " << origSort << "\n"
                      << "  destCountry: " << destCountry << "\n"
                      << "  destSlic: " << destSlic << "\n"
                      << "  destIbi: " << destIbi << "\n"
                      << "  destPostalCode: " << destPostalCode << "\n"
                      << "  shipper: " << shipper << "\n"
                      << "  service: " << service << "\n"
                      << "  packageType: " << packageType << "\n"
                      << "  accessorials: " << accessorials << "\n"
                      << "  pieces: " << records[i].pieces << "\n"
                      << "  weight: " << records[i].weight << "\n"
                      << "  cube: " << records[i].cube << "\n"
                      << "  committedTnt: " << committedTnt << "\n"
                      << "  committedDate: " << committedDate << "\n";
        }
    }

    hid_t attr = H5Aopen(dataset, "GIT root revision", H5P_DEFAULT);
    if (attr >= 0) {
        hid_t attr_type = H5Aget_type(attr);
        hsize_t size = H5Tget_size(attr_type);
        char attr_value[size];
        status = H5Aread(attr, attr_type, attr_value);
        if (status >= 0) {
            std::cout << "Attribute 'GIT root revision': " << attr_value << std::endl;
        }
        H5Tclose(attr_type);
        H5Aclose(attr);
    }

    H5Tclose(file_type);
    H5Sclose(dataspace);
    H5Dclose(dataset);
    H5Fclose(file);
    return 0;
}