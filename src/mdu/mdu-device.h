/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-device.h
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

#ifndef __MDU_DEVICE_H
#define __MDU_DEVICE_H

#include <unistd.h>
#include <sys/types.h>

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>

G_BEGIN_DECLS

#define MDU_TYPE_DEVICE           (mdu_device_get_type ())
#define MDU_DEVICE(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_DEVICE, MduDevice))
#define MDU_DEVICE_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_DEVICE,  MduDeviceClass))
#define MDU_IS_DEVICE(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_DEVICE))
#define MDU_IS_DEVICE_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_DEVICE))
#define MDU_DEVICE_GET_CLASS(k)   (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_DEVICE, MduDeviceClass))

typedef struct _MduDeviceClass    MduDeviceClass;
typedef struct _MduDevicePrivate  MduDevicePrivate;

struct _MduDevice
{
        GObject parent;

        /* private */
        MduDevicePrivate *priv;
};

struct _MduDeviceClass
{
        GObjectClass parent_class;

        /* signals */
        void (*changed)     (MduDevice *device);
        void (*job_changed) (MduDevice *device);
        void (*removed)     (MduDevice *device);
};

GType       mdu_device_get_type              (void);
const char *mdu_device_get_object_path       (MduDevice   *device);
MduDevice  *mdu_device_find_parent           (MduDevice   *device);
MduPool    *mdu_device_get_pool              (MduDevice   *device);

dev_t mdu_device_get_dev (MduDevice *device);
guint64 mdu_device_get_detection_time (MduDevice *device);
guint64 mdu_device_get_media_detection_time (MduDevice *device);
const char *mdu_device_get_device_file (MduDevice *device);
const char *mdu_device_get_device_file_presentation (MduDevice *device);
guint64 mdu_device_get_size (MduDevice *device);
guint64 mdu_device_get_block_size (MduDevice *device);
gboolean mdu_device_is_removable (MduDevice *device);
gboolean mdu_device_is_media_available (MduDevice *device);
gboolean mdu_device_is_media_change_detected (MduDevice *device);
gboolean mdu_device_is_media_change_detection_polling (MduDevice *device);
gboolean mdu_device_is_media_change_detection_inhibitable (MduDevice *device);
gboolean mdu_device_is_media_change_detection_inhibited (MduDevice *device);
gboolean mdu_device_is_read_only (MduDevice *device);
gboolean mdu_device_is_system_internal (MduDevice *device);
gboolean mdu_device_is_partition (MduDevice *device);
gboolean mdu_device_is_partition_table (MduDevice *device);
gboolean mdu_device_is_drive (MduDevice *device);
gboolean mdu_device_is_optical_disc (MduDevice *device);
gboolean mdu_device_is_luks (MduDevice *device);
gboolean mdu_device_is_luks_cleartext (MduDevice *device);
gboolean mdu_device_is_linux_md_component (MduDevice *device);
gboolean mdu_device_is_linux_md (MduDevice *device);
gboolean mdu_device_is_linux_lvm2_lv (MduDevice *device);
gboolean mdu_device_is_linux_lvm2_pv (MduDevice *device);
gboolean mdu_device_is_linux_dmmp (MduDevice *device);
gboolean mdu_device_is_linux_dmmp_component (MduDevice *device);
gboolean mdu_device_is_linux_loop (MduDevice *device);
gboolean mdu_device_is_mounted (MduDevice *device);
const char *mdu_device_get_mount_path (MduDevice *device);
char **mdu_device_get_mount_paths (MduDevice *device);
uid_t mdu_device_get_mounted_by_uid (MduDevice *device);
gboolean    mdu_device_get_presentation_hide (MduDevice *device);
gboolean    mdu_device_get_presentation_nopolicy (MduDevice *device);
const char *mdu_device_get_presentation_name (MduDevice *device);
const char *mdu_device_get_presentation_icon_name (MduDevice *device);

gboolean    mdu_device_job_in_progress (MduDevice *device);
const char *mdu_device_job_get_id (MduDevice *device);
uid_t       mdu_device_job_get_initiated_by_uid (MduDevice *device);
gboolean    mdu_device_job_is_cancellable (MduDevice *device);
double      mdu_device_job_get_percentage (MduDevice *device);

