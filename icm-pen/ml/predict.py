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
    input_data = pd.read_csv("data/A/valid/2024-04-17 09_27_18.212832.csv" )
    
    # input_data = np.fft.fft2(input_data)
    # input_data = tf.abs(input_data)
    input_data = tf.reshape(input_data, (1,299, 9, 1))
    
    y = model.predict(input_data)
    
    print(y)
    
def plot_x_acc_y_acc_z_acc_x_gyro_y_gyro_z_gyro_x_mag_y_mag_z_mag(file_path):
    data = pd.read_csv(file_path)
    plot.plot(data["x_acc"], label='x_acc')
    # plot.plot(data["y_acc"], label='y_acc')
    # plot.plot(data["z_acc"], label='z_acc')
    # plot.plot(data["x_gyro"], label='x_gyro')
    # plot.plot(data["y_gyro"], label='y_gyro')
    # plot.plot(data["z_gyro"], label='z_gyro')
    # plot.plot(data["x_mag"], label='x_mag')
    # plot.plot(data["y_mag"], label='y_mag')
    # plot.plot(data["z_mag"], label='z_mag')
    plot.legend(loc='upper right')
    
    

plot_x_acc_y_acc_z_acc_x_gyro_y_gyro_z_gyro_x_mag_y_mag_z_mag("data/A/valid/2024-04-17 09_27_18.212832.csv")
plot_x_acc_y_acc_z_acc_x_gyro_y_gyro_z_gyro_x_mag_y_mag_z_mag("data/A/valid/2024-04-17 09_27_15.220292.csv")
plot_x_acc_y_acc_z_acc_x_gyro_y_gyro_z_gyro_x_mag_y_mag_z_mag("data/A/valid/2024-04-17 09_27_12.219766.csv")
plot.show()
# predict_letters()


