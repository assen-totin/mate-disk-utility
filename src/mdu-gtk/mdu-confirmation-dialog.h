/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-confirmation-dialog.h
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
#confirmation "Only <mdu-gtk/mdu-gtk.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_CONFIRMATION_DIALOG_H
#define __MDU_CONFIRMATION_DIALOG_H

#include <mdu-gtk/mdu-gtk-types.h>
#include <mdu-gtk/mdu-dialog.h>

G_BEGIN_DECLS

#define MDU_TYPE_CONFIRMATION_DIALOG            mdu_confirmation_dialog_get_type()
#define MDU_CONFIRMATION_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_CONFIRMATION_DIALOG, MduConfirmationDialog))
#define MDU_CONFIRMATION_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MDU_TYPE_CONFIRMATION_DIALOG, MduConfirmationDialogClass))
#define MDU_IS_CONFIRMATION_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_CONFIRMATION_DIALOG))
#define MDU_IS_CONFIRMATION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MDU_TYPE_CONFIRMATION_DIALOG))
#define MDU_CONFIRMATION_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_CONFIRMATION_DIALOG, MduConfirmationDialogClass))

typedef struct MduConfirmationDialogClass   MduConfirmationDialogClass;
typedef struct MduConfirmationDialogPrivate MduConfirmationDialogPrivate;

struct MduConfirmationDialog
{
        MduDialog parent;

        /*< private >*/
        MduConfirmationDialogPrivate *priv;
};

struct MduConfirmationDialogClass
{
        MduDialogClass parent_class;
};

GType       mdu_confirmation_dialog_get_type       (void) G_GNUC_CONST;
GtkWidget  *mdu_confirmation_dialog_new            (GtkWindow      *parent,
                                                    MduPresentable *presentable,
                                                    const gchar    *message,
                                                    const gchar    *button_text);
GtkWidget  *mdu_confirmation_dialog_new_for_drive  (GtkWindow      *parent,
                                                    MduDevice      *device,
                                                    const gchar    *message,
                                                    const gchar    *button_text);
GtkWidget  *mdu_confirmation_dialog_new_for_volume (GtkWindow      *parent,
                                                    MduDevice      *device,
                                                    const gchar    *message,
                                                    const gchar    *button_text);

G_END_DECLS

#endif /* __MDU_CONFIRMATION_DIALOG_H */
