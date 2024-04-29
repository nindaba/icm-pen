from tensorflow.keras.models import load_model
import numpy as np
import pandas as pd
import tensorflow as tf
import matplotlib.pyplot as plot

def predict_numbers():
    # Load the model
    model = load_model('ml/bin/additional.keras')

    # Prepare the input data
    input_data = np.array([[5000,1000]])
    input_data = input_data.reshape(1, -1, 2)

    # Use the model to predict the output
    output = model.predict(input_data)

    print(output)

def predict_letters():
    model = load_model('ml/bin/icm_letters.keras')
    input_data = pd.read_csv("data/A/invalid/2024-04-17 09_12_21.407401.csv" )
    input_data = input_data.values
    input_data = np.array([input_data])
    y = model.predict(input_data)
    
    print(y)
    
def predict_digits():
    model = load_model('ml/bin/digits.keras')
    # input_data = pd.read_csv("data/digit/hand_written_digits_training.csv")
    # input_data = input_data[['acc_X','acc_Y','acc_Z']].values
    # input_data = input_data[0:300];
    
    y = model.predict([])
    
    print(y)

predict_letters()
