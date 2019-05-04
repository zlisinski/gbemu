.include "include/common.inc"

.name "wave_channel"

.bank 1
.org $00    ; 0x4000
;.db $11 $23 $56 $78 $99 $98 $76 $67 $9A $DF $FE $C9 $85 $42 $11 $31
.db $00 $00 $00 $00 $00 $00 $00 $00 $FF $FF $FF $FF $FF $FF $FF $FF

.bank 0
.org $100
    nop
    jp $150

.org $150

    ld c, $10           ; Load wave data from $4000 to $FF30
    ld hl, RegWAVE
    ld de, $4000
-   ld a, (de)
    ldi (hl), a
    inc de
    dec c
    jr nz, -

    ld a, $77
    ldh (<RegNR50), a   ; SO1=7, SO2=7

    ld a, $44
    ldh (<RegNR51), a   ; Channel 3 to SO1 and SO2

    ld a, $84
    ldh (<RegNR52), a   ; All sounds on, channel 1 on

    ld a, $80
    ldh (<RegNR30), a   ; Enable channel 3

    ld a, $20
    ldh (<RegNR32), a   ; Full volume

    ld a, $FF
    ldh (<RegNR33), a   ; Frequency

    ld a, $83
    ldh (<RegNR34), a   ; Initialize, continuous

-   nop
    jr -
