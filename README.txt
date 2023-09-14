Hippy Project 0.18 Alpha Version Source Code/Binary Distribution
Unzip into a folder and execute /bin/hippy.exe
Or compile and run /bin/hippy.exe

Changes
-----------
ver. 0.23a
		Assembler re-designed using bison/yacc, now it supports other directives
		and equations ect.
		Tooltip problem is resolved.
ver. 0.22a
		BGT Instruction is fixed.
ver. 0.21a
		Execution main swtich is replaced by member function pointer calls.
		Stack window woks a little better now. 
ver. 0.2a
24.09.2005: BUGS reported by Martin Widler
    TBA,TAB,LDS,LDX, STS operations are modified as to correct CCR manuplations.
    DAA instruction is modified.
    CPX instruction is modified. 
ver. 0.19a
22.09.2005: Martin Widler's detailed list of bugs regarding CCR modifications.
		LOAD, STORE, INCREMNT, SUBTRACT, ADCB, ADCA .... and some other instructions.
ver. 0.18a
16.09.2005: All alu operations are revised in order 
		to correct CCR errors. (Martin Widler)
ver. 0.17a
25.08.2005: BUG Reported by Martin Widler
		SUBC Instruction is corrected (carry flag was problematic)
ver. 0.16a
06.08.2005: BUGS Reported by Martin Widler
		1) Stack window is fixed, now pop and push operations work good
		2) After an interrupt the CCR's I bit is set to one
ver. 0.15a
05.08.2005: BUGS reported by Martin Widler
		1) Stack window painting routings are revised. Now the top line is the top of the stack.
		   Correct enummeration order is preserved.
		2) For rti hippy was triying to pop all regs twice. this was causing weird stack 
		   behaviour, fixed.
		3) Program counter is hopefully painting itself correct now.
ver. 0.14a
30.7.2005: BUGS reported by Martin Widler
		1) I bit of CCR was interpreted incorrectly (reversed, now if I=0 IRQ occurs).
		2) Stack window now displays registers pushed before interrupt service routines.
		3) RTI was assembled to $38 it is corredted to $3B
ver. 0.13a
27.7.2005: BUG reported by Martin Widler
		in order to output 0x13 bytes fixed output the
 		code was generating erroneous output when a yet undeclared label
  		was inserted in two different lines. code generation part is completely new
ver. 0.12a
25.7.2005: BUG reported by Martin Widler
		Some instructions does not support direct addressing mode(CLR,ROL..).
		In the previous release addresses below 0x0100 were interpreted as direct
		addresses but now if the instruction does not support this mode, Hippy
		considers them as four byte addresses.

Thanks
-----------
	Thank you Martin Widler, you really put your energy in Hippy.


Contact : perreal@hotmail.com
Feel free to contact