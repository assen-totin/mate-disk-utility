/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 *  format-window.h
 *
 *  Copyright (C) 2008-2009 Red Hat, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author: Tomas Bzatek <tbzatek@redhat.com>
 *          David Zeuthen <davidz@redhat.com>
 *
 */

#ifndef __MDU_FORMAT_DIALOG_H
#define __MDU_FORMAT_DIALOG_H

#include <gtk/gtk.h>
#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_FORMAT_DIALOG            mdu_format_dialog_get_type()
#define MDU_FORMAT_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_FORMAT_DIALOG, MduFormatDialog))
#define MDU_FORMAT_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MDU_TYPE_FORMAT_DIALOG, MduFormatDialogClass))
#define MDU_IS_FORMAT_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_FORMAT_DIALOG))
#define MDU_IS_FORMAT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MDU_TYPE_FORMAT_DIALOG))
#define MDU_FORMAT_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_FORMAT_DIALOG, MduFormatDialogClass))

typedef struct MduFormatDialogClass   MduFormatDialogClass;
typedef struct MduFormatDialogPrivate MduFormatDialogPrivate;

struct MduFormatDialog
{
        MduDialog parent;

        /*< private >*/
        MduFormatDialogPrivate *priv;
};

struct MduFormatDialogClass
{
        MduDialogClass parent_class;
};

GType       mdu_format_dialog_get_type           (void) G_GNUC_CONST;
GtkWidget  *mdu_format_dialog_new                (GtkWindow            *parent,
                                                  MduPresentable       *presentable,
                                                  MduFormatDialogFlags  flags);
GtkWidget  *mdu_format_dialog_new_for_drive      (GtkWindow            *parent,
                                                  MduDevice            *device,
                                                  MduFormatDialogFlags  flags);
GtkWidget  *mdu_format_dialog_new_for_volume     (GtkWindow            *parent,
                                                  MduDevice            *device,
                                                  MduFormatDialogFlags  flags);
gchar      *mdu_format_dialog_get_fs_type        (MduFormatDialog *dialog);
gchar      *mdu_format_dialog_get_fs_label       (MduFormatDialog *dialog);
gchar     **mdu_format_dialog_get_fs_options     (MduFormatDialog *dialog);
gboolean    mdu_format_dialog_get_encrypt        (MduFormatDialog *dialog);
gboolean    mdu_format_dialog_get_take_ownership (MduFormatDialog *dialog);

GtkWidget  *mdu_format_dialog_get_table          (MduFormatDialog *dialog);

G_END_DECLS

#endif  /* __MDU_FORMAT_DIALOG_H */

