
; fasm example of writing simple EXE program

format MZ

	push	cs
	pop	ds

	mov	ah,9
	mov	dx,hello
	int	21h

	mov	ax,4C00h
	int	21h

hello db 'Hello world!',24h
