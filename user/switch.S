# User-mode context switch
#   void switch_context(struct context **old, struct context *new);
# Save current registers on stack, store stack pointer into *old,
# switch stacks to new, restore registers and return (jump to new eip).

.globl switch_context
switch_context:
  movl 4(%esp), %eax
  movl 8(%esp), %edx

  # Save old callee-saved registers
  pushl %ebp
  pushl %ebx
  pushl %esi
  pushl %edi

  # Switch stacks
  movl %esp, (%eax)
  movl %edx, %esp

  # Load new callee-saved registers
  popl %edi
  popl %esi
  popl %ebx
  popl %ebp
  ret
