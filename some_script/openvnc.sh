#!/bin/sh

su -c "x11vnc -bg -o %HOME/.x11vnc.log.%VNCDISPLAY -display :0 -forever -nopw" debian
