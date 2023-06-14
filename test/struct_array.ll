; ModuleID = 'struct_array.cpp'
source_filename = "struct_array.cpp"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.35.32216"

%struct.User = type { i32, i32 }

; Function Attrs: mustprogress noinline norecurse nounwind optnone uwtable
define dso_local noundef i32 @main(i32 noundef %0, ptr noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca ptr, align 8
  %7 = alloca i64, align 8
  store i32 0, ptr %3, align 4
  store ptr %1, ptr %4, align 8
  store i32 %0, ptr %5, align 4
  %8 = load i32, ptr %5, align 4
  %9 = zext i32 %8 to i64
  %10 = call ptr @llvm.stacksave()
  store ptr %10, ptr %6, align 8
  %11 = alloca %struct.User, i64 %9, align 16
  store i64 %9, ptr %7, align 8
  %12 = getelementptr inbounds %struct.User, ptr %11, i64 0
  %13 = getelementptr inbounds %struct.User, ptr %12, i32 0, i32 0
  store i32 0, ptr %13, align 16
  %14 = mul nuw i64 8, %9
  %15 = udiv i64 %14, 8
  %16 = trunc i64 %15 to i32
  store i32 %16, ptr %3, align 4
  %17 = load ptr, ptr %6, align 8
  call void @llvm.stackrestore(ptr %17)
  %18 = load i32, ptr %3, align 4
  ret i32 %18
}

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare ptr @llvm.stacksave() #1

; Function Attrs: nocallback nofree nosync nounwind willreturn
declare void @llvm.stackrestore(ptr) #1

attributes #0 = { mustprogress noinline norecurse nounwind optnone uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nocallback nofree nosync nounwind willreturn }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 1, !"wchar_size", i32 2}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{!"clang version 16.0.1"}
