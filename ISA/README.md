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
| savjn(Xj, n)   | Save lower (Xj) for n bits                          | 117                      |
| blkjn(Xj, n)   | Blank lower (Xj) for n bits                         | 118                      |
| rol((Xj, n)    | Left rotate (Xj) by n bits                          | 119                      |
| shr(Xj, n)     | Right shift (Xj) by n bits                          | 120                      |


### Integer Arithmetic Instructions

| Instruction       | Description                                 | Page in reference manual |
|-------------------|---------------------------------------------|--------------------------|
| ipjkj(Xj, Xk)     | Integer product of (Xj) times (Xk) to Xj    | 52                       |
| xkj(Xj, k)        | Transmit k to Xj                            | 55                       |
| xmkj(Xj, k)       | Transmit -k to Xj                           | 56                       |
| isjkj(Xj, k)      | Integer sum of (Xj) plus k to Xj            | 57                       |
| idjkj(Xj, k)      | Integer difference of (Xj) minus k to Xj    | 58                       |
| idzkj(Xj, Xk)     | Integer difference of zero minux (Xk) to Xj | 62                       |
| xKj(Xj, K)        | Transmit K to Xj                            | 80                       |
| isjKi(Xi, Xj, K)  | Integer sum of (Xj) plus K to Xi            | 121                      |
| isjki(Xi, Xj, Xk) | Integer sum of (Xj) plus (Xk) to Xi         | 122                      |
| idjki(Xi, Xj, Xk) | Integer difference of (Xj) minux (Xk) to Xi | 123                      |

### Floating-point Arithmetic Instructions

| Instruction          | Description                                 | Page in reference manual |
|----------------------|---------------------------------------------|--------------------------|
|  fdivjkj(Xj, Xk)     | Floating divide of (Xj) by (Xk) to Xj       | 48                       |
|  faddjki(Xi, Xj, Xk) | Floating sum of (Xj) plus (Xk) to Xi        | 124                      |

