with h5py.File("singleint.h5", "r") as hdf_file:
    temp_value = hdf_file["temperature"][()]
    print("Temperature:", temp_value)  # Should print: Temperature: 30
