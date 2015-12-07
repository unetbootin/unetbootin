/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
** Copyright (C) 2001-2004 Roberto Raggi
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TOKENS_H
#define TOKENS_H

#include <qglobal.h>

QT_BEGIN_NAMESPACE

enum Type
{
    Token_eof = 0,
    Token_newline = 10,
    Token_identifier = 1000,
    Token_number_literal,
    Token_char_literal,
    Token_string_literal,
    Token_whitespaces,
    Token_comment,
    Token_preproc,
    Token_line_comment,
    Token_multiline_comment,

    Token_assign = 2000,
    Token_ptrmem,
    Token_ellipsis,
    Token_scope,
    Token_shift,
    Token_eq,
    Token_leq,
    Token_geq,
    Token_incr,
    Token_decr,
    Token_arrow,

    Token_concat,

    Token___typeof,

    Token_K_DCOP,
    Token_k_dcop,
    Token_k_dcop_signals,

    Token_Q_OBJECT,
    Token_signals,
    Token_slots,
    Token_emit,

    Token_and,
    Token_and_eq,
    Token_asm,
    Token_auto,
    Token_bitand,
    Token_bitor,
    Token_bool,
    Token_break,
    Token_case,
    Token_catch,
    Token_char,
    Token_class,
    Token_compl,
    Token_const,
    Token_const_cast,
    Token_continue,
    Token_default,
    Token_delete,
    Token_do,
    Token_double,
    Token_dynamic_cast,
    Token_else,
    Token_enum,
    Token_explicit,
    Token_export,
    Token_extern,
    Token_false,
    Token_float,
    Token_for,
    Token_friend,
    Token_goto,
    Token_if,
    Token_inline,
    Token_int,
    Token_long,
    Token_mutable,
    Token_namespace,
    Token_new,
    Token_not,
    Token_not_eq,
    Token_operator,
    Token_or,
    Token_or_eq,
    Token_private,
    Token_protected,
    Token_public,
    Token_register,
    Token_reinterpret_cast,
    Token_return,
    Token_short,
    Token_signed,
    Token_sizeof,
    Token_static,
    Token_static_cast,
    Token_struct,
    Token_switch,
    Token_template,
    Token_this,
    Token_throw,
    Token_true,
    Token_try,
    Token_typedef,
    Token_typeid,
    Token_typename,
    Token_union,
    Token_unsigned,
    Token_using,
    Token_virtual,
    Token_void,
    Token_volatile,
    Token_wchar_t,
    Token_while,
    Token_xor,
    Token_xor_eq,

    Token_left_shift,
    Token_right_shift,

    // preprecessor
    Token_directive_identifier,
    Token_directive_if,
    Token_directive_elif,
    Token_directive_else,
    Token_directive_undef,
    Token_directive_endif,
    Token_directive_ifdef,
    Token_directive_ifndef,
    Token_directive_define,
    Token_directive_include,
    Token_directive_line,
    Token_directive_error,
    Token_directive_pragma,
    Token_defined
};

QT_END_NAMESPACE

#endif
