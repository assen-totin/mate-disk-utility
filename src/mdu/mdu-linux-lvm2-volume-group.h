/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2007-2010 David Zeuthen
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

#if !defined (__MDU_INSIDE_MDU_H) && !defined (MDU_COMPILATION)
#error "Only <mdu/mdu.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_LINUX_LVM2_VOLUME_GROUP_H
#define __MDU_LINUX_LVM2_VOLUME_GROUP_H

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>
#include <mdu/mdu-drive.h>

G_BEGIN_DECLS

#define MDU_TYPE_LINUX_LVM2_VOLUME_GROUP         (mdu_linux_lvm2_volume_group_get_type ())
#define MDU_LINUX_LVM2_VOLUME_GROUP(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_LINUX_LVM2_VOLUME_GROUP, MduLinuxLvm2VolumeGroup))
#define MDU_LINUX_LVM2_VOLUME_GROUP_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_LINUX_LVM2_VOLUME_GROUP,  MduLinuxLvm2VolumeGroupClass))
#define MDU_IS_LINUX_LVM2_VOLUME_GROUP(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_LINUX_LVM2_VOLUME_GROUP))
#define MDU_IS_LINUX_LVM2_VOLUME_GROUP_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_LINUX_LVM2_VOLUME_GROUP))
#define MDU_LINUX_LVM2_VOLUME_GROUP_GET_CLASS(k) (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_LINUX_LVM2_VOLUME_GROUP, MduLinuxLvm2VolumeGroupClass))

typedef struct _MduLinuxLvm2VolumeGroupClass    MduLinuxLvm2VolumeGroupClass;
typedef struct _MduLinuxLvm2VolumeGroupPrivate  MduLinuxLvm2VolumeGroupPrivate;

struct _MduLinuxLvm2VolumeGroup
{
        MduDrive parent;

        /*< private >*/
        MduLinuxLvm2VolumeGroupPrivate *priv;
};

struct _MduLinuxLvm2VolumeGroupClass
{
        MduDriveClass parent_class;
};

typedef enum {
        MDU_LINUX_LVM2_VOLUME_GROUP_STATE_NOT_RUNNING,
        MDU_LINUX_LVM2_VOLUME_GROUP_STATE_PARTIALLY_RUNNING,
        MDU_LINUX_LVM2_VOLUME_GROUP_STATE_RUNNING,
} MduLinuxLvm2VolumeGroupState;

GType                         mdu_linux_lvm2_volume_group_get_type      (void);
const gchar                  *mdu_linux_lvm2_volume_group_get_uuid      (MduLinuxLvm2VolumeGroup  *vg);
MduLinuxLvm2VolumeGroupState  mdu_linux_lvm2_volume_group_get_state     (MduLinuxLvm2VolumeGroup  *vg);
MduDevice                    *mdu_linux_lvm2_volume_group_get_pv_device (MduLinuxLvm2VolumeGroup  *vg);
guint                         mdu_linux_lvm2_volume_group_get_num_lvs   (MduLinuxLvm2VolumeGroup  *vg);
gboolean                      mdu_linux_lvm2_volume_group_get_lv_info   (MduLinuxLvm2VolumeGroup  *vg,
                                                                         const gchar              *lv_uuid,
                                                                         guint                    *out_position,
                                                                         gchar                   **out_name,
                                                                         guint64                  *out_size);
gboolean                      mdu_linux_lvm2_volume_group_get_pv_info   (MduLinuxLvm2VolumeGroup  *vg,
                                                                         const gchar              *pv_uuid,
                                                                         guint                    *out_position,
                                                                         guint64                  *out_size,
                                                                         guint64                  *out_allocated_size);
gchar                        *mdu_linux_lvm2_volume_group_get_compute_new_lv_name (MduLinuxLvm2VolumeGroup  *vg);

G_END_DECLS

#endif /* __MDU_LINUX_LVM2_VOLUME_GROUP_H */
