/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QXMLSTREAM_P_H
#define QXMLSTREAM_P_H

#include "qxmlstream.h"

#ifndef QT_NO_XMLSTREAM

#ifndef QT_NO_XMLSTREAMREADER

class QXmlStreamReader_Table
{
public:
  enum {
    EOF_SYMBOL = 0,
    AMPERSAND = 5,
    ANY = 41,
    ATTLIST = 31,
    BANG = 25,
    CDATA = 46,
    CDATA_START = 28,
    COLON = 17,
    COMMA = 19,
    DASH = 20,
    DBLQUOTE = 8,
    DIGIT = 27,
    DOCTYPE = 29,
    DOT = 23,
    ELEMENT = 30,
    EMPTY = 40,
    ENTITIES = 50,
    ENTITY = 32,
    ENTITY_DONE = 45,
    EQ = 14,
    ERROR = 43,
    FIXED = 39,
    HASH = 6,
    ID = 47,
    IDREF = 48,
    IDREFS = 49,
    IMPLIED = 38,
    LANGLE = 3,
    LBRACK = 9,
    LETTER = 26,
    LPAREN = 11,
    NDATA = 36,
    NMTOKEN = 51,
    NMTOKENS = 52,
    NOTATION = 33,
    NOTOKEN = 1,
    PARSE_ENTITY = 44,
    PCDATA = 42,
    PERCENT = 15,
    PIPE = 13,
    PLUS = 21,
    PUBLIC = 35,
    QUESTIONMARK = 24,
    QUOTE = 7,
    RANGLE = 4,
    RBRACK = 10,
    REQUIRED = 37,
    RPAREN = 12,
    SEMICOLON = 18,
    SLASH = 16,
    SPACE = 2,
    STAR = 22,
    SYSTEM = 34,
    VERSION = 54,
    XML = 53,

    ACCEPT_STATE = 407,
    RULE_COUNT = 261,
    STATE_COUNT = 418,
    TERMINAL_COUNT = 55,
    NON_TERMINAL_COUNT = 81,

    GOTO_INDEX_OFFSET = 418,
    GOTO_INFO_OFFSET = 925,
    GOTO_CHECK_OFFSET = 925
  };

  static const char  *const spell [];
  static const int            lhs [];
  static const int            rhs [];
  static const int   goto_default [];
  static const int action_default [];
  static const int   action_index [];
  static const int    action_info [];
  static const int   action_check [];

  inline int nt_action (int state, int nt) const
  {
    const int *const goto_index = &action_index [GOTO_INDEX_OFFSET];
    const int *const goto_check = &action_check [GOTO_CHECK_OFFSET];

    const int yyn = goto_index [state] + nt;

    if (yyn < 0 || goto_check [yyn] != nt)
      return goto_default [nt];

    const int *const goto_info = &action_info [GOTO_INFO_OFFSET];
    return goto_info [yyn];
  }

  inline int t_action (int state, int token) const
  {
    const int yyn = action_index [state] + token;

    if (yyn < 0 || action_check [yyn] != token)
      return - action_default [state];

    return action_info [yyn];
  }
};


const char *const QXmlStreamReader_Table::spell [] = {
  "end of file", 0, " ", "<", ">", "&", "#", "\'", "\"", "[",
  "]", "(", ")", "|", "=", "%", "/", ":", ";", ",",
  "-", "+", "*", ".", "?", "!", "[a-zA-Z]", "[0-9]", "[CDATA[", "DOCTYPE",
  "ELEMENT", "ATTLIST", "ENTITY", "NOTATION", "SYSTEM", "PUBLIC", "NDATA", "REQUIRED", "IMPLIED", "FIXED",
  "EMPTY", "ANY", "PCDATA", 0, 0, 0, "CDATA", "ID", "IDREF", "IDREFS",
  "ENTITIES", "NMTOKEN", "NMTOKENS", "<?xml", "version"};

const int QXmlStreamReader_Table::lhs [] = {
  55, 55, 57, 57, 57, 57, 57, 57, 57, 57,
  65, 66, 62, 70, 70, 70, 73, 64, 64, 64,
  64, 77, 76, 78, 78, 78, 78, 78, 78, 79,
  79, 79, 79, 79, 79, 79, 85, 81, 86, 86,
  86, 86, 89, 90, 91, 91, 91, 91, 92, 92,
  94, 94, 94, 95, 95, 96, 96, 97, 97, 98,
  98, 87, 87, 93, 88, 99, 99, 101, 101, 101,
  101, 101, 101, 101, 101, 101, 101, 102, 103, 103,
  103, 103, 105, 106, 107, 107, 82, 82, 108, 108,
  110, 110, 83, 83, 83, 63, 63, 74, 112, 61,
  113, 114, 84, 84, 84, 115, 115, 115, 115, 115,
  115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
  115, 115, 115, 115, 115, 115, 116, 116, 116, 116,
  68, 68, 68, 68, 117, 118, 117, 118, 117, 118,
  117, 118, 120, 120, 120, 120, 120, 120, 120, 120,
  120, 120, 120, 120, 120, 120, 120, 120, 120, 120,
  120, 120, 120, 120, 119, 71, 111, 111, 111, 111,
  121, 122, 121, 122, 121, 122, 121, 122, 123, 123,
  123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
  123, 123, 123, 123, 123, 123, 123, 123, 123, 123,
  123, 123, 123, 104, 104, 104, 104, 126, 127, 126,
  127, 126, 126, 127, 127, 128, 128, 128, 128, 130,
  69, 69, 69, 131, 131, 132, 60, 58, 59, 133,
  80, 125, 129, 124, 134, 134, 134, 134, 56, 56,
  56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
  72, 67, 67, 75, 109, 100, 100, 100, 100, 100,
  135};

const int QXmlStreamReader_Table:: rhs[] = {
  2, 1, 4, 2, 2, 2, 2, 2, 2, 0,
  1, 1, 9, 2, 4, 0, 4, 4, 6, 6,
  4, 1, 3, 1, 1, 1, 2, 2, 2, 1,
  1, 1, 1, 1, 1, 1, 4, 4, 1, 1,
  1, 1, 1, 2, 1, 1, 1, 0, 2, 2,
  2, 6, 6, 1, 5, 1, 5, 3, 5, 0,
  1, 6, 8, 4, 2, 1, 5, 1, 1, 1,
  1, 1, 1, 1, 1, 6, 7, 1, 2, 2,
  1, 4, 3, 3, 1, 2, 5, 6, 4, 6,
  3, 5, 5, 3, 4, 4, 5, 2, 3, 2,
  2, 4, 5, 5, 7, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 3, 3, 2, 2, 2, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 2, 2, 3, 3,
  2, 2, 2, 2, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 2, 2, 3, 3, 2, 2, 2,
  2, 1, 1, 1, 1, 1, 1, 1, 1, 5,
  0, 1, 3, 1, 3, 2, 4, 3, 5, 3,
  3, 3, 3, 4, 1, 1, 2, 2, 2, 4,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 0,
  1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
  2};

