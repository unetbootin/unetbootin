/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "gtksymbols_p.h"

// This file is responsible for resolving all GTK functions we use
// dynamically. This is done to avoid link-time dependancy on GTK
// as well as crashes occurring due to usage of the GTK_QT engines
//
// Additionally we create a map of common GTK widgets that we can pass
// to the GTK theme engine as many engines resort to querying the
// actual widget pointers for details that are not covered by the
// state flags

#include <QtCore/qglobal.h>
#if !defined(QT_NO_STYLE_GTK)

#include <QtCore/QEvent>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QHash>

#include <private/qapplication_p.h>

#include <QtGui/QMenu>
#include <QtGui/QStyle>
#include <QtGui/QApplication>
#include <QtGui/QPixmapCache>
#include <QtGui/QStatusBar>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QX11Info>

#include <X11/Xlib.h>

QT_BEGIN_NAMESPACE

static bool displayDepth  =  -1;

typedef QHash<QString, GtkWidget*> WidgetMap;
Q_GLOBAL_STATIC(WidgetMap, gtkWidgetMap)

Ptr_gtk_container_forall QGtk::gtk_container_forall = 0;
Ptr_gtk_init QGtk::gtk_init = 0;
Ptr_gtk_style_attach QGtk::gtk_style_attach = 0;
Ptr_gtk_window_new QGtk::gtk_window_new = 0;
Ptr_gtk_widget_destroy QGtk::gtk_widget_destroy = 0;
Ptr_gtk_widget_realize QGtk::gtk_widget_realize = 0;
Ptr_gtk_widget_set_default_direction QGtk::gtk_widget_set_default_direction = 0;
Ptr_gtk_widget_modify_color QGtk::gtk_widget_modify_fg = 0;
Ptr_gtk_widget_modify_color QGtk::gtk_widget_modify_bg = 0;
Ptr_gtk_arrow_new QGtk::gtk_arrow_new = 0;
Ptr_gtk_menu_item_new QGtk::gtk_menu_item_new = 0;
Ptr_gtk_check_menu_item_new QGtk::gtk_check_menu_item_new = 0;
Ptr_gtk_menu_bar_new QGtk::gtk_menu_bar_new = 0;
Ptr_gtk_menu_new QGtk::gtk_menu_new = 0;
Ptr_gtk_button_new QGtk::gtk_button_new = 0;
Ptr_gtk_hbutton_box_new QGtk::gtk_hbutton_box_new = 0;
Ptr_gtk_check_button_new QGtk::gtk_check_button_new = 0;
Ptr_gtk_radio_button_new QGtk::gtk_radio_button_new = 0;
Ptr_gtk_spin_button_new QGtk::gtk_spin_button_new = 0;
Ptr_gtk_frame_new QGtk::gtk_frame_new = 0;
Ptr_gtk_expander_new QGtk::gtk_expander_new = 0;
Ptr_gtk_statusbar_new QGtk::gtk_statusbar_new = 0;
Ptr_gtk_entry_new QGtk::gtk_entry_new = 0;
Ptr_gtk_hscale_new QGtk::gtk_hscale_new = 0;
Ptr_gtk_vscale_new QGtk::gtk_vscale_new = 0;
Ptr_gtk_hscrollbar_new QGtk::gtk_hscrollbar_new = 0;
Ptr_gtk_vscrollbar_new QGtk::gtk_vscrollbar_new = 0;
Ptr_gtk_scrolled_window_new QGtk::gtk_scrolled_window_new = 0;
Ptr_gtk_notebook_new QGtk::gtk_notebook_new = 0;
Ptr_gtk_toolbar_new QGtk::gtk_toolbar_new = 0;
Ptr_gtk_toolbar_insert QGtk::gtk_toolbar_insert = 0;
Ptr_gtk_separator_tool_item_new QGtk::gtk_separator_tool_item_new = 0;
Ptr_gtk_tree_view_new QGtk::gtk_tree_view_new = 0;
Ptr_gtk_combo_box_new QGtk::gtk_combo_box_new = 0;
Ptr_gtk_combo_box_entry_new QGtk::gtk_combo_box_entry_new = 0;
Ptr_gtk_progress_bar_new QGtk::gtk_progress_bar_new = 0;
Ptr_gtk_container_add QGtk::gtk_container_add = 0;
Ptr_gtk_menu_shell_append QGtk::gtk_menu_shell_append = 0;
Ptr_gtk_progress_set_adjustment QGtk::gtk_progress_set_adjustment = 0;
Ptr_gtk_range_set_adjustment QGtk::gtk_range_set_adjustment = 0;
Ptr_gtk_range_set_inverted QGtk::gtk_range_set_inverted = 0;
Ptr_gtk_icon_factory_lookup_default QGtk::gtk_icon_factory_lookup_default = 0;
Ptr_gtk_widget_style_get QGtk::gtk_widget_style_get = 0;
Ptr_gtk_icon_set_render_icon QGtk::gtk_icon_set_render_icon = 0;
Ptr_gtk_fixed_new QGtk::gtk_fixed_new = 0;
Ptr_gtk_tree_view_column_new QGtk::gtk_tree_view_column_new = 0;
Ptr_gtk_tree_view_get_column QGtk::gtk_tree_view_get_column = 0;
Ptr_gtk_tree_view_append_column QGtk::gtk_tree_view_append_column = 0;
Ptr_gtk_paint_check QGtk::gtk_paint_check = 0;
Ptr_gtk_paint_box QGtk::gtk_paint_box = 0;
Ptr_gtk_paint_box_gap QGtk::gtk_paint_box_gap = 0;
Ptr_gtk_paint_flat_box QGtk::gtk_paint_flat_box = 0;
Ptr_gtk_paint_option QGtk::gtk_paint_option = 0;
Ptr_gtk_paint_extension QGtk::gtk_paint_extension = 0;
Ptr_gtk_paint_slider QGtk::gtk_paint_slider = 0;
Ptr_gtk_paint_shadow QGtk::gtk_paint_shadow = 0;
Ptr_gtk_paint_resize_grip QGtk::gtk_paint_resize_grip = 0;
Ptr_gtk_paint_focus QGtk::gtk_paint_focus = 0;
Ptr_gtk_paint_arrow QGtk::gtk_paint_arrow = 0;
Ptr_gtk_paint_handle QGtk::gtk_paint_handle = 0;
Ptr_gtk_paint_expander QGtk::gtk_paint_expander = 0;
Ptr_gtk_adjustment_new QGtk::gtk_adjustment_new = 0;
Ptr_gtk_paint_hline QGtk::gtk_paint_hline = 0;
Ptr_gtk_paint_vline QGtk::gtk_paint_vline = 0;
Ptr_gtk_menu_item_set_submenu QGtk::gtk_menu_item_set_submenu = 0;
Ptr_gtk_settings_get_default QGtk::gtk_settings_get_default = 0;
Ptr_gtk_separator_menu_item_new QGtk::gtk_separator_menu_item_new = 0;
Ptr_gtk_widget_size_allocate QGtk::gtk_widget_size_allocate = 0;
Ptr_gtk_widget_set_direction QGtk::gtk_widget_set_direction = 0;
Ptr_gtk_widget_path QGtk::gtk_widget_path = 0;
Ptr_gtk_container_get_type QGtk::gtk_container_get_type = 0;
Ptr_gtk_window_get_type QGtk::gtk_window_get_type = 0;
Ptr_gtk_widget_get_type QGtk::gtk_widget_get_type = 0;
Ptr_gtk_rc_get_style_by_paths QGtk::gtk_rc_get_style_by_paths = 0;
Ptr_gtk_check_version QGtk::gtk_check_version = 0;

