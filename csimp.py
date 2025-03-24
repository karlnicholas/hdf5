import h5py
import numpy as np

# Define the compound datatype with H5T_C_S1 strings
dtype = [
    ("shipmentId", "<u8"),       # 8 bytes, little-endian uint64
    ("origCountry", h5py.string_dtype(encoding='ascii', length=2)),  # 2 bytes, C string
    ("origSlic", h5py.string_dtype(encoding='ascii', length=5)),     # 5 bytes
    ("origSort", h5py.string_dtype(encoding='ascii', length=1)),     # 1 byte
    ("destCountry", h5py.string_dtype(encoding='ascii', length=2)),  # 2 bytes
    ("destSlic", h5py.string_dtype(encoding='ascii', length=5)),     # 5 bytes
    ("destIbi", h5py.string_dtype(encoding='ascii', length=1)),      # 1 byte
    ("destPostalCode", h5py.string_dtype(encoding='ascii', length=9)),  # 9 bytes
    ("shipper", h5py.string_dtype(encoding='ascii', length=10)),     # 10 bytes
    ("service", h5py.string_dtype(encoding='ascii', length=1)),      # 1 byte
    ("packageType", h5py.string_dtype(encoding='ascii', length=1)),  # 1 byte
    ("accessorials", h5py.string_dtype(encoding='ascii', length=1)), # 1 byte
    ("pieces", "<u2"),           # 2 bytes, little-endian uint16
    ("weight", "<u2"),           # 2 bytes
    ("cube", "<u4"),             # 4 bytes, little-endian uint32
    ("committedTnt", h5py.string_dtype(encoding='ascii', length=1)), # 1 byte
    ("committedDate", h5py.string_dtype(encoding='ascii', length=1)),# 1 byte
]

# Number of records
num_records = 1750

# Create the data array
data = np.zeros(num_records, dtype=dtype)

# Fill the data (same as C++ writer)
for i in range(num_records):
    data[i] = (
        1000 + i,              # shipmentId: 1000 to 2749
        b"US",                 # origCountry
        b"SLIC1",              # origSlic
        b"A",                  # origSort
        b"CA",                 # destCountry
        b"SLIC2",              # destSlic
        b"B",                  # destIbi
        b"123456789",          # destPostalCode
        b"SHIPPER123",         # shipper
        b"S",                  # service
        b"P",                  # packageType
        b"Y",                  # accessorials
        5,                     # pieces
        10,                    # weight
        50,                    # cube
        b"T",                  # committedTnt
        b"D"                   # committedDate
    )

# Write to HDF5 file
with h5py.File("demand_data_py.h5", "w") as f:
    # Create dataset with the compound type
    dset = f.create_dataset("Demand", data=data, dtype=dtype)
    
    # Add the attribute as H5T_C_S1 with ASCII encoding
    attr_dtype = h5py.h5t.TypeID.copy(h5py.h5t.C_S1)  # Copy predefined H5T_C_S1
    attr_dtype.set_size(17)  # Fixed length: 17 chars for "Revision: , URL: "
    attr_dtype.set_cset(h5py.h5t.CSET_ASCII)  # Force ASCII encoding
    attr_dtype.set_strpad(h5py.h5t.STR_NULLPAD)  # Match C++ padding
    dset.attrs.create("GIT root revision", b"Revision: , URL: ", dtype=attr_dtype)

print(f"Successfully wrote {num_records} records to 'Demand' dataset in demand_data.h5")