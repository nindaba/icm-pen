import socket
import os
from threading import Thread
from time import sleep, time
import matplotlib.pyplot as plt
import matplotlib.animation as an
import pandas as pd
from statistics import mean

from collections import Counter
from tensorflow.keras.models import load_model
from sklearn.preprocessing import LabelEncoder
import numpy as np
from models import icm_letters
import validate_letters

ESP32_ADDR = "192.168.1.57"
ESP32_PORT = 8020
READ_SIZE = 240
CHAR_SIZES = {"A": 240, "I": 280, "U": 200, "O": 200, "E": 240}
VOWELS = ["A", "I", "U", "O", "E"]

READ_SIZE_MAX = np.max([240, 280, 200])


def save_csv_line(filename, line):
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


def collect(letter):
    count = 0
    files_count = 0
    timestamp = time()
    read_size = CHAR_SIZES[letter]
    while True:
        lines = tcp_client.recv(4096).decode()
        if count == read_size:
            count = 0
            files_count += 1
            timestamp = time()

        for line in lines.split("\n"):
            if files_count == 100:
                print(
                    "#################################### sleeping for 5 seconds start #########################################")
                tcp_client.close()

            filename = os.path.join("data", "letters", f"{letter}_{timestamp}.csv")
            save_csv_line(filename, line + "\n")

        count += 1
        percentage = (count / read_size) * 100
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
            if (count == READ_SIZE):
                count = 0
                now = pd.Timestamp.now()
                delta = now - timestamp
                timestamp = now
                print(f"Time taken to read {READ_SIZE} lines: {delta.total_seconds()}")


def predict():
    model = load_model('ml/bin/icm_letters_v2.maxe.keras')
    le = LabelEncoder()
    le.classes_ = np.load('ml/bin/label_encoder.npy')
    buffer = np.zeros((1, READ_SIZE, 9))
    count = 0
    index = 0
    sentence = ""
    while True:
        lines = tcp_client.recv(4096).decode()
        for line in lines.split("\n"):
            data = line.split(",")
            if (len(data) == 9):
                buffer[0, count] = data
                count = count + 1
                percentage = (count / READ_SIZE) * 100
                print(f"Reading {int(percentage)} %", end="\r")

        if count == READ_SIZE:
            y = model.predict(buffer, verbose=0)
            count = 0
            max_index = np.argmax(y)
            if y[0][max_index] > 0.5:
                predicted = le.inverse_transform([max_index])[0]
                sentence += predicted
                print(f"\nPredicted: {y} -> {predicted}")
                # if predicted == VOWELS[index]:
                #     filename = os.path.join("data", "letters__grouped_test", f"{VOWELS[index]}_{time()}.csv")
                #     np.savetxt(filename, buffer[0], delimiter=",")
                # else:
                # filename = os.path.join("data",
                #                         "letters_grouped_test",
                #                         VOWELS[index],
                #                         f"{VOWELS[index]}_{time()}.csv")
                # np.savetxt(filename, buffer[0], delimiter=",")
                index = (index + 1) % 5
                print(f"#### {index} ⚠️ Random vowel: {VOWELS[index]}")
                sleep(1)
                print("1")
                sleep(1)
                print("2")
                sleep(1)
                print("Go Go Go")


def predict_collect_train_loop():
    le = LabelEncoder()
    buffer = np.zeros((1, READ_SIZE, 9))
    sentence = ""

    while True:
        random_vowel = "U"
        for epochs in range(0, 5):
            model = load_model('ml/bin/icm_letters_v2.maxe.keras')
            le.classes_ = np.load('ml/bin/label_encoder.npy')
            count = 0
            for i in range(0, 20):
                while count < READ_SIZE:
                    lines = tcp_client.recv(4096).decode()
                    for line in lines.split("\n"):
                        data = line.split(",")
                        if (len(data) == 9):
                            buffer[0, count] = data
                            count = count + 1
                            percentage = (count / READ_SIZE) * 100
                            print(f"Reading {int(percentage)} %", end="\r")

                y = model.predict(buffer)
                count = 0
                max_index = np.argmax(y)
                if y[0][max_index] > 0.5:
                    predicted = le.inverse_transform([max_index])[0]
                    sentence += predicted
                    print(f"\nPredicted: {y} -> {sentence}")
                    filename = os.path.join("data", "letters", f"{random_vowel}_{time()}.csv")
                    np.savetxt(filename, buffer[0], delimiter=",")

            # random_vowel = VOWELS[]
            # print(f"#### {} ⚠️ Random vowel: {random_vowel}")
            sleep(3)
        print("### Training Thread Started ###")
        Thread(target=validate_train).start()


def run(fuc, *args, **kwargs):
    try:
        fuc()
    except KeyboardInterrupt as e:
        print("Closing connection")
        tcp_client.close()
        print("Connection closed")
        print("Exiting program")
        exit(0)


def validate_train():
    validate_letters.validate_cleanup('data/letters')
    icm_letters.train()


# run(collect, "O")
run(predict)
# run(plot)
# run(predict_collect_train_loop)

# validate_train()

# loop over data/letters and make group all files this first letter in the file name and make all groups have same number of files

# datapth = "data/letters"
# files = os.listdir(datapth)
# print(Counter([file.split("_")[0] for file in files]))

# import os
# import shutil
#
# source = "data/letters"
# destination = "data/temp_train"
# files = os.listdir(source)
# count = 0
# for file in files:
#     if file.startswith("U"):
#         shutil.move(os.path.join(source, file), os.path.join(destination, file))
#         count += 1
#         if count == 20:
#             break
