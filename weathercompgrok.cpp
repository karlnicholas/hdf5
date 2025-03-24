#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <H5Cpp.h>

using namespace std;
using namespace H5;

#define BIT_PRECISION 25  // Total precision: 25 bits (7 fractional + 18 integer)
#define BIT_OFFSET 7      // Fractional bits

// Structure for fixed-point data (unsigned 32-bit integers)
struct WeatherDataFixed {
    uint32_t date;
    uint32_t max_temp;
    uint32_t min_temp;
    uint32_t rel_humidity;
    uint32_t dew_point;
    uint32_t atm_pressure;
    uint32_t soil_temp_2in;
    uint32_t soil_temp_4in;
    uint32_t soil_temp_8in;
    uint32_t soil_moisture_12in;
    uint32_t precip;
    uint32_t precip_since_feb1;
    uint32_t precip_since_jan1;
    uint32_t total_radiation;
    uint32_t evapotrans;
    uint32_t wind_speed;
    uint32_t max_wind_speed;
};

// Function to convert double to fixed-point (bitOffset = 7)
uint32_t toFixedPoint(double value) {
    // Scale by 2^7 (128) for 7 fractional bits
    return static_cast<uint32_t>(max(0.0, value) * 128.0);
}

int main() {
    // Open the CSV file
    ifstream file("weatherdata.csv");
    if (!file.is_open()) {
        cerr << "Error opening CSV file 'weatherdata.csv'!" << endl;
        return 1;
    }

    string line;
    vector<WeatherDataFixed> data;

    // Skip the first row (labels) since we’re hardcoding them
    getline(file, line);  // Discard header

    // Read and convert data rows
    while (getline(file, line)) {
        stringstream ss(line);
        string field;
        WeatherDataFixed row;

        getline(ss, field, ','); row.date = toFixedPoint(stod(field));
        getline(ss, field, ','); row.max_temp = toFixedPoint(stod(field));
        getline(ss, field, ','); row.min_temp = toFixedPoint(stod(field));
        getline(ss, field, ','); row.rel_humidity = toFixedPoint(stod(field));
        getline(ss, field, ','); row.dew_point = toFixedPoint(stod(field));
        getline(ss, field, ','); row.atm_pressure = toFixedPoint(stod(field));
        getline(ss, field, ','); row.soil_temp_2in = toFixedPoint(stod(field));
        getline(ss, field, ','); row.soil_temp_4in = toFixedPoint(stod(field));
        getline(ss, field, ','); row.soil_temp_8in = toFixedPoint(stod(field));
        getline(ss, field, ','); row.soil_moisture_12in = toFixedPoint(stod(field));
        getline(ss, field, ','); row.precip = toFixedPoint(stod(field));
        getline(ss, field, ','); row.precip_since_feb1 = toFixedPoint(stod(field));
        getline(ss, field, ','); row.precip_since_jan1 = toFixedPoint(stod(field));
        getline(ss, field, ','); row.total_radiation = toFixedPoint(stod(field));
        getline(ss, field, ','); row.evapotrans = toFixedPoint(stod(field));
        getline(ss, field, ','); row.wind_speed = toFixedPoint(stod(field));
        getline(ss, field, ','); row.max_wind_speed = toFixedPoint(stod(field));

        data.push_back(row);
    }
    file.close();

    // Create HDF5 file
    H5File h5file("weather_grok.h5", H5F_ACC_TRUNC);

    // Define custom fixed-point datatype
    IntType fixed_point_type(PredType::STD_U32LE);
    fixed_point_type.setPrecision(BIT_PRECISION);  // 25 bits of precision
    fixed_point_type.setOffset(BIT_OFFSET);        // Shift to bits 7-31

    // Define compound datatype with new labels
    CompType datatype(sizeof(WeatherDataFixed));
    datatype.insertMember("Date", HOFFSET(WeatherDataFixed, date), fixed_point_type);
    datatype.insertMember("Max Temperature", HOFFSET(WeatherDataFixed, max_temp), fixed_point_type);
    datatype.insertMember("Min Temperature", HOFFSET(WeatherDataFixed, min_temp), fixed_point_type);
    datatype.insertMember("Relative Humidity", HOFFSET(WeatherDataFixed, rel_humidity), fixed_point_type);
    datatype.insertMember("Dew Point", HOFFSET(WeatherDataFixed, dew_point), fixed_point_type);
    datatype.insertMember("Pressure", HOFFSET(WeatherDataFixed, atm_pressure), fixed_point_type);
    datatype.insertMember("2in Soil Temp", HOFFSET(WeatherDataFixed, soil_temp_2in), fixed_point_type);
    datatype.insertMember("4in Soil Temp", HOFFSET(WeatherDataFixed, soil_temp_4in), fixed_point_type);
    datatype.insertMember("8in Soil Temp", HOFFSET(WeatherDataFixed, soil_temp_8in), fixed_point_type);
    datatype.insertMember("12in Soil Moisture", HOFFSET(WeatherDataFixed, soil_moisture_12in), fixed_point_type);
    datatype.insertMember("Precipitation", HOFFSET(WeatherDataFixed, precip), fixed_point_type);
    datatype.insertMember("Precipitation since Feb 1", HOFFSET(WeatherDataFixed, precip_since_feb1), fixed_point_type);
    datatype.insertMember("Precipitation since Jan 1", HOFFSET(WeatherDataFixed, precip_since_jan1), fixed_point_type);
    datatype.insertMember("Total Radiation", HOFFSET(WeatherDataFixed, total_radiation), fixed_point_type);
    datatype.insertMember("Evapotranspiration", HOFFSET(WeatherDataFixed, evapotrans), fixed_point_type);
    datatype.insertMember("Wind Speed", HOFFSET(WeatherDataFixed, wind_speed), fixed_point_type);
    datatype.insertMember("Max Wind Speed", HOFFSET(WeatherDataFixed, max_wind_speed), fixed_point_type);

    // Define dataspace (number of rows)
    hsize_t dims[1] = {data.size()};
    DataSpace dataspace(1, dims);

    // Create dataset named "weather"
    DataSet dataset = h5file.createDataSet("weather", datatype, dataspace);

    // Write data to dataset
    dataset.write(data.data(), datatype);

    cout << "HDF5 file 'weather_grok.h5' created successfully! Fixed-point: bitOffset = 7, bitPrecision = 25, total 32 bits." << endl;

    return 0;
}