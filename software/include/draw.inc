;   65o2 Draw library
;   by olel
;   contains functions to draw stuff
;

fill_screen:
    ; This subroutine fills the whole screen (256 * 192px) in one color
    ; Parameters:
    ;   0x0200: Color
    ;
    ; Zero Page usage:
    ;   0x00: Current X
    ;   0x01: Current Y

    ldx #$00  ; Set current X to Zero
    stx $00   ; ^

    ldy #$00  ; Set current Y to Zero
    sty $00  ; ^

fill_screen_loop:
    ; Part of "fill_screen"
    
    ldx $00   ; Increase X
    inx       ; ^
    stx $6000 ; Store X position to display
    stx $00   ; Store X to ZP

    ldy $01
    sty $6001 ; Store Y position to display

    lda $0200
    sta $6002 ; Store color to display

    txa                   ; X -> A
    beq fill_screen_inc_y ; X wrapped around (X == 0)

    jmp fill_screen_loop

fill_screen_inc_y:
    ; Part of "fill_screen"
    ; Increases current Y position
    ldy $01
    iny
    sty $01

    ; Check if display is filled
    tya                 ; Y -> A
    sbc #$c0            ; subtract 192
    beq fill_screen_rts ; on 0 -> rts

    jmp fill_screen_loop

fill_screen_rts:
    rts