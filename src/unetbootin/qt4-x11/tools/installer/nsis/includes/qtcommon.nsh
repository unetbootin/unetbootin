;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
;; Contact: Qt Software Information (qt-info@nokia.com)
;;
;; This file is part of the Windows installer of the Qt Toolkit.
;;
;; $QT_BEGIN_LICENSE:LGPL$
;; Commercial Usage
;; Licensees holding valid Qt Commercial licenses may use this file in
;; accordance with the Qt Commercial License Agreement provided with the
;; Software or, alternatively, in accordance with the terms contained in
;; a written agreement between you and Nokia.
;;
;; GNU Lesser General Public License Usage
;; Alternatively, this file may be used under the terms of the GNU Lesser
;; General Public License version 2.1 as published by the Free Software
;; Foundation and appearing in the file LICENSE.LGPL included in the
;; packaging of this file.  Please review the following information to
;; ensure the GNU Lesser General Public License version 2.1 requirements
;; will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
;;
;; In addition, as a special exception, Nokia gives you certain
;; additional rights. These rights are described in the Nokia Qt LGPL
;; Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
;; package.
;;
;; GNU General Public License Usage
;; Alternatively, this file may be used under the terms of the GNU
;; General Public License version 3.0 as published by the Free Software
;; Foundation and appearing in the file LICENSE.GPL included in the
;; packaging of this file.  Please review the following information to
;; ensure the GNU General Public License version 3.0 requirements will be
;; met: http://www.gnu.org/copyleft/gpl.html.
;;
;; If you are unsure which license is appropriate for your use, please
;; contact the sales department at qt-sales@nokia.com.
;; $QT_END_LICENSE$
;;
;; This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
;; WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
!ifndef QTCOMMON_INCLUDE
!define QTCOMMON_INCLUDE

!macro MakeQtDirectory UN
Function ${UN}MakeQtDirectory
  exch $0

  push $0
  push " "  #replace
  push "-" #with
  call ${UN}ReplaceString
  pop $0

  push $0
  push "("  #replace
  push "" #with
  call ${UN}ReplaceString
  pop $0

  push $0
  push ")"  #replace
  push "" #with
  call ${UN}ReplaceString
  pop $0

  exch $0
FunctionEnd
!macroend
!insertmacro MakeQtDirectory ""
!insertmacro MakeQtDirectory "un."

Function DeleteFloatingLicenseProgram
  exch $1
  push $0
  
  StrCmp $LICENSE_KEY "" end

  ClearErrors
  !ifndef OPENSOURCE_BUILD
  qtnsisext::IsFloatingLicense $LICENSE_KEY
  !endif
  IfErrors end
  pop $0
  
  StrCmp $0 "1" end
    IfFileExists "$1\bin\qtusagereporter.exe" 0 end
      Delete "$1\bin\qtusagereporter.exe"

  end:
  pop $0
  pop $1
FunctionEnd

Function AddStartmenuApplication
  exch $0
  IfFileExists "$0\assistant.exe" 0 +2
    CreateShortCut "$SMPROGRAMS\$STARTMENU_STRING\Assistant.lnk" "$0\assistant.exe"
  IfFileExists "$0\designer.exe" 0 +2
    CreateShortCut "$SMPROGRAMS\$STARTMENU_STRING\Designer.lnk" "$0\designer.exe"
  IfFileExists "$0\linguist.exe" 0 +2
    CreateShortCut "$SMPROGRAMS\$STARTMENU_STRING\Linguist.lnk" "$0\linguist.exe"
  IfFileExists "$0\qtdemo.exe" 0 +2
    CreateShortCut "$SMPROGRAMS\$STARTMENU_STRING\Examples and Demos.lnk" "$0\qtdemo.exe"
  IfFileExists "$0\..\README" 0 ReadMeShortCutFinished
    IfFileExists "$WINDIR\notepad.exe" +3
      CreateShortCut "$SMPROGRAMS\$STARTMENU_STRING\Qt Readme.lnk" "$SYSDIR\notepad.exe" "$0\..\README"
    goto ReadMeShortCutFinished
      CreateShortCut "$SMPROGRAMS\$STARTMENU_STRING\Qt Readme.lnk" "$WINDIR\notepad.exe" "$0\..\README"
  ReadMeShortCutFinished:
  pop $0
