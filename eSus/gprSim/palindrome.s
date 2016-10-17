.text
LA: $0, string_space
LA: $11, string_space
LA: $12, string_space

length_loop;
LB:$13, 0($12)
BEQZ:$13, end_length_loop
ADDI: $12, $12, 1
B: length_loop

end_length_loop;
SUBI: $12, $12, 1

test_loop;
BGE:$11,$12, is_palin
LB:$13, 0($11)
LB:$14, 0($12)
BNE:$13,$14,not_palin

ADDI:$11,$11,1
SUBI:$12,$12,1
B:test_loop

is_palin;
LA:$0, is_palin_msg #loads in TRUE for the sys call
LI:$20,4 #tells syscall to print the boolean value
SYSCALL:
B:exit

not_palin;
LA:$0, not_palin_msg #loads in FALSE for the sys call
LI:$20,4 #tells syscall to print the boolean value
SYSCALL:


exit;
END:

.data 
is_palin_msg:1 #holds the value TRUE for use with the sys call
not_palin_msg:0 #holds the value FALSE for use with the sys call
string_space:777 #starting with string_space, everything until a zero is in our string
string_space2:888
string_space3:999
string_space4:888
string_space5:777
string_space6:0