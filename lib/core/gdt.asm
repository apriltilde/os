; gdt_flush.asm
[BITS 32]
[GLOBAL gdt_flush]

gdt_flush:
    mov eax, [esp + 4]     ; Get pointer to GDT
    lgdt [eax]             ; Load GDT

    mov ax, 0x10           ; Data segment selector (GDT entry 2)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:flush_cs      ; Code segment selector (GDT entry 1)
flush_cs:
    ret

