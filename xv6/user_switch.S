# User-mode context switch (for user programs)
#   void switch_context(struct context **old, struct context *new);
# Save current registers on stack, store stack pointer into *old,
# switch stacks to new, restore registers and return (jump to new eip).

.globl switch_context
switch_context:
  movl 4(%esp), %eax
  movl 8(%esp), %edx

  pushl %ebp
  pushl %ebx
  pushl %esi
  pushl %edi

  movl %esp, (%eax)
  movl %edx, %esp

  popl %edi
  popl %esi
  popl %ebx
  popl %ebp
  ret
