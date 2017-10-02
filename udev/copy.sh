#!/bin/sh
sudo cp 91-my_robot.rules /etc/udev/rules.d/
sudo service udev restart
