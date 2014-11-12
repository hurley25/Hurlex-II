; -------------------------------------------------
;       内核创建线程的统一入口函数
;
;       qianyi.lh  2014/11/12  14:45:54
; -------------------------------------------------

[GLOBAL kern_thread_entry]
[EXTERN do_exit]

kern_thread_entry:
        push edx         ; push args
        call ebx         ; call fn

        push eax         ; push fn(args) return code
        call do_exit
