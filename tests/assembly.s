.data
.text
.global main
main:
	movl $1, -4(%rsp)
	movw $2, -6(%rsp)
	movb $3, -7(%rsp)
	movw -6(%rsp), %ax
	orw -6(%rsp), %ax
	notw %ax
	movw %ax, -6(%rsp)
	movsbw -7(%rsp), %ax
	orw -6(%rsp), %ax
	notw %ax
	movw %ax, -6(%rsp)
	movb -6(%rsp), %al
	orb -7(%rsp), %al
	notb %al
	movb %al, -7(%rsp)
	ret
F0:
	movq 8(%rsp), %rbx
	movw $0, 1(%rbx)
	movb $0, (%rbx)
	ret
F1:
	movq 9(%rsp), %rbx
	movb 8(%rsp), %al
	movb %al, 2(%rbx)
	movb 8(%rsp), %al
	movb %al, 1(%rbx)
	movb 8(%rsp), %al
	movb %al, (%rbx)
	ret
F2:
	movq 11(%rsp), %rbx
	movw 10(%rsp), %ax
	movw %ax, 2(%rbx)
	movb 8(%rsp), %al
	movb %al, (%rbx)
	ret
F3:
	movb 13(%rsp), %al
	addb 10(%rsp), %al
	movb %al, 16(%rsp)
	movb 12(%rsp), %al
	addb 9(%rsp), %al
	movb %al, 15(%rsp)
	movb 11(%rsp), %al
	addb 8(%rsp), %al
	movb %al, 14(%rsp)
	ret
F4:
	movq 8(%rsp), %rbx
	movb 2(%rbx), %sil
	negb %sil
	movb %sil, 18(%rsp)
	movb 1(%rbx), %sil
	negb %sil
	movb %sil, 17(%rsp)
	movb (%rbx), %sil
	negb %sil
	movb %sil, 16(%rsp)
	ret
