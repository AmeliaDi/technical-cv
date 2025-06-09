; bootloader.asm - Custom x86 Bootloader
; Author: Amelia Enora 🌈 Marceline Chavez Barroso
; Description: A custom bootloader with advanced features
; Architecture: x86 (16-bit real mode -> 32-bit protected mode)
; Features: Memory detection, A20 gate, GDT setup, kernel loading

[BITS 16]                   ; Start in 16-bit real mode
[ORG 0x7C00]               ; BIOS loads bootloader at 0x7C00

; Constants
KERNEL_OFFSET equ 0x1000   ; Kernel load address
STACK_BASE equ 0x9000      ; Stack base address

; Entry point
start:
    ; Clear interrupts and set up segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, STACK_BASE
    sti

    ; Clear screen and display welcome message
    call clear_screen
    call print_banner
    
    ; Perform system initialization
    call detect_memory
    call enable_a20
    call load_kernel
    call setup_gdt
    
    ; Switch to protected mode
    call enter_protected_mode
    
    ; Should never reach here
    jmp $

; Clear screen function
clear_screen:
    pusha
    mov ah, 0x06        ; Scroll up function
    mov al, 0x00        ; Clear entire screen
    mov bh, 0x07        ; White on black
    mov cx, 0x0000      ; Top-left corner
    mov dx, 0x184F      ; Bottom-right corner (80x25)
    int 0x10
    
    ; Set cursor position to top-left
    mov ah, 0x02
    mov bh, 0x00
    mov dx, 0x0000
    int 0x10
    popa
    ret

; Print banner
print_banner:
    pusha
    mov si, banner_msg
    call print_string
    mov si, author_msg
    call print_string
    mov si, separator_msg
    call print_string
    popa
    ret

; Print string function (SI = string pointer)
print_string:
    pusha
    mov ah, 0x0E        ; BIOS teletype function
    mov bh, 0x00        ; Page number
    mov bl, 0x07        ; Color (white on black)
.loop:
    lodsb               ; Load byte from SI into AL
    cmp al, 0           ; Check for null terminator
    je .done
    int 0x10            ; BIOS video interrupt
    jmp .loop
.done:
    popa
    ret

; Print hexadecimal number (AX = number)
print_hex:
    pusha
    mov cx, 4           ; 4 hex digits
    mov dx, ax
.loop:
    rol dx, 4           ; Rotate left 4 bits
    mov al, dl
    and al, 0x0F        ; Mask lower 4 bits
    add al, '0'         ; Convert to ASCII
    cmp al, '9'
    jle .print
    add al, 7           ; Convert A-F
.print:
    mov ah, 0x0E
    int 0x10
    loop .loop
    popa
    ret

; Memory detection using BIOS interrupt 0x15, EAX=0xE820
detect_memory:
    pusha
    mov si, memory_msg
    call print_string
    
    ; Initialize for memory detection
    xor ebx, ebx        ; EBX must be 0 to start
    mov edx, 0x534D4150 ; "SMAP" signature
    mov eax, 0xE820     ; Function code
    mov ecx, 24         ; Buffer size
    mov di, memory_map  ; Buffer address
    int 0x15            ; BIOS interrupt
    
    jc .error           ; Carry flag set = error
    cmp eax, 0x534D4150 ; Check if SMAP returned
    jne .error
    
    ; Display first memory region
    mov ax, [memory_map + 8]  ; Base address low
    call print_hex
    mov si, memory_ok_msg
    call print_string
    jmp .done
    
.error:
    mov si, memory_error_msg
    call print_string
    
.done:
    popa
    ret

; Enable A20 gate for access to memory above 1MB
enable_a20:
    pusha
    mov si, a20_msg
    call print_string
    
    ; Method 1: Try BIOS interrupt
    mov ax, 0x2401
    int 0x15
    jnc .test_a20       ; If no carry, success
    
    ; Method 2: Keyboard controller method
    call wait_8042
    mov al, 0xAD        ; Disable keyboard
    out 0x64, al
    
    call wait_8042
    mov al, 0xD0        ; Read output port
    out 0x64, al
    
    call wait_8042_data
    in al, 0x60         ; Read data
    push ax
    
    call wait_8042
    mov al, 0xD1        ; Write output port
    out 0x64, al
    
    call wait_8042
    pop ax
    or al, 2            ; Set A20 bit
    out 0x60, al
    
    call wait_8042
    mov al, 0xAE        ; Enable keyboard
    out 0x64, al
    
.test_a20:
    ; Test if A20 is enabled
    mov ax, 0xFFFF
    mov es, ax
    mov di, 0x0010      ; ES:DI = 0xFFFF:0x0010 = 0x100000
    mov ax, [di]        ; Read from 1MB
    mov bx, ax
    not ax
    mov [di], ax        ; Write inverted value
    cmp ax, [di]        ; Check if it changed
    mov [di], bx        ; Restore original value
    mov ax, 0
    mov es, ax
    
    je .a20_enabled
    mov si, a20_error_msg
    call print_string
    jmp .done
    
