Zephyr RTOS on QEMU: Terminal Dashboard

A small Zephyr application that runs on an emulated x86 board in QEMU. You don't need hardware or admin rights to run it.

It draws a live three-line dashboard in the terminal, fills a progress bar over 60 seconds, prints Done., and then stops updating.

Dashboard running in QEMU

Features
Boots Zephyr v3.7 on the qemu_x86 board
Redraws the screen in place with ANSI escape codes instead of scrolling
Uses k_sleep() for timing and parks the thread with K_FOREVER when the work is done
Reads the firmware version from a VERSION file
Requirements
Tool	Version used
Ubuntu	22.04 (Jammy)
Zephyr	v3.7.2-rc2
Zephyr SDK	0.16.9
QEMU	6.2.0 (qemu-system-i386)
Python	3.10 or newer
Project Structure
zephyr-iot/
├── CMakeLists.txt   # build description
├── prj.conf         # Kconfig options
├── VERSION          # firmware version
├── README.md
├── docs/
│   └── dashboard.png
└── src/
    └── main.c       # application code
Setup
1. Install west
bash
mkdir -p ~/zephyrproject
python3 -m venv ~/zephyrproject/.venv
source ~/zephyrproject/.venv/bin/activate
pip install west
2. Fetch Zephyr
bash
cd ~/zephyrproject
west init
west update
west zephyr-export
pip install -r zephyr/scripts/requirements.txt

Install the Zephyr SDK if you don't already have it.

3. Install QEMU

With sudo:

bash
sudo apt install qemu-system-x86

Without sudo: download the packages and unpack them into your home folder.

bash
mkdir -p ~/debs && cd ~/debs
apt-get download qemu-system-x86 qemu-system-data seabios
mkdir -p ~/qemu-local
dpkg-deb -x qemu-system-x86_*.deb  ~/qemu-local
dpkg-deb -x qemu-system-data_*.deb ~/qemu-local
dpkg-deb -x seabios_*.deb          ~/qemu-local
cp -n ~/qemu-local/usr/share/seabios/bios-256k.bin ~/qemu-local/usr/share/qemu/
export PATH=$HOME/qemu-local/usr/bin:$PATH

Then add a wrapper so that west can find the QEMU firmware folder:

bash
mv ~/qemu-local/usr/bin/qemu-system-i386 ~/qemu-local/usr/bin/qemu-system-i386.real
cat > ~/qemu-local/usr/bin/qemu-system-i386 << 'EOF'
#!/bin/sh
exec "$HOME/qemu-local/usr/bin/qemu-system-i386.real" \
     -L "$HOME/qemu-local/usr/share/qemu" "$@"
EOF
chmod +x ~/qemu-local/usr/bin/qemu-system-i386

To set PATH in every new terminal, add the export PATH=... line to ~/.bashrc.

Build and Run
bash
source ~/zephyrproject/.venv/bin/activate
cd ~/zephyrproject/zephyr

# Build
west build -b qemu_x86 ~/zephyr-iot -d ~/zephyr-iot/build

# Run
west build -t run -d ~/zephyr-iot/build

To exit QEMU, press Ctrl+A, then X. Ctrl+C stops west but leaves QEMU running.

Expected Output
*** Booting Zephyr OS build v3.7.2-rc2 ***
=== Hello from Zephyr ===
Uptime   : 60 s
Progress : [########################################] 100%
Done.
Configuration

prj.conf

CONFIG_LOG=y
CONFIG_HWINFO=y

VERSION

VERSION_MAJOR = 1
VERSION_MINOR = 0
PATCHLEVEL = 0
VERSION_TWEAK = 0

To change the bar width or duration, edit these values in src/main.c:

c
#define BAR_WIDTH   40   /* characters in the bar */
#define CYCLE_SECS  60   /* seconds to reach 100% */
How It Works
Escape code	Effect
\033[2J	Clear the screen once at start
\033[<row>;1H	Move the cursor to a fixed line
\033[K	Clear the rest of the line
\033[32m ... \033[0m	Print in green, then reset the color

Each second, the loop moves the cursor back to lines 2 and 3 and rewrites them, so the frame stays in one place. When step reaches CYCLE_SECS, the loop exits, prints Done., and the thread sleeps forever.

Troubleshooting
Problem	Fix
qemu-system-i386: command not found	Install QEMU (step 3), or run export PATH=$HOME/qemu-local/usr/bin:$PATH again
could not load PC BIOS 'bios-256k.bin'	Unpack the seabios package and copy bios-256k.bin into ~/qemu-local/usr/share/qemu
undefined symbol APP_VERSION_MAJOR	Set the version in the VERSION file, not in prj.conf
zephyr/hwinfo.h: No such file	Use #include <zephyr/drivers/hwinfo.h>
/tmp/slip.sock serial error	Networking options in prj.conf require a host socket. Remove them for this demo.
Output scrolls instead of updating in place	Your terminal doesn't support ANSI codes. Use a standard Linux terminal.