const int QXmlStreamReader_Table::action_default [] = {
  10, 250, 0, 2, 1, 0, 124, 116, 118, 119,
  126, 129, 122, 11, 113, 107, 0, 108, 128, 110,
  114, 112, 120, 123, 125, 106, 109, 111, 117, 115,
  127, 121, 12, 243, 247, 242, 0, 130, 239, 246,
  16, 241, 249, 248, 0, 245, 250, 221, 244, 0,
  0, 255, 0, 236, 235, 0, 238, 237, 234, 230,
  98, 254, 0, 226, 0, 0, 251, 96, 97, 100,
  0, 0, 252, 0, 0, 166, 0, 163, 155, 157,
  158, 132, 144, 161, 152, 146, 147, 143, 149, 153,
  151, 159, 162, 142, 145, 148, 150, 156, 154, 164,
  160, 140, 165, 0, 134, 138, 136, 141, 131, 139,
  0, 137, 133, 135, 0, 15, 14, 253, 0, 22,
  21, 252, 0, 0, 20, 0, 0, 31, 36, 30,
  0, 32, 252, 0, 33, 0, 24, 0, 34, 0,
  26, 35, 25, 0, 231, 40, 39, 252, 42, 48,
  252, 41, 0, 43, 252, 48, 252, 0, 252, 0,
  48, 0, 47, 45, 46, 50, 51, 252, 252, 0,
  56, 252, 53, 252, 0, 57, 0, 54, 252, 52,
  252, 0, 55, 64, 0, 252, 60, 252, 0, 58,
  61, 62, 0, 252, 0, 0, 59, 63, 44, 49,
  65, 0, 38, 0, 0, 252, 0, 93, 94, 0,
  0, 0, 0, 252, 0, 200, 191, 193, 195, 168,
  180, 198, 189, 183, 181, 184, 179, 186, 188, 196,
  199, 178, 182, 185, 187, 192, 190, 194, 197, 201,
  203, 202, 176, 0, 0, 232, 170, 174, 172, 0,
  0, 92, 177, 167, 175, 0, 173, 169, 171, 91,
  0, 95, 0, 0, 0, 0, 0, 252, 85, 252,
  0, 253, 0, 86, 0, 88, 68, 73, 72, 69,
  70, 71, 252, 74, 75, 0, 0, 0, 260, 259,
  257, 258, 256, 66, 252, 0, 252, 0, 0, 67,
  76, 252, 0, 252, 0, 0, 77, 0, 78, 0,
  81, 84, 0, 0, 205, 215, 214, 0, 217, 219,
  218, 216, 0, 233, 207, 211, 209, 213, 204, 212,
  0, 210, 206, 208, 0, 80, 79, 0, 82, 0,
  83, 87, 99, 0, 37, 0, 0, 0, 0, 90,
  89, 0, 102, 23, 27, 29, 28, 0, 0, 252,
  253, 0, 252, 0, 105, 104, 252, 0, 103, 101,
  0, 0, 18, 252, 17, 0, 19, 0, 0, 240,
  0, 252, 0, 229, 0, 222, 228, 0, 227, 224,
  252, 252, 253, 223, 225, 0, 252, 0, 220, 252,
  0, 252, 0, 221, 0, 0, 13, 261, 9, 5,
  8, 4, 0, 7, 250, 6, 0, 3};

const int QXmlStreamReader_Table::goto_default [] = {
  2, 4, 3, 46, 379, 41, 35, 48, 45, 39,
  240, 49, 118, 75, 384, 72, 76, 117, 40, 44,
  160, 121, 122, 137, 136, 140, 129, 127, 131, 138,
  130, 150, 151, 148, 159, 158, 200, 156, 155, 157,
  178, 171, 187, 191, 294, 293, 286, 312, 311, 310,
  270, 268, 269, 133, 52, 132, 213, 36, 33, 139,
  37, 38, 110, 103, 321, 102, 255, 243, 242, 239,
  241, 330, 317, 316, 320, 389, 390, 47, 43, 55,
  0};

const int QXmlStreamReader_Table::action_index [] = {
  -30, -55, 39, 102, 870, 94, -55, -55, -55, -55,
  -55, -55, -55, -55, -55, -55, 84, -55, -55, -55,
  -55, -55, -55, -55, -55, -55, -55, -55, -55, -55,
  -55, -55, -55, -55, -55, -55, 40, -55, -55, -55,
  53, -55, -55, -55, 93, -55, -55, 80, -55, 3,
  72, -55, 2, -55, -55, 89, -55, -55, -55, -55,
  -55, -55, 10, -55, 43, 20, -55, -55, -55, -55,
  44, 64, 25, 234, 260, -55, 80, -55, -55, -55,
  -55, -55, -55, -55, -55, -55, -55, -55, -55, -55,
  -55, -55, -55, -55, -55, -55, -55, -55, -55, -55,
  -55, -55, -55, 287, -55, -55, -55, -55, -55, -55,
  340, -55, -55, -55, 55, -55, -55, -55, 57, -55,
  -55, 80, 143, 26, -55, 45, 10, -55, -55, -55,
  119, -55, 56, 142, -55, 148, -55, 172, -55, 42,
  -55, -55, -55, 13, -55, -55, -55, 23, -55, 105,
  80, -55, 122, -55, 80, 110, 80, 22, 80, -9,
  120, 49, -55, -55, -55, -55, 61, 80, 80, 91,
  -55, 24, 9, 27, 86, -55, 67, -55, 27, 4,
  27, 68, -55, -55, 71, 27, -7, 80, -5, -55,
  -55, -55, 83, 27, -3, 6, -55, -55, -55, -55,
  -55, 19, -55, -4, 14, 27, 12, -55, -55, 658,
  128, 499, 128, 80, 75, -55, -55, -55, -55, -55,
  -55, -55, -55, -55, -55, -55, -55, -55, -55, -55,
  -55, -55, -55, -55, -55, -55, -55, -55, -55, -55,
  -55, -55, -55, 605, 28, -55, -55, -55, -55, 80,
  128, -55, -55, -55, -55, 711, -55, -55, -55, -55,
  34, -55, 17, 15, 16, 88, 21, 80, -55, 80,
  173, 11, 36, -55, 37, -55, -55, -55, -55, -55,
  -55, -55, 80, -55, -55, 38, 117, 169, -55, -55,
  -55, -55, -55, -55, 27, 52, 27, 27, 146, -55,
  -55, 27, 144, 27, 41, 27, -55, 446, -55, 393,
  -55, -55, 100, 85, -55, -55, -55, 764, -55, -55,
  -55, -55, -8, -55, -55, -55, -55, -55, -55, -55,
  552, -55, -55, -55, 27, -55, -55, 82, -55, 27,
  -55, -55, -55, 27, -55, 27, 27, -19, 27, -55,
  -55, 27, -55, -55, -55, -55, -55, 128, 128, 27,
  128, 5, 27, 8, -55, -55, 27, 7, -55, -55,
  -26, 181, -55, 27, -55, 1, -55, 817, 127, -55,
  -20, 27, -2, -55, 69, -26, -55, -1, -55, -55,
  27, 27, -26, -55, -55, -13, 27, 48, -55, 27,
  -6, 27, 128, 27, -11, 0, -55, -55, -55, -55,
  -55, -55, 30, -55, -55, -55, 817, -55,

  -81, -81, -81, 191, 73, -81, -81, -81, -81, -81,
  -81, -81, -81, -81, -81, -81, 0, -81, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, -81, -81, -81, -81, 3, 53, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, 49, -81, 55, -81, -81, -81, -81, -81,
  -81, 61, -81, 29, 38, -81, 46, -81, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, -81, 64, -81, -81, -81, -81, -81, -81,
  -34, -81, -81, -81, 24, -81, -81, -81, -81, -81,
  -81, 27, 110, -81, -81, -81, -14, -81, -81, -81,
  12, -81, 30, -81, -81, -81, -81, 116, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, 19, -81, -81,
  22, -81, -81, -81, 20, 18, 40, -81, 43, -81,
  33, -81, -81, -81, -81, -81, -81, 35, 34, 41,
  -81, 36, -81, 32, 21, -81, 16, -81, 31, -81,
  37, 28, -81, -81, -81, 113, -81, 45, 42, -81,
  -81, -81, -81, 39, -81, 25, -81, -81, -81, -81,
  -81, -81, -81, -21, -81, 15, -81, -81, -81, -81,
  13, -51, 23, 26, -13, -81, -81, -81, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, -81, -33, -81, -81, -81, -81, -81, 67,
  66, -81, -81, -81, -81, -12, -81, -81, -81, -81,
  -81, -81, 62, -81, 60, 58, 63, 82, -81, 97,
  -81, 65, -81, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, 69, -81, -81, -81, -81, -81, -81, -81,
  -81, -81, -81, -81, 14, -81, 48, 50, 59, -81,
  -81, 47, 51, 52, -81, 56, -81, 76, -81, 98,
  -81, -81, -81, 57, -81, -81, -81, 105, -81, -81,
  -81, -81, -81, -81, -81, -81, -81, -81, -81, -81,
  96, -81, -81, -81, 54, -81, -81, 17, -81, 44,
  -81, -81, -81, 8, -81, -11, 11, -38, 5, -81,
  -81, -8, -81, -81, -81, -81, -81, -3, 2, 79,
  10, -81, -2, -81, -81, -81, 6, -81, -81, -81,
  -20, 80, -81, -7, -81, -81, -81, 77, -19, -81,
  4, -4, -81, -81, -81, -6, -81, -81, -81, -81,
  88, -1, 68, -81, -81, -81, 9, -37, -81, 7,
  -81, -5, -10, 75, -81, -81, -81, -81, -81, -81,
  -81, -81, -81, -81, 1, -81, 103, -81};

