section .text
  global _start

_start:
  mov rax,1       ; [1] - sys_write
  mov rdi,1       ; 0 = stdin / 1 = stdout / 2 = stderr
  lea rsi,[rel msg]     ; pointer(mem address) to msg (*char[])
  mov rdx, msg_end - msg      ; msg size
  syscall         ; calls the function stored in rax

  mov rax, 0x1111111111111111
  jmp rax

  mov rax,60      ; [60] - sys_exit
  mov rdi,42      ; exit with code rdx(0)
  syscall

align 8
  msg     db 0x1b,'[31msuch infected, much wow!',0x1b,'[0m',0x0a,0
  msg_end db 0x0
