##############################################
# Project: C:\uC51\Eigene-Projekte\1018_APPL_DS1820\ds1820_dem.mak
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
ds1820_dem.obj: ds1820_dem.c
terminal.obj: terminal.c
ow.obj: ow.c
ds1820.obj: ds1820.c
ds1820_dem.bin: cc03er.obj ds1820_dem.obj terminal.obj ow.obj ds1820.obj
ds1820_dem.hex: ds1820_dem.bin