Ptr_pango_font_description_get_size QGtk::pango_font_description_get_size = 0;
Ptr_pango_font_description_get_weight QGtk::pango_font_description_get_weight = 0;
Ptr_pango_font_description_get_family QGtk::pango_font_description_get_family = 0;
Ptr_pango_font_description_get_style QGtk::pango_font_description_get_style = 0;

Ptr_gtk_file_filter_new QGtk::gtk_file_filter_new = 0;
Ptr_gtk_file_filter_set_name QGtk::gtk_file_filter_set_name = 0;
Ptr_gtk_file_filter_add_pattern QGtk::gtk_file_filter_add_pattern = 0;
Ptr_gtk_file_chooser_add_filter QGtk::gtk_file_chooser_add_filter = 0;
Ptr_gtk_file_chooser_set_filter QGtk::gtk_file_chooser_set_filter = 0;
Ptr_gtk_file_chooser_dialog_new QGtk::gtk_file_chooser_dialog_new = 0;
Ptr_gtk_file_chooser_set_current_folder QGtk::gtk_file_chooser_set_current_folder = 0;
Ptr_gtk_file_chooser_get_filename QGtk::gtk_file_chooser_get_filename = 0;
Ptr_gtk_file_chooser_get_filenames QGtk::gtk_file_chooser_get_filenames = 0;
Ptr_gtk_file_chooser_set_current_name QGtk::gtk_file_chooser_set_current_name = 0;
Ptr_gtk_dialog_run QGtk::gtk_dialog_run = 0;
Ptr_gtk_file_chooser_set_filename QGtk::gtk_file_chooser_set_filename = 0;

Ptr_gdk_pixbuf_get_pixels QGtk::gdk_pixbuf_get_pixels = 0;
Ptr_gdk_pixbuf_get_width QGtk::gdk_pixbuf_get_width = 0;
Ptr_gdk_pixbuf_get_height QGtk::gdk_pixbuf_get_height = 0;
Ptr_gdk_pixmap_new QGtk::gdk_pixmap_new = 0;
Ptr_gdk_pixbuf_new QGtk::gdk_pixbuf_new = 0;
Ptr_gdk_pixbuf_get_from_drawable QGtk::gdk_pixbuf_get_from_drawable = 0;
Ptr_gdk_draw_rectangle QGtk::gdk_draw_rectangle = 0;
Ptr_gdk_pixbuf_unref QGtk::gdk_pixbuf_unref = 0;
Ptr_gdk_drawable_unref QGtk::gdk_drawable_unref = 0;
Ptr_gdk_drawable_get_depth QGtk::gdk_drawable_get_depth = 0;
Ptr_gdk_color_free QGtk::gdk_color_free = 0;
Ptr_gdk_x11_window_set_user_time QGtk::gdk_x11_window_set_user_time = 0;
Ptr_gdk_x11_drawable_get_xid QGtk::gdk_x11_drawable_get_xid = 0;
Ptr_gdk_x11_drawable_get_xdisplay QGtk::gdk_x11_drawable_get_xdisplay = 0;

Ptr_gconf_client_get_default QGtk::gconf_client_get_default = 0;
Ptr_gconf_client_get_string QGtk::gconf_client_get_string = 0;

static QString classPath(GtkWidget *widget)
{
    char* class_path;
    QGtk::gtk_widget_path (widget, NULL, &class_path, NULL);
    QString path = QLS(class_path);
    g_free(class_path);

    // Remove the prefixes
    path.remove(QLS("GtkWindow."));
    path.remove(QLS("GtkFixed."));
    return path;
}

