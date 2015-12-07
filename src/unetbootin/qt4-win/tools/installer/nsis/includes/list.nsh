;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
;;
;; This file is part of the Windows installer of the Qt Toolkit.
;;
;; This file may be used under the terms of the GNU General Public
;; License versions 2.0 or 3.0 as published by the Free Software
;; Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
;; included in the packaging of this file.  Alternatively you may (at
;; your option) use any later version of the GNU General Public
;; License if such license has been publicly approved by Trolltech ASA
;; (or its successors, if any) and the KDE Free Qt Foundation. In
;; addition, as a special exception, Trolltech gives you certain
;; additional rights. These rights are described in the Trolltech GPL
;; Exception version 1.2, which can be found at
;; http://www.trolltech.com/products/qt/gplexception/ and in the file
;; GPL_EXCEPTION.txt in this package.
;;
;; Please review the following information to ensure GNU General
;; Public Licensing requirements will be met:
;; http://trolltech.com/products/qt/licenses/licensing/opensource/. If
;; you are unsure which license is appropriate for your use, please
;; review the following information:
;; http://trolltech.com/products/qt/licenses/licensing/licensingoverview
;; or contact the sales department at sales@trolltech.com.
;;
;; In addition, as a special exception, Trolltech, as the sole
;; copyright holder for Qt Designer, grants users of the Qt/Eclipse
;; Integration plug-in the right for the Qt/Eclipse Integration to
;; link to functionality provided by Qt Designer and its related
;; libraries.
;;
;; This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
;; INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
;; A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
;; granted herein.
;;
;; This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
;; WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!ifndef LIST_INCLUDE
!define LIST_INCLUDE

; usage:
; push item
; push list
; call ItemInList
; returns 1 or 0
!macro ItemInList UN
Function ${UN}ItemInList
  exch $0 ;list
  exch
  exch $1 ;item
  push $2 ;counter
  push $3 ;substr
  push $4 ;char
  
  strcpy $3 ""
  strcpy $2 "0"

  loop:
    strcpy $4 $0 1 $2
    strcmp "$4" "" atend
    intop $2 $2 + 1

    strcmp "$4" "|" 0 +4
      strcmp "$3" "$1" found
      strcpy $3 "" ;reset substr
      goto +2
    strcpy $3 "$3$4" ;append char to substr
    goto loop

  found:
    strcpy $0 "1"
    goto done
    
  atend:
    strcmp "$3" "$1" found
    strcpy $0 "0"

  done:
  pop $4
  pop $3
  pop $2
  pop $1
  exch $0
FunctionEnd
!macroend

!insertmacro ItemInList ""
!insertmacro ItemInList "un."

Function GetItemInList
  exch $0 ;list
  exch
  exch $1 ;index
  push $2 ;counter
  push $3 ;substr
  push $4 ;char
  push $5 ;current index

  strcpy $3 ""
  strcpy $2 "0"
  strcpy $5 "1"

  loop:
    strcpy $4 $0 1 $2
    strcmp "$4" "" atend
    intop $2 $2 + 1

    strcmp "$4" "|" 0 +5
      strcmp "$5" "$1" found
      strcpy $3 "" ;reset substr
      intop $5 $5 + 1
      goto +2
    strcpy $3 "$3$4" ;append char to substr
    goto loop

  found:
    strcpy $0 "$3"
    goto done

  atend:
    strcmp "$5" "$1" found
    strcpy $0 ""

  done:
  pop $5
  pop $4
  pop $3
  pop $2
  pop $1
  exch $0
FunctionEnd

!endif ;LIST_INCLUDE