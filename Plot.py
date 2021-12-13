#!/usr/bin/env python
from collections import deque
from os import close, write, read

from fcntl import flock, LOCK_EX, LOCK_UN
from sys import argv, exit
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# switcher for comand line argument
def switch_demo(argument):
    switcher = {
        "Phi": 1,
        "Psi": 2,
        "Theta": 3,
        "p": 4,
        "q": 5,
        "r": 6
    }
    return switcher.get(argument, None)

# check comand line parametrs
if len(argv) != 2:
    print ("Invalid amount of argv\n")
    exit(1)

# select the displayed parameter
param_num = switch_demo(argv[1])
if param_num == None:
    print("Invalid param value\n")
    exit(1)

# amount of points displayed on the screen
npoints = 250
time = deque([0], maxlen=npoints)
param = deque([0], maxlen=npoints)
fig, ax = plt.subplots()
[line] = ax.step(time, param)

# open data file
file = open('data.txt', 'r')

# function for anomation -- give chosen parametr from data file
def GiveParam():
    # lock file for other processes (in critical section)
    flock(file, LOCK_EX)
    d_param = file.readline().split()[param_num]
    # unlock file for other processes
    flock(file, LOCK_UN)
    yield d_param

# function for anomation -- update old values by new parametrs from data file
def UpdateParam(d_param):
    time.append(time[-1] + 1)  # update data
    param.append(float(d_param))
    print("current time: %d" %(time[-1]))
    print("curent phi: %f" %float(d_param))

    line.set_xdata(time)  # update plot data
    line.set_ydata(param)

    ax.relim()  # update axes limits
    ax.autoscale_view(True, True, True)
    return line, ax

if __name__ == "__main__":

    try :
        animation = FuncAnimation(
            fig, # фигура, где отображается анимация
            func = UpdateParam, # функция обновления текущего кадра
            frames = GiveParam, # параметр, меняющийся от кадра к кадру
            interval = 100, # задержка между кадрами (мс)
            blit = False, # использовать ли двойную буфферизацию
            repeat = True # зацикливать ли анимацию
        )

        ax.grid(color = 'grey', linewidth = 1, linestyle = '--', alpha = 0.75)
        plt.title('Test')
        plt.xlabel('time')
        plt.ylabel('Param')
        plt.show()
    except KeyboardInterrupt:
        print("\nclose file\n")
        file.close()