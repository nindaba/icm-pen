import socket
import os
from time import sleep
import matplotlib.pyplot as plt
import matplotlib.animation as an
import pandas as pd
from statistics import mean

ESP32_ADDR = "192.168.1.57"
ESP32_PORT = 8020

def save_csv_line(filename,line):
    with open(filename, mode='a', newline='') as file:
        if len(line.split(",")) == 9:
            file.writelines(line)


def move_data_to_pure():
    input_file = 'data/icm.csv'  # Input file
    output_file = 'data/icm_pure.csv'  # Output file  # Output file

    # Open the input file for reading and output file for writing
    with open(input_file, 'r') as f_input, open(output_file, 'w') as f_output:
        # Iterate over each line in the input file
        f_output.write("acc_x, acc_y, acc_z,mgn_x,mgn_y,mgn_z,gyr_x,gyr_y,gyr_z\n")
        for line in f_input:
            # Check if the line has non-zero length
            # if len(line.strip()) >= len("65343.000000,170.000000,16906.000000,65343.000000,170.000000,16906.000000,65343.000000,170.000000,16"):
            # Write the line to the output file if it has non-zero length
            if line.__contains__("NULL") == False:
                f_output.write(line)
            print(line)
            # time.sleep(1)


def create_pen_client():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 4096)
    message = "Request For Connection".encode()
    destination = (ESP32_ADDR, ESP32_PORT)

    # Send the message
    sock.sendto(message, destination)
    sock.settimeout(2)
    return sock


fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)
# ser = serial.Serial("/dev/tty.usbserial-SI88480", 2000000)
tcp_client = create_pen_client()
x = []
y = []
z = []
zDelta = 0

x_off = 0
y_off = 0
z_off = 0


# Function to update the graph with new data
def animate(i):
    # lines = ser.readall().decode()

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

    # Draw x and y lists
    ax.clear()
    ax.plot(x, label="x")
    ax.plot(y, label="y")
    ax.plot(z, label="z")

    # Format plot
    plt.xticks(rotation=45, ha='right')
    plt.title('Accelerometer Sensor')
    plt.ylabel('acceleration m/s2')
    plt.ylim(-2, 2)
    plt.legend()
    # plt.axis([1, None, 0, 1.1])  # Use for arbitrary number of trials
    # plt.axis([1, 100, 0, 1.1]) #Use for 100 trial demo


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
    # Create a figure and axis
    # fig, ax = plt.subplots()
    # Animate the graph using FuncAnimation
    ani = an.FuncAnimation(fig, animate, interval=0)  # Update every second (1000 milliseconds)
    # ani.save('animation.mp4')
    # Show the animated plot

    plt.show()

def collect():
    count = 0
    timestamp = pd.Timestamp.now()
    files_count = 0
    while True:
        lines = tcp_client.recv(4096).decode()
        if count ==300:
            timestamp = pd.Timestamp.now()
            count = 0
            files_count += 1
        
        for line in lines.split("\n"):
            #get timestamp for naming the file
            # Use the timestamp to name the file
            print(line)
            
            if files_count == 100:
                print("#################################### sleeping for 5 seconds start #########################################")
                sleep(5)
                print("#################################### sleeping for 5 seconds end   #########################################")
                
                
            filename = os.path.join("data", "A","valid" , f"{timestamp}.csv")
            save_csv_line(filename, line + "\n")
            
        
        count += 1
        
if __name__ == '__main__':
    # remove_broken_lines()
    # read_pen()
    # threading.Thread(target=move_data_to_pure).start()

    # create a thread for moving the data to icm_pure.csv using move_data_to_pure() and start it
    # thread = threading.Thread(target=move_data_to_pure)
    # thread.start()
    # time.sleep(1)
    # x = z
    # plot()
    # move_data_to_pure()
    # read_pen()
    
        
