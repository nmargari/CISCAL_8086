	.xlist
	include	stdlib.a
	includelib	stdlib.lib
	.list

xseg	segment	public 'code'
	assume	cs:xseg, ds:xseg, ss:xseg
	org	100h

main	proc	near
	call	near ptr _test_0
	mov	ax, 4C00h
	int	21h
main	endp

L0:
_foo_1	proc	near
	push	bp
	mov	bp, sp
	sub	sp, 16

L1:
	mov	ax, word ptr [bp + 10]
	mov	si, word ptr [bp + 8]
	mov	dx, word ptr [si]
	cmp	ax, dx
	jg	L3

L2:
	jmp	L8

L3:
	mov	ax, word ptr [bp + 10]
	mov	si, word ptr [bp + 8]
	mov	dx, word ptr [si]
	add	ax, dx
	mov	word ptr [bp + -4], ax

L4:
	mov	ax, word ptr [bp + -4]
	mov	word ptr [bp + -2], ax

L5:
	mov	ax, word ptr [bp + -2]
	mov	si, word ptr [bp + 6]
	movi	word ptr [si], ax

L6:
	jmp	foo_1_END

L7:
	jmp	L10

L8:
	mov	ax, word ptr [bp + 10]
	mov	si, word ptr [bp + 6]
	movi	word ptr [si], ax

L9:
	jmp	foo_1_END

L10:
foo_1_END:
	mov	sp, bp
	pop	bp
	ret
_foo_1	endp

L11:
_bar_1	proc	near
	push	bp
	mov	bp, sp
	sub	sp, 10

L12:
	mov	ax, 1024
	mov	word ptr [bp + -2], ax

L13:
	mov	ax, word ptr [bp + -2]
	puti
	putcr

L14:
bar_1_END:
	mov	sp, bp
	pop	bp
	ret
_bar_1	endp

L15:
_test_0	proc	near
	push	bp
	mov	bp, sp
	sub	sp, 52

L16:
	mov	ax, 8
	mov	word ptr [bp + -2], ax

L17:
	mov	ax, 2
	mov	cx, word ptr [bp + -2]
	imul	cx
	mov	word ptr [bp + -10], ax

L18:
	mov	ax, 6
	cwd
	mov	cx, word ptr [bp + -10]
	idiv	cx
	mov	word ptr [bp + -12], ax

L19:
	mov	ax, word ptr [bp + -2]
	mov	dx, word ptr [bp + -12]
	sub	ax, dx
	mov	word ptr [bp + -14], ax

L20:
	mov	ax, 3
	mov	cx, word ptr [bp + -14]
	imul	cx
	mov	word ptr [bp + -16], ax

L21:
	mov	ax, 10
	mov	dx, word ptr [bp + -16]
	add	ax, dx
	mov	word ptr [bp + -18], ax

L22:
	mov	ax, word ptr [bp + -18]
	mov	word ptr [bp + -4], ax

L23:
	mov	ax, word ptr [bp + -2]
	push	ax

L24:
	lea	si, word ptr [bp + -4]
	push	si

L25:
	lea	si, word ptr [bp + -20]
	push	si

L26:
	push	bp
	call	near ptr _foo_1
	add	sp, 8

L27:
	mov	ax, word ptr [bp + -20]
	mov	word ptr [bp + -6], ax

L28:
	mov	ax, 0
	mov	word ptr [bp + -8], ax

L29:
	mov	ax, word ptr [bp + -2]
	mov	dx, 1
	add	ax, dx
	mov	word ptr [bp + -22], ax

L30:
	mov	ax, word ptr [bp + -22]
	mov	word ptr [bp + -2], ax

L31:
	mov	ax, word ptr [bp + -2]
	mov	dx, 10
	cmp	ax, dx
	je	L33

L32:
	jmp	L35

L33:
	jmp	L60

L34:
	jmp	L35

L35:
	mov	ax, word ptr [bp + -4]
	mov	dx, 1
	add	ax, dx
	mov	word ptr [bp + -24], ax

L36:
	mov	ax, word ptr [bp + -24]
	mov	word ptr [bp + -4], ax

L37:
	mov	ax, word ptr [bp + -4]
	mov	dx, 10
	cmp	ax, dx
	je	L39