.a20_enabled:
    mov si, a20_ok_msg
    call print_string
    
.done:
    popa
    ret

; Wait for keyboard controller
wait_8042:
    in al, 0x64
    test al, 2
    jnz wait_8042
    ret

wait_8042_data:
    in al, 0x64
    test al, 1
    jz wait_8042_data
    ret

; Load kernel from disk
load_kernel:
    pusha
    mov si, kernel_msg
    call print_string
    
    ; Reset disk system
    mov ah, 0x00
    mov dl, 0x80        ; First hard drive
    int 0x13
    jc .error
    
    ; Read kernel sectors
    mov ah, 0x02        ; Read sectors function
    mov al, 15          ; Number of sectors to read
    mov ch, 0x00        ; Cylinder 0
    mov cl, 0x02        ; Start from sector 2 (after bootloader)
    mov dh, 0x00        ; Head 0
    mov dl, 0x80        ; Drive 0x80 (first hard drive)
    mov bx, KERNEL_OFFSET ; Load address
    int 0x13
    jc .error
    
    mov si, kernel_ok_msg
    call print_string
    jmp .done
    
.error:
    mov si, kernel_error_msg
    call print_string
    
.done:
    popa
    ret

; Set up Global Descriptor Table
setup_gdt:
    pusha
    mov si, gdt_msg
    call print_string
    
    ; Load GDT
    lgdt [gdt_descriptor]
    
    mov si, gdt_ok_msg
    call print_string
    popa
    ret

; Enter 32-bit protected mode
enter_protected_mode:
    mov si, protected_msg
    call print_string
    
    ; Disable interrupts
    cli
    
    ; Enable protected mode
    mov eax, cr0
    or eax, 1           ; Set PE bit
    mov cr0, eax
    
    ; Far jump to flush pipeline and enter protected mode
    jmp CODE_SEG:protected_mode_start

[BITS 32]
protected_mode_start:
    ; Set up segments for protected mode
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up stack
    mov esp, 0x90000
    
    ; Jump to kernel
    jmp KERNEL_OFFSET

; Global Descriptor Table
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0
    
gdt_code:
    ; Code segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0000       ; Base (bits 0-15)
    db 0x00         ; Base (bits 16-23)
    db 10011010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x00         ; Base (bits 24-31)
    
gdt_data:
    ; Data segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0000       ; Base (bits 0-15)
    db 0x00         ; Base (bits 16-23)
    db 10010010b    ; Access byte
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x00         ; Base (bits 24-31)
    
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                ; GDT address

; Constants for segment selectors
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Data section
banner_msg db '╔══════════════════════════════════════════════════════════════════════════════════════╗', 13, 10
           db '║                        🌈 AMELIA\'S CUSTOM BOOTLOADER 🌈                            ║', 13, 10
           db '║                     Advanced x86 Boot System v1.0                                   ║', 13, 10
           db '╚══════════════════════════════════════════════════════════════════════════════════════╝', 13, 10, 0

author_msg db 'Author: Amelia Enora 🌈 Marceline Chavez Barroso', 13, 10
           db 'Features: Memory detection, A20 gate, Protected mode transition', 13, 10, 0

separator_msg db '────────────────────────────────────────────────────────────────────────────────────────', 13, 10, 0

memory_msg db '[*] Detecting system memory...', 0
memory_ok_msg db ' [OK]', 13, 10, 0
memory_error_msg db ' [ERROR]', 13, 10, 0

a20_msg db '[*] Enabling A20 gate...', 0
a20_ok_msg db ' [OK]', 13, 10, 0
a20_error_msg db ' [ERROR]', 13, 10, 0

kernel_msg db '[*] Loading kernel from disk...', 0
kernel_ok_msg db ' [OK]', 13, 10, 0
kernel_error_msg db ' [ERROR]', 13, 10, 0

gdt_msg db '[*] Setting up Global Descriptor Table...', 0
gdt_ok_msg db ' [OK]', 13, 10, 0

protected_msg db '[*] Entering 32-bit protected mode...', 13, 10, 0

; Memory map buffer
memory_map times 24 db 0

; Padding and boot signature
times 510-($-$$) db 0   ; Pad to 510 bytes
dw 0xAA55               ; Boot signature

; Second sector - Simple kernel stub
[BITS 32]
kernel_start:
    ; Simple kernel that displays a message
    mov esi, kernel_welcome
    mov edi, 0xB8000    ; VGA text buffer
    mov ah, 0x07        ; White on black
    
.loop:
    lodsb
    cmp al, 0
    je .done
    stosw               ; Store character and attribute
    jmp .loop
    
.done:
    ; Infinite loop
    jmp $

kernel_welcome db 'Kernel loaded successfully! Welcome to Amelia\'s OS! 🌈', 0

; Pad second sector
times 1024-($-kernel_start) db 0 