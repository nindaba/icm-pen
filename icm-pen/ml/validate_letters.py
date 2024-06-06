import os
import pandas as pd

LETTER_SIZE = 240


def validate_cleanup(directory):
    for filename in os.listdir(directory):
        if filename.endswith(".csv"):
            if os.path.getsize(os.path.join(directory, filename)) == 0:
                print(f'{filename} is empty, this file will be deleted')
                os.remove(os.path.join(directory, filename))
                continue

            df = pd.read_csv(os.path.join(directory, filename))
            rows, cols = df.shape

            if rows > LETTER_SIZE:
                print(f'{filename} has more than {LETTER_SIZE} rows, triming the values to 299')
                df = df.head(LETTER_SIZE)
                df.to_csv(os.path.join(directory, filename), index=False)
            if rows < LETTER_SIZE:  # add 0 padding till LETTER_SIZE
                print(f'{filename} has less than {LETTER_SIZE} rows, padding with 0')
                padding = pd.DataFrame([[0] * 9] * (LETTER_SIZE - rows), columns=df.columns)
                df = pd.concat([df, padding], ignore_index=True)
                df.to_csv(os.path.join(directory, filename), index=False)

            if cols != 9:
                print(
                    f'{filename} has {cols} columns, expected 9 columns with {LETTER_SIZE} values each, this file will be deleted')
                os.remove(os.path.join(directory, filename))


validate_cleanup('data/letters')