static void resolveGtk()
{
    const QString GTK_PATH(QLS("gtk-x11-2.0"));
    QGtk::gtk_init = (Ptr_gtk_init)QLibrary::resolve(GTK_PATH, 0, "gtk_init");
    QGtk::gtk_window_new = (Ptr_gtk_window_new)QLibrary::resolve(GTK_PATH, 0, "gtk_window_new");
    QGtk::gtk_style_attach = (Ptr_gtk_style_attach)QLibrary::resolve(GTK_PATH, 0, "gtk_style_attach");
    QGtk::gtk_widget_destroy = (Ptr_gtk_widget_destroy)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_destroy");
    QGtk::gtk_widget_realize = (Ptr_gtk_widget_realize)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_realize");

    QGtk::gtk_file_chooser_set_current_folder = (Ptr_gtk_file_chooser_set_current_folder)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_set_current_folder");
    QGtk::gtk_file_filter_new = (Ptr_gtk_file_filter_new)QLibrary::resolve(GTK_PATH, 0, "gtk_file_filter_new");
    QGtk::gtk_file_filter_set_name = (Ptr_gtk_file_filter_set_name)QLibrary::resolve(GTK_PATH, 0, "gtk_file_filter_set_name");
    QGtk::gtk_file_filter_add_pattern = (Ptr_gtk_file_filter_add_pattern)QLibrary::resolve(GTK_PATH, 0, "gtk_file_filter_add_pattern");
    QGtk::gtk_file_chooser_add_filter = (Ptr_gtk_file_chooser_add_filter)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_add_filter");
    QGtk::gtk_file_chooser_set_filter = (Ptr_gtk_file_chooser_set_filter)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_set_filter");
    QGtk::gtk_file_chooser_dialog_new = (Ptr_gtk_file_chooser_dialog_new)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_dialog_new");
    QGtk::gtk_file_chooser_set_current_folder = (Ptr_gtk_file_chooser_set_current_folder)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_set_current_folder");
    QGtk::gtk_file_chooser_get_filename = (Ptr_gtk_file_chooser_get_filename)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_get_filename");
    QGtk::gtk_file_chooser_get_filenames = (Ptr_gtk_file_chooser_get_filenames)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_get_filenames");
    QGtk::gtk_file_chooser_set_current_name = (Ptr_gtk_file_chooser_set_current_name)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_set_current_name");
    QGtk::gtk_dialog_run = (Ptr_gtk_dialog_run)QLibrary::resolve(GTK_PATH, 0, "gtk_dialog_run");
    QGtk::gtk_file_chooser_set_filename = (Ptr_gtk_file_chooser_set_filename)QLibrary::resolve(GTK_PATH, 0, "gtk_file_chooser_set_filename");

    QGtk::gdk_pixbuf_get_pixels = (Ptr_gdk_pixbuf_get_pixels)QLibrary::resolve(GTK_PATH, 0, "gdk_pixbuf_get_pixels");
    QGtk::gdk_pixbuf_get_width = (Ptr_gdk_pixbuf_get_width)QLibrary::resolve(GTK_PATH, 0, "gdk_pixbuf_get_width");
    QGtk::gdk_pixbuf_get_height = (Ptr_gdk_pixbuf_get_height)QLibrary::resolve(GTK_PATH, 0, "gdk_pixbuf_get_height");
    QGtk::gdk_pixmap_new = (Ptr_gdk_pixmap_new)QLibrary::resolve(GTK_PATH, 0, "gdk_pixmap_new");
    QGtk::gdk_pixbuf_new = (Ptr_gdk_pixbuf_new)QLibrary::resolve(GTK_PATH, 0, "gdk_pixbuf_new");
    QGtk::gdk_pixbuf_get_from_drawable = (Ptr_gdk_pixbuf_get_from_drawable)QLibrary::resolve(GTK_PATH, 0, "gdk_pixbuf_get_from_drawable");
    QGtk::gdk_draw_rectangle = (Ptr_gdk_draw_rectangle)QLibrary::resolve(GTK_PATH, 0, "gdk_draw_rectangle");
    QGtk::gdk_pixbuf_unref = (Ptr_gdk_pixbuf_unref)QLibrary::resolve(GTK_PATH, 0, "gdk_pixbuf_unref");
    QGtk::gdk_drawable_unref = (Ptr_gdk_drawable_unref)QLibrary::resolve(GTK_PATH, 0, "gdk_drawable_unref");
    QGtk::gdk_drawable_get_depth = (Ptr_gdk_drawable_get_depth)QLibrary::resolve(GTK_PATH, 0, "gdk_drawable_get_depth");
    QGtk::gdk_color_free = (Ptr_gdk_color_free)QLibrary::resolve(GTK_PATH, 0, "gdk_color_free");
    QGtk::gdk_x11_window_set_user_time = (Ptr_gdk_x11_window_set_user_time)QLibrary::resolve(GTK_PATH, 0, "gdk_x11_window_set_user_time");
    QGtk::gdk_x11_drawable_get_xid = (Ptr_gdk_x11_drawable_get_xid)QLibrary::resolve(GTK_PATH, 0, "gdk_x11_drawable_get_xid");
    QGtk::gdk_x11_drawable_get_xdisplay = (Ptr_gdk_x11_drawable_get_xdisplay)QLibrary::resolve(GTK_PATH, 0, "gdk_x11_drawable_get_xdisplay");

    QGtk::gtk_widget_set_default_direction = (Ptr_gtk_widget_set_default_direction)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_set_default_direction");
    QGtk::gtk_widget_modify_fg = (Ptr_gtk_widget_modify_color)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_modify_fg");
    QGtk::gtk_widget_modify_bg = (Ptr_gtk_widget_modify_color)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_modify_bg");
    QGtk::gtk_arrow_new = (Ptr_gtk_arrow_new)QLibrary::resolve(GTK_PATH, 0, "gtk_arrow_new");
    QGtk::gtk_menu_item_new = (Ptr_gtk_menu_item_new)QLibrary::resolve(GTK_PATH, 0, "gtk_menu_item_new");
    QGtk::gtk_check_menu_item_new = (Ptr_gtk_check_menu_item_new)QLibrary::resolve(GTK_PATH, 0, "gtk_check_menu_item_new");
    QGtk::gtk_menu_bar_new = (Ptr_gtk_menu_bar_new)QLibrary::resolve(GTK_PATH, 0, "gtk_menu_bar_new");
    QGtk::gtk_menu_new = (Ptr_gtk_menu_new)QLibrary::resolve(GTK_PATH, 0, "gtk_menu_new");
    QGtk::gtk_toolbar_new = (Ptr_gtk_toolbar_new)QLibrary::resolve(GTK_PATH, 0, "gtk_toolbar_new");
    QGtk::gtk_separator_tool_item_new = (Ptr_gtk_separator_tool_item_new)QLibrary::resolve(GTK_PATH, 0, "gtk_separator_tool_item_new");
    QGtk::gtk_toolbar_insert = (Ptr_gtk_toolbar_insert)QLibrary::resolve(GTK_PATH, 0, "gtk_toolbar_insert");
    QGtk::gtk_button_new = (Ptr_gtk_button_new)QLibrary::resolve(GTK_PATH, 0, "gtk_button_new");
    QGtk::gtk_hbutton_box_new = (Ptr_gtk_hbutton_box_new)QLibrary::resolve(GTK_PATH, 0, "gtk_hbutton_box_new");
    QGtk::gtk_check_button_new = (Ptr_gtk_check_button_new)QLibrary::resolve(GTK_PATH, 0, "gtk_check_button_new");
    QGtk::gtk_radio_button_new = (Ptr_gtk_radio_button_new)QLibrary::resolve(GTK_PATH, 0, "gtk_radio_button_new");
    QGtk::gtk_notebook_new = (Ptr_gtk_notebook_new)QLibrary::resolve(GTK_PATH, 0, "gtk_notebook_new");
    QGtk::gtk_progress_bar_new = (Ptr_gtk_progress_bar_new)QLibrary::resolve(GTK_PATH, 0, "gtk_progress_bar_new");
    QGtk::gtk_spin_button_new = (Ptr_gtk_spin_button_new)QLibrary::resolve(GTK_PATH, 0, "gtk_spin_button_new");
    QGtk::gtk_hscale_new = (Ptr_gtk_hscale_new)QLibrary::resolve(GTK_PATH, 0, "gtk_hscale_new");
    QGtk::gtk_vscale_new = (Ptr_gtk_vscale_new)QLibrary::resolve(GTK_PATH, 0, "gtk_vscale_new");
    QGtk::gtk_hscrollbar_new = (Ptr_gtk_hscrollbar_new)QLibrary::resolve(GTK_PATH, 0, "gtk_hscrollbar_new");
    QGtk::gtk_vscrollbar_new = (Ptr_gtk_vscrollbar_new)QLibrary::resolve(GTK_PATH, 0, "gtk_vscrollbar_new");
    QGtk::gtk_scrolled_window_new = (Ptr_gtk_scrolled_window_new)QLibrary::resolve(GTK_PATH, 0, "gtk_scrolled_window_new");
    QGtk::gtk_menu_shell_append = (Ptr_gtk_menu_shell_append)QLibrary::resolve(GTK_PATH, 0, "gtk_menu_shell_append");
    QGtk::gtk_entry_new = (Ptr_gtk_entry_new)QLibrary::resolve(GTK_PATH, 0, "gtk_entry_new");
    QGtk::gtk_tree_view_new = (Ptr_gtk_tree_view_new)QLibrary::resolve(GTK_PATH, 0, "gtk_tree_view_new");
    QGtk::gtk_combo_box_new = (Ptr_gtk_combo_box_new)QLibrary::resolve(GTK_PATH, 0, "gtk_combo_box_new");
    QGtk::gtk_progress_set_adjustment = (Ptr_gtk_progress_set_adjustment)QLibrary::resolve(GTK_PATH, 0, "gtk_progress_set_adjustment");
    QGtk::gtk_range_set_adjustment = (Ptr_gtk_range_set_adjustment)QLibrary::resolve(GTK_PATH, 0, "gtk_range_set_adjustment");
    QGtk::gtk_range_set_inverted = (Ptr_gtk_range_set_inverted)QLibrary::resolve(GTK_PATH, 0, "gtk_range_set_inverted");
    QGtk::gtk_container_add = (Ptr_gtk_container_add)QLibrary::resolve(GTK_PATH, 0, "gtk_container_add");
    QGtk::gtk_icon_factory_lookup_default = (Ptr_gtk_icon_factory_lookup_default)QLibrary::resolve(GTK_PATH, 0, "gtk_icon_factory_lookup_default");
    QGtk::gtk_widget_style_get = (Ptr_gtk_widget_style_get)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_style_get");
    QGtk::gtk_icon_set_render_icon = (Ptr_gtk_icon_set_render_icon)QLibrary::resolve(GTK_PATH, 0, "gtk_icon_set_render_icon");
    QGtk::gtk_fixed_new = (Ptr_gtk_fixed_new)QLibrary::resolve(GTK_PATH, 0, "gtk_fixed_new");
    QGtk::gtk_tree_view_column_new = (Ptr_gtk_tree_view_column_new)QLibrary::resolve(GTK_PATH, 0, "gtk_tree_view_column_new");
    QGtk::gtk_tree_view_append_column= (Ptr_gtk_tree_view_append_column )QLibrary::resolve(GTK_PATH, 0, "gtk_tree_view_append_column");
    QGtk::gtk_tree_view_get_column = (Ptr_gtk_tree_view_get_column )QLibrary::resolve(GTK_PATH, 0, "gtk_tree_view_get_column");
    QGtk::gtk_paint_check = (Ptr_gtk_paint_check)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_check");
    QGtk::gtk_paint_box = (Ptr_gtk_paint_box)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_box");
    QGtk::gtk_paint_flat_box = (Ptr_gtk_paint_flat_box)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_flat_box");
    QGtk::gtk_paint_check = (Ptr_gtk_paint_check)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_check");
    QGtk::gtk_paint_box = (Ptr_gtk_paint_box)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_box");
    QGtk::gtk_paint_resize_grip = (Ptr_gtk_paint_resize_grip)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_resize_grip");
    QGtk::gtk_paint_focus = (Ptr_gtk_paint_focus)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_focus");
    QGtk::gtk_paint_shadow = (Ptr_gtk_paint_shadow)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_shadow");
    QGtk::gtk_paint_slider = (Ptr_gtk_paint_slider)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_slider");
    QGtk::gtk_paint_expander = (Ptr_gtk_paint_expander)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_expander");
    QGtk::gtk_paint_handle = (Ptr_gtk_paint_handle)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_handle");
    QGtk::gtk_paint_option = (Ptr_gtk_paint_option)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_option");
    QGtk::gtk_paint_arrow = (Ptr_gtk_paint_arrow)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_arrow");
    QGtk::gtk_paint_box_gap = (Ptr_gtk_paint_box_gap)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_box_gap");
    QGtk::gtk_paint_extension = (Ptr_gtk_paint_extension)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_extension");
    QGtk::gtk_paint_hline = (Ptr_gtk_paint_hline)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_hline");
    QGtk::gtk_paint_vline = (Ptr_gtk_paint_vline)QLibrary::resolve(GTK_PATH, 0, "gtk_paint_vline");
    QGtk::gtk_adjustment_new = (Ptr_gtk_adjustment_new)QLibrary::resolve(GTK_PATH, 0, "gtk_adjustment_new");
    QGtk::gtk_menu_item_set_submenu = (Ptr_gtk_menu_item_set_submenu)QLibrary::resolve(GTK_PATH, 0, "gtk_menu_item_set_submenu");
    QGtk::gtk_settings_get_default = (Ptr_gtk_settings_get_default)QLibrary::resolve(GTK_PATH, 0, "gtk_settings_get_default");
    QGtk::gtk_separator_menu_item_new = (Ptr_gtk_separator_menu_item_new)QLibrary::resolve(GTK_PATH, 0, "gtk_separator_menu_item_new");
    QGtk::gtk_frame_new = (Ptr_gtk_frame_new)QLibrary::resolve(GTK_PATH, 0, "gtk_frame_new");
    QGtk::gtk_expander_new = (Ptr_gtk_expander_new)QLibrary::resolve(GTK_PATH, 0, "gtk_expander_new");
    QGtk::gtk_statusbar_new = (Ptr_gtk_statusbar_new)QLibrary::resolve(GTK_PATH, 0, "gtk_statusbar_new");
    QGtk::gtk_combo_box_entry_new = (Ptr_gtk_combo_box_entry_new)QLibrary::resolve(GTK_PATH, 0, "gtk_combo_box_entry_new");
    QGtk::gtk_container_forall = (Ptr_gtk_container_forall)QLibrary::resolve(GTK_PATH, 0, "gtk_container_forall");
    QGtk::gtk_widget_size_allocate =(Ptr_gtk_widget_size_allocate)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_size_allocate");
    QGtk::gtk_widget_set_direction =(Ptr_gtk_widget_set_direction)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_set_direction");
    QGtk::gtk_widget_path =(Ptr_gtk_widget_path)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_path");
    QGtk::gtk_container_get_type =(Ptr_gtk_container_get_type)QLibrary::resolve(GTK_PATH, 0, "gtk_container_get_type");
    QGtk::gtk_window_get_type =(Ptr_gtk_window_get_type)QLibrary::resolve(GTK_PATH, 0, "gtk_window_get_type");
    QGtk::gtk_widget_get_type =(Ptr_gtk_widget_get_type)QLibrary::resolve(GTK_PATH, 0, "gtk_widget_get_type");
    QGtk::gtk_rc_get_style_by_paths =(Ptr_gtk_rc_get_style_by_paths)QLibrary::resolve(GTK_PATH, 0, "gtk_rc_get_style_by_paths");
    QGtk::gtk_check_version =(Ptr_gtk_check_version)QLibrary::resolve(GTK_PATH, 0, "gtk_check_version");
    QGtk::pango_font_description_get_size = (Ptr_pango_font_description_get_size)QLibrary::resolve(GTK_PATH, 0, "pango_font_description_get_size");
    QGtk::pango_font_description_get_weight = (Ptr_pango_font_description_get_weight)QLibrary::resolve(GTK_PATH, 0, "pango_font_description_get_weight");
    QGtk::pango_font_description_get_family = (Ptr_pango_font_description_get_family)QLibrary::resolve(GTK_PATH, 0, "pango_font_description_get_family");
    QGtk::pango_font_description_get_style = (Ptr_pango_font_description_get_style)QLibrary::resolve(GTK_PATH, 0, "pango_font_description_get_style");
}

