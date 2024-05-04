import socket
import os
from time import sleep, time
import matplotlib.pyplot as plt
import matplotlib.animation as an
import pandas as pd
from statistics import mean
from tensorflow.keras.models import load_model
import numpy as np

ESP32_ADDR = "192.168.1.57"
ESP32_PORT = 8020
READ_SIZE = 280
CHAR_SIZES = [320,280,200]

READ_SIZE_MAX = np.max( [320,280,200] )

def save_csv_line(filename,line):
    with open(filename, mode='a', newline='') as file:
        if len(line.split(",")) == 9:
            file.writelines(line)


def create_pen_client():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 4096)
    message = "Request For Connection".encode()
    destination = (ESP32_ADDR, ESP32_PORT)

    sock.sendto(message, destination)
    sock.settimeout(2)
    return sock


fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
tcp_client = create_pen_client()
x = []
y = []
z = []
zDelta = 0

x_off = 0
y_off = 0
z_off = 0


def animate(i):
    lines = tcp_client.recv(4096).decode()

    for line in lines.split("\n"):
        data = line.split(",")
        if (len(data) == 9):
            _x, _y, _z = data[:3]
            z.append(float(_z) - z_off)
            y.append(float(_y) - y_off)
            x.append(float(_x) - x_off)
            print(line)

    if (len(x) > 200):
        x.clear()
        y.clear()
        z.clear()

    ax.clear()
    ax.plot(x, label="x")
    ax.plot(y, label="y")
    ax.plot(z, label="z")

    plt.xticks(rotation=45, ha='right')
    plt.title('Accelerometer Sensor')
    plt.ylabel('acceleration m/s2')
    plt.ylim(-2, 2)
    plt.legend()


def find_offsets():
    print("sampling...")
    samples = 100
    x_sample = []
    y_sample = []
    z_sample = []

    for i in range(0, samples):
        lines = tcp_client.recv(4096).decode()

        for line in lines.split("\n"):
            data = line.split(",")
            if (len(data) == 9):
                _x, _y, _z = data[:3]
                z_sample.append(float(_z))
                y_sample.append(float(_y))
                x_sample.append(float(_x))
                print(x_sample[-1], y_sample[-1], z_sample[-1])

    global x_off, y_off, z_off
    x_off = mean(x_sample)
    y_off = mean(y_sample)
    z_off = mean(z_sample)

    print("end of sampling...")


def plot():
    find_offsets()
    an.FuncAnimation(fig, animate, interval=0)

    plt.show()

def collect():
    count = 0
    files_count = 0
    timestamp = time()
    while True:
        lines = tcp_client.recv(4096).decode()
        if count == READ_SIZE:
            count = 0
            files_count += 1
            timestamp = time()
        
        for line in lines.split("\n"):
            if files_count == 100:
                print("#################################### sleeping for 5 seconds start #########################################")
                tcp_client.close()
                
            filename = os.path.join("data", "letters" , f"U_{timestamp}.csv")
            save_csv_line(filename, line + "\n")
            
        count += 1
        percentage = (count / READ_SIZE) * 100
        print(f"Reading {int(percentage)} %", end="\r")
        
def client_speedometer():
    count = 0
    timestamp = pd.Timestamp.now()
    while True:
        lines = tcp_client.recv(4096).decode()
        for line in lines.split("\n"):
            count += 1
            read = (count / READ_SIZE) * 100
            print(f"Reading {int(read)}%", end="\r")
            if(count == READ_SIZE):
                count = 0
                now = pd.Timestamp.now()
                delta =  now - timestamp
                timestamp = now
                print(f"Time taken to read {READ_SIZE} lines: {delta.total_seconds()}")
            
        
def predict():
    model = load_model('ml/bin/icm_letters_v2.keras')
    buffer = np.zeros((1, READ_SIZE, 9))
    count = 0
    while True:
        lines = tcp_client.recv(4096).decode()
        for line in lines.split("\n"):
            data = line.split(",")
            if (len(data) == 9):
                buffer[0, count] = data
                count = count + 1
                percentage = (count / READ_SIZE) * 100
                print(f"Reading {int(percentage)} %", end="\r")
        
        if  count == READ_SIZE:
            labels = ["A", "I", "U"]
            y = model.predict(buffer)
            count = 0
            max_index = np.argmax(y)
            if y[0][max_index] > 0.5:
                print(f"\nPredicted: {y} -> {labels[max_index]}")
                
def run(fuc):
    try:
        fuc()
    except KeyboardInterrupt as e:
        print("Closing connection")
        tcp_client.close()
        print("Connection closed")
        print("Exiting program")
        exit(0)     

# run(collect)
run(predict)