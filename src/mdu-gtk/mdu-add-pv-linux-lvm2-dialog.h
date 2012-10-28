/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* Copyright (C) 2009 David Zeuthen
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

#ifndef __MDU_ADD_PV_LINUX_LVM2_DIALOG_H
#define __MDU_ADD_PV_LINUX_LVM2_DIALOG_H

#include <mdu-gtk/mdu-gtk-types.h>
#include <mdu-gtk/mdu-dialog.h>

G_BEGIN_DECLS

#define MDU_TYPE_ADD_PV_LINUX_LVM2_DIALOG            mdu_add_pv_linux_lvm2_dialog_get_type()
#define MDU_ADD_PV_LINUX_LVM2_DIALOG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_ADD_PV_LINUX_LVM2_DIALOG, MduAddPvLinuxLvm2Dialog))
#define MDU_ADD_PV_LINUX_LVM2_DIALOG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MDU_TYPE_ADD_PV_LINUX_LVM2_DIALOG, MduAddPvLinuxLvm2DialogClass))
#define MDU_IS_ADD_PV_LINUX_LVM2_DIALOG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_ADD_PV_LINUX_LVM2_DIALOG))
#define MDU_IS_ADD_PV_LINUX_LVM2_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MDU_TYPE_ADD_PV_LINUX_LVM2_DIALOG))
#define MDU_ADD_PV_LINUX_LVM2_DIALOG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_ADD_PV_LINUX_LVM2_DIALOG, MduAddPvLinuxLvm2DialogClass))

typedef struct MduAddPvLinuxLvm2DialogClass   MduAddPvLinuxLvm2DialogClass;
typedef struct MduAddPvLinuxLvm2DialogPrivate MduAddPvLinuxLvm2DialogPrivate;

struct MduAddPvLinuxLvm2Dialog
{
        MduDialog parent;

        /*< private >*/
        MduAddPvLinuxLvm2DialogPrivate *priv;
};

struct MduAddPvLinuxLvm2DialogClass
{
        MduDialogClass parent_class;
};

GType         mdu_add_pv_linux_lvm2_dialog_get_type  (void) G_GNUC_CONST;
GtkWidget    *mdu_add_pv_linux_lvm2_dialog_new       (GtkWindow                    *parent,
                                                      MduLinuxLvm2VolumeGroup      *vg);
MduDrive     *mdu_add_pv_linux_lvm2_dialog_get_drive (MduAddPvLinuxLvm2Dialog      *dialog);
guint64       mdu_add_pv_linux_lvm2_dialog_get_size  (MduAddPvLinuxLvm2Dialog      *dialog);

G_END_DECLS

#endif /* __MDU_ADD_PV_LINUX_LVM2_DIALOG_H */
