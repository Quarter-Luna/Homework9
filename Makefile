# 
# Makefile
# Author: Ian Moon
# Date: Apr 19, 2022
#
# COSC 3750, Homework 8,9,10
#
# This is a Makefile designed to compile the wyscanner.c/h, 
# simple.c, and wyshell.c
# Collaborated with Zach Chrimmel on this Homework
#

CC=gcc
CFLAGS= -ggdb -Wall
RM= rm -f

.PHONY: new clean tidy

new: wyshell

wyshell: wyscanner.c
	${CC} ${CFLAGS} wyscanner.c wyshell.c -o wyshell

clean:
	${RM} *.o wyshell

tidy:
	${RM} a.out core.* wyshell