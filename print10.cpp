#include <iostream>
#include <H5Cpp.h>
#include <vector>

using namespace H5;

// const H5std_string FILE_NAME("test.h5");
// const H5std_string FILE_NAME("ForecastedVolume_2025-01-10.h5");
// const H5std_string FILE_NAME("ExportedNodeShips.h5");
const H5std_string DATASET_NAME("Demand");

// ✅ Define the structure to match Python's NumPy dtype
struct Shipment {
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
} __attribute__((packed));

const int NUM_ENTRIES_TO_READ = 10;

int main() {
    try {
        // Open the existing HDF5 file
        H5File file(FILE_NAME, H5F_ACC_RDONLY);

        // Open the dataset
        DataSet dataset = file.openDataSet(DATASET_NAME);

        // Define the compound data type to match Python’s NumPy dtype
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

        // Get the dataset dimensions
        DataSpace dataspace = dataset.getSpace();
        hsize_t num_records;
        dataspace.getSimpleExtentDims(&num_records, nullptr);

        // Limit the number of records read
        hsize_t count = std::min(num_records, static_cast<hsize_t>(NUM_ENTRIES_TO_READ));
        std::vector<Shipment> shipments(count);

        // Select the first `count` records to read
        hsize_t offset = 0;
        dataspace.selectHyperslab(H5S_SELECT_SET, &count, &offset);

        // Create memory space for the records
        DataSpace memspace(1, &count);

        // Read the data into the vector
        dataset.read(shipments.data(), compoundType, memspace, dataspace);

        // ✅ Print the first `count` records
        std::cout << "First " << count << " Shipment Records:\n";
        for (size_t i = 0; i < count; i++) {
            std::cout << "Shipment " << shipments[i].shipmentId << " | "
                      << "Orig: " << std::string(shipments[i].origCountry, 2) << "-"
                      << std::string(shipments[i].origSlic, 5) << "-"
                      << shipments[i].origSort << " | "
                      << "Dest: " << std::string(shipments[i].destCountry, 2) << "-"
                      << std::string(shipments[i].destSlic, 5) << "-"
                      << shipments[i].destIbi << " | "
                      << "Postal: " << std::string(shipments[i].destPostalCode, 9) << " | "
                      << "Shipper: " << std::string(shipments[i].shipper, 10) << " | "
                      << "Service: " << shipments[i].service << " | "
                      << "Pieces: " << shipments[i].pieces << " | "
                      << "Weight: " << shipments[i].weight << " | "
                      << "Cube: " << shipments[i].cube << " | "
                      << "TNT: " << shipments[i].committedTnt << " | "
                      << "Date: " << shipments[i].committedDate
                      << std::endl;
        }

        // Close the dataset and file
        dataset.close();
        file.close();

    } catch (H5::Exception &e) {
        std::cerr << "HDF5 Exception: " << e.getCDetailMsg() << std::endl;
        return 1;
    }

    return 0;
}
