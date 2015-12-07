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
!ifndef INSTDIR_1
  !macro INSTDIR_INITIALIZE
    !define MUI_DIRECTORYPAGE_VARIABLE $${INSTDIR_0}_INSTDIR
    !ifdef ${INSTDIR_0}_ValidateDirectory
      !define MUI_PAGE_CUSTOMFUNCTION_LEAVE "${INSTDIR_0}_ValidateDirectoryFunc"
    !endif
    !insertmacro MUI_PAGE_DIRECTORY
  !macroend
  !macro INSTDIR_FUNCTIONS
  !macroend
  !macro INSTDIR_STARTUP
  !macroend
!else
!macro INSTDIR_INITIALIZE
  !define INSTDIR_INI_FILE "instdir.ini"
  !define INSTDIR_0_DIRFIELD "Field 10"
  !define INSTDIR_0_TEXTFIELD "Field 13"
  !define INSTDIR_1_DIRFIELD "Field 7"
  !define INSTDIR_1_TEXTFIELD "Field 11"
  !define INSTDIR_2_DIRFIELD "Field 5"
  !define INSTDIR_2_TEXTFIELD "Field 8"
  !define INSTDIR_3_DIRFIELD "Field 3"
  !define INSTDIR_3_TEXTFIELD "Field 6"
  !define INSTDIR_4_DIRFIELD "Field 2"
  !define INSTDIR_4_TEXTFIELD "Field 4"
  !define INSTDIR_5_DIRFIELD "Field 9"
  !define INSTDIR_5_TEXTFIELD "Field 12"
  !define INSTDIR_DIRHEIGHT 18

  Page custom InitInstDirs UpdateInstDirs
  
  LangString InstDirLicenseTitle ${LANG_ENGLISH} "Installation Directories"
  LangString InstDirLicenseTitleDescription ${LANG_ENGLISH} "Select the directories where you want the software installed."
!macroend

!macro INSTDIR_FUNCTIONS
  Function InitInstDirs
    push $0
    push $1
    push $2
    push $3
    
    !insertmacro MUI_HEADER_TEXT "$(InstDirLicenseTitle)" "$(InstDirLicenseTitleDescription)"
    strcpy $0 "129"

!ifdef INSTDIR_0
    SectionGetFlags ${${INSTDIR_0}_SEC01} $1
!ifdef ${INSTDIR_0}_SEC02
    SectionGetFlags ${${INSTDIR_0}_SEC02} $3
    IntOp $1 $1 | $3
!endif
    IntOp $2 $1 & 1 ;just care about the first flag
    StrCpy $1 "READONLY"
    StrCmp "$2" "1" +2
      StrCpy $1 "DISABLED"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_0_TEXTFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_0_DIRFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_0_TEXTFIELD}" "Text" "${INSTDIR_0_TEXT}"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_0_DIRFIELD}" "State" $${INSTDIR_0}_INSTDIR
!else
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_0_DIRFIELD}" "Type" "Unknown"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_0_TEXTFIELD}" "Type" "Unknown"
    intop $0 $0 - ${INSTDIR_DIRHEIGHT}
!endif
!ifdef INSTDIR_1
    SectionGetFlags ${${INSTDIR_1}_SEC01} $1
!ifdef ${INSTDIR_1}_SEC02
    SectionGetFlags ${${INSTDIR_1}_SEC02} $3
    IntOp $1 $1 | $3
!endif
    IntOp $2 $1 & 1 ;just care about the first flag
    StrCpy $1 "READONLY"
    StrCmp "$2" "1" +2
      StrCpy $1 "DISABLED"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_1_TEXTFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_1_DIRFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_1_TEXTFIELD}" "Text" "${INSTDIR_1_TEXT}"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_1_DIRFIELD}" "State" $${INSTDIR_1}_INSTDIR
!else
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_1_DIRFIELD}" "Type" "Unknown"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_1_TEXTFIELD}" "Type" "Unknown"
    intop $0 $0 - ${INSTDIR_DIRHEIGHT}
!endif
!ifdef INSTDIR_2
    SectionGetFlags ${${INSTDIR_2}_SEC01} $1
!ifdef ${INSTDIR_2}_SEC02
    SectionGetFlags ${${INSTDIR_2}_SEC02} $3
    IntOp $1 $1 | $3
!endif
    IntOp $2 $1 & 1 ;just care about the first flag
    StrCpy $1 "READONLY"
    StrCmp "$2" "1" +2
      StrCpy $1 "DISABLED"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_2_TEXTFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_2_DIRFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_2_TEXTFIELD}" "Text" "${INSTDIR_2_TEXT}"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_2_DIRFIELD}" "State" $${INSTDIR_2}_INSTDIR
!else
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_2_DIRFIELD}" "Type" "Unknown"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_2_TEXTFIELD}" "Type" "Unknown"
    intop $0 $0 - ${INSTDIR_DIRHEIGHT}
!endif
!ifdef INSTDIR_3
    SectionGetFlags ${${INSTDIR_3}_SEC01} $1
!ifdef ${INSTDIR_3}_SEC02
    SectionGetFlags ${${INSTDIR_3}_SEC02} $3
    IntOp $1 $1 | $3