const char *mdu_device_id_get_usage (MduDevice *device);
const char *mdu_device_id_get_type (MduDevice *device);
const char *mdu_device_id_get_version (MduDevice *device);
const char *mdu_device_id_get_label (MduDevice *device);
const char *mdu_device_id_get_uuid (MduDevice *device);

const char *mdu_device_partition_get_slave (MduDevice *device);
const char *mdu_device_partition_get_scheme (MduDevice *device);
const char *mdu_device_partition_get_type (MduDevice *device);
const char *mdu_device_partition_get_label (MduDevice *device);
const char *mdu_device_partition_get_uuid (MduDevice *device);
char **mdu_device_partition_get_flags (MduDevice *device);
int mdu_device_partition_get_number (MduDevice *device);
guint64 mdu_device_partition_get_offset (MduDevice *device);
guint64 mdu_device_partition_get_size (MduDevice *device);
guint64 mdu_device_partition_get_alignment_offset (MduDevice *device);

const char *mdu_device_partition_table_get_scheme (MduDevice *device);
int         mdu_device_partition_table_get_count (MduDevice *device);

const char *mdu_device_luks_get_holder (MduDevice *device);

const char *mdu_device_luks_cleartext_get_slave (MduDevice *device);
uid_t mdu_device_luks_cleartext_unlocked_by_uid (MduDevice *device);

const char *mdu_device_drive_get_vendor (MduDevice *device);
const char *mdu_device_drive_get_model (MduDevice *device);
const char *mdu_device_drive_get_revision (MduDevice *device);
const char *mdu_device_drive_get_serial (MduDevice *device);
const char *mdu_device_drive_get_wwn (MduDevice *device);
const char *mdu_device_drive_get_connection_interface (MduDevice *device);
guint64 mdu_device_drive_get_connection_speed (MduDevice *device);
char **mdu_device_drive_get_media_compatibility (MduDevice *device);
const gchar *mdu_device_get_automount_hint(MduDevice *device);
const char *mdu_device_drive_get_media (MduDevice *device);
gboolean mdu_device_drive_get_is_media_ejectable (MduDevice *device);
gboolean mdu_device_drive_get_requires_eject (MduDevice *device);
gboolean mdu_device_drive_get_can_detach (MduDevice *device);
gboolean mdu_device_drive_get_can_spindown (MduDevice *device);
gboolean mdu_device_drive_get_is_rotational (MduDevice *device);
guint    mdu_device_drive_get_rotation_rate (MduDevice *device);
const char *mdu_device_drive_get_write_cache (MduDevice *device);
const char *mdu_device_drive_get_adapter (MduDevice *device);
char **mdu_device_drive_get_ports (MduDevice *device);
char **mdu_device_drive_get_similar_devices (MduDevice *device);

gboolean mdu_device_optical_disc_get_is_blank (MduDevice *device);
gboolean mdu_device_optical_disc_get_is_appendable (MduDevice *device);
gboolean mdu_device_optical_disc_get_is_closed (MduDevice *device);
guint mdu_device_optical_disc_get_num_tracks (MduDevice *device);
guint mdu_device_optical_disc_get_num_audio_tracks (MduDevice *device);
guint mdu_device_optical_disc_get_num_sessions (MduDevice *device);

const char *mdu_device_linux_md_component_get_level (MduDevice *device);
int         mdu_device_linux_md_component_get_position (MduDevice *device);
int         mdu_device_linux_md_component_get_num_raid_devices (MduDevice *device);
const char *mdu_device_linux_md_component_get_uuid (MduDevice *device);
const char *mdu_device_linux_md_component_get_home_host (MduDevice *device);
const char *mdu_device_linux_md_component_get_name (MduDevice *device);
const char *mdu_device_linux_md_component_get_version (MduDevice *device);
const char *mdu_device_linux_md_component_get_holder (MduDevice *device);
char       **mdu_device_linux_md_component_get_state (MduDevice *device);

