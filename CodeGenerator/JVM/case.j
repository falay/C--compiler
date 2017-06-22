.class public testphase
.super java/lang/Object

; standard initializer
  .method public <init>()V
   aload_0
   invokenonvirtual java/lang/Object/<init>()V
   return
.end method

.method public static main([Ljava/lang/String;)V
  .limit stack 2
  .limit locals 3

  ; assign something
  iconst_1
  iconst_1
  iadd
  istore_1

  ; assign something
  iconst_2
  iconst_2
  isub
  istore_2

  ; Writeln
  getstatic java/lang/System/out Ljava/io/PrintStream;
  sipush 1000
  invokevirtual java/io/PrintStream/println(I)V

  ; Writeln variable 2
  getstatic java/lang/System/out Ljava/io/PrintStream;
  iload_2
  invokevirtual java/io/PrintStream/println(I)V

  return
.end method
