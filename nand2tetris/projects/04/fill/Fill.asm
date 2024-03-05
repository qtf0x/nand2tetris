// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen
// by writing 'black' in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen by writing
// 'white' in every pixel;
// the screen should remain fully clear as long as no key is pressed.

    // pixel = SCREEN
    @SCREEN
    D=A
    @pixel
    M=D
(LOOP)
    // if (KBD == 0) goto WHITE
    @KBD
    D=M
    @WHITE
    D;JEQ
(BLACK)
    // *(pixel) = -1
    @pixel
    A=M
    M=-1
    // goto INC
    @INC
    0;JMP
(WHITE)
    // *(pixel) = 0
    @pixel
    A=M
    M=0
(INC)
    // pixel += 1
    @pixel
    M=M+1
    // if (pixel < &KBD) goto END
    D=M
    @KBD
    D=D-A
    @END
    D;JLT
    // pixel = SCREEN
    @SCREEN
    D=A
    @pixel
    M=D
(END)
//     goto LOOP
    @LOOP
    0;JMP
