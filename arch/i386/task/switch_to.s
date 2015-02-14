; -------------------------------------------------
; 	任务切换的实现
;
; 	qianyi.lh  2014/11/12  16:38:56
;
; -------------------------------------------------

[global switch_to]

switch_to:
        mov eax, [esp+4]
        
        push ebx
        mov ebx, [esp+4]
        mov [eax], ebx
        pop ebx
        
        mov [eax+4], esp
        mov [eax+8], ebp
        mov [eax+12], ecx
        mov [eax+16], edx
        mov [eax+20], esi
        mov [eax+24], edi
        mov [eax+28], ebp

        mov eax, [esp+8]

        mov ebp, [eax+28]
        mov edi, [eax+24]
        mov esi, [eax+20]
        mov edx, [eax+16]
        mov ecx, [eax+12]
        mov ebx, [eax+8]
        mov esp, [eax+4]
        
        push ebx
        push ebx
        mov ebx, [eax]
        mov [esp+4], ebx
        pop ebx
 	
        ret