const int QXmlStreamReader_Table::action_info [] = {
  61, 396, 383, 388, 406, 376, 61, 51, 401, 365,
  323, 368, 364, 405, 1, 190, 207, 180, 208, 197,
  59, 61, 51, 202, 68, 66, 66, 66, 173, 66,
  124, 144, 61, 198, 183, 342, 51, 61, 261, 407,
  341, 275, 61, 61, 69, 66, 245, 51, 0, 301,
  263, 74, 73, 305, 296, 309, 307, 399, 66, 370,
  154, 120, 74, 73, 297, 296, 119, 65, 0, 62,
  60, 74, 73, 386, 168, 61, 358, 357, 154, 154,
  167, 50, 66, 186, 185, 387, 0, 71, 70, 309,
  307, 50, 203, 61, 61, 194, 193, 154, 54, 53,
  50, 51, 154, 345, 408, 16, 0, 58, 62, 60,
  61, 51, 61, 263, 51, 57, 56, 61, 0, 0,
  51, 369, 370, 308, 309, 307, 162, 164, 153, 163,
  147, 162, 164, 154, 163, 74, 73, 336, 335, 334,
  0, 162, 164, 380, 163, 66, 125, 0, 61, 211,
  209, 62, 60, 61, 0, 32, 0, 0, 126, 146,
  145, 288, 0, 288, 289, 0, 289, 291, 263, 291,
  292, 290, 292, 290, 66, 125, 212, 210, 264, 262,
  265, 266, 353, 66, 282, 372, 288, 126, 13, 289,
  119, 0, 291, 0, 0, 292, 290, 0, 0, 0,
  0, 0, 0, 0, 0, 278, 285, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 13, 0, 276,
  279, 280, 281, 277, 283, 284, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 99, 0, 94, 0,
  85, 93, 81, 86, 95, 88, 96, 90, 84, 89,
  98, 78, 97, 79, 80, 91, 100, 83, 92, 77,
  87, 82, 99, 0, 94, 0, 85, 108, 107, 86,
  95, 88, 96, 90, 84, 89, 98, 78, 97, 79,
  80, 91, 100, 83, 92, 77, 87, 82, 0, 99,
  0, 94, 0, 85, 105, 104, 86, 95, 88, 96,
  90, 84, 89, 98, 78, 97, 79, 80, 91, 100,
  83, 92, 77, 87, 82, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 99, 0, 94, 0, 85, 112, 111, 86,
  95, 88, 96, 90, 84, 89, 98, 78, 97, 79,
  80, 91, 100, 83, 92, 77, 87, 82, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 99, 0, 94, 313, 85,
  328, 327, 86, 95, 88, 96, 90, 84, 89, 98,
  78, 97, 79, 80, 91, 100, 83, 92, 77, 87,
  82, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 13, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 99, 0,
  94, 313, 85, 315, 314, 86, 95, 88, 96, 90,
  84, 89, 98, 78, 97, 79, 80, 91, 100, 83,
  92, 77, 87, 82, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 13, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 237, 224, 232, 214, 223, 253, 252, 225, 233,
  227, 234, 228, 222, 0, 236, 216, 235, 217, 218,
  229, 238, 221, 230, 215, 226, 220, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 13, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 99, 0, 94, 313, 85, 332,
  331, 86, 95, 88, 96, 90, 84, 89, 98, 78,
  97, 79, 80, 91, 100, 83, 92, 77, 87, 82,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 13, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 237, 224, 232,
  214, 223, 247, 246, 225, 233, 227, 234, 228, 222,
  0, 236, 216, 235, 217, 218, 229, 238, 221, 230,
  215, 226, 220, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  13, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  237, 224, 232, 214, 223, 231, 219, 225, 233, 227,
  234, 228, 222, 0, 236, 216, 235, 217, 218, 229,
  238, 221, 230, 215, 226, 220, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 13, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 237, 224, 232, 214, 223, 257, 256,
  225, 233, 227, 234, 228, 222, 0, 236, 216, 235,
  217, 218, 229, 238, 221, 230, 215, 226, 220, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 13, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 99, 0, 94, 313,
  85, 325, 324, 86, 95, 88, 96, 90, 84, 89,
  98, 78, 97, 79, 80, 91, 100, 83, 92, 77,
  87, 82, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 13,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 30,
  378, 25, 5, 15, 24, 10, 17, 26, 19, 27,
  21, 14, 20, 29, 7, 28, 8, 9, 22, 31,
  12, 23, 6, 18, 11, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 13, 0, 0, 0, 0, 0, 0, 0,
  32, 0, 30, 16, 25, 5, 15, 24, 10, 17,
  26, 19, 27, 21, 14, 20, 29, 7, 28, 8,
  9, 22, 31, 12, 23, 6, 18, 11, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 13, 0, 0, 0, 0,
  0, 0, 0, 32, 0,

  371, 63, 416, 403, 377, 375, 347, 402, 382, 352,
  363, 395, 398, 359, 391, 366, 348, 254, 367, 400,
  63, 397, 349, 362, 381, 344, 295, 206, 350, 249,
  113, 152, 161, 205, 201, 248, 259, 115, 260, 123,
  143, 244, 204, 179, 174, 196, 176, 169, 172, 181,
  149, 195, 166, 177, 165, 184, 258, 192, 175, 302,
  298, 340, 189, 114, 304, 182, 338, 300, 0, 199,
  385, 337, 67, 306, 116, 0, 0, 0, 170, 251,
  343, 287, 267, 42, 250, 339, 319, 42, 391, 404,
  0, 361, 385, 101, 272, 303, 360, 374, 0, 271,
  393, 373, 109, 64, 299, 392, 319, 417, 319, 274,
  0, 322, 346, 42, 271, 319, 128, 351, 141, 0,
  134, 0, 128, 0, 141, 188, 134, 142, 106, 135,
  0, 0, 0, 356, 0, 135, 0, 0, 0, 0,
  354, 355, 34, 394, 0, 318, 34, 0, 273, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 318, 0, 318, 0, 333,
  0, 329, 34, 0, 318, 0, 0, 0, 326, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 414, 0, 411, 409, 415, 413,
  410, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  412, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0};