void QGtk::cleanup_gtk_widgets()
{
    if (gtkWidgetMap()->contains(QLS("GtkWindow"))) // Gtk will destroy all children
        QGtk::gtk_widget_destroy(gtkWidgetMap()->value(QLS("GtkWindow")));
}

static bool resolveGConf()
{
    if (!QGtk::gconf_client_get_default) {
        QGtk::gconf_client_get_default = (Ptr_gconf_client_get_default)QLibrary::resolve(QLS("gconf-2"), 4, "gconf_client_get_default");
        QGtk::gconf_client_get_string =  (Ptr_gconf_client_get_string)QLibrary::resolve(QLS("gconf-2"), 4, "gconf_client_get_string");
    }
    return (QGtk::gconf_client_get_default !=0);
}

typedef int (*x11ErrorHandler)(Display*, XErrorEvent*);

static QString getGConfString(const QString &value)
{
    QString retVal;
    if (resolveGConf()) {
        g_type_init();
        GConfClient* client = QGtk::gconf_client_get_default();
        GError *err = 0;
        char *str = QGtk::gconf_client_get_string(client, qPrintable(value), &err);
        if (!err) {
            retVal = QString::fromUtf8(str);
            g_free(str);
        }
        g_object_unref(client);
        if (err)
            g_error_free (err);
    }
    return retVal;
}

