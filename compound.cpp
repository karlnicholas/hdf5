#include <iostream>
#include <H5Cpp.h>
#include <vector>
#include <cstring>  // For memcpy

using namespace H5;

const H5std_string FILE_NAME("test.h5");
const H5std_string DATASET_NAME("Demand");
const H5std_string ATTRIBUTE_NAME("GIT root revision");

// ✅ Define the packed structure
struct __attribute__((packed)) Shipment {
    uint64_t shipmentId;
    char origCountry[2];
    char origSlic[5];
    uint8_t origSort;
    char destCountry[2];
    char destSlic[5];
    uint8_t destIbi;
    char destPostalCode[9];
    char shipper[10];
    uint8_t service;
    uint8_t packageType;
    uint8_t accessorials;
    uint16_t pieces;
    uint16_t weight;
    uint32_t cube;
    uint8_t committedTnt;
    uint8_t committedDate;
};

// Number of records
const int NUM_RECORDS = 1750;

int main() {
    try {
        // Create a new HDF5 file
        H5File file(FILE_NAME, H5F_ACC_TRUNC);

        // ✅ Define the compound data type directly from `Shipment`
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
        hsize_t maxdim[1] = { NUM_RECORDS }; // Explicitly fix maxdims
        DataSpace dataspace(1, dim, maxdim);

        // ✅ Create dataset
        DataSet dataset = file.createDataSet(DATASET_NAME, compoundType, dataspace);

        // ✅ ADD ATTRIBUTE: "GIT root revision"
        H5std_string attribute_value = "Revision: , URL: ";
        StrType attr_type(PredType::C_S1, attribute_value.size());
        DataSpace attr_space(H5S_SCALAR);
        Attribute attribute = dataset.createAttribute(ATTRIBUTE_NAME, attr_type, attr_space);
        attribute.write(attr_type, attribute_value);
        attribute.close();

        // ✅ Allocate a vector of `Shipment` structs
        std::vector<Shipment> shipments(NUM_RECORDS);

        // ✅ Fill the struct array
        for (int i = 0; i < NUM_RECORDS; i++) {
            shipments[i].shipmentId = i + 1000;
            memcpy(shipments[i].origCountry, "US", 2);
            memcpy(shipments[i].origSlic, "12345", 5);
            shipments[i].origSort = 4;
            memcpy(shipments[i].destCountry, "CA", 2);
            memcpy(shipments[i].destSlic, "67890", 5);
            shipments[i].destIbi = 0;
            memcpy(shipments[i].destPostalCode, "A1B2C3", std::min(sizeof("A1B2C3") - 1, sizeof(shipments[i].destPostalCode)));
            memcpy(shipments[i].shipper, "FedEx", std::min(sizeof("FedEx") - 1, sizeof(shipments[i].shipper)));
            shipments[i].service = 0;
            shipments[i].packageType = 3;
            shipments[i].accessorials = 0;
            shipments[i].pieces = 2;
            shipments[i].weight = 50;
            shipments[i].cube = 1200;
            shipments[i].committedTnt = 255;
            shipments[i].committedDate = 3;
        }

        // ✅ Write to dataset
        dataset.write(shipments.data(), compoundType);

        // ✅ Close resources
        dataset.close();
        file.close();

        std::cout << "HDF5 file created and written successfully!\n";
    } catch (Exception &e) {
        std::cerr << "HDF5 Exception: " << e.getCDetailMsg() << std::endl;
        return 1;
    }

    return 0;
}
