import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score

# List of CSV file paths
csv_files = ["a.csv", "b.csv", "c.csv"]

# Corresponding labels
labels = ["A", "B", "C"]

# Initialize an empty DataFrame for storing the features
features_df = pd.DataFrame()

# Load each CSV file as a DataFrame and flatten into a single row
for file in csv_files:
    df = pd.read_csv(f"data/forest/{file}")
    flattened_df = pd.DataFrame(df.values.flatten()).T
    features_df = features_df.append(flattened_df, ignore_index=True)

# Encode the labels
encoder = LabelEncoder()
encoded_labels = encoder.fit_transform(labels)

# Split the data into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(features_df, encoded_labels, test_size=0.2, random_state=42)

# Train a model
model = RandomForestClassifier()
model.fit(X_train, y_train)

# Make predictions on the testing set
y_pred = model.predict(X_test)

# Evaluate the model
print("Accuracy:", accuracy_score(y_test, y_pred))