static QString getThemeName()
{
    QString themeName;
    // We try to parse the gtkrc file first
    // primarily to avoid resolving Gtk functions if
    // the KDE 3 "Qt" style is currently in use
    QString rcPaths = QString::fromLocal8Bit(qgetenv("GTK2_RC_FILES"));
    if (!rcPaths.isEmpty()) {
        QStringList paths = rcPaths.split(QLS(":"));
        foreach (const QString &rcPath, paths) {
            if (!rcPath.isEmpty()) {
                QFile rcFile(rcPath);
                if (rcFile.exists() && rcFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&rcFile);
                    while(!in.atEnd()) {
                        QString line = in.readLine();
                        if (line.contains(QLS("gtk-theme-name"))) {
                            line = line.right(line.length() - line.indexOf(QLS("=")) - 1);
                            line.remove(QLS("\""));
                            line = line.trimmed();
                            themeName = line;
                            break;
                        }
                    }
                }
            }
            if (!themeName.isEmpty())
                break;
        }
    }

    // Fall back to gconf
    if (themeName.isEmpty() && resolveGConf())
        themeName = getGConfString(QLS("/desktop/gnome/interface/gtk_theme"));

    return themeName;
}

static void init_gtk_window()
{
    static QString themeName;
    if (!gtkWidgetMap()->contains(QLS("GtkWindow")) && themeName.isEmpty()) {
        themeName = getThemeName();
        // Due to namespace conflicts with Qt3 and obvious recursion with Qt4,
        // we cannot support the GTK_Qt Gtk engine
        if (!(themeName.isEmpty() || themeName == QLS("Qt") || themeName == QLS("Qt4"))) {
            resolveGtk();
            if (QGtk::gtk_init) {
                // Gtk will set the Qt error handler so we have to reset it afterwards
                x11ErrorHandler qt_x_errhandler = XSetErrorHandler(0);
                QGtk::gtk_init (NULL, NULL);
                XSetErrorHandler(qt_x_errhandler);

                GtkWidget* gtkWindow = QGtk::gtk_window_new(GTK_WINDOW_POPUP);
                QGtk::gtk_widget_realize(gtkWindow);
                if (displayDepth == -1)
                    displayDepth = QGtk::gdk_drawable_get_depth(gtkWindow->window);
                gtkWidgetMap()->insert(QLS("GtkWindow"), gtkWindow);
            }
            else {
                qWarning("QGtkStyle could not resolve GTK. Make sure you have installed the proper libraries.");
            }
        } else {
            qWarning("QGtkStyle cannot be used together with the GTK_Qt engine.");
        }
    }
}

static void setup_gtk_widget(GtkWidget* widget)
{
    if (Q_GTK_IS_WIDGET(widget)) {
        static GtkWidget* protoLayout = 0;
        if (!protoLayout) {
            protoLayout = QGtk::gtk_fixed_new();
            QGtk::gtk_container_add((GtkContainer*)(gtkWidgetMap()->value(QLS("GtkWindow"))), protoLayout);
        }
        Q_ASSERT(protoLayout);

        QGtk::gtk_container_add((GtkContainer*)(protoLayout), widget);
        QGtk::gtk_widget_realize(widget);
    }
}

static void add_widget_to_map(GtkWidget *widget)
{
    if (Q_GTK_IS_WIDGET(widget)) {
       QGtk::gtk_widget_realize(widget);
       gtkWidgetMap()->insert(classPath(widget), widget);
    }
 }

static void add_all_sub_widgets(GtkWidget *widget, gpointer v = 0)
{
    Q_UNUSED(v);
    add_widget_to_map(widget);
    if (GTK_CHECK_TYPE ((widget), Q_GTK_TYPE_CONTAINER))
        QGtk::gtk_container_forall((GtkContainer*)widget, add_all_sub_widgets, NULL);
}

