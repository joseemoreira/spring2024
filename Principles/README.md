# Principles of Operation

The *principles of operation* for a computer inlcude both its instruction set architecture (ISA) and additional conventions necessary to write working assembly language programs.
These conventions include the memory map, role of supervisor code, application binary interface, and other aspects that control how a program executes in the computer.

## Memory map

As defined by the ISA, the physical memory of the CDC 8600 consists of a vector $`\mathbf{M}[0:N)`$ of 64-bit words.
Although the ISA supports $`N \leq 1048576`$, we are going to consider the specific case of $`N=262144`$, as defined in the reference manual.
By convention, we are going to divide this 256-KiW memory into $`32 \times 8`$-KiW pages. We will assign a specific role to each page.
The CDC 8600 ISA does not know anything about pages.
This is just our convention.

| Page # | Addresses | Purpose |
|--------|-----------|---------|
| 0      | $`[0,8192)`$  | Context frames |
| 1      | [8192,16384)  | Supervisor code |
| 2      | [16384,24576) | Supervisor data |
