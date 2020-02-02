This is my Gamboy emulator. This is a personal project, so for now I'm not looking to make it work with **your** environment.

This project is primarily written to work on Ubuntu 18.04, using g++ 7.3.0 and gnu make 4.1. It also works on Raspbian 10.

* [Setup on Kubuntu 18.04](#setup-on-kubuntu-1804)
* [Setup on Kubuntu 18.10+, and Raspbian 10](#setup-on-kubuntu-1810-and-raspbian-10)
* [Build Emulator](#build-emulator)
* [Build Tests](#build-tests)


## Setup on Kubuntu 18.04

Ubuntu 18.04 doesn't include the libqt5gamepad5 package. You'll need to either build your own custom Qt build, or go without gamepad support. There are #ifdefs in the code so it will build if the gamepad library is missing.

### Setup without Gamepad support

    sudo apt-get install build-essential cmake qt5-qmake qtbase5-dev qtmultimedia5-dev

### Adding Gamepad support

I build this on Kubuntu, so instructions assume there is already Qt installed. If you are using Ubuntu/Xubuntu/Mint/etc, instructions may differ slightly. These instructions only build enough Qt features to run the emulator, so features of Qt that are not used by the emulator are not built.

#### Building Qt

    sudo apt-get install build-essential libsdl2-dev libfontconfig1-dev
    wget http://download.qt.io/official_releases/qt/5.12/5.12.1/single/qt-everywhere-src-5.12.1.tar.xz
    tar xf qt-everywhere-src-5.12.1.tar.xz
    cd qt-everywhere-src-5.12.1
    ./configure -opensource
    make -j$(nproc)
    sudo make install

Add /usr/local/Qt-5.12.1/bin to the front of your $PATH variable.

## Setup on Kubuntu 18.10+, and Raspbian 10

Since these versions have the libqt5gamepad5 package, you won't need to build your own Qt version.

    sudo apt-get install build-essential cmake libsdl2-dev qt5-qmake qtbase5-dev libqt5gamepad5-dev qtmultimedia5-dev

## Build Emulator

    git clone https://github.com/zlisinski/zlgb.git
    cd zlgb
    make

## Build Tests

	make test