static void init_gtk_menu()
{
    // Create menubar
    GtkWidget *gtkMenuBar = QGtk::gtk_menu_bar_new();
    setup_gtk_widget(gtkMenuBar);

    GtkWidget *gtkMenuBarItem = QGtk::gtk_menu_item_new();
    QGtk::gtk_menu_shell_append((GtkMenuShell*)(gtkMenuBar), gtkMenuBarItem);
    QGtk::gtk_widget_realize(gtkMenuBarItem);

    // Create menu
    GtkWidget *gtkMenu = QGtk::gtk_menu_new();
    QGtk::gtk_menu_item_set_submenu((GtkMenuItem*)(gtkMenuBarItem), gtkMenu);
    QGtk::gtk_widget_realize(gtkMenu);

    GtkWidget *gtkMenuItem = QGtk::gtk_menu_item_new();
    QGtk::gtk_menu_shell_append((GtkMenuShell*)gtkMenu, gtkMenuItem);
    QGtk::gtk_widget_realize(gtkMenuItem);

    GtkWidget *gtkCheckMenuItem = QGtk::gtk_check_menu_item_new();
    QGtk::gtk_menu_shell_append((GtkMenuShell*)gtkMenu, gtkCheckMenuItem);
    QGtk::gtk_widget_realize(gtkCheckMenuItem);

    GtkWidget *gtkMenuSeparator = QGtk::gtk_separator_menu_item_new();
    QGtk::gtk_menu_shell_append((GtkMenuShell*)gtkMenu, gtkMenuSeparator);

    add_all_sub_widgets(gtkMenuBar);
    add_all_sub_widgets(gtkMenu);
}

// Updates window/windowtext palette based on the indicated gtk widget
static void ensureWidgetPalette(QWidget* widget, const QString &gtkWidgetName)
{
    GtkWidget *gtkWidget = QGtk::gtkWidget(gtkWidgetName);
    Q_ASSERT(gtkWidget);
    QPalette pal = widget->palette();
    GdkColor gdkBg = gtkWidget->style->bg[GTK_STATE_NORMAL];
    GdkColor gdkText = gtkWidget->style->fg[GTK_STATE_NORMAL];
    GdkColor gdkDisabledText = gtkWidget->style->fg[GTK_STATE_INSENSITIVE];
    QColor bgColor(gdkBg.red>>8, gdkBg.green>>8, gdkBg.blue>>8);
    QColor textColor(gdkText.red>>8, gdkText.green>>8, gdkText.blue>>8);
    QColor disabledTextColor(gdkDisabledText.red>>8, gdkDisabledText.green>>8, gdkDisabledText.blue>>8);
    pal.setBrush(QPalette::Window, bgColor);
    pal.setBrush(QPalette::Button, bgColor);
    pal.setBrush(QPalette::All, QPalette::WindowText, textColor);
    pal.setBrush(QPalette::Disabled, QPalette::WindowText, disabledTextColor);
    pal.setBrush(QPalette::All, QPalette::ButtonText, textColor);
    pal.setBrush(QPalette::Disabled, QPalette::ButtonText, disabledTextColor);
    widget->setPalette(pal);
    widget->setAttribute(Qt::WA_SetPalette, false);
}

bool QGtk::isKDE4Session()
{
    static int version = -1;
    if (version == -1)
        version = qgetenv("KDE_SESSION_VERSION").toInt();
    return (version == 4);
}

// Maps a Gtk widget palettes to a Qt widget
void QGtk::applyGtkSystemPalette(QWidget *widget)
{
    // Do not apply if the widget has a custom palette;
    if (widget->testAttribute(Qt::WA_SetPalette))
        return;

    QPalette pal;
    if (QStatusBar *statusbar = qobject_cast<QStatusBar*> (widget))
        ensureWidgetPalette(statusbar, QLS("GtkStatusbar"));
    else if (QMenuBar *menubar = qobject_cast<QMenuBar*> (widget))
        ensureWidgetPalette(menubar, QLS("GtkMenuBar"));
    else if (QToolBar *toolbar = qobject_cast<QToolBar*> (widget))
        ensureWidgetPalette(toolbar, QLS("GtkToolbar"));
    else if (QMenu *menubar = qobject_cast<QMenu*> (widget)) {
        // This really applies to the combo box rendering since
        // QComboBox copies the palette from a QMenu
        QPalette pal = widget->palette();
        GdkColor gdkBg = QGtk::gtkWidget(QLS("GtkMenu"))->style->bg[GTK_STATE_NORMAL];
        QColor bgColor(gdkBg.red>>8, gdkBg.green>>8, gdkBg.blue>>8);
        pal.setBrush(QPalette::Base, bgColor);
        menubar->setPalette(pal);
    }
    widget->setAttribute(Qt::WA_SetPalette, false);
}

static void gtkStyleSetCallback(GtkWidget*, GtkStyle*, void*)
{
    static QString oldTheme(QLS("qt_not_set"));
    QPixmapCache::clear();
    qApp->setFont(QGtk::getThemeFont());
    QGtk::initGtkWidgets();
    if (oldTheme != getThemeName()) {
        oldTheme = getThemeName();
        QApplicationPrivate::setSystemPalette(qApp->style()->standardPalette());
        QList<QWidget*> widgets = QApplication::allWidgets();
        foreach (QWidget *widget, widgets) {
            QGtk::applyGtkSystemPalette(widget);
        }
    }
}

static void add_widget(GtkWidget *widget)
{
    if (widget) {
        setup_gtk_widget(widget);
        add_all_sub_widgets(widget);
    }
}

static void init_gtk_treeview()
{
    GtkWidget *gtkTreeView = QGtk::gtk_tree_view_new();
    QGtk::gtk_tree_view_append_column((GtkTreeView*)gtkTreeView, QGtk::gtk_tree_view_column_new());
    QGtk::gtk_tree_view_append_column((GtkTreeView*)gtkTreeView, QGtk::gtk_tree_view_column_new());
    QGtk::gtk_tree_view_append_column((GtkTreeView*)gtkTreeView, QGtk::gtk_tree_view_column_new());
    add_widget(gtkTreeView);
}


