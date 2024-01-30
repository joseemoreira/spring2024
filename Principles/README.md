# Principles of Operation

The *principles of operation* for a computer inlcude both its instruction set architecture (ISA) and additional conventions necessary to write working assembly language programs.
These conventions include the memory map, role of supervisor code, application binary interface, and other aspects that control how a program executes in the computer.

## Memory map

As defined by the ISA, the physical memory of the CDC 8600 consists of a vector $`\mathbf{M}[0:N)`$ of 64-bit words.
Although the ISA supports $`N \leq 1048576`$, we are going to consider the specific case of $`N=262144`$, as defined in the reference manual.
By convention, we are going to divide this 256-KiW memory into $`32 \times 8`$-KiW pages. We will assign a specific role to each page.
The CDC 8600 ISA does not know anything about pages.
This is just our convention.

| Page #     | Addresses           | Purpose                  |
|------------|---------------------|--------------------------|
| $`0`$      | $`[     0,  8192)`$ | Context frames           |
| $`1`$      | $`[  8192, 16384)`$ | Supervisor code and data |
| $`2`$      | $`[ 16384, 24576)`$ | User code                |
| $`3`$      | $`[ 24576, 32768)`$ | User static data         |
| $`[4,28)`$ | $`[ 32768,229376)`$ | Heap                     |
| $`28`$     | $`[229376,237568)`$ | Stack for processor 0    |
| $`29`$     | $`[237568,245760)`$ | Stack for processor 1    |
| $`30`$     | $`[245760,253952)`$ | Stack for processor 2    |
| $`31`$     | $`[253952,262144)`$ | Stack for processor 3    |

### The context frames

Page 0 of memory (addresses $`[0,8192)`$) contains $`256 \times 32`$-word context frames.
The first 17 words of a context frame contain an *exchange package*, definig the values of the 16 general-purpose registers (X(0) through X(15)) and an additional *exchange package word* (XPW) with various additional registers.

By convention, we are going to reserve all 8 KiWs in page 0 for context frames.
(The ISA does not say you have to reserve all of them.)
For the time being, we are going to assign specific use to the first 8 frames.

| Frame # | Addresses     | Purpose                            |
|---------|---------------|------------------------------------|
| $`0`$   | $`[  0, 32)`$ | Supervisor context for processor 0 |
| $'1'$   | $'[ 32, 64)'$ | Supervisor context for processor 1 |
| $`2`$   | $`[ 64, 96)`$ | Supervisor context for processor 0 |
| $`3`$   | $`[ 96,128)`$ | Supervisor context for processor 0 |
