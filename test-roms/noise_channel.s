.include "include/common.inc"

.name "noiseChannel"

.bank 0 slot 0
.org $100
    nop
    jp $150

.org $150
    ld a, $77
    ldh (<RegNR50), a    ; SO1=7, SO2=7

    ld a, $88
    ldh (<RegNR51), a    ; Channel 4 to SO1 and SO2

    ld a, $88
    ldh (<RegNR52), a    ; All sounds on, channel 1 on

    ld a, $00
    ldh (<RegNR41), a    ; Sound length = 64-0

    ld a, $F0
    ldh (<RegNR42), a    ; Volume = 0x0F

    ld a, $00
    ldh (<RegNR43), a    ; 

    ld a, $C0
    ldh (<RegNR44), a    ; Initialize, counter

-   nop
    jr -