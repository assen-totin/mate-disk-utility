/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-private.h
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

#if defined (__MDU_INSIDE_MDU_H)
#error "Can't include a private header in the public header file."
#endif

#ifndef __MDU_PRIVATE_H
#define __MDU_PRIVATE_H

#include "mdu-types.h"

#define ATA_SMART_ATTRIBUTE_STRUCT_TYPE (dbus_g_type_get_struct ("GValueArray", \
                                                                 G_TYPE_UINT, \
                                                                 G_TYPE_STRING, \
                                                                 G_TYPE_UINT, \
                                                                 G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, \
                                                                 G_TYPE_UCHAR, G_TYPE_BOOLEAN, \
                                                                 G_TYPE_UCHAR, G_TYPE_BOOLEAN, \
                                                                 G_TYPE_UCHAR, G_TYPE_BOOLEAN, \
                                                                 G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, \
                                                                 G_TYPE_UINT, G_TYPE_UINT64, \
                                                                 dbus_g_type_get_collection ("GArray", G_TYPE_UCHAR), \
                                                                 G_TYPE_INVALID))

#define ATA_SMART_HISTORICAL_DATA_STRUCT_TYPE (dbus_g_type_get_struct ("GValueArray",   \
                                                                       G_TYPE_UINT64, \
                                                                       G_TYPE_BOOLEAN, \
                                                                       G_TYPE_BOOLEAN, \
                                                                       G_TYPE_BOOLEAN, \
                                                                       G_TYPE_BOOLEAN, \
                                                                       G_TYPE_DOUBLE, \
                                                                       G_TYPE_UINT64, \
                                                                       dbus_g_type_get_collection ("GPtrArray", ATA_SMART_ATTRIBUTE_STRUCT_TYPE), \
                                                                       G_TYPE_INVALID))

#define KNOWN_FILESYSTEMS_STRUCT_TYPE (dbus_g_type_get_struct ("GValueArray",   \
                                                               G_TYPE_STRING, \
                                                               G_TYPE_STRING, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_UINT, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_BOOLEAN, \
                                                               G_TYPE_INVALID))

#define PROCESS_STRUCT_TYPE (dbus_g_type_get_struct ("GValueArray",   \
                                                     G_TYPE_UINT,     \
                                                     G_TYPE_UINT,     \
                                                     G_TYPE_STRING,   \
                                                     G_TYPE_INVALID))

DBusGConnection *_mdu_pool_get_connection (MduPool *pool);

MduKnownFilesystem    *_mdu_known_filesystem_new       (gpointer data);

MduProcess            * _mdu_process_new               (gpointer data);

void _mdu_error_fixup (GError *error);

MduDevice  *_mdu_device_new_from_object_path  (MduPool     *pool, const char  *object_path);

MduVolume   *_mdu_volume_new_from_device      (MduPool *pool, MduDevice *volume, MduPresentable *enclosing_presentable);
MduDrive    *_mdu_drive_new_from_device       (MduPool *pool, MduDevice *drive, MduPresentable *enclosing_presentable);
MduVolumeHole   *_mdu_volume_hole_new       (MduPool *pool, guint64 offset, guint64 size, MduPresentable *enclosing_presentable);


MduLinuxMdDrive   *_mdu_linux_md_drive_new             (MduPool              *pool,
                                                        const gchar          *uuid,
                                                        const gchar          *device_file,
                                                        MduPresentable *enclosing_presentable);

gboolean _mdu_linux_md_drive_has_uuid (MduLinuxMdDrive  *drive,
                                       const gchar      *uuid);


gboolean    _mdu_device_changed               (MduDevice   *device);
void        _mdu_device_job_changed           (MduDevice   *device,
                                               gboolean     job_in_progress,
                                               const char  *job_id,
                                               uid_t        job_initiated_by_uid,
                                               gboolean     job_is_cancellable,
                                               double       job_percentage);

MduAdapter *_mdu_adapter_new_from_object_path (MduPool *pool, const char *object_path);
gboolean    _mdu_adapter_changed              (MduAdapter   *adapter);

MduExpander *_mdu_expander_new_from_object_path (MduPool *pool, const char *object_path);
gboolean    _mdu_expander_changed               (MduExpander   *expander);

MduHub     *_mdu_hub_new                        (MduPool        *pool,
                                                 MduHubUsage     usage,
                                                 MduAdapter     *adapter,
                                                 MduExpander    *expander,
                                                 const gchar    *name,
                                                 const gchar    *vpd_name,
                                                 GIcon          *icon,
                                                 MduPresentable *enclosing_presentable);

MduPort    *_mdu_port_new_from_object_path (MduPool *pool, const char *object_path);
gboolean    _mdu_port_changed               (MduPort   *port);

MduMachine *_mdu_machine_new (MduPool *pool);

MduLinuxLvm2VolumeGroup *
_mdu_linux_lvm2_volume_group_new (MduPool        *pool,
                                  const gchar    *vg_uuid,
                                  MduPresentable *enclosing_presentable);

void _mdu_linux_lvm2_volume_group_rewrite_enclosing_presentable (MduLinuxLvm2VolumeGroup *vg);

MduLinuxLvm2Volume *_mdu_linux_lvm2_volume_new (MduPool        *pool,
                                                const gchar    *group_uuid,
                                                const gchar    *uuid,
                                                MduPresentable *enclosing_presentable);

void _mdu_linux_lvm2_volume_rewrite_enclosing_presentable (MduLinuxLvm2Volume *volume);

MduLinuxLvm2VolumeHole *_mdu_linux_lvm2_volume_hole_new (MduPool        *pool,
                                                         MduPresentable *enclosing_presentable);

void _mdu_linux_lvm2_volume_hole_rewrite_enclosing_presentable (MduLinuxLvm2VolumeHole *volume_hole);


void _mdu_hub_rewrite_enclosing_presentable (MduHub *hub);
void _mdu_drive_rewrite_enclosing_presentable (MduDrive *drive);
void _mdu_linux_md_drive_rewrite_enclosing_presentable (MduLinuxMdDrive *drive);
void _mdu_volume_rewrite_enclosing_presentable (MduVolume *volume);
void _mdu_volume_hole_rewrite_enclosing_presentable (MduVolumeHole *volume_hole);

gchar *_mdu_volume_get_names_and_desc (MduPresentable  *presentable,
                                       gchar          **out_vpd_name,
                                       gchar          **out_desc);

#endif /* __MDU_PRIVATE_H */
