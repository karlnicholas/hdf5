#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <H5Cpp.h>

#define CSV_FILE_NAME "weatherdata.csv"
#define H5_FILE_NAME "weather_gpt.h5"
#define DATASET_NAME "weather"
#define BIT_OFFSET 7  // Integer part uses 7 bits
#define FIXED_POINT_SCALE (2 << BIT_OFFSET)  // 2 << 7 = 128

using namespace H5;
using namespace std;

// Structure for storing weather data (all fields as unsigned 32-bit fixed-point values)
struct WeatherData {
    uint32_t date;
    uint32_t max_temp;
    uint32_t min_temp;
    uint32_t rel_humidity;
    uint32_t dew_point;
    uint32_t pressure;
    uint32_t soil_temp_2in;
    uint32_t soil_temp_4in;
    uint32_t soil_temp_8in;
    uint32_t soil_moisture_12in;
    uint32_t precipitation;
    uint32_t precip_feb1;
    uint32_t precip_jan1;
    uint32_t total_radiation;
    uint32_t evapotranspiration;
    uint32_t wind_speed;
    uint32_t max_wind_speed;
};

// Convert double to **unsigned** fixed-point representation
uint32_t to_fixed_point(double value) {
    return static_cast<uint32_t>(value * FIXED_POINT_SCALE);
}

// Read CSV file and parse data
vector<WeatherData> read_csv() {
    ifstream file(CSV_FILE_NAME);
    vector<WeatherData> data;
    string line;
    
    // Skip the first line (headers)
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string cell;
        WeatherData record;
        vector<double> values;

        // Read all values in the line
        while (getline(ss, cell, ',')) {
            values.push_back(stod(cell));
        }

        if (values.size() != 17) {
            cerr << "Invalid row data, skipping..." << endl;
            continue;
        }

        // Populate structure with **unsigned** fixed-point conversions
        record.date = static_cast<uint32_t>(values[0]);  // Date remains as-is
        record.max_temp = to_fixed_point(values[1]);
        record.min_temp = to_fixed_point(values[2]);
        record.rel_humidity = to_fixed_point(values[3]);
        record.dew_point = to_fixed_point(values[4]);
        record.pressure = to_fixed_point(values[5]);
        record.soil_temp_2in = to_fixed_point(values[6]);
        record.soil_temp_4in = to_fixed_point(values[7]);
        record.soil_temp_8in = to_fixed_point(values[8]);
        record.soil_moisture_12in = to_fixed_point(values[9]);
        record.precipitation = to_fixed_point(values[10]);
        record.precip_feb1 = to_fixed_point(values[11]);
        record.precip_jan1 = to_fixed_point(values[12]);
        record.total_radiation = to_fixed_point(values[13]);
        record.evapotranspiration = to_fixed_point(values[14]);
        record.wind_speed = to_fixed_point(values[15]);
        record.max_wind_speed = to_fixed_point(values[16]);

        data.push_back(record);
    }

    return data;
}

// Write data to HDF5 with proper fixed-point definition
void write_hdf5(const vector<WeatherData>& data) {
    H5File file(H5_FILE_NAME, H5F_ACC_TRUNC);

    // Create HDF5 fixed-point datatype (32-bit, unsigned)
    IntType fixed_point_type(PredType::STD_U32LE);
    fixed_point_type.setPrecision(25);  // Total bits
    fixed_point_type.setOffset(BIT_OFFSET);  // Bit offset = 7

    // Create compound datatype (all fields as fixed-point)
    CompType weather_type(sizeof(WeatherData));
    weather_type.insertMember("Date", HOFFSET(WeatherData, date), PredType::STD_U32LE);
    weather_type.insertMember("Max Temperature", HOFFSET(WeatherData, max_temp), fixed_point_type);
    weather_type.insertMember("Min Temperature", HOFFSET(WeatherData, min_temp), fixed_point_type);
    weather_type.insertMember("Relative Humidity", HOFFSET(WeatherData, rel_humidity), fixed_point_type);
    weather_type.insertMember("Dew Point", HOFFSET(WeatherData, dew_point), fixed_point_type);
    weather_type.insertMember("Pressure", HOFFSET(WeatherData, pressure), fixed_point_type);
    weather_type.insertMember("2in Soil Temp", HOFFSET(WeatherData, soil_temp_2in), fixed_point_type);
    weather_type.insertMember("4in Soil Temp", HOFFSET(WeatherData, soil_temp_4in), fixed_point_type);
    weather_type.insertMember("8in Soil Temp", HOFFSET(WeatherData, soil_temp_8in), fixed_point_type);
    weather_type.insertMember("12in Soil Moisture", HOFFSET(WeatherData, soil_moisture_12in), fixed_point_type);
    weather_type.insertMember("Precipitation", HOFFSET(WeatherData, precipitation), fixed_point_type);
    weather_type.insertMember("Precipitation since Feb 1", HOFFSET(WeatherData, precip_feb1), fixed_point_type);
    weather_type.insertMember("Precipitation since Jan 1", HOFFSET(WeatherData, precip_jan1), fixed_point_type);
    weather_type.insertMember("Total Radiation", HOFFSET(WeatherData, total_radiation), fixed_point_type);
    weather_type.insertMember("Evapotranspiration", HOFFSET(WeatherData, evapotranspiration), fixed_point_type);
    weather_type.insertMember("Wind Speed", HOFFSET(WeatherData, wind_speed), fixed_point_type);
    weather_type.insertMember("Max Wind Speed", HOFFSET(WeatherData, max_wind_speed), fixed_point_type);

    // Create dataset
    hsize_t dims[1] = { data.size() };
    DataSpace dataspace(1, dims);
    DataSet dataset = file.createDataSet(DATASET_NAME, weather_type, dataspace);

    // Write data
    dataset.write(data.data(), weather_type);
}

int main() {
    vector<WeatherData> weather_data = read_csv();

    if (weather_data.empty()) {
        cerr << "No data read from CSV file!" << endl;
        return 1;
    }

    write_hdf5(weather_data);

    cout << "HDF5 file '" << H5_FILE_NAME << "' written successfully!" << endl;
    return 0;
}
