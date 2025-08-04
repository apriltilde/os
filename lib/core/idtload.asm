[BITS 32]
global idt_load

; void idt_load(struct idt_ptr *idtp);
; Loads the IDT from the pointer passed in EAX (cdecl calling convention).

idt_load:
    mov eax, [esp + 4]    ; get pointer to idt_ptr struct from stack
    lidt [eax]            ; load IDT from the pointer's address
    ret

