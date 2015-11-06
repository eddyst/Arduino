##############################################
# Project: C:\uC51\Eigene-Projekte\1018_APPL_DS1820\ds1820_1.mak
# (generated with MakeWiz)
# uC/51 ANSI C Compiler - www.Wickenhaeuser.de
##############################################

L51FLAGS = -r$0,$0
SIOTYPE = K
MODEL = small
B2HFLAGS = -s
A51FLAGS = -g
C51FLAGS = -dCPU_NSEC=1085

cc03er.obj: cc03er.c
ds1820_1.obj: ds1820_1.c
terminal.obj: terminal.c
ds1820_1.bin: cc03er.obj ds1820_1.obj terminal.obj
ds1820_1.hex: ds1820_1.bin

