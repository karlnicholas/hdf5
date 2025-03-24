#include <H5Cpp.h>
#include <iostream>

const H5std_string FILE_NAME("singleint.h5");
const H5std_string GROUP_NAME("Sample");
const H5std_string DATASET_NAME("temperature");

int main() {
    try {
        // Create a new HDF5 file (overwrite if exists)
        H5::H5File file(FILE_NAME, H5F_ACC_TRUNC);

        // Create the "Sample" group under the root
        H5::Group group = file.createGroup(GROUP_NAME);

        // Define the data type (int64)
        H5::IntType datatype(H5::PredType::NATIVE_INT64);

        // Create dataspace for a single value
        hsize_t dims[1] = {1};  // 1-element dataset
        H5::DataSpace dataspace(1, dims);

        // Create the dataset inside the "Sample" group
        H5::DataSet dataset = group.createDataSet(DATASET_NAME, datatype, dataspace);

        // Data to write
        int64_t temp_value = 30;

        // Write the data
        dataset.write(&temp_value, H5::PredType::NATIVE_INT64);

        // Explicitly close the dataset, group, and file
        dataset.close();
        group.close();
        file.close();

        std::cout << "HDF5 file '" << FILE_NAME << "' created with dataset '/Sample/temperature' successfully.\n";

    } catch (H5::Exception &error) {
        std::cerr << "HDF5 Exception: " << error.getDetailMsg() << std::endl;
        return -1;
    }

    return 0;
}