#!/bin/bash

echo "$(date)" > serial.txt

./zlgb ../gb-test-roms/cpu_instrs/individual/01-special.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/02-interrupts.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/03-op\ sp\,hl.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/04-op\ r\,imm.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/05-op\ rp.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/06-ld\ r\,r.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/08-misc\ instrs.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/09-op\ r,r.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/10-bit\ ops.gb > /dev/null
./zlgb ../gb-test-roms/cpu_instrs/individual/11-op\ a,\(hl\).gb > /dev/null
./zlgb ../gb-test-roms/instr_timing/instr_timing.gb > /dev/null
./zlgb ../gb-test-roms/mem_timing/individual/01-read_timing.gb > /dev/null
./zlgb ../gb-test-roms/mem_timing/individual/02-write_timing.gb > /dev/null
./zlgb ../gb-test-roms/mem_timing/individual/03-modify_timing.gb > /dev/null


colorize() {
    esc=$(printf "\033[")
    red="31m"
    green="32m"
    normal="0m"
    sed '/^$/d' "$@" | sed -e "s/Passed/$esc$green&$esc$normal\n/g" -e "s/^.*Failed.*$/$esc$red&$esc$normal\n/g"
}

cat serial.txt | colorize
