from tensorflow.keras.models import load_model
import numpy as np
import pandas as pd
import tensorflow as tf
import matplotlib.pyplot as plot
from sklearn.preprocessing import LabelEncoder
# from transformers import GPT2LMHeadModel, GPT2Tokenizer
# from transformers import TFGPT2LMHeadModel


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
    model = load_model('ml/bin/icm_letters_v2.keras')
    input_data = pd.read_csv('data/letters/U_1714822330.668097.csv')
    input_data = input_data.values
    input_data = np.array([input_data])
    y = model.predict(input_data)
    letters = ['A', 'I', 'U']
    print(letters[np.argmax(y)])
    for Y in y:
        print(Y)

predict_letters()
   
def predict_digits():
    model = load_model('ml/bin/digits.keras')
    # input_data = pd.read_csv("data/digit/hand_written_digits_training.csv")
    # input_data = input_data[['acc_X','acc_Y','acc_Z']].values
    # input_data = input_data[0:300];
    
    y = model.predict([])
    
    print(y)



def predict_next_letter():
    tokenizer = GPT2Tokenizer.from_pretrained("gpt2")
    model = TFGPT2LMHeadModel.from_pretrained("gpt2")

    sequence = "The next letter after A is"

    inputs = tokenizer.encode(sequence, return_tensors="pt")
    outputs = model.generate(inputs, max_length=30, temperature=1.0, do_sample=True)

    predicted_text = tokenizer.decode(outputs[0], skip_special_tokens=True)

    print(predicted_text)
    