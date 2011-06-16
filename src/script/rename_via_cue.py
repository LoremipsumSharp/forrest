#!/usr/bin/python
# -*- coding: utf-8 -*-

# Batch rename flac filenames. New filenames are based on CUE info.
# author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/

import sys
import os
import subprocess
import re

def print_usage_and_exit():
    print "Usage: "+sys.argv[0]+" cue_file flac_filename_prefix [-r]"
    sys.exit(10)

def massage(filename):
    return re.sub(os.sep, ",", filename.strip())

if __name__ == '__main__':
    argc = len(sys.argv) - 1
    if argc == 2:
        direction = 0
    elif argc == 3:
        if sys.argv[3] == "-r":
            direction = 1
        else:
            print_usage_and_exit()
    else:
        print_usage_and_exit()

    cuefile = sys.argv[1]
    prefix  = sys.argv[2]

    cuetype = subprocess.Popen(["file", cuefile], stdout=subprocess.PIPE).communicate()[0].strip(" \n")
    # re_str = cuefile+": UTF-8 Unicode text.*"        # doesn't work when cuefile contains `[' and `]', like this: 陈升.-.[鸦片玫瑰].专辑.(APE).cue
    re_str = ".*: UTF-8 Unicode text.*"
    m = re.match(re_str, cuetype)
    if not m:
        print "Please recode the CUE file to UTF8 first. Current character set:"
        print cuetype
        sys.exit(20)

    cuef = open(cuefile)
    current = "00"
    album = {}
    for line in cuef:
        m = re.match(r"\s*TRACK\s*([0-9]+)\s*AUDIO\s*", line)
        if m:
            current = m.group(1)

        m = re.match(r'\s*TITLE\s*"(.*)"\s*', line)
        if m:
            album[current] = m.group(1)

    root = "."
    if direction == 0:
        for item in os.listdir(root):
            if os.path.isfile(os.path.join(root, item)):
                re_str = prefix+"([0-9]+).flac"
                m = re.match(re_str, item)
                if m:
                    n = m.group(1)
                    print n+" <"+album[n]+">"
                    newfile = n+" - "+massage(album[n])+".flac"
                    print item, "->", newfile
                    os.rename(item, newfile)
    elif direction == 1:
        for item in os.listdir(root):
            if os.path.isfile(os.path.join(root, item)):
                re_str = "([0-9]+) - (.*).flac"
                m = re.match(re_str, item)
                if m:
                    n = m.group(1)
                    if massage(album[n]) == m.group(2):
                        newfile = prefix+n+".flac"
                        print item, "->", newfile
                        os.rename(item, newfile)
                    else:
                        print item, "and", cuefile, "do not match."
    else:
        assert 0

    sys.exit(0)