!endif
    IntOp $2 $1 & 1 ;just care about the first flag
    StrCpy $1 "READONLY"
    StrCmp "$2" "1" +2
      StrCpy $1 "DISABLED"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_3_TEXTFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_3_DIRFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_3_TEXTFIELD}" "Text" "${INSTDIR_3_TEXT}"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_3_DIRFIELD}" "State" $${INSTDIR_3}_INSTDIR
!else
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_3_DIRFIELD}" "Type" "Unknown"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_3_TEXTFIELD}" "Type" "Unknown"
    intop $0 $0 - ${INSTDIR_DIRHEIGHT}
!endif
!ifdef INSTDIR_4
    SectionGetFlags ${${INSTDIR_4}_SEC01} $1
!ifdef ${INSTDIR_4}_SEC02
    SectionGetFlags ${${INSTDIR_4}_SEC02} $3
    IntOp $1 $1 | $3
!endif
    IntOp $2 $1 & 1 ;just care about the first flag
    StrCpy $1 "READONLY"
    StrCmp "$2" "1" +2
      StrCpy $1 "DISABLED"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_4_TEXTFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_4_DIRFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_4_TEXTFIELD}" "Text" "${INSTDIR_4_TEXT}"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_4_DIRFIELD}" "State" $${INSTDIR_4}_INSTDIR
!else
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_4_DIRFIELD}" "Type" "Unknown"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_4_TEXTFIELD}" "Type" "Unknown"
    intop $0 $0 - ${INSTDIR_DIRHEIGHT}
!endif
!ifdef INSTDIR_5
    SectionGetFlags ${${INSTDIR_5}_SEC01} $1
!ifdef ${INSTDIR_5}_SEC02
    SectionGetFlags ${${INSTDIR_5}_SEC02} $3
    IntOp $1 $1 | $3
!endif
    IntOp $2 $1 & 1 ;just care about the first flag
    StrCpy $1 "READONLY"
    StrCmp "$2" "1" +2
      StrCpy $1 "DISABLED"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_5_TEXTFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_5_DIRFIELD}" "Flags" "$1"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_5_TEXTFIELD}" "Text" "${INSTDIR_5_TEXT}"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_5_DIRFIELD}" "State" $${INSTDIR_5}_INSTDIR
!else
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_5_DIRFIELD}" "Type" "Unknown"
    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "${INSTDIR_5_TEXTFIELD}" "Type" "Unknown"
    intop $0 $0 - ${INSTDIR_DIRHEIGHT}
!endif

    !insertmacro MUI_INSTALLOPTIONS_WRITE "${INSTDIR_INI_FILE}" "Field 1" "Bottom" "$0"
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${INSTDIR_INI_FILE}"

    pop $3
    pop $2
    pop $1
    pop $0
  FunctionEnd

  Function UpdateInstDirs
!ifdef INSTDIR_0
    !insertmacro MUI_INSTALLOPTIONS_READ $${INSTDIR_0}_INSTDIR "${INSTDIR_INI_FILE}" "${INSTDIR_0_DIRFIELD}" "State"
    !ifdef ${INSTDIR_0}_ValidateDirectory
      call ${INSTDIR_0}_ValidateDirectoryFunc
    !endif
!endif
!ifdef INSTDIR_1
    !insertmacro MUI_INSTALLOPTIONS_READ $${INSTDIR_1}_INSTDIR "${INSTDIR_INI_FILE}" "${INSTDIR_1_DIRFIELD}" "State"
    !ifdef ${INSTDIR_1}_ValidateDirectory
      call ${INSTDIR_1}_ValidateDirectoryFunc
    !endif
!endif
!ifdef INSTDIR_2
    !insertmacro MUI_INSTALLOPTIONS_READ $${INSTDIR_2}_INSTDIR "${INSTDIR_INI_FILE}" "${INSTDIR_2_DIRFIELD}" "State"
    !ifdef ${INSTDIR_2}_ValidateDirectory
      call ${INSTDIR_2}_ValidateDirectoryFunc
    !endif
!endif
!ifdef INSTDIR_3
    !insertmacro MUI_INSTALLOPTIONS_READ $${INSTDIR_3}_INSTDIR "${INSTDIR_INI_FILE}" "${INSTDIR_3_DIRFIELD}" "State"
    !ifdef ${INSTDIR_3}_ValidateDirectory
      call ${INSTDIR_3}_ValidateDirectoryFunc
    !endif
!endif
!ifdef INSTDIR_4
    !insertmacro MUI_INSTALLOPTIONS_READ $${INSTDIR_4}_INSTDIR "${INSTDIR_INI_FILE}" "${INSTDIR_4_DIRFIELD}" "State"
    !ifdef ${INSTDIR_4}_ValidateDirectory
      call ${INSTDIR_4}_ValidateDirectoryFunc
    !endif
!endif
!ifdef INSTDIR_5
    !insertmacro MUI_INSTALLOPTIONS_READ $${INSTDIR_5}_INSTDIR "${INSTDIR_INI_FILE}" "${INSTDIR_5_DIRFIELD}" "State"
    !ifdef ${INSTDIR_5}_ValidateDirectory
      call ${INSTDIR_5}_ValidateDirectoryFunc
    !endif
!endif
  FunctionEnd
!macroend

!macro INSTDIR_STARTUP
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${INSTDIR_INI_FILE}"
!macroend

!endif ;ifndef INSTDIR_1