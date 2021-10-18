  ;; Author: Moss Gallagher
  ;; Data: 13-Oct-21

%ifndef _Mycelium_std_sys_
%define _Mycelium_std_sys_

  ;; Get platform specific file
%ifdef _sys_x86_32_
  %include "std/sys/x86_32.asm"
%else
  %include "std/sys/x86_64.asm"
%endif                          ; End platform includes

%endif                          ; ifdef guard
