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
!ifdef MODULE_OPENSOURCE
!macro OPENSOURCE_INITIALIZE
  !define MODULE_OPENSOURCE_PAGE "opensource.ini"
  page custom ModuleOpenSourceShowPage
!macroend
!macro OPENSOURCE_SECTIONS
  Section -ModuleOpenSourceSection
    !ifdef MODULE_OPENSOURCE_ROOT
      SetOutPath "$INSTDIR"
      File "${MODULE_OPENSOURCE_ROOT}\OPENSOURCE-NOTICE.TXT"
    !endif
  SectionEnd

  Function ModuleOpenSourceShowPage
    !insertmacro MUI_HEADER_TEXT "Open Source Edition" " "
    !insertmacro MUI_INSTALLOPTIONS_DISPLAY "${MODULE_OPENSOURCE_PAGE}"
    strcpy "$LICENSEE" "Open Source"
    strcpy "$LICENSE_PRODUCT" "OpenSource"
  FunctionEnd
!macroend
!macro OPENSOURCE_DESCRIPTION
!macroend
!macro OPENSOURCE_STARTUP
  !insertmacro MUI_INSTALLOPTIONS_EXTRACT "${MODULE_OPENSOURCE_PAGE}"
!macroend
!macro OPENSOURCE_FINISH
!macroend
!macro OPENSOURCE_UNSTARTUP
!macroend
!macro OPENSOURCE_UNINSTALL
  Section -un.ModuleOpenSourceSection
    Delete "$SMPROGRAMS\$STARTMENU_STRING\OpenSource Notice.lnk"
  SectionEnd
!macroend
!macro OPENSOURCE_UNFINISH
!macroend
!else ;MODULE_OPENSOURCE
!macro OPENSOURCE_INITIALIZE
!macroend
!macro OPENSOURCE_SECTIONS
!macroend
!macro OPENSOURCE_DESCRIPTION
!macroend
!macro OPENSOURCE_STARTUP
!macroend
!macro OPENSOURCE_FINISH
!macroend
!macro OPENSOURCE_UNSTARTUP
!macroend
!macro OPENSOURCE_UNINSTALL
!macroend
!macro OPENSOURCE_UNFINISH
!macroend
!endif ;MODULE_OPENSOURCE

