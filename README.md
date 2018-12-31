This is my Gamboy emulator. This is a personal project, so for now I'm not looking to make it work with **your** environment.

This project is primarily written to work on Ubuntu 18.04, using g++ 7.3.0 and gnu make 4.1. It also runs on CentOS 7, using g++ 4.8.5 and gnu make 3.82, although compilation on this platform is secondary, and not tested for each commit.

Building on Ubuntu:
	`sudo apt-get install build-essential libsdl2-dev libsdl2-ttf-dev`
	`make`
Building on CentOS:
	`sudo yum install gcc-c++ make SDL2-devel SDL2_ttf-devel`
	`make`

