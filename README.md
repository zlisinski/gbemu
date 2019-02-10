This is my Gamboy emulator. This is a personal project, so for now I'm not looking to make it work with **your** environment.

This project is primarily written to work on Ubuntu 18.04, using g++ 7.3.0 and gnu make 4.1. ~~It also compiles on CentOS 7, using g++ 4.8.5 and gnu make 3.82. Compilation on CentOS is secondary, and not tested for each commit.~~ CentOS coming soon.

## Setup on Kubuntu 18.04

Ubuntu 18.04 doesn't include the libqt5gamepad5 package, so you'll need to build your own custom Qt build. The gamepad library is included in Ubuntu 18.10. I build this on Kubuntu, so instructions assume there is already Qt installed. If you are using Ubuntu/Xubuntu/Mint/etc, instructions may differ slightly. These instructions only build enough Qt features to run the emulator, so features of Qt that are not used by the emulator are not built.

### Building Qt

    sudo apt-get install build-essential libsdl2-dev libfontconfig1-dev
    wget http://download.qt.io/official_releases/qt/5.12/5.12.1/single/qt-everywhere-src-5.12.1.tar.xz
    tar xf qt-everywhere-src-5.12.1.tar.xz
    cd qt-everywhere-src-5.12.1
    ./configure -opensource
    make -j$(nproc)
    sudo make install

### Setting up qtchooser

If you aren't using Kubuntu or don't already have qtchooser installed, you'll have to figure out yourself how to get the Qt binaries and libs in your path. I'm sure google can help you.

    mkdir ~/.config/qtchooser
    cat > ~/.config/qtchooser/5.12.1.conf <<EOF
    /usr/local/Qt-5.12.1/bin
    /usr/local/Qt-5.12.1/lib
    EOF
    echo "export QT_SELECT=5.12.1" >> ~/.bashrc
    export QT_SELECT=5.12.1

## Setup on Kubuntu 18.10

Since 18.10 has the libqt5gamepad5 package, you won't need to build your own Qt version. Note that I've only compiled this in a VM, and haven't tested if the gamepad actually works.

    sudo apt-get install build-essential libsdl2-dev qt5-qmake qtbase5-dev libqt5gamepad5-dev
    echo "export QT_SELECT=5" >> ~/.bashrc
    export QT_SELECT=5

## Build Emulator

    git clone https://github.com/zlisinski/gbemu.git
    cd gbemu
    qmake
    make

## Build Tests

	make -f Makefile.test test
