; ModuleID = 'Program'
source_filename = "Program"

@output = external global i32
@k = common global i32 0

declare i32 @readint32()

declare i32 @writeint32()

define i32 @main() {
main:
  br label %con

con:                                              ; preds = %body, %main
  %tmp = load i32, i32* @k
  %tmp_lt = icmp slt i32 %tmp, 512
  br i1 %tmp_lt, label %body, label %next

body:                                             ; preds = %con
  %tmp1 = load i32, i32* @k
  %tmp_add = add i32 %tmp1, 1
  store i32 %tmp_add, i32* @k
  br label %con

next:                                             ; preds = %con
  %tmp2 = load i32, i32* @k
  store i32 %tmp2, i32* @output
  %calltmp = call i32 @writeint32()
  ret i32 0
}