const int QXmlStreamReader_Table::action_check [] = {
  26, 14, 4, 4, 4, 4, 26, 26, 14, 4,
  18, 4, 4, 24, 44, 22, 4, 13, 4, 22,
  18, 26, 26, 4, 4, 2, 2, 2, 19, 2,
  4, 18, 26, 42, 12, 20, 26, 26, 4, 0,
  4, 4, 26, 26, 4, 2, 18, 26, -1, 11,
  20, 7, 8, 12, 13, 7, 8, 54, 2, 29,
  11, 4, 7, 8, 12, 13, 9, 24, -1, 24,
  25, 7, 8, 4, 13, 26, 34, 35, 11, 11,
  19, 6, 2, 12, 13, 16, -1, 34, 35, 7,
  8, 6, 36, 26, 26, 12, 13, 11, 26, 27,
  6, 26, 11, 15, 2, 3, -1, 18, 24, 25,
  26, 26, 26, 20, 26, 26, 27, 26, -1, -1,
  26, 28, 29, 6, 7, 8, 21, 22, 6, 24,
  11, 21, 22, 11, 24, 7, 8, 37, 38, 39,
  -1, 21, 22, 16, 24, 2, 3, -1, 26, 7,
  8, 24, 25, 26, -1, 53, -1, -1, 15, 40,
  41, 17, -1, 17, 20, -1, 20, 23, 20, 23,
  26, 27, 26, 27, 2, 3, 34, 35, 30, 31,
  32, 33, 10, 2, 11, 4, 17, 15, 45, 20,
  9, -1, 23, -1, -1, 26, 27, -1, -1, -1,
  -1, -1, -1, -1, -1, 32, 33, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, 45, -1, 46,
  47, 48, 49, 50, 51, 52, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, 2, -1, 4, -1,
  6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 2, -1, 4, -1, 6, 7, 8, 9,
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, -1, 2,
  -1, 4, -1, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, 2, -1, 4, -1, 6, 7, 8, 9,
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, 2, -1, 4, 5, 6,
  7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
  27, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, 45, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, 2, -1,
  4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
  14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, 45, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
  11, 12, 13, 14, -1, 16, 17, 18, 19, 20,
  21, 22, 23, 24, 25, 26, 27, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, 45, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, 2, -1, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, 45, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, 2, 3, 4,
  5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
  -1, 16, 17, 18, 19, 20, 21, 22, 23, 24,
  25, 26, 27, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  45, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
  12, 13, 14, -1, 16, 17, 18, 19, 20, 21,
  22, 23, 24, 25, 26, 27, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 45, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, 2, 3, 4, 5, 6, 7, 8,
  9, 10, 11, 12, 13, 14, -1, 16, 17, 18,
  19, 20, 21, 22, 23, 24, 25, 26, 27, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, 45, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, 2, -1, 4, 5,
  6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, 45,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, 2,
  3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
  23, 24, 25, 26, 27, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, 45, -1, -1, -1, -1, -1, -1, -1,
  53, -1, 2, 3, 4, 5, 6, 7, 8, 9,
  10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, 45, -1, -1, -1, -1,
  -1, -1, -1, 53, -1,

  20, 20, 1, 13, 1, 12, 17, 12, 12, 17,
  12, 12, 49, 16, 20, 13, 54, 68, 12, 12,
  20, 12, 17, 13, 20, 17, 12, 12, 17, 16,
  64, 12, 12, 54, 12, 68, 13, 13, 12, 12,
  54, 54, 12, 12, 12, 20, 12, 12, 12, 12,
  38, 12, 12, 37, 36, 12, 68, 12, 37, 12,
  12, 17, 20, 17, 12, 37, 49, 17, -1, 36,
  17, 17, 17, 17, 13, -1, -1, -1, 37, 13,
  20, 12, 20, 10, 17, 20, 10, 10, 20, 14,
  -1, 12, 17, 64, 12, 44, 17, 17, -1, 17,
  12, 21, 64, 54, 45, 17, 10, 4, 10, 12,
  -1, 54, 54, 10, 17, 10, 6, 54, 8, -1,
  10, -1, 6, -1, 8, 12, 10, 17, 64, 19,
  -1, -1, -1, 17, -1, 19, -1, -1, -1, -1,
  24, 25, 69, 75, -1, 69, 69, -1, 51, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, 69, -1, 69, -1, 73,
  -1, 73, 69, -1, 69, -1, -1, -1, 73, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, 3, -1, 5, 6, 7, 8,
  9, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  19, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1};

#endif // QT_NO_XMLSTREAMREADER

template <typename T> class QXmlStreamSimpleStack {
    T *data;
    int tos, cap;
public:
    inline QXmlStreamSimpleStack():data(0), tos(-1), cap(0){}
    inline ~QXmlStreamSimpleStack(){ if (data) qFree(data); }

    inline void reserve(int extraCapacity) {
        if (tos + extraCapacity + 1 > cap) {
            cap = qMax(tos + extraCapacity + 1, cap << 1 );
            data = reinterpret_cast<T *>(qRealloc(data, cap * sizeof(T)));
        }
    }

    inline T &push() { reserve(1); return data[++tos]; }
    inline T &rawPush() { return data[++tos]; }
    inline const T &top() const { return data[tos]; }
    inline T &top() { return data[tos]; }
    inline T &pop() { return data[tos--]; }
    inline T &operator[](int index) { return data[index]; }
    inline const T &at(int index) const { return data[index]; }
    inline int size() const { return tos + 1; }
    inline void resize(int s) { tos = s - 1; }
    inline bool isEmpty() const { return tos < 0; }
    inline void clear() { tos = -1; }
};


class QXmlStream
{
    Q_DECLARE_TR_FUNCTIONS(QXmlStream)
};

class QXmlStreamPrivateTagStack {
public:
    struct NamespaceDeclaration
    {
        QStringRef prefix;
        QStringRef namespaceUri;
    };

    struct Tag
    {
        QStringRef name;
        QStringRef qualifiedName;
        NamespaceDeclaration namespaceDeclaration;
        int tagStackStringStorageSize;
        int namespaceDeclarationsSize;
    };


    QXmlStreamPrivateTagStack();
    QXmlStreamSimpleStack<NamespaceDeclaration> namespaceDeclarations;
    QString tagStackStringStorage;
    int tagStackStringStorageSize;
    bool tagsDone;

    inline QStringRef addToStringStorage(const QStringRef &s) {
        int pos = tagStackStringStorageSize;
	int sz = s.size();
	if (pos != tagStackStringStorage.size())
	    tagStackStringStorage.resize(pos);
        tagStackStringStorage.insert(pos, s.unicode(), sz);
        tagStackStringStorageSize += sz;
        return QStringRef(&tagStackStringStorage, pos, sz);
    }
    inline QStringRef addToStringStorage(const QString &s) {
        int pos = tagStackStringStorageSize;
	int sz = s.size();
	if (pos != tagStackStringStorage.size())
	    tagStackStringStorage.resize(pos);
        tagStackStringStorage.insert(pos, s.unicode(), sz);
        tagStackStringStorageSize += sz;
        return QStringRef(&tagStackStringStorage, pos, sz);
    }

    QXmlStreamSimpleStack<Tag> tagStack;


    inline Tag &tagStack_pop() {
        Tag& tag = tagStack.pop();
        tagStackStringStorageSize = tag.tagStackStringStorageSize;
        namespaceDeclarations.resize(tag.namespaceDeclarationsSize);
        tagsDone = tagStack.isEmpty();
        return tag;
    }
    inline Tag &tagStack_push() {
        Tag &tag = tagStack.push();
        tag.tagStackStringStorageSize = tagStackStringStorageSize;
        tag.namespaceDeclarationsSize = namespaceDeclarations.size();
        return tag;
    }
};


class QXmlStreamEntityResolver;

#ifndef QT_NO_XMLSTREAMREADER

class QXmlStreamReaderPrivate : public QXmlStreamReader_Table, public QXmlStreamPrivateTagStack{
    QXmlStreamReader *q_ptr;
    Q_DECLARE_PUBLIC(QXmlStreamReader)
public:
    QXmlStreamReaderPrivate(QXmlStreamReader *q);
    ~QXmlStreamReaderPrivate();
    void init();

    QByteArray rawReadBuffer;
    QByteArray dataBuffer;
    uchar firstByte;
    qint64 nbytesread;
    QString readBuffer;
    int readBufferPos;
    QXmlStreamSimpleStack<uint> putStack;
    struct Entity {
        Entity(const QString& str = QString())
            :value(str), external(false), unparsed(false), literal(false),
             hasBeenParsed(false), isCurrentlyReferenced(false){}
        static inline Entity createLiteral(const QString &entity)
            { Entity result(entity); result.literal = result.hasBeenParsed = true; return result; }
        QString value;
        uint external : 1;
        uint unparsed : 1;
        uint literal : 1;
        uint hasBeenParsed : 1;
        uint isCurrentlyReferenced : 1;
    };
    QHash<QString, Entity> entityHash;
    QHash<QString, Entity> parameterEntityHash;
    QXmlStreamSimpleStack<Entity *>entityReferenceStack;
    inline bool referenceEntity(Entity &entity) {
        if (entity.isCurrentlyReferenced) {
            raiseWellFormedError(QXmlStream::tr("Recursive entity detected."));
            return false;
        }
        entity.isCurrentlyReferenced = true;
        entityReferenceStack.push() = &entity;
        injectToken(ENTITY_DONE);
        return true;
    }


    QIODevice *device;
    bool deleteDevice;
#ifndef QT_NO_TEXTCODEC
    QTextCodec *codec;
    QTextDecoder *decoder;
#endif
    bool atEnd;