FunctionEnd

Function un.RemoveStartmenuApplication
  Delete "$SMPROGRAMS\$STARTMENU_STRING\Assistant.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_STRING\Designer.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_STRING\Linguist.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_STRING\Examples and Demos.lnk"
  Delete "$SMPROGRAMS\$STARTMENU_STRING\Qt Readme.lnk"
FunctionEnd

#patch the licence information
Function PatchLicenseInformation
  exch $0
  push $1
  push $2
  
  DetailPrint "Patching license information..."

  IfFileExists "$0\src\corelib\global\qconfig.h" 0 +3
  strcpy $2 "$0\src\corelib\global\qconfig.h"
  goto PatchLicensee

  IfFileExists "$0\include\Qt\qconfig.h" 0 PatchConfigPriFile
  strcpy $2 "$0\include\Qt\qconfig.h"

  PatchLicensee:
  push $2
  push '#define QT_PRODUCT_LICENSEE "'
  push '#define QT_PRODUCT_LICENSEE "$LICENSEE"$\r$\n'
  call PatchLine

  push $2
  push '#define QT_PRODUCT_LICENSE "'
  push '#define QT_PRODUCT_LICENSE "$LICENSE_PRODUCT"$\r$\n'
  call PatchLine

  push $2
  ${StrCase} $1 "$LICENSE_PRODUCT" "U"
  push '#  define QT_EDITION QT_EDITION_'
  push '#  define QT_EDITION QT_EDITION_$1$\r$\n'
  call PatchLine
  
  PatchConfigPriFile:
  IfFileExists "$0\mkspecs\qconfig.pri" 0 PatchLicenseProductDone
  push "$0\mkspecs\qconfig.pri"
  push "QT_EDITION = "
  push "QT_EDITION = $LICENSE_PRODUCT$\r$\n"
  call PatchLine

  PatchLicenseProductDone:
  pop $2
  pop $1
  pop $0
FunctionEnd

Function PatchCommonBinaryFiles
  exch $2
  push $0
  push $1

  IfFileExists "$2\bin\qmake.exe" 0 +5
  DetailPrint "Patching paths in qmake..."
  push $2
  push "$2\bin\qmake.exe"
  call PatchBinaryPaths

  DetailPrint "Patching paths in core..."
  FindFirst $0 $1 "$2\bin\QtCore*.dll"
  StrCmp $1 "" ErrorPatching
  push $2
  push "$2\bin\$1"
  call PatchBinaryPaths

  FindNext $0 $1
  StrCmp $1 "" ErrorPatching
  push $2
  push "$2\bin\$1"
  call PatchBinaryPaths

  ErrorPatching:

  pop $1
  pop $0
  pop $2
FunctionEnd

Function PatchBinaryPaths
  exch $0
  exch
  exch $2
  push $1

!ifndef OPENSOURCE_BUILD
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_prfxpath=" "qt_prfxpath=$2"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_docspath=" "qt_docspath=$2\doc"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_hdrspath=" "qt_hdrspath=$2\include"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_libspath=" "qt_libspath=$2\lib"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_binspath=" "qt_binspath=$2\bin"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_plugpath=" "qt_plugpath=$2\plugins"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_datapath=" "qt_datapath=$2"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_trnspath=" "qt_trnspath=$2\translations"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_xmplpath=" "qt_xmplpath=$2\examples"
!ifdef MODULE_LICENSECHECK
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_lcnsuser=" "qt_lcnsuser=$LICENSEE"
  qtnsisext::PatchBinary /NOUNLOAD $0 "qt_lcnsprod=" "qt_lcnsprod=$LICENSE_PRODUCT"
!endif
  qtnsisext::PatchBinary $0 "qt_demopath=" "qt_demopath=$2\demos"
!endif

  pop $1
  pop $2
  pop $0
FunctionEnd

