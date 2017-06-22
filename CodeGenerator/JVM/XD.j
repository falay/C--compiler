.class public output
.super java/lang/Object
.field public static _sc Ljava/util/Scanner;
.field public static a I


.method public static main([Ljava/lang/String;)V
.limit stack 100
.limit locals 100
new java/util/Scanner
dup
getstatic java/lang/System/in Ljava/io/InputStream;
invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V
putstatic output/_sc Ljava/util/Scanner;


getstatic java/lang/System/out Ljava/io/PrintStream;

getstatic output/_sc Ljava/util/Scanner;
invokevirtual java/util/Scanner/nextInt()I

;getstatic output/_sc Ljava/util/Scanner;
;invokevirtual java/util/Scanner/nextInt()I

invokestatic output/factorial(I)I
invokevirtual java/io/PrintStream/print(I)V

return
.end method


.method public static sub(II)I
.limit stack 2
.limit locals 2
iload 0
iload 1
isub
ireturn 
.end method


.method public static factorial(I)I
.limit stack 100
.limit locals 10
iload 0
iconst_0
isub
ifeq Ltrue
iconst_0
goto Lfalse
Ltrue:
iconst_1
Lfalse:

ifeq Lelse
iconst_1
ireturn 


Lelse:

iload 0
iload 0
iconst_1
isub
invokestatic output/factorial(I)I
imul
ireturn
	

.end method

