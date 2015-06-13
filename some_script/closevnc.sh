#!/bin/sh
su -c "x11vnc -R disconnect:all" debian
su -c "x11vnc -R stop" debian


