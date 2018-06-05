; ModuleID = 'Program'
source_filename = "Program"

@output = external global i32
@guess = common global i32 0
@res = common global i32 0

declare i32 @readint32()

declare i32 @writeint32()

define i32 @main() {
main:
  store i32 0, i32* @res
  store i32 563, i32* @guess
  br label %con

con:                                              ; preds = %body, %main
  %tmp = load i32, i32* @guess
  %tmp1 = load i32, i32* @res
  %tmp_neq = icmp ne i32 %tmp, %tmp1
  br i1 %tmp_neq, label %body, label %next

body:                                             ; preds = %con
  %calltmp = call i32 @readint32()
  store i32 %calltmp, i32* @res
  br label %con

next:                                             ; preds = %con
  %tmp2 = load i32, i32* @guess
  store i32 %tmp2, i32* @output
  %calltmp3 = call i32 @writeint32()
  ret i32 0
}