#patching the prl files
Function PatchPrlFiles
  exch $2
  exch
  exch $3 ;buildDir
  push $0
  push $1

  FindFirst $0 $1 "$2\lib\*.prl"
  loop:
    StrCmp $1 "" done
    DetailPrint "Patching $1..."

    push "$2\lib\$1"
    push $3
    push $2
    call PatchPath

    FindNext $0 $1
    Goto loop
  done:
  pop $1
  pop $0
  pop $3
  pop $2
FunctionEnd

#
# patch line in text files
# push "qtcore4.prl"  #Filename
# push "#define ..." #START WITH
# push "c:\qt"  #REPLACE WITH
# call PatchLine
#
Function PatchLine
  exch $2 ;replacement line
  exch 2
  exch $1 ;Filename
  exch
  exch $0 ;start with
  push $3 ; tmp filename
  push $4 ; handle (tmp)
  push $5 ; handle (org)
  push $6 ; string

  ClearErrors
  GetTempFileName $3
  IfErrors done
  FileOpen $4 $3 w
  IfErrors done
  FileOpen $5 $1 r
  IfErrors done

nextline:
  FileRead $5 $6
  IfErrors renameFile
  push $6
  push $0
  push $2
  call ReplaceLine
  pop $6
  FileWrite $4 $6
  goto nextline

renameFile:
  FileClose $4
  FileClose $5
  SetDetailsPrint none
  Delete $1
  Rename $3 $1
  SetDetailsPrint both

  done:
  pop $6
  pop $5
  pop $4
  pop $3
  pop $0
  pop $1
  pop $2
FunctionEnd

#
# replaces a string that starts with something, with another string
# push string
# push "#define ..." #START WITH
# push "c:\qt"  #REPLACE WITH
# call ReplaceLine
# pop $0 #new string
#
Function ReplaceLine
  exch $2 ;new line
  exch 2
  exch $1 ;string
  exch
  exch $0 ;start with

  push $3 ; tmp string
  push $4 ; counter
  push $5 ; strlen

  StrCpy $4 "-1"
  StrLen $5 $1

  loop:
  IntOp $4 $4 + 1 ;increase counter
  StrCpy $3 $1 $4 ;get substring
  IntCmp $4 $5 copystring ; check for end
  StrCmp $3 $0 done ;start with found
  goto loop

  copystring:
  StrCpy $2 $1
  goto done

  done:
  pop $5
  pop $4
  pop $3
  pop $0
  pop $1
  exch $2
FunctionEnd

#
# patch paths in text files
# push "qtcore4.prl"  #Filename
# push "c:\compile" #OLD_QTDIR
# push "c:\qt"  #QTDIR
# call PatchPath
#
Function PatchPath
  exch $2 ;NEW
  exch 2
  exch $1 ;Filename
  exch
  exch $0 ;OLD
  push $3 ;readline
  push $4 ;file 1
  push $5 ;file 2
  push $6 ;tmpfilename

  push $7 ;forward slash NEW
  push $8 ;forward slash OLD

  push $2
  push "\"
  push "/"
  call ReplaceString
  pop $7

  push $0
  push "\"
  push "/"
  call ReplaceString
  pop $8

  ClearErrors
  GetTempFileName $6
  IfErrors done
  FileOpen $5 $6 w
  IfErrors done
  FileOpen $4 $1 r
  IfErrors done

nextline:
  FileRead $4 $3
  IfErrors renameFile
  push $3
  push $0
  push $2
  call ReplaceString ;replace backward slash path
  push $8
  push $7
  call ReplaceString ;replace forward slash path
  pop $3
  FileWrite $5 $3
  goto nextline

renameFile:
  FileClose $5
  FileClose $4
  SetDetailsPrint none
  Delete $1
  Rename $6 $1
  SetDetailsPrint both

done:
  pop $8
  pop $7
  pop $6
  pop $5
  pop $4
  pop $3
  pop $0
  pop $1
  pop $2
FunctionEnd

