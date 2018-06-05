#!/usr/bin/env python
# coding=utf-8
import os
import sys

if len(sys.argv)!=2:
    print("Usage: ./compile.py sourcefile")
    sys.exit(-1)

if os.path.exists(sys.argv[1]) == False:
    print("%s not exist!" % sys.argv[1])
    sys.exit(-1)

if os.path.exists("./parser") == False:
    os.system("make clean")
    os.system("make")

# generate IR 
(filepath,tempfilename) = os.path.split(sys.argv[1])
(shotname,extension) = os.path.splitext(tempfilename)

util="util/io.o"
irfile= "%s/%s.ll" % (filepath,shotname)
assemfile= "%s/%s.s" % (filepath,shotname)
binfile= "%s/%s" % (filepath,shotname)
os.system("./parser %s 2>%s" %(sys.argv[1],irfile))

# generate assembly

os.system("llc %s"% irfile)

# compile to executable

os.system("gcc -o %s %s %s" %(binfile,assemfile,util))
