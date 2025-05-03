.globl _PutPixel_ASM
_PutPixel_ASM:
    push    %ebp
    mov     %esp,       %ebp

    # Get params
    mov     8(%ebp),    %ecx    # x
    mov     12(%ebp),   %edx    # y   
    
    # Get address of renderer structure
    mov     $_renderer, %eax
    
    # Get framebuffer width and calculate offset
    mov     4(%eax),    %ebx    # Load renderer.framebuffer.w
    imul    $4,         %ebx    # width * 4
    imul    %edx,       %ebx    # result * y
    
    # Add x component to offset
    imul    $4,         %ecx    # x * 4
    add     %ecx,       %ebx    # Total offset in %ebx
    
    # Get buffer pointer and add offset
    mov     12(%eax),   %eax    # Get renderer.framebuffer.buf
    add     %ebx,       %eax    # Final pixel address in %eax
    
    # Set pixel values to white, BGRA
    movb    $255,       (%eax)
    movb    $255,       1(%eax)
    movb    $255,       2(%eax)
    movb    $255,       3(%eax)
    
    # Clean up, return
    mov     %ebp, %esp
    pop     %ebp
    ret
