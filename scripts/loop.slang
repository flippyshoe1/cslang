PRINT lets count to 5!
PUSH 6
PUSH 1

loop:
GOEQ done
PRINT
PUSH 1
ADD
GOTO loop

done:
PRINT easy peasy!
HALT
