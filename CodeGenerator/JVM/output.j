.class public output
.super java/lang/Object
.field public static _sc Ljava/util/Scanner;
.field public static a I


.method public static main([Ljava/lang/String;)V
.limit stack 100
.limit locals 100
iconst_0
istore 2
iload 2 ; local variable number of d is 2
ldc 1
isub
ifeq Ltrue_0
iconst_0
goto Lfalse_0
Ltrue_0:
iconst_1
Lfalse_0:
ifeq Lelse_1
ldc 5
istore 1 ; local variable number of i is 1
goto Lexit_1
Lelse_1:
ldc 10
istore 1 ; local variable number of i is 1
Lexit_1:
return
.end method


