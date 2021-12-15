#!/usr/bin/env python3
from collections import deque
from os import close, write, read

from fcntl import flock, LOCK_EX, LOCK_UN
from sys import argv, exit
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# switcher for comand line argument
def switch_demo(argument):
    switcher = {
        "Theta": 1, "theta": 1,
        "Psi": 2,   "psi": 2,
        "Phi": 3,   "phi": 3,
        "p": 4,     "P": 4,
        "q": 5,     "Q": 5,
        "r": 6,     "R": 6
    }
    return switcher.get(argument, None)

# switcher for plot title
def switch_Plot_Title(argument):
    switcher = {
        "Phi": r'$\varphi(t)$',    "phi": r'$\varphi(t)$',
        "Psi": r'$\psi(t)$',       "psi": r'$\psi(t)$',
        "Theta": r'$\theta(t)$',   "theta": r'$\theta(t)$',
        "p": r'$\omega_1(t)$',     "P": r'$\omega_1(t)$',
        "q": r'$\omega_2(t)$',     "Q": r'$\omega_2(t)$',
        "r": r'$\omega_3(t)$',     "R": r'$\omega_3(t)$'
    }
    return switcher.get(argument, "default")

# switcher for y label
def switch_ylabel(argument):
    switcher = {
        "Phi"  : r'$\varphi$, rad',        "phi"  : r'$\varphi$, rad',
        "Psi"  : r'$\psi$, rad',           "psi"  : r'$\psi$, rad',
        "Theta": r'$\theta$, rad',         "theta": r'$\theta$, rad',
        "p"    : r'$\omega_1$, rad/s',     "P"    : r'$\omega_1$, rad/s',
        "q"    : r'$\omega_2$, rad/s',     "Q"    : r'$\omega_2$, rad/s',
        "r"    : r'$\omega_3$, rad/s',     "R"    : r'$\omega_3$, rad/s'
    }
    return switcher.get(argument, "default")

# check comand line parametrs
if len(argv) != 2:
    print ("Invalid amount of argv\n")
    exit(1)
print(argv[0])

# select the displayed parameter
param_num = switch_demo(argv[1])
if param_num == None:
    #print("Invalid param value\n")
    exit(1)

PlotTitle = switch_Plot_Title(argv[1])
YLable = switch_ylabel(argv[1])

# amount of points displayed on the screen
npoints = 250
time = deque([0], maxlen=npoints)
param = deque([0], maxlen=npoints)
fig, ax = plt.subplots()
line, = ax.plot([], [], lw=2)

# open data file
file = open('data.dat', 'r')

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
    time.append(time[-1] + 0.01)  # update data
    param.append(float(d_param))
    # print("current time: %d" %(time[-1]))
    # print("curent phi: %f" %float(d_param))

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
            interval = 10, # задержка между кадрами (мс)
            blit = False, # использовать ли двойную буфферизацию
            repeat = True # зацикливать ли анимацию
        )

        ax.minorticks_on()
        ax.grid(which = 'major', color = 'black', linewidth = 1, linestyle = '-', alpha = 0.75)
        ax.grid(which = 'minor', color = 'grey', linewidth = 1, linestyle = '--', alpha = 0.5)
        plt.title(PlotTitle)
        plt.xlabel('time, sec')
        plt.ylabel(YLable)
        plt.show()
    except KeyboardInterrupt:
        print("\nclose file\n")
        file.close()