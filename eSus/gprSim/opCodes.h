#ifndef OPCODES_H_INCLUDED
#define OPCODES_H_INCLUDED
#define SYSCALL_PRINT_BOOLEAN 4



#define SUBI     17
#define LIINST   16
#define LBINST   15
#define LAINST   14
#define BNE      13
#define BGE      12
#define BEQZ     11
#define BINST    10
#define ADDI     9

#define SYSCALL  8
#define STO      7
#define LOAD     6
#define POP      5
#define PUSH     4
#define MUL      3
#define SUB      2
#define ADD      1
#define END      0

#define RTYPE 0
#define ITYPE 1
#define JTYPE 2



#define SUBI_TYPE    4
#define LIINST_TYPE  2
#define LBINST_TYPE  4
#define LAINST_TYPE  2
#define BNE_TYPE     4
#define BGE_TYPE     4
#define BEQZ_TYPE    2
#define BINST_TYPE   3
#define ADDI_TYPE    4

#define SYSCALL_TYPE 0
#define STO_TYPE     1
#define LOAD_TYPE    1
#define POP_TYPE     1
#define PUSH_TYPE    1
#define MUL_TYPE     1
#define SUB_TYPE     1
#define ADD_TYPE     1
#define END_TYPE     0

#define SUBI_STRING    "SUBI"
#define LIINST_STRING  "LI"
#define LBINST_STRING  "LB"
#define LAINST_STRING  "LA"
#define BNE_STRING     "BNE"
#define BGE_STRING     "BGE"
#define BEQZ_STRING    "BEQZ"
#define BINST_STRING   "B"
#define ADDI_STRING    "ADDI"

#define SYSCALL_STRING "SYSCALL"
#define STO_STRING     "STO"
#define LOAD_STRING    "LOAD"
#define POP_STRING     "POP"
#define PUSH_STRING    "PUSH"
#define MUL_STRING     "MUL"
#define SUB_STRING     "SUB"
#define ADD_STRING     "ADD"
#define END_STRING     "END"

#endif // OPCODES_H_INCLUDED
