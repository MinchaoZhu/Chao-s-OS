.section .text

.global switch_context
.type switch_context, @function
switch_context:
        mov 4(%esp), %eax  # current contex pointer
        mov %esp, 0(%eax)  # esp
        mov %ebp, 4(%eax)  # ebp
        mov %ebx, 8(%eax)  # ebx
        mov %esi, 12(%eax) # esi
        mov %edi, 16(%eax) # edi
        pushf
        pop %ecx
        mov %ecx, 20(%eax) # eflags
        mov 8(%esp), %eax  # next context pointer
        mov 0(%eax), %esp  # retrive esp
        mov 4(%eax), %ebp  # retrive ebp
        mov 8(%eax), %ebx
        mov 12(%eax), %esi
        mov 16(%eax), %edi
        mov 20(%eax), %ecx
        push %ecx
        popf
        ret