#
# replaces a string with another string
# push string
# push "c:\qt"  #replace
# push "c:\compile" #with
# call ReplaceString
# pop $0 #new string
#
!macro ReplaceString UN
Function ${UN}ReplaceString
  exch $2 ;NEW
  exch 2
  exch $1 ;string
  exch
  exch $0 ;OLD

  push $3 ; tmp string
  push $4 ; counter
  push $5 ; result

  push $6 ; old strlen

  StrCpy $4 "-1"
  StrCpy $5 ""

  StrLen $6 $0

  loop:
  IntOp $4 $4 + 1 ;increase counter
  StrCpy $3 $1 $6 $4 ;get substring
  StrCmp $3 "" done ; check for end
  StrCmp $3 $0 replace ;replace if old
  StrCpy $3 $1 "1" $4
  StrCpy $5 $5$3 ;append character to result
  goto loop

  replace:
  StrCpy $5 $5$2 ;insert new qtdir
  IntOp $4 $4 + $6 ;increase offset
  IntOp $4 $4 - 1 ;decrease offset one more
  goto loop

  done:
  StrCpy $2 $5
  pop $6
  pop $5
  pop $4
  pop $3
  pop $0
  pop $1
  exch $2
FunctionEnd
!macroend
!insertmacro ReplaceString ""
!insertmacro ReplaceString "un."

Function CommonCheckDirectory
  exch $4
  exch
  exch $5
  push $0
  push $1
  push $2
  push $3

  ; check if qt is already installed
  IfFileExists "$4\bin\qmake.exe" 0 +2
  IfFileExists "$4\uninst.exe" qtExistsError
  
  ; check if directory is empty
  FindFirst $0 $1 "$4\*"
  CommonCheckDirectory_FileSearchLoop:
    StrCmp $1 "" CommonCheckDirectory_DirDoesNotExist
    StrCmp $1 "." CommonCheckDirectory_ContinueSearchLoop
    StrCmp $1 ".." CommonCheckDirectory_ContinueSearchLoop
    goto CommonCheckDirectory_FoundFile
    CommonCheckDirectory_ContinueSearchLoop:
    FindNext $0 $1
    goto CommonCheckDirectory_FileSearchLoop

CommonCheckDirectory_FoundFile:
  FindClose $0
  MessageBox MB_YESNO|MB_ICONEXCLAMATION "This directory already has contents. Are you sure you want to use this directory?" IDYES CommonCheckDirectory_DirDoesNotExist
  Goto errorInDirectory
CommonCheckDirectory_DirDoesNotExist:
  FindClose $0

  GetInstDirError $0
  IntCmp 0 $0 0 instDirError

  StrLen $0 $4

!ifdef USE_NODIRLENGTHCHECK
  StrCpy $1 "400"
!else
  StrLen $1 $5
!endif

  IntCmp $1 $0 0 directoryToLong

  ;check for spaces
  StrCpy $2 "-1"
  StrCpy $3 ""

  loop:
  IntOp $2 $2 + 1 ;increase counter
  StrCpy $3 $4 "1" $2 ;get char
  StrCmp $3 "" directoryOk ; check for end
  StrCmp $3 " " spaceInDirectory ;check for space
  goto loop

qtExistsError:
  MessageBox MB_OK|MB_ICONEXCLAMATION "Qt is already installed in this directory. Please uninstall the previous version and try again."
  Goto errorInDirectory

instDirError:
  MessageBox MB_OK|MB_ICONEXCLAMATION "This is not a valid installation directory."
  Goto errorInDirectory

spaceInDirectory:
  MessageBox MB_OK|MB_ICONEXCLAMATION "The installation path can't contain spaces."
  Goto errorInDirectory

directoryToLong:
  MessageBox MB_OK|MB_ICONEXCLAMATION "The installation directory is to long."
  Goto errorInDirectory

errorInDirectory:
  pop $3
  pop $2
  pop $1
  pop $0
  pop $5
  pop $4
  Abort
  goto done

directoryOk:
  pop $3
  pop $2
  pop $1
  pop $0
  pop $5
  pop $4
done:
FunctionEnd

!endif ;QTCOMMON_INCLUDE