L38:
	jmp	L41

L39:
	jmp	L60

L40:
	jmp	L41

L41:
	mov	ax, word ptr [bp + -2]
	push	ax

L42:
	lea	si, word ptr [bp + -4]
	push	si

L43:
	lea	si, word ptr [bp + -26]
	push	si

L44:
	push	bp
	call	near ptr _foo_1
	add	sp, 8

L45:
	mov	ax, word ptr [bp + -8]
	mov	dx, word ptr [bp + -26]
	add	ax, dx
	mov	word ptr [bp + -28], ax

L46:
	mov	ax, word ptr [bp + -28]
	mov	word ptr [bp + -8], ax

L47:
	mov	ax, word ptr [bp + -8]
	mov	dx, 40
	cmp	ax, dx
	jge	L49

L48:
	jmp	L56

L49:
	mov	ax, word ptr [bp + -8]
	mov	dx, 60
	cmp	ax, dx
	jle	L51

L50:
	jmp	L56

L51:
	mov	ax, word ptr [bp + -2]
	mov	dx, 2
	add	ax, dx
	mov	word ptr [bp + -30], ax

L52:
	mov	ax, word ptr [bp + -30]
	mov	word ptr [bp + -2], ax

L53:
	mov	ax, word ptr [bp + -8]
	mov	dx, 1
	add	ax, dx
	mov	word ptr [bp + -32], ax

L54:
	mov	ax, word ptr [bp + -32]
	mov	word ptr [bp + -8], ax

L55:
	jmp	L56

L56:
	mov	ax, word ptr [bp + -8]
	mov	dx, 100
	cmp	ax, dx
	jle	L58

L57:
	jmp	L60

L58:
	mov	ax, word ptr [bp + -8]
	mov	dx, 50
	cmp	ax, dx
	je	L60

L59:
	jmp	L29

L60:
	mov	ax, word ptr [bp + -2]
	mov	dx, 0
	cmp	ax, dx
	jg	L62

L61:
	jmp	L83

L62:
	mov	ax, 3
	mov	dx, 1
	sub	ax, dx
	mov	word ptr [bp + -34], ax

L63:
	mov	ax, word ptr [bp + -4]
	cwd
	mov	cx, word ptr [bp + -34]
	idiv	cx
	mov	word ptr [bp + -36], ax

L64:
	mov	ax, word ptr [bp + -2]
	mov	dx, word ptr [bp + -36]
	cmp	ax, dx
	jl	L68

L65:
	jmp	L66

L66:
	mov	ax, word ptr [bp + -2]
	mov	dx, word ptr [bp + -4]
	cmp	ax, dx
	jg	L68

L67:
	jmp	L70

L68:
	mov	ax, word ptr [bp + -6]
	mov	dx, 0
	cmp	ax, dx
	jg	L81

L69:
	jmp	L70

L70:
	mov	ax, word ptr [bp + -8]
	mov	dx, 100
	cmp	ax, dx
	je	L72

L71:
	jmp	L83

L72:
	mov	ax, word ptr [bp + -2]
	push	ax

L73:
	lea	si, word ptr [bp + -4]
	push	si

L74:
	lea	si, word ptr [bp + -38]
	push	si

L75:
	push	bp
	call	near ptr _foo_1
	add	sp, 8

L76:
	mov	ax, word ptr [bp + -2]
	mov	dx, 6
	sub	ax, dx
	mov	word ptr [bp + -40], ax

L77:
	mov	ax, 3
	mov	cx, word ptr [bp + -40]
	imul	cx
	mov	word ptr [bp + -42], ax

L78:
	mov	ax, 10
	mov	dx, word ptr [bp + -42]
	add	ax, dx
	mov	word ptr [bp + -44], ax

L79:
	mov	ax, word ptr [bp + -38]
	mov	dx, word ptr [bp + -44]
	cmp	ax, dx
	jg	L83

L80:
	jmp	L81

L81:
	mov	ax, word ptr [bp + -8]
	puti
	putcr

L82:
	jmp	L83

L83:
	jmp	test_0_END

L84:
test_0_END:
	mov	sp, bp
	pop	bp
	ret
_test_0	endp

xseg	ends
	end	main
