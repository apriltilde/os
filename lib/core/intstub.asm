[BITS 32]

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15

extern isr_handler

%macro ISR 1
isr%1:
    cli                   ; Optional: prevent nested interrupts
    pushad                ; Save all general-purpose registers
    push ds
    push es
    push fs
    push gs

    push dword 32 + %1    ; Push IRQ number
    call isr_handler
    add esp, 4            ; Clean IRQ number

    pop gs
    pop fs
    pop es
    pop ds
    popad

    sti                   ; Re-enable interrupts
    iretd
%endmacro

ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15

