.include "include/common.inc"

.name "sound_ch1_1khz"

.bank 0 slot 0
.org $100
    nop
    jp $150

.org $150
    ld a, $77
    ldh ($24), a    ; NR50 SO1=7, SO2=7

    ld a, $11
    ldh ($25), a    ; NR51 Channel 1 to SO1 and SO2

    ld a, $81
    ldh ($26), a    ; NR52 All sounds on, channel 1 on

    xor a
    ldh ($10), a    ; NR10 No sweep

    ld a, $80
    ldh ($11), a    ; NR11 50% duty cycle

    ld a, $F0
    ldh ($12), a    ; NR12 Volume = 0x0F

    ld a, $F4
    ldh ($13), a    ; NR13 Frequecy = 0x5F4, 1000hz

    ld a, $85
    ldh ($14), a    ; NR14 Initialize, continuous sound, high frequency = 0x05

-   nop
    jr -