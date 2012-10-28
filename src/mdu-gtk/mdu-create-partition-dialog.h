/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
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
 *  Author: David Zeuthen <davidz@redhat.com>
 *
 */

#ifndef __MDU_CREATE_PARTITION_DIALOG_H
#define __MDU_CREATE_PARTITION_DIALOG_H

#include <gtk/gtk.h>
#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_CREATE_PARTITION_DIALOG            mdu_create_partition_dialog_get_type()
#define MDU_CREATE_PARTITION_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_CREATE_PARTITION_DIALOG, MduCreatePartitionDialog))
#define MDU_CREATE_PARTITION_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MDU_TYPE_CREATE_PARTITION_DIALOG, MduCreatePartitionDialogClass))
#define MDU_IS_CREATE_PARTITION_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_CREATE_PARTITION_DIALOG))
#define MDU_IS_CREATE_PARTITION_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MDU_TYPE_CREATE_PARTITION_DIALOG))
#define MDU_CREATE_PARTITION_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_CREATE_PARTITION_DIALOG, MduCreatePartitionDialogClass))

typedef struct MduCreatePartitionDialogClass   MduCreatePartitionDialogClass;
typedef struct MduCreatePartitionDialogPrivate MduCreatePartitionDialogPrivate;

struct MduCreatePartitionDialog
{
        MduFormatDialog parent;

        /*< private >*/
        MduCreatePartitionDialogPrivate *priv;
};

struct MduCreatePartitionDialogClass
{
        MduFormatDialogClass parent_class;
};

GType       mdu_create_partition_dialog_get_type       (void) G_GNUC_CONST;
GtkWidget  *mdu_create_partition_dialog_new            (GtkWindow                *parent,
                                                        MduPresentable           *presentable,
                                                        guint64                   max_size,
                                                        MduFormatDialogFlags      flags);
GtkWidget  *mdu_create_partition_dialog_new_for_drive  (GtkWindow                *parent,
                                                        MduDevice                *device,
                                                        guint64                   max_size,
                                                        MduFormatDialogFlags      flags);
guint64     mdu_create_partition_dialog_get_size       (MduCreatePartitionDialog *dialog);
guint64     mdu_create_partition_dialog_get_max_size   (MduCreatePartitionDialog *dialog);

G_END_DECLS

#endif  /* __MDU_CREATE_PARTITION_DIALOG_H */

