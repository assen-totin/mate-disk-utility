/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-drive.h
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

#if !defined (__MDU_INSIDE_MDU_H) && !defined (MDU_COMPILATION)
#error "Only <mdu/mdu.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_DRIVE_H
#define __MDU_DRIVE_H

#include <mdu/mdu-types.h>

G_BEGIN_DECLS

#define MDU_TYPE_DRIVE         (mdu_drive_get_type ())
#define MDU_DRIVE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_DRIVE, MduDrive))
#define MDU_DRIVE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_DRIVE,  MduDriveClass))
#define MDU_IS_DRIVE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_DRIVE))
#define MDU_IS_DRIVE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_DRIVE))
#define MDU_DRIVE_GET_CLASS(k) (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_DRIVE, MduDriveClass))

typedef struct _MduDriveClass    MduDriveClass;
typedef struct _MduDrivePrivate  MduDrivePrivate;

/* TODO: move to mdu-enums.h */
/**
 * MduCreateVolumeFlags:
 * @MDU_CREATE_VOLUME_FLAGS_NONE: No flags are set.
 * @MDU_CREATE_VOLUME_FLAGS_LINUX_MD: The volume is to be used for Linux MD RAID.
 * @MDU_CREATE_VOLUME_FLAGS_LINUX_LVM2: The volume is to be used for Linux LVM2.
 *
 * Flags used in mdu_drive_create_volume().
 */
typedef enum {
        MDU_CREATE_VOLUME_FLAGS_NONE = 0x00,
        MDU_CREATE_VOLUME_FLAGS_LINUX_MD = (1<<0),
        MDU_CREATE_VOLUME_FLAGS_LINUX_LVM2 = (1<<1)
} MduCreateVolumeFlags;

struct _MduDrive
{
        GObject parent;

        /*< private >*/
        MduDrivePrivate *priv;
};

struct _MduDriveClass
{
        GObjectClass parent_class;

        /*< public >*/
        /* VTable */
        gboolean    (*is_active)             (MduDrive              *drive);
        gboolean    (*is_activatable)        (MduDrive              *drive);
        gboolean    (*can_deactivate)        (MduDrive              *drive);
        gboolean    (*can_activate)          (MduDrive              *drive,
                                              gboolean              *out_degraded);
        void        (*activate)              (MduDrive              *drive,
                                              MduDriveActivateFunc   callback,
                                              gpointer               user_data);
        void        (*deactivate)            (MduDrive              *drive,
                                              MduDriveDeactivateFunc callback,
                                              gpointer               user_data);

        gboolean   (*can_create_volume)     (MduDrive        *drive,
                                             gboolean        *out_is_uninitialized,
                                             guint64         *out_largest_contiguous_free_segment,
                                             guint64         *out_total_free,
                                             MduPresentable **out_presentable);

        void       (*create_volume)         (MduDrive              *drive,
                                             guint64                size,
                                             const gchar           *name,
                                             MduCreateVolumeFlags   flags,
                                             GAsyncReadyCallback    callback,
                                             gpointer               user_data);

        MduVolume *(*create_volume_finish) (MduDrive              *drive,
                                            GAsyncResult          *res,
                                            GError               **error);
};

GType       mdu_drive_get_type           (void);

gboolean    mdu_drive_is_active             (MduDrive              *drive);
gboolean    mdu_drive_is_activatable        (MduDrive              *drive);
gboolean    mdu_drive_can_deactivate        (MduDrive              *drive);
gboolean    mdu_drive_can_activate          (MduDrive              *drive,
                                             gboolean              *out_degraded);
void        mdu_drive_activate              (MduDrive              *drive,
                                             MduDriveActivateFunc   callback,
                                             gpointer               user_data);
void        mdu_drive_deactivate            (MduDrive              *drive,
                                             MduDriveDeactivateFunc callback,
                                             gpointer               user_data);


gboolean    mdu_drive_can_create_volume     (MduDrive        *drive,
                                             gboolean        *out_is_uninitialized,
                                             guint64         *out_largest_contiguous_free_segment,
                                             guint64         *out_total_free,
                                             MduPresentable **out_presentable);

void        mdu_drive_create_volume         (MduDrive              *drive,
                                             guint64                size,
                                             const gchar           *name,
                                             MduCreateVolumeFlags   flags,
                                             GAsyncReadyCallback    callback,
                                             gpointer               user_data);

MduVolume  *mdu_drive_create_volume_finish  (MduDrive              *drive,
                                             GAsyncResult          *res,
                                             GError               **error);

gboolean    mdu_drive_count_mbr_partitions  (MduDrive        *drive,
                                             guint           *out_num_primary_partitions,
                                             gboolean        *out_has_extended_partition);

GList      *mdu_drive_get_volumes           (MduDrive  *drive);

G_END_DECLS

#endif /* __MDU_DRIVE_H */
