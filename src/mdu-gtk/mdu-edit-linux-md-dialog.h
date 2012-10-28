/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2009 Red Hat, Inc.
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
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#ifndef __MDU_EDIT_LINUX_MD_DIALOG_H
#define __MDU_EDIT_LINUX_MD_DIALOG_H

#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_EDIT_LINUX_MD_DIALOG         (mdu_edit_linux_md_dialog_get_type())
#define MDU_EDIT_LINUX_MD_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_EDIT_LINUX_MD_DIALOG, MduEditLinuxMdDialog))
#define MDU_EDIT_LINUX_MD_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_EDIT_LINUX_MD_DIALOG, MduEditLinuxMdDialogClass))
#define MDU_IS_EDIT_LINUX_MD_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_EDIT_LINUX_MD_DIALOG))
#define MDU_IS_EDIT_LINUX_MD_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_EDIT_LINUX_MD_DIALOG))
#define MDU_EDIT_LINUX_MD_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_EDIT_LINUX_MD_DIALOG, MduEditLinuxMdDialogClass))

typedef struct MduEditLinuxMdDialogClass   MduEditLinuxMdDialogClass;
typedef struct MduEditLinuxMdDialogPrivate MduEditLinuxMdDialogPrivate;

struct MduEditLinuxMdDialog
{
        MduDialog parent;

        /*< private >*/
        MduEditLinuxMdDialogPrivate *priv;
};

struct MduEditLinuxMdDialogClass
{
        MduDialogClass parent_class;

        void (*add_spare_button_clicked) (MduEditLinuxMdDialog *dialog);
        void (*expand_button_clicked)    (MduEditLinuxMdDialog *dialog);

        void (*attach_button_clicked) (MduEditLinuxMdDialog *dialog,
                                       MduDevice            *slave);
        void (*remove_button_clicked) (MduEditLinuxMdDialog *dialog,
                                       MduDevice            *slave);
};

GType       mdu_edit_linux_md_dialog_get_type           (void) G_GNUC_CONST;
GtkWidget*  mdu_edit_linux_md_dialog_new                (GtkWindow        *parent,
                                                         MduLinuxMdDrive  *linux_md_drive);

G_END_DECLS

#endif  /* __MDU_EDIT_LINUX_MD_DIALOG_H */

