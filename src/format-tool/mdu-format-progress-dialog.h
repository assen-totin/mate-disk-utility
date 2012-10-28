/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-ata-smart-dialog.h
 *
 * Copyright (C) 2009 David Zeuthen
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

#ifndef __MDU_FORMAT_PROGRESS_DIALOG_H
#define __MDU_FORMAT_PROGRESS_DIALOG_H

#include <mdu/mdu.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_FORMAT_PROGRESS_DIALOG         mdu_format_progress_dialog_get_type()
#define MDU_FORMAT_PROGRESS_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_FORMAT_PROGRESS_DIALOG, MduFormatProgressDialog))
#define MDU_FORMAT_PROGRESS_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_FORMAT_PROGRESS_DIALOG, MduFormatProgressDialogClass))
#define MDU_IS_FORMAT_PROGRESS_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_FORMAT_PROGRESS_DIALOG))
#define MDU_IS_FORMAT_PROGRESS_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_FORMAT_PROGRESS_DIALOG))
#define MDU_FORMAT_PROGRESS_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_FORMAT_PROGRESS_DIALOG, MduFormatProgressDialogClass))

typedef struct MduFormatProgressDialog        MduFormatProgressDialog;
typedef struct MduFormatProgressDialogClass   MduFormatProgressDialogClass;
typedef struct MduFormatProgressDialogPrivate MduFormatProgressDialogPrivate;

struct MduFormatProgressDialog
{
        GtkDialog parent;

        /*< private >*/
        MduFormatProgressDialogPrivate *priv;
};

struct MduFormatProgressDialogClass
{
        GtkDialogClass parent_class;
};

GType       mdu_format_progress_dialog_get_type (void) G_GNUC_CONST;
GtkWidget*  mdu_format_progress_dialog_new      (GtkWindow               *parent,
                                                 MduDevice               *device,
                                                 const gchar             *text);
void        mdu_format_progress_dialog_set_text (MduFormatProgressDialog *dialog,
                                                 const gchar             *text);

G_END_DECLS

#endif /* __MDU_FORMAT_PROGRESS_DIALOG_H */
