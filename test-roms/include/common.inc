.memorymap
    defaultslot 0
    slot 0 $0000 size $4000
    slot 1 $4000 size $4000
    slot 2 $C000 size $2000
.endme

.rombanksize $4000
.rombanks 2

.gbheader
    licenseecodenew "ZL"
    cartridgetype $00
    ramsize $00
    countrycode $01
    nintendologo
    version $00
    romdmg
.endgb

.define RegP1   $FF00
.define RegSB   $FF01
.define RegSC   $FF02
.define RegDIV  $FF04
.define RegTIMA $FF05
.define RegTMA  $FF06
.define RegTAC  $FF07
.define RegIF   $FF0F
.define RegNR10 $FF10
.define RegNR11 $FF11
.define RegNR12 $FF12
.define RegNR13 $FF13
.define RegNR14 $FF14
.define RegNR21 $FF16
.define RegNR22 $FF17
.define RegNR23 $FF18
.define RegNR24 $FF19
.define RegNR30 $FF1A
.define RegNR31 $FF1B
.define RegNR32 $FF1C
.define RegNR33 $FF1D
.define RegNR34 $FF1E
.define RegNR41 $FF20
.define RegNR42 $FF21
.define RegNR43 $FF22
.define RegNR44 $FF23
.define RegNR50 $FF24
.define RegNR51 $FF25
.define RegNR52 $FF26
.define RegWAVE $FF30
.define RegLCDC $FF40
.define RegSTAT $FF41
.define RegSCY  $FF42
.define RegSCX  $FF43
.define RegLY   $FF44
.define RegLYC  $FF45
.define RegDMA  $FF46
.define RegBGP  $FF47
.define RegOBP0 $FF48
.define RegOBP1 $FF49
.define RegWY   $FF4A
.define RegWX   $FF4B
.define RegIE   $FFFF