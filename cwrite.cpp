#include <iostream>
#include <H5Cpp.h>

using namespace H5;
using namespace std;

const H5std_string FILE_NAME("demand_data.h5");
const H5std_string DATASET_NAME("Demand");
const int NUM_RECORDS = 1750;

// ✅ Define the struct (matches Python `align=True`)
struct __attribute__((packed)) Shipment {
    uint64_t shipmentId;
    char origCountry[2];
    char origSlic[5];
    char origSort;
    char destCountry[2];
    char destSlic[5];
    char destIbi;
    char destPostalCode[9];
    char shipper[10];
    char service;
    char packageType;
    char accessorials;
    uint16_t pieces;
    uint16_t weight;
    uint32_t cube;
    char committedTnt;
    char committedDate;
};

int main() {
    try {
        // ✅ Create HDF5 file
        H5File file(FILE_NAME, H5F_ACC_TRUNC);

        // ✅ Define compound type
        CompType compoundType(sizeof(Shipment));
        compoundType.insertMember("shipmentId", HOFFSET(Shipment, shipmentId), PredType::NATIVE_UINT64);
        compoundType.insertMember("origCountry", HOFFSET(Shipment, origCountry), StrType(PredType::C_S1, 2));
        compoundType.insertMember("origSlic", HOFFSET(Shipment, origSlic), StrType(PredType::C_S1, 5));
        compoundType.insertMember("origSort", HOFFSET(Shipment, origSort), PredType::NATIVE_UINT8);
        compoundType.insertMember("destCountry", HOFFSET(Shipment, destCountry), StrType(PredType::C_S1, 2));
        compoundType.insertMember("destSlic", HOFFSET(Shipment, destSlic), StrType(PredType::C_S1, 5));
        compoundType.insertMember("destIbi", HOFFSET(Shipment, destIbi), PredType::NATIVE_UINT8);
        compoundType.insertMember("destPostalCode", HOFFSET(Shipment, destPostalCode), StrType(PredType::C_S1, 9));
        compoundType.insertMember("shipper", HOFFSET(Shipment, shipper), StrType(PredType::C_S1, 10));
        compoundType.insertMember("service", HOFFSET(Shipment, service), PredType::NATIVE_UINT8);
        compoundType.insertMember("packageType", HOFFSET(Shipment, packageType), PredType::NATIVE_UINT8);
        compoundType.insertMember("accessorials", HOFFSET(Shipment, accessorials), PredType::NATIVE_UINT8);
        compoundType.insertMember("pieces", HOFFSET(Shipment, pieces), PredType::NATIVE_UINT16);
        compoundType.insertMember("weight", HOFFSET(Shipment, weight), PredType::NATIVE_UINT16);
        compoundType.insertMember("cube", HOFFSET(Shipment, cube), PredType::NATIVE_UINT32);
        compoundType.insertMember("committedTnt", HOFFSET(Shipment, committedTnt), PredType::NATIVE_UINT8);
        compoundType.insertMember("committedDate", HOFFSET(Shipment, committedDate), PredType::NATIVE_UINT8);

        // ✅ Create data space
        hsize_t dim[1] = { NUM_RECORDS };
        DataSpace space(1, dim);

        // ✅ Create dataset
        DataSet dataset = file.createDataSet(DATASET_NAME, compoundType, space);

        // ✅ Allocate array
        Shipment *shipments = new Shipment[NUM_RECORDS];

        // ✅ Populate data
        for (int i = 0; i < NUM_RECORDS; i++) {
            shipments[i].shipmentId = 1000 + i;
            memcpy(shipments[i].origCountry, "US", 2);
            memcpy(shipments[i].origSlic, "12345", 5);
            shipments[i].origSort = 'L';
            memcpy(shipments[i].destCountry, "CA", 2);
            memcpy(shipments[i].destSlic, "67890", 5);
            shipments[i].destIbi = 'N';
            memcpy(shipments[i].destPostalCode, "A1B2C3", 9);
            memcpy(shipments[i].shipper, "FedEx", 10);
            shipments[i].service = 'E';
            shipments[i].packageType = 'B';
            shipments[i].accessorials = 'N';
            shipments[i].pieces = 2;
            shipments[i].weight = 50;
            shipments[i].cube = 1200;
            shipments[i].committedTnt = 'Y';
            shipments[i].committedDate = 'D';
        }

        // ✅ Write to dataset
        dataset.write(shipments, compoundType);

        // ✅ Clean up
        delete[] shipments;
        dataset.close();
        file.close();

        cout << "HDF5 file '" << FILE_NAME << "' created successfully." << endl;
    } catch (Exception &e) {
        cerr << "HDF5 Exception: " << e.getCDetailMsg() << endl;
        return 1;
    }

    return 0;
}
