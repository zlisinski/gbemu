.include "include/common.inc"

.name "lengthAfterInit"

.bank 0 slot 0
.org $100
    nop
    jp $150

.org $150
    ld a, $77
    ldh (<RegNR50), a    ; NR50 SO1=7, SO2=7

    ld a, $11
    ldh (<RegNR51), a    ; NR51 Channel 1 to SO1 and SO2

    ld a, $81
    ldh (<RegNR52), a    ; NR52 All sounds on, channel 1 on

    xor a
    ldh (<RegNR10), a    ; NR10 No sweep

    ld a, $80
    ldh (<RegNR11), a    ; NR11 50% duty cycle

    ld a, $F0
    ldh (<RegNR12), a    ; NR12 Volume = 0x0F

    ld a, $F4
    ldh (<RegNR13), a    ; NR13 Frequecy = 0x5F4, 1000hz

    ld a, $C5
    ldh (<RegNR14), a    ; NR14 Initialize, counter, high frequency = 0x05

    ld hl, $FFFF         ; Loop for a while.
-   dec hl               ; 16-bit DEC doesn't set flags,
    ld a, h              ; so OR each byte to check for zero.
    or l
    jr nz, -

    ld a, $C5
    ldh (<RegNR14), a    ; Replay sound without resetting length counter.

-   nop
    jr -