    /*!
      \sa setType()
     */
    QXmlStreamReader::TokenType type;
    QXmlStreamReader::Error error;
    QString errorString;

    qint64 lineNumber, lastLineStart, characterOffset;


    void write(const QString &);
    void write(const char *);


    QXmlStreamAttributes attributes;
    QStringRef namespaceForPrefix(const QStringRef &prefix);
    void resolveTag();
    void resolvePublicNamespaces();
    void resolveDtd();
    uint resolveCharRef(int symbolIndex);
    bool checkStartDocument();
    void startDocument();
    void parseError();
    void checkPublicLiteral(const QStringRef &publicId);

    bool scanDtd;
    QStringRef lastAttributeValue;
    bool lastAttributeIsCData;
    struct DtdAttribute {
        QStringRef tagName;
        QStringRef attributeQualifiedName;
        QStringRef attributePrefix;
        QStringRef attributeName;
        QStringRef defaultValue;
        bool isCDATA;
        bool isNamespaceAttribute;
    };
    QXmlStreamSimpleStack<DtdAttribute> dtdAttributes;
    struct NotationDeclaration {
        QStringRef name;
        QStringRef publicId;
        QStringRef systemId;
    };
    QXmlStreamSimpleStack<NotationDeclaration> notationDeclarations;
    QXmlStreamNotationDeclarations publicNotationDeclarations;
    QXmlStreamNamespaceDeclarations publicNamespaceDeclarations;

    struct EntityDeclaration {
        QStringRef name;
        QStringRef notationName;
        QStringRef publicId;
        QStringRef systemId;
        QStringRef value;
        bool parameter;
        bool external;
        inline void clear() {
            name.clear();
            notationName.clear();
            publicId.clear();
            systemId.clear();
            value.clear();
            parameter = external = false;
        }
    };
    QXmlStreamSimpleStack<EntityDeclaration> entityDeclarations;
    QXmlStreamEntityDeclarations publicEntityDeclarations;

    QStringRef text;

    QStringRef prefix, namespaceUri, qualifiedName, name;
    QStringRef processingInstructionTarget, processingInstructionData;
    QStringRef dtdName, dtdPublicId, dtdSystemId;
    QStringRef documentVersion, documentEncoding;
    uint isEmptyElement : 1;
    uint isWhitespace : 1;
    uint isCDATA : 1;
    uint standalone : 1;
    uint hasCheckedStartDocument : 1;
    uint normalizeLiterals : 1;
    uint hasSeenTag : 1;
    uint inParseEntity : 1;
    uint referenceToUnparsedEntityDetected : 1;
    uint referenceToParameterEntityDetected : 1;
    uint hasExternalDtdSubset : 1;
    uint lockEncoding : 1;
    uint namespaceProcessing : 1;

    int resumeReduction;
    void resume(int rule);

    inline bool entitiesMustBeDeclared() const {
        return (!inParseEntity
                && (standalone
                    || (!referenceToUnparsedEntityDetected
                        && !referenceToParameterEntityDetected // Errata 13 as of 2006-04-25
                        && !hasExternalDtdSubset)));
    }

    // qlalr parser
    int tos;
    int stack_size;
    struct Value {
        int pos;
        int len;
        int prefix;
        ushort c;
    };

    Value *sym_stack;
    int *state_stack;
    inline void reallocateStack();
    inline Value &sym(int index) const
    { return sym_stack[tos + index - 1]; }
    QString textBuffer;
    inline void clearTextBuffer() {
        if (!scanDtd) {
            textBuffer.resize(0);
            textBuffer.reserve(256);
        }
    }
    struct Attribute {
        Value key;
        Value value;
    };
    QXmlStreamSimpleStack<Attribute> attributeStack;

    inline QStringRef symString(int index) {
        const Value &symbol = sym(index);
        return QStringRef(&textBuffer, symbol.pos + symbol.prefix, symbol.len - symbol.prefix);
    }
    inline QStringRef symName(int index) {
        const Value &symbol = sym(index);
        return QStringRef(&textBuffer, symbol.pos, symbol.len);
    }
    inline QStringRef symString(int index, int offset) {
        const Value &symbol = sym(index);
        return QStringRef(&textBuffer, symbol.pos + symbol.prefix + offset, symbol.len - symbol.prefix -  offset);
    }
    inline QStringRef symPrefix(int index) {
        const Value &symbol = sym(index);
        if (symbol.prefix)
            return QStringRef(&textBuffer, symbol.pos, symbol.prefix - 1);
        return QStringRef();
    }
    inline QStringRef symString(const Value &symbol) {
        return QStringRef(&textBuffer, symbol.pos + symbol.prefix, symbol.len - symbol.prefix);
    }
    inline QStringRef symName(const Value &symbol) {
        return QStringRef(&textBuffer, symbol.pos, symbol.len);
    }
    inline QStringRef symPrefix(const Value &symbol) {
        if (symbol.prefix)
            return QStringRef(&textBuffer, symbol.pos, symbol.prefix - 1);
        return QStringRef();
    }

    inline void clearSym() { Value &val = sym(1); val.pos = textBuffer.size(); val.len = 0; }


    short token;
    ushort token_char;

    uint filterCarriageReturn();
    inline uint getChar();
    inline uint peekChar();
    inline void putChar(uint c) { putStack.push() = c; }
    inline void putChar(QChar c) { putStack.push() =  c.unicode(); }
    void putString(const QString &s, int from = 0);
    void putStringLiteral(const QString &s);
    void putReplacement(const QString &s);
    void putReplacementInAttributeValue(const QString &s);
    ushort getChar_helper();

    bool scanUntil(const char *str, short tokenToInject = -1);
    bool scanString(const char *str, short tokenToInject, bool requireSpace = true);
    inline void injectToken(ushort tokenToInject) {
        putChar(int(tokenToInject) << 16);
    }

    QString resolveUndeclaredEntity(const QString &name);
    void parseEntity(const QString &value);
    QXmlStreamReaderPrivate *entityParser;

    bool scanAfterLangleBang();
    bool scanPublicOrSystem();
    bool scanNData();
    bool scanAfterDefaultDecl();
    bool scanAttType();


    // scan optimization functions. Not strictly necessary but LALR is
    // not very well suited for scanning fast
    int fastScanLiteralContent();
    int fastScanSpace();
    int fastScanContentCharList();
    int fastScanName(int *prefix = 0);
    inline int fastScanNMTOKEN();


    bool parse();
    inline void consumeRule(int);

    void raiseError(QXmlStreamReader::Error error, const QString& message = QString());
    void raiseWellFormedError(const QString &message);

    QXmlStreamEntityResolver *entityResolver;

private:
    /*! \internal
       Never assign to variable type directly. Instead use this function.

       This prevents errors from being ignored.
     */
    inline void setType(const QXmlStreamReader::TokenType t)
    {
        if(type != QXmlStreamReader::Invalid)
            type = t;
    }
};

