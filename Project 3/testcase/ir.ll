; ModuleID = 'Program'
source_filename = "Program"

@k = common global i32 0

define i32 @main() {
main:
  br label %con

con:                                              ; preds = %body, %main
  %tmp = load i32, i32* @k
  %tmp_lt = icmp slt i32 %tmp, 233
  br i1 %tmp_lt, label %body, label %next

body:                                             ; preds = %con
  %tmp1 = load i32, i32* @k
  %tmp_add = add i32 %tmp1, 1
  store i32 %tmp_add, i32* @k
  br label %con

next:                                             ; preds = %con
  %tmp2 = load i32, i32* @k
  ret i32 %tmp2
}
