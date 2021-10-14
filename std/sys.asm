  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21

%ifndef _sys_main_asm_
%define _sys_main_asm_

  ;; Get platform specific file
%ifdef _sys_x86_32_
  %include "std/sys/x86_32.asm"
%else
  %include "std/sys/x86_64.asm"
%endif
%endif
