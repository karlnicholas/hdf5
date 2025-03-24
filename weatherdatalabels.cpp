#include <H5Cpp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> // For std::max
#include <cstring>   // Added for std::strncpy

const H5std_string FILE_NAME("weather_data_labels.h5");
const H5std_string LABELS_DATASET("labels");

int main() {
    try {
        // Open and read the CSV file (only headers)
        std::ifstream csvFile("weatherdata.csv");
        if (!csvFile.is_open()) {
            throw std::runtime_error("Could not open weatherdata.csv");
        }

        std::vector<std::string> headers;
        std::string line;

        if (std::getline(csvFile, line)) {
            std::stringstream ss(line);
            std::string header;
            while (std::getline(ss, header, ',')) {
                headers.push_back(header);
            }
        }
        csvFile.close();

        // Create HDF5 file
        H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);

        // Write Labels dataset
        // Find the maximum length of headers (plus 1 for null terminator)
        size_t maxHeaderLength = 0;
        for (const auto& header : headers) {
            maxHeaderLength = std::max(maxHeaderLength, header.length());
        }
        const int LABEL_SIZE = maxHeaderLength + 1; // Fixed size including null terminator

        hsize_t labelsDims[1] = {headers.size()};
        H5::DataSpace labelsSpace(1, labelsDims);

        // Define fixed-length string datatype (null-terminated C strings)
        H5::StrType labelsType(H5T_C_S1, LABEL_SIZE);
        labelsType.setCset(H5T_CSET_ASCII); // Use ASCII (type 0 equivalent)
        labelsType.setStrpad(H5T_STR_NULLTERM); // Ensure null-terminated strings

        // Prepare data as a vector of fixed-size char arrays
        std::vector<char> labelsData(headers.size() * LABEL_SIZE, 0); // Initialize with zeros
        for (size_t i = 0; i < headers.size(); ++i) {
            std::strncpy(&labelsData[i * LABEL_SIZE], headers[i].c_str(), LABEL_SIZE - 1);
            labelsData[i * LABEL_SIZE + LABEL_SIZE - 1] = '\0'; // Ensure null termination
        }

        // Create and write labels dataset
        H5::DataSet labelsDataset = file.createDataSet(LABELS_DATASET, labelsType, labelsSpace);
        labelsDataset.write(labelsData.data(), labelsType);
        labelsDataset.close();

        file.close();

        std::cout << "HDF5 file '" << FILE_NAME << "' created successfully with 'labels' dataset.\n";

    } catch (H5::Exception& error) {
        std::cerr << "HDF5 Exception: " << error.getDetailMsg() << std::endl;
        return -1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}