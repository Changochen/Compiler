; ModuleID = 'Program'
source_filename = "Program"

@output = external global i32
@res = common global i32 0

declare i32 @readint32()

declare i32 @writeint32()

define i32 @square() {
square:
  %tmp = load i32, i32* @res
  %tmp1 = load i32, i32* @res
  %tmp_mul = mul i32 %tmp, %tmp1
  ret i32 %tmp_mul
}

define i32 @main() {
main:
  store i32 1, i32* @res
  br label %con

con:                                              ; preds = %body, %main
  %tmp = load i32, i32* @res
  %tmp_lt = icmp slt i32 %tmp, 32
  br i1 %tmp_lt, label %body, label %next

body:                                             ; preds = %con
  %calltmp = call i32 @square()
  store i32 %calltmp, i32* @output
  %calltmp1 = call i32 @writeint32()
  %tmp2 = load i32, i32* @res
  %tmp_add = add i32 %tmp2, 1
  store i32 %tmp_add, i32* @res
  br label %con

next:                                             ; preds = %con
  ret i32 0
}
