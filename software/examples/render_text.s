  .org $8400

message:
  .string "Hello, world!",13,10,"Textrendering & Multiline, NICE!",13,10,10,"Two lines space!",13,10,10,10,10,10,"And fancy special characters: ",13,10,"   ",$9a," ",$97," ",$80," ",$8c, 0

reset:
  ldx #$ff       ; Reset stackpointer to 0xFF
  txs  
  ldx #$00

  ; Drawing a Text:
  ;  0x0200 -> X-pos
  ;  0x0201 -> Y-pos
  ;  0x0202 -> Leave zero, will be used for currently used character
  ;  0x0203 -> BG Color
  ;  0x0204 -> FG Color
  ;  0x0206 -> lsb of text addr
  ;  0x0207 -> msb of text addr
  lda #00111111b ; Background Color
  sta $0203      ; 
  lda #00000000b ; Foreground Color
  sta $0204      ; 
  lda #$0        ; X pos
  sta $0200      ; 
  lda #$0        ; Y pos
  sta $0201      ; 
  lda #<message  ; LSB of message
  sta $0206      ; 
  lda #>message  ; MSB of message
  sta $0207      ; 

  jsr render_text


loop:
  jmp loop

  INCLUDE include/textrendering.inc 

words:
  .org $fffc
  .word reset
  .word $0000
