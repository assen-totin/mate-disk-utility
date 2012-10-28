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

#ifndef __MDU_CREATE_LINUX_MD_DIALOG_H
#define __MDU_CREATE_LINUX_MD_DIALOG_H

#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_CREATE_LINUX_MD_DIALOG         (mdu_create_linux_md_dialog_get_type())
#define MDU_CREATE_LINUX_MD_DIALOG(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_CREATE_LINUX_MD_DIALOG, MduCreateLinuxMdDialog))
#define MDU_CREATE_LINUX_MD_DIALOG_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_CREATE_LINUX_MD_DIALOG, MduCreateLinuxMdDialogClass))
#define MDU_IS_CREATE_LINUX_MD_DIALOG(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_CREATE_LINUX_MD_DIALOG))
#define MDU_IS_CREATE_LINUX_MD_DIALOG_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_CREATE_LINUX_MD_DIALOG))
#define MDU_CREATE_LINUX_MD_DIALOG_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_CREATE_LINUX_MD_DIALOG, MduCreateLinuxMdDialogClass))

typedef struct MduCreateLinuxMdDialogClass   MduCreateLinuxMdDialogClass;
typedef struct MduCreateLinuxMdDialogPrivate MduCreateLinuxMdDialogPrivate;

struct MduCreateLinuxMdDialog
{
        GtkDialog parent;

        /*< private >*/
        MduCreateLinuxMdDialogPrivate *priv;
};

struct MduCreateLinuxMdDialogClass
{
        GtkDialogClass parent_class;
};

GType       mdu_create_linux_md_dialog_get_type           (void) G_GNUC_CONST;
GtkWidget*  mdu_create_linux_md_dialog_new                (GtkWindow               *parent,
                                                           MduPool                 *pool);
gchar      *mdu_create_linux_md_dialog_get_level          (MduCreateLinuxMdDialog  *dialog);
gchar      *mdu_create_linux_md_dialog_get_name           (MduCreateLinuxMdDialog  *dialog);
guint64     mdu_create_linux_md_dialog_get_size           (MduCreateLinuxMdDialog  *dialog);
guint64     mdu_create_linux_md_dialog_get_component_size (MduCreateLinuxMdDialog  *dialog);
guint64     mdu_create_linux_md_dialog_get_stripe_size    (MduCreateLinuxMdDialog  *dialog);
GPtrArray  *mdu_create_linux_md_dialog_get_drives         (MduCreateLinuxMdDialog  *dialog);

G_END_DECLS

#endif  /* __MDU_CREATE_LINUX_MD_DIALOG_H */

