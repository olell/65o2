;   65o2 Draw library
;   by olel
;   contains functions to draw stuff
;

fill_screen:
    lda #$ff   ; Width -> 256
    sta $0203
    lda #$c0   ; Height -> 192
    sta $0204
    lda #$00
    sta $0201
    sta $0202
    
    jsr fill_box
    rts

fill_box:
    ; This subroutine fills a given box in one color
    ; Parameters:
    ;   0x0200: Color
    ;   0x0201: X
    ;   0x0202: Y
    ;   0x0203: Width
    ;   0x0204: Height
    ;
    ; Zero Page usage:
    ;   0x00: Current X
    ;   0x01: Current Y

    ldx $0201 ; Set current X to start value
    stx $00   ; ^

    ldy $0202 ; Set current Y to start value
    sty $01   ; ^

    ; Calculate X+wd, Y+hg
    lda $0201
    clc
    adc $0203
    sta $0203
    lda $0202
    clc
    adc $0204
    sta $0204

fill_box_loop:
    ; Part of "fill_box"
    
    ldx $00   ; Increase X
    inx       ; ^
    stx $6000 ; Store X position to display
    stx $00   ; Store X to ZP

    ldy $01
    sty $6001 ; Store Y position to display

    lda $0200
    sta $6002 ; Store color to display

    txa                   ; X -> A
    sec
    sbc $0203             ; Subtract width
    beq fill_box_inc_y ; x == 0: y ++

    jmp fill_box_loop

fill_box_inc_y:
    ; Part of "fill_box"

    ; Reset X position
    ldx $0201
    stx $00

    ; Increases current Y position
    ldy $01
    iny
    sty $01

    ; Check if display is filled
    tya                 ; Y -> A
    sbc $0204           ; subtract height
    beq fill_box_rts ; on 0 -> rts

    jmp fill_box_loop

fill_box_rts:
    rts