#!/usr/bin python3

# need to execute this file in the work directory like libav-12.3
# copy this file to the dir

# function:
# remove the sub-directory "queue" and "hangs"
# can specify like python3 delete-dir.py queue # just remove queue
import os
import sys
import re

del_dir = []

if len(sys.argv) == 1:
    del_dir.append("queue")
    del_dir.append("hangs")
elif len(sys.argv) > 1:
    for d in sys.argv[1:]:
        if d in ("queue", "hangs"):
            del_dir.append(d)

cwd = os.getcwd()

dir_file_list = os.listdir()
pattern = re.compile(r".*-exp")

for df in dir_file_list:
    if not (os.path.isdir(df) and pattern.match(df)):
        dir_file_list.remove(df)

for df in dir_file_list:
    for dirpath, dirnames, filenames in os.walk(df):
        for d in dirnames:
            if d in del_dir:
                os.system("rm -rf {0}".format(os.path.join(dirpath, d)))
