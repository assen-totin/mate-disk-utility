/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-dialog.h
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

#if !defined (__MDU_GTK_INSIDE_MDU_GTK_H) && !defined (MDU_GTK_COMPILATION)
#error "Only <mdu-gtk/mdu-gtk.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_DIALOG_H
#define __MDU_DIALOG_H

#include <mdu-gtk/mdu-gtk-types.h>

G_BEGIN_DECLS

#define MDU_TYPE_DIALOG            mdu_dialog_get_type()
#define MDU_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_DIALOG, MduDialog))
#define MDU_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MDU_TYPE_DIALOG, MduDialogClass))
#define MDU_IS_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_DIALOG))
#define MDU_IS_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MDU_TYPE_DIALOG))
#define MDU_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_DIALOG, MduDialogClass))

typedef struct MduDialogClass   MduDialogClass;
typedef struct MduDialogPrivate MduDialogPrivate;

struct MduDialog
{
        GtkDialog parent;

        /*< private >*/
        MduDialogPrivate *priv;
};

struct MduDialogClass
{
        GtkDialogClass parent_class;
};

GType           mdu_dialog_get_type        (void) G_GNUC_CONST;
MduPresentable *mdu_dialog_get_presentable (MduDialog *dialog);
MduDevice      *mdu_dialog_get_device      (MduDialog *dialog);
MduPool        *mdu_dialog_get_pool        (MduDialog *dialog);

G_END_DECLS

#endif /* __MDU_DIALOG_H */
