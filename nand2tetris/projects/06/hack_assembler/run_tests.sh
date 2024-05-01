#!/bin/bash

rm test/*.hack

make clean && make

./Assembler test/Add.asm
./Assembler test/Max.asm
./Assembler test/MaxL.asm
./Assembler test/Rect.asm
./Assembler test/RectL.asm
./Assembler test/Pong.asm
./Assembler test/PongL.asm

cd test/

diff -s Add.hack Add.key
diff -s Max.hack Max.key
diff -s MaxL.hack MaxL.key
diff -s Rect.hack Rect.key
diff -s RectL.hack RectL.key
diff -s Pong.hack Pong.key
diff -s PongL.hack PongL.key

rm ./*.hack

../Assembler Add.asm
../Assembler Max.asm
../Assembler MaxL.asm
../Assembler Rect.asm
../Assembler RectL.asm
../Assembler Pong.asm
../Assembler PongL.asm

diff -s Add.hack Add.key
diff -s Max.hack Max.key
diff -s MaxL.hack MaxL.key
diff -s Rect.hack Rect.key
diff -s RectL.hack RectL.key
diff -s Pong.hack Pong.key
diff -s PongL.hack PongL.key

cd ..
make clean
rm test/*.hack