bool QXmlStreamReaderPrivate::parse()
{
    // cleanup currently reported token

    switch (type) {
    case QXmlStreamReader::StartElement:
        name.clear();
        prefix.clear();
	qualifiedName.clear();
        namespaceUri.clear();
        if (publicNamespaceDeclarations.size())
            publicNamespaceDeclarations.clear();
        if (attributes.size())
            attributes.resize(0);
        if (isEmptyElement) {
            setType(QXmlStreamReader::EndElement);
            Tag &tag = tagStack_pop();
            namespaceUri = tag.namespaceDeclaration.namespaceUri;
            name = tag.name;
	    qualifiedName = tag.qualifiedName;
            isEmptyElement = false;
            return true;
        }
        clearTextBuffer();
        break;
    case QXmlStreamReader::EndElement:
        name.clear();
        prefix.clear();
	qualifiedName.clear();
        namespaceUri.clear();
        clearTextBuffer();
        break;
    case QXmlStreamReader::DTD:
        publicNotationDeclarations.clear();
        publicEntityDeclarations.clear();
        dtdName.clear();
        dtdPublicId.clear();
        dtdSystemId.clear();
        // fall through
    case QXmlStreamReader::Comment:
    case QXmlStreamReader::Characters:
        isCDATA = isWhitespace = false;
        text.clear();
        clearTextBuffer();
        break;
    case QXmlStreamReader::EntityReference:
        text.clear();
        name.clear();
        clearTextBuffer();
        break;
    case QXmlStreamReader::ProcessingInstruction:
        processingInstructionTarget.clear();
        processingInstructionData.clear();
	clearTextBuffer();
        break;
    case QXmlStreamReader::NoToken:
    case QXmlStreamReader::Invalid:
        break;
    case QXmlStreamReader::StartDocument:
	lockEncoding = true;
        documentVersion.clear();
        documentEncoding.clear();
#ifndef QT_NO_TEXTCODEC
	if(decoder->hasFailure()) {
	    raiseWellFormedError(QXmlStream::tr("Encountered incorrectly encoded content."));
	    readBuffer.clear();
	    return false;
	}
#endif
        // fall through
    default:
        clearTextBuffer();
        ;
    }

    setType(QXmlStreamReader::NoToken);


    // the main parse loop
    int act, r;

    if (resumeReduction) {
        act = state_stack[tos-1];
        r = resumeReduction;
        resumeReduction = 0;
        goto ResumeReduction;
    }

    act = state_stack[tos];

    forever {
        if (token == -1 && - TERMINAL_COUNT != action_index[act]) {
            uint cu = getChar();
            token = NOTOKEN;
            token_char = cu;
            if (cu & 0xff0000) {
                token = cu >> 16;
            } else switch (token_char) {
            case 0xfffe:
            case 0xffff:
                token = ERROR;
                break;
            case '\r':
                token = SPACE;
                if (cu == '\r') {
                    if ((token_char = filterCarriageReturn())) {
                        ++lineNumber;
                        lastLineStart = characterOffset + readBufferPos;
                        break;
                    }
                } else {
                    break;
                }
                // fall through
            case '\0': {
                token = EOF_SYMBOL;
                if (!tagsDone && !inParseEntity) {
                    int a = t_action(act, token);
                    if (a < 0) {
                        raiseError(QXmlStreamReader::PrematureEndOfDocumentError);
                        return false;
                    }
                }

            } break;
            case '\n':
                ++lineNumber;
                lastLineStart = characterOffset + readBufferPos;
            case ' ':
            case '\t':
                token = SPACE;
                break;
            case '&':
                token = AMPERSAND;
                break;
            case '#':
                token = HASH;
                break;
            case '\'':
                token = QUOTE;
                break;
            case '\"':
                token = DBLQUOTE;
                break;
            case '<':
                token = LANGLE;
                break;
            case '>':
                token = RANGLE;
                break;
            case '[':
                token = LBRACK;
                break;
            case ']':
                token = RBRACK;
                break;
            case '(':
                token = LPAREN;
                break;
            case ')':
                token = RPAREN;
                break;
            case '|':
                token = PIPE;
                break;
            case '=':
                token = EQ;
                break;
            case '%':
                token = PERCENT;
                break;
            case '/':
                token = SLASH;
                break;
            case ':':
                token = COLON;
                break;
            case ';':
                token = SEMICOLON;
                break;
            case ',':
                token = COMMA;
                break;
            case '-':
                token = DASH;
                break;
            case '+':
                token = PLUS;
                break;
            case '*':
                token = STAR;
                break;
            case '.':
                token = DOT;
                break;
            case '?':
                token = QUESTIONMARK;
                break;
            case '!':
                token = BANG;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                token = DIGIT;
                break;
            default:
                if (cu < 0x20)
                    token = NOTOKEN;
                else
                    token = LETTER;
                break;
            }
        }

        act = t_action (act, token);
        if (act == ACCEPT_STATE) {
            // reset the parser in case someone resumes (process instructions can follow a valid document)
            tos = 0;
            state_stack[tos++] = 0;
            state_stack[tos] = 0;
            return true;
        } else if (act > 0) {
            if (++tos == stack_size)
                reallocateStack();

            Value &val = sym_stack[tos];
            val.c = token_char;
            val.pos = textBuffer.size();
            val.prefix = 0;
            val.len = 1;
            if (token_char)
                textBuffer += QChar(token_char);

            state_stack[tos] = act;
            token = -1;


        } else if (act < 0) {
            r = - act - 1;

#if defined (QLALR_DEBUG)
            int ridx = rule_index[r];
            printf ("%3d) %s ::=", r + 1, spell[rule_info[ridx]]);
            ++ridx;
            for (int i = ridx; i < ridx + rhs[r]; ++i) {
                int symbol = rule_info[i];
                if (const char *name = spell[symbol])
                    printf (" %s", name);
                else
                    printf (" #%d", symbol);
            }
            printf ("\n");
#endif

            tos -= rhs[r];
            act = state_stack[tos++];
        ResumeReduction:
            switch (r) {

        case 0:
            setType(QXmlStreamReader::EndDocument);
        break;

        case 1:
            if (type != QXmlStreamReader::Invalid) {
                if (hasSeenTag || inParseEntity) {
                    setType(QXmlStreamReader::EndDocument);
                } else {
                    raiseError(QXmlStreamReader::NotWellFormedError, QXmlStream::tr("Start tag expected."));
                    // reset the parser
                    tos = 0;
                    state_stack[tos++] = 0;
                    state_stack[tos] = 0;
                    return false;
                }
            }
        break;

        case 10:
            entityReferenceStack.pop()->isCurrentlyReferenced = false;
            clearSym();
        break;

        case 11:
            if (!scanString(spell[VERSION], VERSION, false) && atEnd) {
                resume(11);
                return false;
            }
        break;

        case 12:
            setType(QXmlStreamReader::StartDocument);
            documentVersion = symString(6);
            startDocument();
        break;

        case 13:
            hasExternalDtdSubset = true;
            dtdSystemId = symString(2);
        break;

        case 14:
            checkPublicLiteral(symString(2));
            dtdPublicId = symString(2);
            dtdSystemId = symString(4);
            hasExternalDtdSubset = true;
        break;

        case 16:
            if (!scanPublicOrSystem() && atEnd) {
                resume(16);
                return false;
            }
            dtdName = symString(3);
        break;

        case 17:
        case 18:
            dtdName = symString(3);
            // fall through

        case 19:
        case 20:
            setType(QXmlStreamReader::DTD);
            text = &textBuffer;
        break;

        case 21:
            scanDtd = true;
        break;

        case 22:
            scanDtd = false;
        break;

        case 36:
            if (!scanString(spell[EMPTY], EMPTY, false)
                && !scanString(spell[ANY], ANY, false)
                && atEnd) {
                resume(36);
                return false;
            }
        break;

        case 42:
            if (!scanString(spell[PCDATA], PCDATA, false) && atEnd) {
                resume(42);
                return false;
            }
        break;

        case 67: {
            lastAttributeIsCData = true;
        } break;

        case 77:
            if (!scanAfterDefaultDecl() && atEnd) {
                resume(77);
                return false;
            }
        break;

        case 82:
                sym(1) = sym(2);
                lastAttributeValue.clear();
                lastAttributeIsCData = false;
                if (!scanAttType() && atEnd) {
                    resume(82);
                    return false;
                }
        break;

        case 83: {
            DtdAttribute &dtdAttribute = dtdAttributes.push();
            dtdAttribute.tagName.clear();
            dtdAttribute.isCDATA = lastAttributeIsCData;
            dtdAttribute.attributePrefix = addToStringStorage(symPrefix(1));
            dtdAttribute.attributeName = addToStringStorage(symString(1));
            dtdAttribute.attributeQualifiedName = addToStringStorage(symName(1));
            dtdAttribute.isNamespaceAttribute = (dtdAttribute.attributePrefix == QLatin1String("xmlns")
                                                 || (dtdAttribute.attributePrefix.isEmpty()
                                                     && dtdAttribute.attributeName == QLatin1String("xmlns")));
            if (lastAttributeValue.isNull()) {
                dtdAttribute.defaultValue.clear();
            } else {
                if (dtdAttribute.isCDATA)
                    dtdAttribute.defaultValue = addToStringStorage(lastAttributeValue);
                else
                    dtdAttribute.defaultValue = addToStringStorage(lastAttributeValue.toString().simplified());

            }
        } break;

        case 87: {
            if (referenceToUnparsedEntityDetected && !standalone)
                break;
            int n = dtdAttributes.size();
            QStringRef tagName = addToStringStorage(symName(3));
            while (n--) {
                DtdAttribute &dtdAttribute = dtdAttributes[n];
                if (!dtdAttribute.tagName.isNull())
                    break;
                dtdAttribute.tagName = tagName;
                for (int i = 0; i < n; ++i) {
                    if ((dtdAttributes[i].tagName.isNull() || dtdAttributes[i].tagName == tagName)
                        && dtdAttributes[i].attributeQualifiedName == dtdAttribute.attributeQualifiedName) {
                        dtdAttribute.attributeQualifiedName.clear(); // redefined, delete it
                        break;
                    }
                }
            }
        } break;

        case 88: {
            if (!scanPublicOrSystem() && atEnd) {
                resume(88);
                return false;
            }
            EntityDeclaration &entityDeclaration = entityDeclarations.push();
            entityDeclaration.clear();
            entityDeclaration.name = symString(3);
        } break;

        case 89: {
            if (!scanPublicOrSystem() && atEnd) {
                resume(89);
                return false;
            }
            EntityDeclaration &entityDeclaration = entityDeclarations.push();
            entityDeclaration.clear();
            entityDeclaration.name = symString(5);
            entityDeclaration.parameter = true;
        } break;

        case 90: {
            if (!scanNData() && atEnd) {
                resume(90);
                return false;
            }
            EntityDeclaration &entityDeclaration = entityDeclarations.top();
            entityDeclaration.systemId = symString(3);
            entityDeclaration.external = true;
        } break;

        case 91: {
            if (!scanNData() && atEnd) {
                resume(91);
                return false;
            }
            EntityDeclaration &entityDeclaration = entityDeclarations.top();
            checkPublicLiteral((entityDeclaration.publicId = symString(3)));
            entityDeclaration.systemId = symString(5);
            entityDeclaration.external = true;
        } break;

        case 92: {
            EntityDeclaration &entityDeclaration = entityDeclarations.top();
            entityDeclaration.notationName = symString(3);
            if (entityDeclaration.parameter)
                raiseWellFormedError(QXmlStream::tr("NDATA in parameter entity declaration."));
        }
        //fall through

        case 93:
        case 94: {
            if (referenceToUnparsedEntityDetected && !standalone) {
                entityDeclarations.pop();
                break;
            }
            EntityDeclaration &entityDeclaration = entityDeclarations.top();
            if (!entityDeclaration.external)
                entityDeclaration.value = symString(2);
            QString entityName = entityDeclaration.name.toString();
            QHash<QString, Entity> &hash = entityDeclaration.parameter ? parameterEntityHash : entityHash;
            if (!hash.contains(entityName)) {
                Entity entity(entityDeclaration.value.toString());
                entity.unparsed = (!entityDeclaration.notationName.isNull());
                entity.external = entityDeclaration.external;
                hash.insert(entityName, entity);
            }
        } break;

        case 95: {
            setType(QXmlStreamReader::ProcessingInstruction);
            int pos = sym(4).pos + sym(4).len;
            processingInstructionTarget = symString(3);
            if (scanUntil("?>")) {
                processingInstructionData = QStringRef(&textBuffer, pos, textBuffer.size() - pos - 2);
                const QString piTarget(processingInstructionTarget.toString());
                if (!piTarget.compare(QLatin1String("xml"), Qt::CaseInsensitive)) {
                    raiseWellFormedError(QXmlStream::tr("XML declaration not at start of document."));
                }
                else if(!QXmlUtils::isNCName(piTarget))
                    raiseWellFormedError(QXmlStream::tr("%1 is an invalid processing instruction name.").arg(piTarget));
            } else if (type != QXmlStreamReader::Invalid){
                resume(95);
                return false;
            }
        } break;

        case 96:
            setType(QXmlStreamReader::ProcessingInstruction);
            processingInstructionTarget = symString(3);
            if (!processingInstructionTarget.toString().compare(QLatin1String("xml"), Qt::CaseInsensitive))
                raiseWellFormedError(QXmlStream::tr("Invalid processing instruction name."));
        break;

        case 97:
            if (!scanAfterLangleBang() && atEnd) {
                resume(97);
                return false;
            }
        break;

        case 98:
            if (!scanUntil("--")) {
                resume(98);
                return false;
            }
        break;

        case 99: {
            setType(QXmlStreamReader::Comment);
            int pos = sym(1).pos + 4;
            text = QStringRef(&textBuffer, pos, textBuffer.size() - pos - 3);
        } break;

        case 100: {
            setType(QXmlStreamReader::Characters);
            isCDATA = true;
            int pos = sym(2).pos;
            if (scanUntil("]]>", -1)) {
                text = QStringRef(&textBuffer, pos, textBuffer.size() - pos - 3);
            } else {
                resume(100);
                return false;
            }
        } break;

        case 101: {
            if (!scanPublicOrSystem() && atEnd) {
                resume(101);
                return false;
            }
            NotationDeclaration &notationDeclaration = notationDeclarations.push();
            notationDeclaration.name = symString(3);
        } break;

        case 102: {
            NotationDeclaration &notationDeclaration = notationDeclarations.top();
            notationDeclaration.systemId = symString(3);
            notationDeclaration.publicId.clear();
        } break;

        case 103: {
            NotationDeclaration &notationDeclaration = notationDeclarations.top();
            notationDeclaration.systemId.clear();
            checkPublicLiteral((notationDeclaration.publicId = symString(3)));
        } break;

        case 104: {
            NotationDeclaration &notationDeclaration = notationDeclarations.top();
            checkPublicLiteral((notationDeclaration.publicId = symString(3)));
            notationDeclaration.systemId = symString(5);
        } break;

        case 126:
            isWhitespace = true;
            // fall through

        case 127:
        case 128:
        case 129:
            setType(QXmlStreamReader::Characters);
            sym(1).len += fastScanContentCharList();
            if (atEnd && !inParseEntity) {
                resume(129);
                return false;
            }
            text = &textBuffer;
        break;

        case 130:
        case 131:
            clearSym();
        break;

        case 132:
        case 133:
            sym(1) = sym(2);
        break;

        case 134:
        case 135:
        case 136:
        case 137:
            sym(1).len += sym(2).len;
        break;

        case 163:
	    if (normalizeLiterals)
                textBuffer.data()[textBuffer.size()-1] = QLatin1Char(' ');
        break;

        case 164:
            sym(1).len += fastScanLiteralContent();
            if (atEnd) {
                resume(164);
                return false;
            }
        break;

        case 165: {
            if (!QXmlUtils::isPublicID(symString(1).toString())) {
                raiseWellFormedError(QXmlStream::tr("%1 is an invalid PUBLIC identifier.").arg(symString(1).toString()));
                resume(165);
                return false;
            }
        } break;

        case 166:
        case 167:
            clearSym();
        break;

        case 168:
        case 169:
	    sym(1) = sym(2);
        break;

        case 170:
        case 171:
        case 172:
        case 173:
            sym(1).len += sym(2).len;
        break;

        case 203:
        case 204:
            clearSym();
        break;

        case 205:
        case 206:
            sym(1) = sym(2);
            lastAttributeValue = symString(1);
        break;

        case 207:
        case 208:
        case 209:
        case 210:
            sym(1).len += sym(2).len;
        break;

        case 219: {
            QStringRef prefix = symPrefix(1);
            if (prefix.isEmpty() && symString(1) == QLatin1String("xmlns") && namespaceProcessing) {
                NamespaceDeclaration &namespaceDeclaration = namespaceDeclarations.push();
                namespaceDeclaration.prefix.clear();

                const QStringRef ns(symString(5));
                if(ns == QLatin1String("http://www.w3.org/2000/xmlns/") ||
                   ns == QLatin1String("http://www.w3.org/XML/1998/namespace"))
                    raiseWellFormedError(QXmlStream::tr("Illegal namespace declaration."));
                else
                    namespaceDeclaration.namespaceUri = addToStringStorage(ns);
            } else {
                Attribute &attribute = attributeStack.push();
                attribute.key = sym(1);
                attribute.value = sym(5);

                QStringRef attributeQualifiedName = symName(1);
                bool normalize = false;
                for (int a = 0; a < dtdAttributes.size(); ++a) {
                    DtdAttribute &dtdAttribute = dtdAttributes[a];
                    if (!dtdAttribute.isCDATA
                        && dtdAttribute.tagName == qualifiedName
                        && dtdAttribute.attributeQualifiedName == attributeQualifiedName
                        ) {
                        normalize = true;
                        break;
                    }
                }
                if (normalize) {
                    // normalize attribute value (simplify and trim)
                    int pos = textBuffer.size();
                    int n = 0;
                    bool wasSpace = true;
                    for (int i = 0; i < attribute.value.len; ++i) {
                        QChar c = textBuffer.at(attribute.value.pos + i);
                        if (c.unicode() == ' ') {
                            if (wasSpace)
                                continue;
                            wasSpace = true;
                        } else {
                            wasSpace = false;
                        }
                        textBuffer += textBuffer.at(attribute.value.pos + i);
                        ++n;
                    }
                    if (wasSpace)
                        while (n && textBuffer.at(pos + n - 1).unicode() == ' ')
                            --n;
                    attribute.value.pos = pos;
                    attribute.value.len = n;
                }
                if (prefix == QLatin1String("xmlns") && namespaceProcessing) {
                    NamespaceDeclaration &namespaceDeclaration = namespaceDeclarations.push();
                    QStringRef namespacePrefix = symString(attribute.key);
                    QStringRef namespaceUri = symString(attribute.value);
                    attributeStack.pop();
                    if ((namespacePrefix == QLatin1String("xml")
                         ^ namespaceUri == QLatin1String("http://www.w3.org/XML/1998/namespace"))
                        || namespaceUri == QLatin1String("http://www.w3.org/2000/xmlns/")
                        || namespaceUri.isEmpty()
                        || namespacePrefix == QLatin1String("xmlns"))
                        raiseWellFormedError(QXmlStream::tr("Illegal namespace declaration."));

                    namespaceDeclaration.prefix = addToStringStorage(namespacePrefix);
                    namespaceDeclaration.namespaceUri = addToStringStorage(namespaceUri);
                }
            }
        } break;

        case 225: {
            normalizeLiterals = true;
            Tag &tag = tagStack_push();
            prefix = tag.namespaceDeclaration.prefix  = addToStringStorage(symPrefix(2));
            name = tag.name = addToStringStorage(symString(2));
            qualifiedName = tag.qualifiedName = addToStringStorage(symName(2));
            if ((!prefix.isEmpty() && !QXmlUtils::isNCName(prefix)) || !QXmlUtils::isNCName(name))
                raiseWellFormedError(QXmlStream::tr("Invalid XML name."));
        } break;

        case 226:
            isEmptyElement = true;
        // fall through

        case 227:
            setType(QXmlStreamReader::StartElement);
            resolveTag();
            if (tagStack.size() == 1 && hasSeenTag && !inParseEntity)
                raiseWellFormedError(QXmlStream::tr("Extra content at end of document."));
            hasSeenTag = true;
        break;

        case 228: {
            setType(QXmlStreamReader::EndElement);
            Tag &tag = tagStack_pop();

            namespaceUri = tag.namespaceDeclaration.namespaceUri;
            name = tag.name;
            qualifiedName = tag.qualifiedName;
            if (qualifiedName != symName(3))
                raiseWellFormedError(QXmlStream::tr("Opening and ending tag mismatch."));
        } break;

        case 229: {
            sym(1).len += sym(2).len + 1;
            QString reference = symString(2).toString();
            if (entityHash.contains(reference)) {
                Entity &entity = entityHash[reference];
                if (entity.unparsed) {
                    raiseWellFormedError(QXmlStream::tr("Reference to unparsed entity '%1'.").arg(reference));
                } else {
                    if (!entity.hasBeenParsed) {
                        parseEntity(entity.value);
                        entity.hasBeenParsed = true;
                    }
                    if (entity.literal)
                        putStringLiteral(entity.value);
                    else if (referenceEntity(entity))
                        putReplacement(entity.value);
                    textBuffer.chop(2 + sym(2).len);
                    clearSym();
                }
                break;
            }

            if (entityResolver) {
                QString replacementText = resolveUndeclaredEntity(reference);
                if (!replacementText.isNull()) {
                    putReplacement(replacementText);
                    textBuffer.chop(2 + sym(2).len);
                    clearSym();
                    break;
                }
            }
            if (entitiesMustBeDeclared()) {
                raiseWellFormedError(QXmlStream::tr("Entity '%1' not declared.").arg(reference));
                break;
            }
            setType(QXmlStreamReader::EntityReference);
            name = symString(2);

        } break;

        case 230: {
            sym(1).len += sym(2).len + 1;
            QString reference = symString(2).toString();
            if (parameterEntityHash.contains(reference)) {
                referenceToParameterEntityDetected = true;
                Entity &entity = parameterEntityHash[reference];
                if (entity.unparsed || entity.external) {
                    referenceToUnparsedEntityDetected = true;
                } else {
                    if (referenceEntity(entity))
                        putString(entity.value);
                    textBuffer.chop(2 + sym(2).len);
                    clearSym();
                }
            } else if (entitiesMustBeDeclared()) {
                raiseWellFormedError(QXmlStream::tr("Entity '%1' not declared.").arg(symString(2).toString()));
            }
        } break;

        case 231:
            sym(1).len += sym(2).len + 1;
        break;

        case 232: {
            sym(1).len += sym(2).len + 1;
            QString reference = symString(2).toString();
            if (entityHash.contains(reference)) {
                Entity &entity = entityHash[reference];
                if (entity.unparsed || entity.value.isNull()) {
                    raiseWellFormedError(QXmlStream::tr("Reference to external entity '%1' in attribute value.").arg(reference));
                    break;
                }
                if (!entity.hasBeenParsed) {
                    parseEntity(entity.value);
                    entity.hasBeenParsed = true;
                }
                if (entity.literal)
                    putStringLiteral(entity.value);
                else if (referenceEntity(entity))
                    putReplacementInAttributeValue(entity.value);
                textBuffer.chop(2 + sym(2).len);
                clearSym();
                break;
            }

            if (entityResolver) {
                QString replacementText = resolveUndeclaredEntity(reference);
                if (!replacementText.isNull()) {
                    putReplacement(replacementText);
                    textBuffer.chop(2 + sym(2).len);
                    clearSym();
                    break;
                }
            }
            if (entitiesMustBeDeclared()) {
                raiseWellFormedError(QXmlStream::tr("Entity '%1' not declared.").arg(reference));
            }
        } break;

        case 233: {
            if (uint s = resolveCharRef(3)) {
                if (s >= 0xffff)
                    putStringLiteral(QString::fromUcs4(&s, 1));
                else
                    putChar((LETTER << 16) | s);

                textBuffer.chop(3 + sym(3).len);
                clearSym();
            } else {
                raiseWellFormedError(QXmlStream::tr("Invalid character reference."));
            }
        } break;

        case 236:
        case 237:
            sym(1).len += sym(2).len;
        break;

        case 250:
            sym(1).len += fastScanSpace();
            if (atEnd) {
                resume(250);
                return false;
            }
        break;

        case 253: {
            sym(1).len += fastScanName(&sym(1).prefix);
            if (atEnd) {
                resume(253);
                return false;
            }
        } break;

        case 254:
            sym(1).len += fastScanName();
            if (atEnd) {
                resume(254);
                return false;
            }
        break;

        case 255:
        case 256:
        case 257:
        case 258:
        case 259:
            sym(1).len += fastScanNMTOKEN();
            if (atEnd) {
                resume(259);
                return false;
            }

        break;

    default:
        ;
    } // switch
            act = state_stack[tos] = nt_action (act, lhs[r] - TERMINAL_COUNT);
            if (type != QXmlStreamReader::NoToken)
                return true;
        } else {
            parseError();
            break;
        }
    }
    return false;
}
#endif // QT_NO_XMLSTREAMREADER

#endif // QT_NO_XMLSTREAM
#endif // QXMLSTREAM_P_H

