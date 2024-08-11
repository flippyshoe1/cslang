RAND 100
RSET 7
PRINT what number am i thinking of right now?
GOTO loop

low:
PRINT too low!
POP
GOTO loop

high:
PRINT too high!
POP
GOTO loop

win:
PUSH 7
GOREQ lucky
PRINT drats... foiled again!
HALT

lucky:
PRINT HOW DID YOU KNOW THAT????
HALT

lose:
PRINT haha... stupid idiot!!! the answer is actually:
POP
POP
PRINT
HALT



loop:
READ
GOEQ win

RDEC
PUSH 0
GOREQ lose
POP

GOLO low
GOHI high

