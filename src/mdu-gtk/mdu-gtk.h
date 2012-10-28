/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-gtk.h
 *
 * Copyright (C) 2007 David Zeuthen
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef MDU_GTK_H
#define MDU_GTK_H

#ifndef MDU_GTK_API_IS_SUBJECT_TO_CHANGE
#error  libmdu-gtk is unstable API. You must define MDU_GTK_API_IS_SUBJECT_TO_CHANGE before including mdu-gtk/mdu-gtk.h
#endif

#define __MDU_GTK_INSIDE_MDU_GTK_H
#include <mdu-gtk/mdu-gtk-types.h>
#include <mdu-gtk/mdu-gtk-enumtypes.h>
#include <mdu-gtk/mdu-time-label.h>
#include <mdu-gtk/mdu-pool-tree-view.h>
#include <mdu-gtk/mdu-pool-tree-model.h>
#include <mdu-gtk/mdu-size-widget.h>
#include <mdu-gtk/mdu-create-linux-md-dialog.h>
#include <mdu-gtk/mdu-ata-smart-dialog.h>
#include <mdu-gtk/mdu-spinner.h>
#include <mdu-gtk/mdu-volume-grid.h>
#include <mdu-gtk/mdu-details-table.h>
#include <mdu-gtk/mdu-details-element.h>
#include <mdu-gtk/mdu-error-dialog.h>
#include <mdu-gtk/mdu-confirmation-dialog.h>
#include <mdu-gtk/mdu-button-element.h>
#include <mdu-gtk/mdu-button-table.h>
#include <mdu-gtk/mdu-dialog.h>
#include <mdu-gtk/mdu-edit-partition-dialog.h>
#include <mdu-gtk/mdu-format-dialog.h>
#include <mdu-gtk/mdu-partition-dialog.h>
#include <mdu-gtk/mdu-create-partition-dialog.h>
#include <mdu-gtk/mdu-edit-name-dialog.h>
#include <mdu-gtk/mdu-disk-selection-widget.h>
#include <mdu-gtk/mdu-add-component-linux-md-dialog.h>
#include <mdu-gtk/mdu-edit-linux-md-dialog.h>
#include <mdu-gtk/mdu-edit-linux-lvm2-dialog.h>
#include <mdu-gtk/mdu-drive-benchmark-dialog.h>
#include <mdu-gtk/mdu-connect-to-server-dialog.h>
#include <mdu-gtk/mdu-create-linux-lvm2-volume-dialog.h>
#include <mdu-gtk/mdu-add-pv-linux-lvm2-dialog.h>
#undef __MDU_GTK_INSIDE_MDU_GTK_H

G_BEGIN_DECLS

gboolean mdu_util_dialog_show_filesystem_busy (GtkWidget *parent_window, MduPresentable *presentable);


char *mdu_util_dialog_ask_for_new_secret (GtkWidget      *parent_window,
                                          gboolean       *save_in_keyring,
                                          gboolean       *save_in_keyring_session);

char *mdu_util_dialog_ask_for_secret (GtkWidget       *parent_window,
                                      MduPresentable  *presentable,
                                      gboolean         bypass_keyring,
                                      gboolean         indicate_wrong_passphrase,
                                      gboolean        *asked_user);

gboolean mdu_util_dialog_change_secret (GtkWidget       *parent_window,
                                        MduPresentable  *presentable,
                                        char           **old_secret,
                                        char           **new_secret,
                                        gboolean        *save_in_keyring,
                                        gboolean        *save_in_keyring_session,
                                        gboolean         bypass_keyring,
                                        gboolean         indicate_wrong_passphrase);

gboolean mdu_util_delete_confirmation_dialog (GtkWidget   *parent_window,
                                              const char  *title,
                                              const char  *primary_text,
                                              const char  *secondary_text,
                                              const char  *affirmative_action_button_mnemonic);

/* ---------------------------------------------------------------------------------------------------- */

GtkWidget *mdu_util_fstype_combo_box_create         (MduPool *pool,
                                                     const char *include_extended_partitions_for_scheme);
void       mdu_util_fstype_combo_box_rebuild        (GtkWidget  *combo_box,
                                                     MduPool *pool,
                                                     const char *include_extended_partitions_for_scheme);
void       mdu_util_fstype_combo_box_set_desc_label (GtkWidget *combo_box, GtkWidget *desc_label);
gboolean   mdu_util_fstype_combo_box_select         (GtkWidget  *combo_box,
                                                     const char *fstype);
char      *mdu_util_fstype_combo_box_get_selected   (GtkWidget  *combo_box);

/* ---------------------------------------------------------------------------------------------------- */

GtkWidget *mdu_util_part_type_combo_box_create       (const char *part_scheme);
void       mdu_util_part_type_combo_box_rebuild      (GtkWidget  *combo_box,
                                                      const char *part_scheme);
gboolean   mdu_util_part_type_combo_box_select       (GtkWidget  *combo_box,
                                                      const char *part_type);
char      *mdu_util_part_type_combo_box_get_selected (GtkWidget  *combo_box);

/* ---------------------------------------------------------------------------------------------------- */

GtkWidget *mdu_util_part_table_type_combo_box_create         (void);
void       mdu_util_part_table_type_combo_box_set_desc_label (GtkWidget *combo_box, GtkWidget *desc_label);
gboolean   mdu_util_part_table_type_combo_box_select         (GtkWidget  *combo_box,
                                                              const char *part_table_type);
char      *mdu_util_part_table_type_combo_box_get_selected   (GtkWidget  *combo_box);

/* ---------------------------------------------------------------------------------------------------- */

GdkPixbuf *mdu_util_get_pixbuf_for_presentable (MduPresentable *presentable, GtkIconSize size);

GdkPixbuf *mdu_util_get_pixbuf_for_presentable_at_pixel_size (MduPresentable *presentable, gint pixel_size);

void       mdu_util_get_mix_color (GtkWidget    *widget,
                                   GtkStateType  state,
                                   gchar        *color_buf,
                                   gsize         color_buf_size);


G_END_DECLS

#endif /* MDU_GTK_H */