// Fetch the application font from the pango font description
// contained in the theme.
QFont QGtk::getThemeFont()
{
    QFont font;
    GtkStyle *style = gtkStyle();
    if (style && qApp->desktopSettingsAware())
    {
        PangoFontDescription *gtk_font = style->font_desc;
        font.setPointSizeF((float)(pango_font_description_get_size(gtk_font))/PANGO_SCALE);

        QString family = QString::fromLatin1(pango_font_description_get_family(gtk_font));
        if (!family.isEmpty())
            font.setFamily(family);

        int weight = pango_font_description_get_weight(gtk_font);
        if (weight >= PANGO_WEIGHT_HEAVY)
            font.setWeight(QFont::Black);
        else if (weight >= PANGO_WEIGHT_BOLD)
            font.setWeight(QFont::Bold);
        else if (weight >= PANGO_WEIGHT_SEMIBOLD)
            font.setWeight(QFont::DemiBold);
        else if (weight >= PANGO_WEIGHT_NORMAL)
            font.setWeight(QFont::Normal);
        else
            font.setWeight(QFont::Light);

        PangoStyle fontstyle = pango_font_description_get_style(gtk_font);
        if (fontstyle == PANGO_STYLE_ITALIC)
            font.setStyle(QFont::StyleItalic);
        else if (fontstyle == PANGO_STYLE_OBLIQUE)
            font.setStyle(QFont::StyleOblique);
        else
            font.setStyle(QFont::StyleNormal);
    }
    return font;
}

GtkWidget* QGtk::gtkWidget(const QString &path)
{
    GtkWidget *widget = gtkWidgetMap()->value(path);
    if (!widget) {
        // Theme might have rearranged widget internals
        widget = gtkWidgetMap()->value(path);
    }
    return widget;
}

GtkStyle* QGtk::gtkStyle(const QString &path)
{
    if (gtkWidgetMap()->contains(path))
        return gtkWidgetMap()->value(path)->style;
    return 0;
}
QT_END_NAMESPACE

int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);

QT_BEGIN_NAMESPACE
void QGtk::initGtkWidgets()
{
    // From gtkmain.c
    uid_t ruid, rgid, euid, egid, suid, sgid;
    if (getresuid (&ruid, &euid, &suid) != 0 || getresgid (&rgid, &egid, &sgid) != 0) {
        suid = ruid = getuid ();
        sgid = rgid = getgid ();
        euid = geteuid ();
        egid = getegid ();
    }
    if (ruid != euid || ruid != suid || rgid != egid || rgid != sgid) {
        qWarning("\nThis process is currently running setuid or setgid.\nGTK+ does not allow this "
                 "therefore Qt cannot use the GTK+ integration.\nTry launching your app using \'gksudo\', "
                 "\'kdesudo\' or a similar tool.\n\n"
                 "See http://www.gtk.org/setuid.html for more information.\n");
        return;
    }
  
    init_gtk_window();

    if (QGtk::gtk_init) {

        // Make all widgets respect the text direction
        if (qApp->layoutDirection() == Qt::RightToLeft)
            QGtk::gtk_widget_set_default_direction(GTK_TEXT_DIR_RTL);

        if (!gtkWidgetMap()->contains(QLS("GtkButton"))) {
            GtkWidget *gtkButton = QGtk::gtk_button_new();
            add_widget(gtkButton);
            g_signal_connect(gtkButton, "style-set", G_CALLBACK(gtkStyleSetCallback), NULL);
            add_widget(QGtk::gtk_arrow_new(GTK_ARROW_DOWN, GTK_SHADOW_NONE));
            add_widget(QGtk::gtk_hbutton_box_new());
            add_widget(QGtk::gtk_check_button_new());
            add_widget(QGtk::gtk_radio_button_new(NULL));
            add_widget(QGtk::gtk_combo_box_new());
            add_widget(QGtk::gtk_combo_box_entry_new());
            add_widget(QGtk::gtk_entry_new());
            add_widget(QGtk::gtk_frame_new(NULL));
            add_widget(QGtk::gtk_expander_new(""));
            add_widget(QGtk::gtk_statusbar_new());
            add_widget(QGtk::gtk_hscale_new((GtkAdjustment*)(QGtk::gtk_adjustment_new(1, 0, 1, 0, 0, 0))));
            add_widget(QGtk::gtk_hscrollbar_new(NULL));
            add_widget(QGtk::gtk_scrolled_window_new(NULL, NULL));
            init_gtk_menu();
            add_widget(QGtk::gtk_notebook_new());
            add_widget(QGtk::gtk_progress_bar_new());
            add_widget(QGtk::gtk_spin_button_new((GtkAdjustment*)
                                             (QGtk::gtk_adjustment_new(1, 0, 1, 0, 0, 0)), 0.1, 3));
            GtkWidget *toolbar = QGtk::gtk_toolbar_new();
            QGtk::gtk_toolbar_insert((GtkToolbar*)toolbar, QGtk::gtk_separator_tool_item_new(), -1);
            add_widget(toolbar);
            init_gtk_treeview();
            add_widget(QGtk::gtk_vscale_new((GtkAdjustment*)(QGtk::gtk_adjustment_new(1, 0, 1, 0, 0, 0))));
            add_widget(QGtk::gtk_vscrollbar_new(NULL));
        }
        else // Rebuild map
        {
            // When styles change subwidgets can get rearranged
            // as with the combo box. We need to update the widget map
            // to reflect this;
            QHash<QString, GtkWidget*> oldMap = *gtkWidgetMap();
            gtkWidgetMap()->clear();
            QHashIterator<QString, GtkWidget*> it(oldMap);
            while (it.hasNext()) {
                it.next();
                if (!it.key().contains(QLS("."))) {
                    add_all_sub_widgets(it.value());
                }
            }
        }
    }
}

// ----------- Native file dialogs -----------

// Extract filter list from expressions of type: foo (*.a *.b *.c)"
static QStringList extract_filter(const QString &rawFilter)
{
    QString result = rawFilter;
    QRegExp r(QString::fromLatin1("^([^()]*)\\(([a-zA-Z0-9_.*? +;#\\-\\[\\]@\\{\\}/!<>\\$%&=^~:\\|]*)\\)$"));
    int index = r.indexIn(result);
    if (index >= 0)
        result = r.cap(2);
    return result.split(QLatin1Char(' '));
}

extern QStringList qt_make_filter_list(const QString &filter);

