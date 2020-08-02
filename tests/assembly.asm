.code
main PROC
	mov DWORD PTR [rsp - 4], 1
	mov WORD PTR [rsp - 6], 2
	mov BYTE PTR [rsp - 7], 3
	mov ax, WORD PTR [rsp - 6]
	or ax, WORD PTR [rsp - 6]
	not ax
	mov WORD PTR [rsp - 6], ax
	movsx ax, BYTE PTR [rsp - 7]
	or ax, WORD PTR [rsp - 6]
	not ax
	mov WORD PTR [rsp - 6], ax
	mov al, BYTE PTR [rsp - 6]
	or al, BYTE PTR [rsp - 7]
	not al
	mov BYTE PTR [rsp - 7], al
	ret
F0:
	mov rbx, QWORD PTR [rsp + 8]
	mov WORD PTR [rbx + 1], 0
	mov BYTE PTR [rbx], 0
	ret
F1:
	mov rbx, QWORD PTR [rsp + 9]
	mov al, BYTE PTR [rsp + 8]
	mov BYTE PTR [rbx + 2], al
	mov al, BYTE PTR [rsp + 8]
	mov BYTE PTR [rbx + 1], al
	mov al, BYTE PTR [rsp + 8]
	mov BYTE PTR [rbx], al
	ret
F2:
	mov rbx, QWORD PTR [rsp + 11]
	mov ax, WORD PTR [rsp + 10]
	mov WORD PTR [rbx + 2], ax
	mov al, BYTE PTR [rsp + 8]
	mov BYTE PTR [rbx], al
	ret
F3:
	mov al, BYTE PTR [rsp + 13]
	add al, BYTE PTR [rsp + 10]
	mov BYTE PTR [rsp + 16], al
	mov al, BYTE PTR [rsp + 12]
	add al, BYTE PTR [rsp + 9]
	mov BYTE PTR [rsp + 15], al
	mov al, BYTE PTR [rsp + 11]
	add al, BYTE PTR [rsp + 8]
	mov BYTE PTR [rsp + 14], al
	ret
F4:
	mov rbx, QWORD PTR [rsp + 8]
	mov sil, BYTE PTR [rbx + 2]
	neg sil
	mov BYTE PTR [rsp + 18], sil
	mov sil, BYTE PTR [rbx + 1]
	neg sil
	mov BYTE PTR [rsp + 17], sil
	mov sil, BYTE PTR [rbx]
	neg sil
	mov BYTE PTR [rsp + 16], sil
	ret
main ENDP
END
