#! /bin/bash
set -x
find -name "*.[h|c]" | xargs clang-format -i
make clean
make
[[ -z $1 ]] && qemu-system-x86_64 -curses -drive id=boot,format=raw,file=$USER.img,if=none -drive id=data,format=raw,file=$USER-data.img,if=none -device ahci,id=ahci -device ide-drive,drive=boot,bus=ahci.0 -device ide-drive,drive=data,bus=ahci.1 -gdb tcp::9999 -no-reboot