static void setupGtkFileChooser(GtkWidget* gtkFileChooser, QWidget *parent,
                                const QString &dir, const QString &filter, QString *selectedFilter,
                                QFileDialog::Options options, bool isSaveDialog = false)
{
    g_object_set(gtkFileChooser, "do-overwrite-confirmation", gboolean(!(options & QFileDialog::DontConfirmOverwrite)), NULL);
    g_object_set(gtkFileChooser, "local_only", gboolean(true), NULL);
    if (!filter.isEmpty()) {
        QStringList filters = qt_make_filter_list(filter);
        foreach (const QString &rawfilter, filters) {
            GtkFileFilter *gtkFilter = QGtk::gtk_file_filter_new ();
            QString name = rawfilter.left(rawfilter.indexOf(QLatin1Char('(')));
            QGtk::gtk_file_filter_set_name(gtkFilter, qPrintable(name));

            QStringList extensions = extract_filter(rawfilter);
            foreach (const QString &fileExtension, extensions) {
                QGtk::gtk_file_filter_add_pattern (gtkFilter, qPrintable(fileExtension));
            }
            QGtk::gtk_file_chooser_add_filter((GtkFileChooser*)gtkFileChooser, gtkFilter);
            if (selectedFilter && (rawfilter == *selectedFilter))
                QGtk::gtk_file_chooser_set_filter((GtkFileChooser*)gtkFileChooser, gtkFilter);
        }
    }

    // Using the currently active window is not entirely correct, however
    // it gives more sensible behavior for applications that do not provide a
    // parent
    QWidget *modalFor = parent ? parent->window() : qApp->activeWindow();
    if (modalFor) {
        QGtk::gtk_widget_realize(gtkFileChooser); // Creates X window
        XSetTransientForHint(QGtk::gdk_x11_drawable_get_xdisplay(gtkFileChooser->window),
                             QGtk::gdk_x11_drawable_get_xid(gtkFileChooser->window),
                             modalFor->winId());
        QGtk::gdk_x11_window_set_user_time (gtkFileChooser->window, QX11Info::appUserTime());

    }

    QFileInfo fileinfo(dir);
    if (dir.isEmpty())
        fileinfo.setFile(QDir::currentPath());
    fileinfo.makeAbsolute();
    if (fileinfo.isDir()) {
        QGtk::gtk_file_chooser_set_current_folder((GtkFileChooser*)gtkFileChooser, qPrintable(dir));
    } else if (isSaveDialog) {
        QGtk::gtk_file_chooser_set_current_folder((GtkFileChooser*)gtkFileChooser, qPrintable(fileinfo.absolutePath()));
        QGtk::gtk_file_chooser_set_current_name((GtkFileChooser*)gtkFileChooser, qPrintable(fileinfo.fileName()));
    } else {
        QGtk::gtk_file_chooser_set_filename((GtkFileChooser*)gtkFileChooser, qPrintable(dir));
    }
}

QString QGtk::openFilename(QWidget *parent, const QString &caption, const QString &dir, const QString &filter,
                            QString *selectedFilter, QFileDialog::Options options)
{

    GtkWidget *gtkFileChooser = QGtk::gtk_file_chooser_dialog_new (qPrintable(caption),
                                                             NULL,
                                                             GTK_FILE_CHOOSER_ACTION_OPEN,
                                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                             GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                             NULL);

    setupGtkFileChooser(gtkFileChooser, parent, dir, filter, selectedFilter, options);

    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);

    QString filename;
    if (QGtk::gtk_dialog_run ((GtkDialog*)gtkFileChooser) == GTK_RESPONSE_ACCEPT) {
        char *gtk_filename = QGtk::gtk_file_chooser_get_filename ((GtkFileChooser*)gtkFileChooser);
        filename = QString::fromUtf8(gtk_filename);
        g_free (gtk_filename);
    }

    QApplicationPrivate::leaveModal(&modal_widget);
    gtk_widget_destroy (gtkFileChooser);
    return filename;
}


QString QGtk::openDirectory(QWidget *parent, const QString &caption, const QString &dir, QFileDialog::Options options)
{
    GtkWidget *gtkFileChooser = QGtk::gtk_file_chooser_dialog_new (qPrintable(caption),
                                                             NULL,
                                                             GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                             GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                             NULL);

    setupGtkFileChooser(gtkFileChooser, parent, dir, QString(), 0, options);
    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);

    QString filename;
    if (QGtk::gtk_dialog_run ((GtkDialog*)gtkFileChooser) == GTK_RESPONSE_ACCEPT) {
        char *gtk_filename = QGtk::gtk_file_chooser_get_filename ((GtkFileChooser*)gtkFileChooser);
        filename = QString::fromUtf8(gtk_filename);
        g_free (gtk_filename);
    }

    QApplicationPrivate::leaveModal(&modal_widget);
    gtk_widget_destroy (gtkFileChooser);
    return filename;
}

QStringList QGtk::openFilenames(QWidget *parent, const QString &caption, const QString &dir, const QString &filter,
                                 QString *selectedFilter, QFileDialog::Options options)
{
    QStringList filenames;
    GtkWidget *gtkFileChooser = QGtk::gtk_file_chooser_dialog_new (qPrintable(caption),
                                                             NULL,
                                                             GTK_FILE_CHOOSER_ACTION_OPEN,
                                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                             GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                             NULL);

    setupGtkFileChooser(gtkFileChooser, parent, dir, filter, selectedFilter, options);
    g_object_set(gtkFileChooser, "select-multiple", gboolean(true), NULL);

    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);

    if (gtk_dialog_run ((GtkDialog*)gtkFileChooser) == GTK_RESPONSE_ACCEPT) {
        GSList *gtk_file_names = QGtk::gtk_file_chooser_get_filenames((GtkFileChooser*)gtkFileChooser);
        for (GSList *iterator  = gtk_file_names ; iterator; iterator = iterator->next)
            filenames << QString::fromUtf8((const char*)iterator->data);
        g_slist_free(gtk_file_names);
    }

    QApplicationPrivate::leaveModal(&modal_widget);
    gtk_widget_destroy (gtkFileChooser);
    return filenames;
}

QString QGtk::saveFilename(QWidget *parent, const QString &caption, const QString &dir, const QString &filter,
                           QString *selectedFilter, QFileDialog::Options options)
{
    GtkWidget *gtkFileChooser = QGtk::gtk_file_chooser_dialog_new (qPrintable(caption),
                                                             NULL,
                                                             GTK_FILE_CHOOSER_ACTION_SAVE,
                                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                                             GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                                             NULL);
    setupGtkFileChooser(gtkFileChooser, parent, dir, filter, selectedFilter, options, true);

    QWidget modal_widget;
    modal_widget.setAttribute(Qt::WA_NoChildEventsForParent, true);
    modal_widget.setParent(parent, Qt::Window);
    QApplicationPrivate::enterModal(&modal_widget);

    QString filename;
    if (QGtk::gtk_dialog_run ((GtkDialog*)gtkFileChooser) == GTK_RESPONSE_ACCEPT) {
        char *gtk_filename = QGtk::gtk_file_chooser_get_filename ((GtkFileChooser*)gtkFileChooser);
        filename = QString::fromUtf8(gtk_filename);
        g_free (gtk_filename);
    }

    QApplicationPrivate::leaveModal(&modal_widget);
    gtk_widget_destroy (gtkFileChooser);
    return filename;
}

QT_END_NAMESPACE

#endif // !defined(QT_NO_STYLE_GTK)
