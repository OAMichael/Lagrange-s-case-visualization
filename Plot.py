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
        "Phi": 1,   "phi": 1,
        "Psi": 2,   "psi": 2,
        "Theta": 3, "theta": 3,
        "p": 4,     "P": 4,
        "q": 5,     "Q": 5,
        "r": 6,     "R": 6
    }
    return switcher.get(argument, None)

# switcher for plot title
def switch_Plot_Title(argument):
    switcher = {
        "Phi": r'График зависимости $\phi$ от t',           "phi": r'График зависимости $\phi$ от t',
        "Psi": r'График зависимости $\psi$ от t',           "psi": r'График зависимости $\psi$ от t',
        "Theta": r'График зависимости $\theta$ от t',       "theta": r'График зависимости $\theta$ от t',
        "p": r'График зависимости $\omega_1$ от t',     "P": r'График зависимости $\omega_1$ от t',
        "q": r'График зависимости $\omega_2$ от t',     "Q": r'График зависимости $\omega_2$ от t',
        "r": r'График зависимости $\omega_3$ от t',     "R": r'График зависимости $\omega_3$ от t'
    }
    return switcher.get(argument, "default")

# switcher for y label
def switch_ylabel(argument):
    switcher = {
        "Phi": r'$\phi$',       "phi": r'$\phi$',
        "Psi": r'$\psi$',       "psi": r'$\psi$',
        "Theta": r'$\theta$',   "theta": r'$\theta$',
        "p": r'$\omega_1$', "P": r'$\omega_1$',
        "q": r'$\omega_2$', "Q": r'$\omega_2$',
        "r": r'$\omega_3$', "R": r'$\omega_3$'
    }
    return switcher.get(argument, "default")

# check comand line parametrs
if len(argv) != 2:
    print ("Invalid amount of argv\n")
    exit(1)

# select the displayed parameter
param_num = switch_demo(argv[1])
if param_num == None:
    print("Invalid param value\n")
    exit(1)

PlotTitle = switch_Plot_Title(argv[1])
YLable = switch_ylabel(argv[1])

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
            interval = 100, # задержка между кадрами (мс)
            blit = False, # использовать ли двойную буфферизацию
            repeat = True # зацикливать ли анимацию
        )

        ax.grid(color = 'grey', linewidth = 1, linestyle = '--', alpha = 0.75)
        plt.title(PlotTitle)
        plt.xlabel('time')
        plt.ylabel(YLable)
        plt.show()
    except KeyboardInterrupt:
        print("\nclose file\n")
        file.close()