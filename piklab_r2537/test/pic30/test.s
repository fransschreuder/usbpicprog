         .title " Sample dsPIC Assembler Source Code"
         .sbttl " For illustration only."
         ; dsPIC registers
         .equ CORCONL, CORCON
         .equ PSV,2
         .section .const,psv
hello:
         .ascii "Hello world!\n\0"
         .text
         .global __reset
__reset:
         ; set PSVPAG to page that contains 'hello'
         mov     #psvpage(hello),w0
         mov     w0,PSVPAG
         ; enable Program Space Visibility
         bset.b CORCONL,#PSV
         ; make a pointer to 'hello'
         mov     #psvoffset(hello),w0
         .end
