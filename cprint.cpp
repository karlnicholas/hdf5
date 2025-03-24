#include <iostream>
#include <H5Cpp.h>

using namespace H5;
using namespace std;

const H5std_string FILE_NAME("test.h5");
// const H5std_string FILE_NAME("ExportedNodeShips.h5");
// const H5std_string FILE_NAME("ForecastedVolume_2025-01-10.h5");
const H5std_string DATASET_NAME("Demand");

// ✅ Define the struct (packed to match Python)
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
        // ✅ Open the HDF5 file
        H5File file(FILE_NAME, H5F_ACC_RDONLY);

        // ✅ Open the dataset
        DataSet dataset = file.openDataSet(DATASET_NAME);

        // ✅ Create a compound datatype matching the struct
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

        // ✅ Get dataset space and dimensions
        DataSpace dataspace = dataset.getSpace();
        hsize_t num_records;
        dataspace.getSimpleExtentDims(&num_records, nullptr);

        // ✅ Read only the first 10 records
        hsize_t count = 10;
        hsize_t offset = 0;
        dataspace.selectHyperslab(H5S_SELECT_SET, &count, &offset);
        DataSpace memspace(1, &count);
        Shipment records[10];
        dataset.read(records, compoundType, memspace, dataspace);

        // ✅ Print records
        for (int i = 0; i < 10; i++) {
            cout << "Record " << i << ": "
                 << "shipmentId=" << records[i].shipmentId
                 << ", origCountry=" << string(records[i].origCountry, 2)
                 << ", origSlic=" << string(records[i].origSlic, 5)
                 << ", origSort=" << records[i].origSort
                 << ", destCountry=" << string(records[i].destCountry, 2)
                 << ", destSlic=" << string(records[i].destSlic, 5)
                 << ", destIbi=" << records[i].destIbi
                 << ", destPostalCode=" << string(records[i].destPostalCode, 9)
                 << ", shipper=" << string(records[i].shipper, 10)
                 << ", service=" << records[i].service
                 << ", packageType=" << records[i].packageType
                 << ", accessorials=" << records[i].accessorials
                 << ", pieces=" << records[i].pieces
                 << ", weight=" << records[i].weight
                 << ", cube=" << records[i].cube
                 << ", committedTnt=" << records[i].committedTnt
                 << ", committedDate=" << records[i].committedDate
                 << endl;
        }

        // ✅ Close resources
        dataset.close();
        file.close();

    } catch (Exception &e) {
        cerr << "HDF5 Exception: " << e.getCDetailMsg() << endl;
        return 1;
    }

    return 0;
}