const char *mdu_device_linux_md_get_state (MduDevice *device);
const char *mdu_device_linux_md_get_level (MduDevice *device);
int         mdu_device_linux_md_get_num_raid_devices (MduDevice *device);
const char *mdu_device_linux_md_get_uuid (MduDevice *device);
const char *mdu_device_linux_md_get_home_host (MduDevice *device);
const char *mdu_device_linux_md_get_name (MduDevice *device);
const char *mdu_device_linux_md_get_version (MduDevice *device);
char      **mdu_device_linux_md_get_slaves (MduDevice *device);
gboolean    mdu_device_linux_md_is_degraded (MduDevice *device);
const char *mdu_device_linux_md_get_sync_action (MduDevice *device);
double      mdu_device_linux_md_get_sync_percentage (MduDevice *device);
guint64     mdu_device_linux_md_get_sync_speed (MduDevice *device);

const char *mdu_device_linux_lvm2_lv_get_name (MduDevice *device);
const char *mdu_device_linux_lvm2_lv_get_uuid (MduDevice *device);
const char *mdu_device_linux_lvm2_lv_get_group_name (MduDevice *device);
const char *mdu_device_linux_lvm2_lv_get_group_uuid (MduDevice *device);

const char *mdu_device_linux_lvm2_pv_get_uuid (MduDevice *device);
guint       mdu_device_linux_lvm2_pv_get_num_metadata_areas (MduDevice *device);
const char *mdu_device_linux_lvm2_pv_get_group_name (MduDevice *device);
const char *mdu_device_linux_lvm2_pv_get_group_uuid (MduDevice *device);
guint64     mdu_device_linux_lvm2_pv_get_group_size (MduDevice *device);
guint64     mdu_device_linux_lvm2_pv_get_group_unallocated_size (MduDevice *device);
guint64     mdu_device_linux_lvm2_pv_get_group_extent_size (MduDevice *device);
guint64     mdu_device_linux_lvm2_pv_get_group_sequence_number (MduDevice *device);
gchar     **mdu_device_linux_lvm2_pv_get_group_physical_volumes (MduDevice *device);
gchar     **mdu_device_linux_lvm2_pv_get_group_logical_volumes (MduDevice *device);

const char *mdu_device_linux_dmmp_component_get_holder (MduDevice *device);
const char *mdu_device_linux_dmmp_get_name (MduDevice *device);
char **mdu_device_linux_dmmp_get_slaves (MduDevice *device);
const char *mdu_device_linux_dmmp_get_parameters (MduDevice *device);

const char *mdu_device_linux_loop_get_filename (MduDevice *device);

gboolean      mdu_device_drive_ata_smart_get_is_available (MduDevice *device);
guint64       mdu_device_drive_ata_smart_get_time_collected (MduDevice *device);
const gchar  *mdu_device_drive_ata_smart_get_status (MduDevice *device);
gconstpointer mdu_device_drive_ata_smart_get_blob (MduDevice *device, gsize *out_size);

/* ---------------------------------------------------------------------------------------------------- */

