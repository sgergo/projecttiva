Tiva Template
==================

https://github.com/ericevenchick

## Toolchain

### Dependencies:

* flex
* bison
* libgmp3-dev
* libmpfr-dev
* libncurses5-dev
* libmpc-dev
* autoconf
* texinfo
* libftdi-dev
* python-yaml
* zlib1g-dev

To get all dependencies on Ubuntu:

    apt-get install flex bison libgmp3-dev libmpfr-dev libncurses5-dev \
    libmpc-dev autoconf texinfo build-essential libftdi-dev python-yaml \
    zlib1g-dev

You will need an ARM bare-metal toolchain to build code for Tiva targets.
You can get a toolchain from the
[gcc-arm-embedded](https://launchpad.net/gcc-arm-embedded) project that is
pre-built for your platform. Extract the package and add the `bin` folder to
your PATH. PATH=$PATH:/opt/gcc-arm-none-eabi-4_8-2014q3/bin

If using 64 bit OS:

sudo apt-get install lib32z1 lib32ncurses5 lib32bz2-1.0

The TivaWare package contains all of the header files and drivers for
Tiva parts. Grab the file *SW-TM4C-1.1.exe* from
[here](http://software-dl.ti.com/tiva-c/SW-TM4C/latest/index_FDS.html) and unzip it into a directory
then run `make` to build TivaWare.

    mkdir tivaware
    cd tivaware
    unzip SW-TM4C-1.1.exe
    make

Note: for the Tiva Connected Launchpad get [SW-EK-TM4C1294XL-2.1.0.12573.exe](http://www.ti.com/tool/sw-ek-tm4c1294xl).

## Writing and Building Firmware

1. Clone the
   [tiva-template](https://github.com/uctools/tiva-template)
   repository (or fork it and clone your own repository).

	git clone git@github.com:uctools/tiva-template

2. Modify the Makefile:
    * Set TARGET to the desired name of the output file (eg: TARGET = main)
    * Set SOURCES to a list of your sources (eg: SOURCES = main.c
      startup\_gcc.c)
    * Set TIVAWARE\_PATH to the full path to where you extracted and built
      TivaWare (eg: TIVAWARE_PATH = /home/eric/code/tivaware)

3. Run `make`

4. The output files will be created in the 'build' folder

## Flashing

The easiest way to flash your device is using lm4flash. First, grab lm4tools
from Git.

    git clone git://github.com/utzig/lm4tools.git

Then build lm4flash and run it:

    cd lm4tools/lm4flash
    make
    lm4flash /path/to/executable.bin

If you need libusb: 
    sudo apt-get install libusb-1.0-0-dev


Copy lm4tools into /opt: `sudo cp -avr lm4tools /opt`
Add to PATH: `PATH=$PATH:/opt/lm4tools/lm4flash`

Plug TI launchpad while monitoring the process:

    tail -f /var/log/syslog

*Write idVendor/idProduct values on a notepad.

(you may stop modem manager: sudo stop modemmanager)

Use `lm4flash main.bin` command to program your board with main.bin. It may complain:

    sg@sg-sajat:~/__WORK__/tivac/tiva-template/build$ lm4flash main.bin
    Unable to open USB device: LIBUSB_ERROR_ACCESS
    Unable to find any ICDI devices

That is, you have no access to the device. You need to create a rule:

    sudo gedit 30-tivac.rules

Add this to the empty file (you need to replace idVendor/idProduct with your own ones):

    KERNEL=="ttyACM[0-9]*",ATTRS{idVendor}=="1cbe",ATTRS{idProduct}=="00fd",MODE:="0666"

Execute `udevadm trigger`. Unplug-replug your board.

    sg@sg-sajat:~/__WORK__/tivac/tiva-template/build$ lm4flash main.bin
    Found ICDI device with serial: 0E200FBE
    ICDI version: 9270

## Debugging with gdb

These chips are supported in openocd HEAD ([credit to Karl Palsson](http://sourceforge.net/p/openocd/mailman/message/32139143/)). The [openocd website](http://openocd.sourceforge.net/) has instructions on how to install it.

With openocd installed, run gdb with this command:
```
arm-none-eabi-gdb -ex 'target extended-remote | openocd -f board/ek-tm4c1294xl.cfg -c "gdb_port pipe; log_output openocd.log"; monitor reset; monitor halt'
```
lmicdi can also support gdb but it appears openocd has better support for breakpoints and source-level debugging right now. You may want to check back later for updates on the situation.

## Credits

Thanks to Recursive Labs for their
[guide](http://recursive-labs.com/blog/2012/10/28/stellaris-launchpad-gnu-linux-getting-started/)
which this template is based on.

Thanks to [Rob Stoddard](http://www.robstoddard.com/stellaris.php) for digging into the issue on soft FP versus hardware FP.
