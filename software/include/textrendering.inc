
draw_char:
  ; Draw character function
  ; RAM Parameters: 0x200: X pos
  ;                 0x201: Y pos
  ;                 0x202: Character
  ;                 0x203: BG Color
  ;                 0x204: FG Color

  lda $0202      ; Load char from RAM
  
  clc
  rol A          ; Next lines: rotating a register left (without carry)
  php            ; Push status to stack
  tax            ; Transfer A to X
  pla            ; Load status to A
  and #00000001b ; Mask only carry bit
  sta $00        ; Store Carry in zp
  txa            ; Transfer X to A
  ora $00        ; Or with Carry bit
  clc
  rol A          ; Next lines: rotating a register left (without carry)
  php            ; Push status to stack
  tax            ; Transfer A to X
  pla            ; Load status to A
  and #00000001b ; Mask only carry bit
  sta $00        ; Store Carry in zp
  txa            ; Transfer X to A
  ora $00        ; Or with Carry bit


  pha            ; Push rotated character to stack
  and #11111100b ; Mask | A now contains lsb of char addr
  sta $00        ; Store lsb of char addr in zp
  pla            ; Read rotated character back to A
  and #00000011b ; Mask | A now contains msb of char addr
  ora #10000000b ; OR to add 0x8000 to address
  sta $01        ; Store msb of char addr in zp

  
  ldy #$00
  lda ($00),Y    ; Loading first byte of character bitmap
  sta $02
  ldy #$01
  lda ($00),Y    ; Loading second byte of character bitmap
  sta $03
  ldy #$02
  lda ($00),Y    ; Loading third byte of character bitmap
  sta $04
  ldy #$03
  lda ($00),Y    ; Loading fourth byte of character bitmap
  sta $05

  ; Resseting stuff
  ldx #$00       ; Byte 0
  stx $06
  ldy #00000001b ; Bit
  sty $07
  lda #$00
  sta $09
  lda #$00
  sta $0a
  clc
  jsr _draw_char_render

  rts

_draw_char_render:

  ; Get current pixel's value
  ldx $06        ; ZP 06 -> Current byte
  ldy $07        ; ZP 07 -> Current bit
  lda $02,X      ; A register contains now current byte of char bitmap
  and $07        ; Mask with current bit
  sta $08        ; ZP 08 -> 0 / >0

  jmp _draw_char_render_set_color

_draw_char_render_dp:
  ; Draw pixel
  ; ...
  lda $09        ; Rel X Pos
  adc $0200      ; Add Abs X Pos
  sta $6000

  lda $0a        ; Rel Y Pos
  adc $0201      ; Add Abs Y Pos
  sta $6001

  lda $08
  sta $6002

  ; Increase current pos
  jmp _draw_char_render_inc_pos

_draw_char_render_apx:
  clc

  ; Increase current bit / byte  
  lda $07        ; Transfer bit to A
  rol            ; Rotate A right
  sta $07        ; Buffer A in ZP 07
  beq _draw_char_render_inc_byte  ; If bit is wrapped around, increase byte

  ; Repeat as long as not all bits read
  jmp _draw_char_render

_draw_char_render_inc_byte:
  ldy #00000001b ; Reset current bit value
  sty $07        ; Store current bit value
  ldx $06        ; Load current byte value 
  inx            ; Increment current byte value
  stx $06
  txa            ; Check if current byte is 4
  sbc #$04       ;            ..
  beq _draw_char_render_rts  ; If current byte is 4 return

  jmp _draw_char_render           ; continue rendering

_draw_char_render_rts:
  rts                             ; RTS from 'draw_char'

_draw_char_render_inc_pos:
  ldx $09        ; Load X Position to X Register
  inx            ; Increase X Position
  stx $09        ; Store X position to ZP 09
  txa            ; X -> A
  sbc #$04       ; -5
  beq _draw_char_render_inc_pos_y

  jmp _draw_char_render_apx

_draw_char_render_inc_pos_y:
  ldx #$00
  stx $09
  ldy $0a
  iny
  sty $0a
  tya
  sbc #$06
  beq _draw_char_render_rts

  jmp _draw_char_render_apx

_draw_char_render_set_color:
  lda $08
  beq _draw_char_render_set_color_zero
  jmp _draw_char_render_set_color_nzero

_draw_char_render_set_color_zero:
  lda $0203
  sta $08
  jmp _draw_char_render_dp
_draw_char_render_set_color_nzero:
  lda $0204
  sta $08
  jmp _draw_char_render_dp

;
;   TEXT RENDERING
;

render_text:
  lda $0206
  sta $0c
  lda $0207
  sta $0d
  lda #$00       ; Char index 0
  sta $0205      ; Store char index
  lda $0200      ; First X position to ZP
  sta $0b

render_text_loop:
  ldy $0205
  lda ($0c),Y
  sta $0202
  beq render_done ; Char is 0x00

  sec
  lda $0202
  sbc #$0A        ; New line
  beq render_text_nl
  
  sec
  lda $0202
  sbc #$0D        ; Carriage Return
  beq render_text_cr

  jsr draw_char

  ; Increase X position
  lda $0200
  adc #$05
  sta $0200

  jmp render_text_inc_char_idx
  
  jmp render_text_loop

render_text_cr:
  lda $0b
  sta $0200
  jmp render_text_inc_char_idx

render_text_nl:
  lda $0201
  adc #$07
  sta $0201
  jmp render_text_inc_char_idx

render_text_inc_char_idx:
  ; Increase char index
  ldx $0205
  inx
  stx $0205
  jmp render_text_loop

render_done:
  rts
