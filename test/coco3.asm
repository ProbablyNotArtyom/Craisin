*
* tm.asm - memory test program for Radio Shack color computer
*
* Jan 1996 WJY
*  2-Feb-96 855pm WJY - v0.3 - data & program in contiguous memory
*  9-Feb-96 150am WJY - v0.4 - corrected hex output routine
* 11-Feb-96 250pm WJY - v0.5 - map in RAM, keep current counters,
*                              mark page by inverse video,
*                              correctly index error count table
* 11-Feb-96 650pm WJY - v0.6 - fixed pattern number increment
* 14-Feb-96 1130am WJY - v0.7 - load .y for page test,
*                               fixed hex1a
* 20-Feb-96 1235mn WJY - v0.8 - check for & warn about address wrap
*                               fill map with X's for wrapped part
*

		ORG $3000

***************************************************************************
* initialized data
*

* 6847 hacked ASCII constants
SPACE   equ $60
ZERO    equ $70
DASH    equ $6d
COLON   equ $7a
DOT     equ $6e

display:
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE
		fcb SPACE,SPACE,SPACE,SPACE,'C,'O,'C,'O,SPACE,'A,'S,'S,'E,'M,'B
		fcb 'L,'E,'R,SPACE,'T,'E,'S,'T,SPACE,SPACE,SPACE,SPACE

* display offsets
dispag  equ 100            list of page numbers
dismap  equ 132            memory map
disetb  equ 150            error table
dispas  equ 423            pass number
dispat  equ 442            pattern number
diserr  equ 457            error count
dismsg  equ 482            message area

stcktop equ *
start:  orcc #$50          disable interrupts
		leas stcktop,pcr   initialize the stack pointer
		leax display,pcr   point SAM to the display
		tfr x,d
		lsra
		ldx #samdisp
		ldb #7
		lbsr sam
		lda #1             tell SAM to fORGet the ROMs
		ldx #samtype
		ldb #1
		lbsr sam
s0:		jmp s0
		
sam:    lsra           set .b SAM bits from .a, starting at .x
		bcs sam1
		sta ,x
		bra sam2
sam1:   sta 1,x
sam2:   leax 2,x
		decb
		bne sam
		rts
		
***************************************************************************
* SAM addresses
*

samvdgm equ $ffc0          video display mode
samdisp equ $ffc6          display address offset
sampage equ $ffd4          page #1
samrate equ $ffd6          cpu rate
samsize equ $ffda          memory size
samtype equ $ffde          memory map type

		end start
