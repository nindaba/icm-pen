import os
import pandas as pd

def validate_files(directory):
    for filename in os.listdir(directory):
        if filename.endswith(".csv"):  # assuming the files are CSVs
            df = pd.read_csv(os.path.join(directory, filename))
            rows, cols = df.shape
            if rows > 299 :
                print(f'{filename} has more than 299 rows, triming the values to 299')
                df = df.head(299)
                df.to_csv(os.path.join(directory, filename), index=False)
            if cols != 9 or rows < 299:
                print(f'{filename} has {cols} columns, expected 9 columns with 300 values each, this file will be deleted')
                os.remove(os.path.join(directory, filename))
                
validate_files('data/A/valid')
validate_files('data/A/invalid')