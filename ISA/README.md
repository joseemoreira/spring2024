# CDC 8600 Instruction Set Architecture

## Architected State

## Arithmetic and Logic Instructions

### Logic Instructions

| Instruction    | Description                                         | Page in reference manual |
|----------------|-----------------------------------------------------|--------------------------|
| lpjkj(Xj, Xk)  | Logical product of (Xj) and (Xk) to Xj              | 37                       |
| lsjkj(Xj, Xk)  | Logical sum of (Xj) plus (Xk) to Xj                 | 38                       |
| ldjkj(Xj, Xk)  | Logical difference of (Xj) minux (Xk) to Xj         | 39                       |
| copykj(Xj, Xk) | Copy (Xk) to Xj                                     | 40                       |
| rolp(Xj, Xk)   | Rotate (Xj) left by (Xk) into Xj if (Xk) positive   | 42                       |
| shrn(Xj, Xk)   | Shift (Xj) right by (Xk) into Xj if (Xk) negative   | 42                       |
| roln(Xj, Xk)   | Rotate (Xj) left by (Xk) into Xj if (Xk) negative   | 43                       |
| shrp(Xj, Xk)   | Shift (Xj) right by (Xk) into Xj if (Xk) positive   | 43                       |
| popcnt(Xj, Xk) | Population count of (Xk) to Xj                      | 49                       |

### Integer Arithmetic Instructions

| Instruction    | Description                                 | Page in reference manual |
|----------------|---------------------------------------------|--------------------------|
| ipjkj(Xj, Xk)  | Integer product of (Xj) times (Xk) to Xj    | 52                       |

### Floating-point Arithmetic Instructions

| Instruction      | Description                                 | Page in reference manual |
|------------------|---------------------------------------------|--------------------------|
|  fdivjkj(Xj, Xk) | Floating divide of (Xj) by (Xk) to Xj       | 48                       |

