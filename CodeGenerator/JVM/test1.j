; Line #1: // test1.c
; test1.j
.class public test1
.super java/lang/Object
.field public static _sc Ljava/util/Scanner;
; Line #2: int a, b;
.field public static a I
.field public static b I
; Line #3: bool d;
.field public static d Z
; Line #4: int foo( int a ) {
; Line #5: int i, result;
.method public static foo(I)I
.limit stack 100 ;Sets the maximum size of the operand stack required by the method
.limit locals 100
; Line #6: result = 0;
ldc 0
istore 2
; Line #7: i = 1;
ldc 1
istore 1
; Line #8: while( i <= a ) {
Lbegin_0:
iload 1
iload 0
isub
ifle Ltrue_1
iconst_0
goto Lfalse_1
Ltrue_1:
iconst_1
Lfalse_1:
ifeq Lexit_0
; Line #9: result = result + i;
iload 2
iload 1
iadd
istore 2
; Line #10: i = i + 1;
iload 1
ldc 1
iadd
istore 1
goto Lbegin_0
; Line #11: }
Lexit_0:
; Line #12: return result;
iload 2
ireturn
; Line #13: }
.end method
; Line #14: int main() {
; Line #15: int c ;
.method public static main([Ljava/lang/String;)V
.limit stack 100 ; up to 100 items can be pushed
.limit locals 100
new java/util/Scanner
dup
getstatic java/lang/System/in Ljava/io/InputStream;
invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V
putstatic test1/_sc Ljava/util/Scanner;
; Line #16: read a;
getstatic test1/_sc Ljava/util/Scanner;
invokevirtual java/util/Scanner/nextInt()I
putstatic test1/a I
; Line #17: c = foo( a );
getstatic test1/a I
invokestatic test1/foo(I)I
istore 1
; Line #18: print c;
getstatic java/lang/System/out Ljava/io/PrintStream;
iload 1
invokevirtual java/io/PrintStream/print(I)V ; this is print
; Line #19: print "\n";
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "\n"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V ; this is print
; Line #20: if( c >= 100 ) {
iload 1
ldc 100
isub
ifge Ltrue_2
iconst_0
goto Lfalse_2
Ltrue_2:
iconst_1
Lfalse_2:
ifeq Lelse_3
; Line #21: print "c >= 100 \n";
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "c >= 100 \n"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V ; this is print
; Line #22: }
goto Lexit_3
; Line #23: else {
Lelse_3:
; Line #24: print "c < 100 \n";
getstatic java/lang/System/out Ljava/io/PrintStream;
ldc "c < 100 \n"
invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V ; this is print
; Line #25: }
Lexit_3:
; Line #26: }
return
.end method
