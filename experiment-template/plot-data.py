#!/usr/bin python3
import os
import shutil
import sys

if len(sys.argv) == 1:
    cwd = os.getcwd()
elif len(sys.argv) == 2:
    cwd = os.path.abspath(sys.argv[1])
else:
    print("Oos, Error argvs...")
    exit(0)

folds = cwd.split(os.sep)
tool_name = folds[-1]

times_dict = {
    "first": "1st",
    "second": "2nd",
    "third": "3rd",
    "fourth" : "4th",
    "fifth": "5th"
}

if not os.path.exists(os.path.join(cwd, tool_name)):
    os.makedirs(os.path.join(cwd, tool_name))
# ./libtiff-4.0.9/first-exp/out-AFL-Depth/plot_data
res = []
for dirpath, dirnames, filenames in os.walk(cwd):
    for f in filenames:
        if f == "plot_data":
            folds = dirpath.split(os.sep)
            times = folds[-2].split('-')[0]
            times = times_dict[times]
            metric = folds[-1].split('-')[-1]
            filename = '_'.join([tool_name, times, metric])
            shutil.copyfile(os.path.join(dirpath, f), os.path.join(cwd, tool_name, filename))