gboolean mdu_device_should_ignore (MduDevice *device);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_filesystem_mount                   (MduDevice                             *device,
                                                       gchar                                **options,
                                                       MduDeviceFilesystemMountCompletedFunc  callback,
                                                       gpointer                               user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_filesystem_unmount                 (MduDevice                               *device,
                                                       MduDeviceFilesystemUnmountCompletedFunc  callback,
                                                       gpointer                                 user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_filesystem_check                 (MduDevice                             *device,
                                                     MduDeviceFilesystemCheckCompletedFunc  callback,
                                                     gpointer                               user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_partition_delete        (MduDevice                             *device,
                                            MduDevicePartitionDeleteCompletedFunc  callback,
                                            gpointer                               user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_partition_modify        (MduDevice                             *device,
                                            const char                            *type,
                                            const char                            *label,
                                            char                                 **flags,
                                            MduDevicePartitionModifyCompletedFunc  callback,
                                            gpointer                               user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_partition_table_create  (MduDevice                                  *device,
                                            const char                                 *scheme,
                                            MduDevicePartitionTableCreateCompletedFunc  callback,
                                            gpointer                                    user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_luks_unlock       (MduDevice   *device,
                                      const char *secret,
                                      MduDeviceLuksUnlockCompletedFunc callback,
                                      gpointer user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_luks_lock          (MduDevice                           *device,
                                       MduDeviceLuksLockCompletedFunc  callback,
                                       gpointer                             user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_luks_change_passphrase (MduDevice   *device,
                                           const char  *old_secret,
                                           const char  *new_secret,
                                           MduDeviceLuksChangePassphraseCompletedFunc callback,
                                           gpointer user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_filesystem_set_label (MduDevice                                *device,
                                         const char                               *new_label,
                                         MduDeviceFilesystemSetLabelCompletedFunc  callback,
                                         gpointer                                  user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_drive_ata_smart_initiate_selftest      (MduDevice                                        *device,
                                                           const char                                       *test,
                                                           MduDeviceDriveAtaSmartInitiateSelftestCompletedFunc  callback,
                                                           gpointer                                          user_data);

/* ---------------------------------------------------------------------------------------------------- */

void  mdu_device_drive_ata_smart_refresh_data (MduDevice                                  *device,
                                               MduDeviceDriveAtaSmartRefreshDataCompletedFunc callback,
                                               gpointer                                    user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_linux_md_stop     (MduDevice                         *device,
                                      MduDeviceLinuxMdStopCompletedFunc  callback,
                                      gpointer                           user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_linux_md_check    (MduDevice                           *device,
                                      gchar                              **options,
                                      MduDeviceLinuxMdCheckCompletedFunc   callback,
                                      gpointer                             user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_linux_md_add_spare (MduDevice                             *device,
                                       const char                            *component_objpath,
                                       MduDeviceLinuxMdAddSpareCompletedFunc  callback,
                                       gpointer                               user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_linux_md_expand (MduDevice                           *device,
                                    GPtrArray                           *component_objpaths,
                                    MduDeviceLinuxMdExpandCompletedFunc  callback,
                                    gpointer                             user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_linux_md_remove_component (MduDevice                                    *device,
                                              const char                                   *component_objpath,
                                              MduDeviceLinuxMdRemoveComponentCompletedFunc  callback,
                                              gpointer                                      user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_filesystem_create (MduDevice                              *device,
                                      const char                             *fstype,
                                      const char                             *fslabel,
                                      const char                             *encrypt_passphrase,
                                      gboolean                                fs_take_ownership,
                                      MduDeviceFilesystemCreateCompletedFunc  callback,
                                      gpointer                                user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_partition_create       (MduDevice   *device,
                                           guint64      offset,
                                           guint64      size,
                                           const char  *type,
                                           const char  *label,
                                           char       **flags,
                                           const char  *fstype,
                                           const char  *fslabel,
                                           const char  *encrypt_passphrase,
                                           gboolean     fs_take_ownership,
                                           MduDevicePartitionCreateCompletedFunc callback,
                                           gpointer user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_cancel_job (MduDevice *device,
                               MduDeviceCancelJobCompletedFunc callback,
                               gpointer user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_filesystem_list_open_files (MduDevice                                     *device,
                                            MduDeviceFilesystemListOpenFilesCompletedFunc  callback,
                                            gpointer                                       user_data);

GList *mdu_device_filesystem_list_open_files_sync (MduDevice  *device,
                                                   GError    **error);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_drive_eject                 (MduDevice                        *device,
                                                MduDeviceDriveEjectCompletedFunc  callback,
                                                gpointer                          user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_drive_detach                (MduDevice                        *device,
                                                MduDeviceDriveDetachCompletedFunc callback,
                                                gpointer                          user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_drive_poll_media                 (MduDevice                        *device,
                                                     MduDeviceDrivePollMediaCompletedFunc   callback,
                                                     gpointer                          user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_drive_benchmark (MduDevice                             *device,
                                    gboolean                               do_write_benchmark,
                                    const gchar* const *                   options,
                                    MduDeviceDriveBenchmarkCompletedFunc   callback,
                                    gpointer                               user_data);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_device_op_linux_lvm2_lv_stop     (MduDevice                             *device,
                                           MduDeviceLinuxLvm2LVStopCompletedFunc  callback,
                                           gpointer                               user_data);

G_END_DECLS

#endif /* __MDU_DEVICE_H */
