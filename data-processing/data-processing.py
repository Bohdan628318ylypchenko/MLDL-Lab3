import pandas as pd
import numpy as np
from sklearn.preprocessing import LabelEncoder
from shutil import rmtree
from os.path import exists, isdir
from os import makedirs

DATA_NAME = "original_data.csv"
DATA_OUTPUT_DIR = "./out/data"
OUTPUT_DIR = "./out"

BINARY_OUTPUT = "pnn.bin"

# Read data
data = pd.read_csv(DATA_NAME)

# Show column info
print(">>> Original data >>>")
for column in data.columns:
    unique_values_count = data[column].nunique()
    unique_values = data[column].unique()
    print(f"Column: {column}")
    print(f"Unique Values Count: {unique_values_count}")
    print(f"Unique Values: {unique_values}")
    print()
print(">>> Column types >>>\n", data.dtypes, "\n")

# Show unique classes and their count
print(">>> Class | Count >>>")
print(data["label"].value_counts(), "\n")

# Drop lnum_outbound_cmds and is_host_login columns since values are same for all rows
data = data.drop(columns = ["lnum_outbound_cmds", "is_host_login"])

# Convert protocol_type, service, flag columns to numeric
nonnumeric_columns = ["protocol_type", "service", "flag"]
label_encoder = LabelEncoder()
for column in nonnumeric_columns:
    data[column] = label_encoder.fit_transform(data[column])

# Show column info
print(">>> Data: removed useless columns, all numeric >>>")
for column in data.columns:
    unique_values_count = data[column].nunique()
    unique_values = data[column].unique()
    print(f"Column: {column}")
    print(f"Unique Values Count: {unique_values_count}")
    print(f"Unique Values: {unique_values}")
    print()
print(">>> Column types >>>\n", data.dtypes, "\n")

# Show unique classes and their count
print(">>> Class | Count >>>")
print(data["label"].value_counts(), "\n")

# Delete old run data and recreate out directory tree
if exists(OUTPUT_DIR) and isdir(OUTPUT_DIR):
    rmtree(OUTPUT_DIR)
makedirs(DATA_OUTPUT_DIR)

# Group original data into dictionary by class name { class name -> dataframe }
# and save result in out directory
property_count = len(data.columns.tolist()) - 1
rows_by_class = dict(tuple(data.groupby("label")))
for k,v in rows_by_class.items():
    v_final = v.drop(columns = ["label"])
    v_final.insert(0, "property_count", property_count)
    #v_final.insert(1, "reference_count", v_final.shape[0])
    v_final.to_csv(DATA_OUTPUT_DIR + "/" + k + "-" + str(v.shape[0]) + ".csv", header = False, index = True, lineterminator = ",")

# Save general info to csv
with open(OUTPUT_DIR + "/info.csv", "w", newline = "\n") as info:
    info.write(",".join([str(property_count),
                         str(data.shape[0]),
                         str(len(data["label"].unique()))]))