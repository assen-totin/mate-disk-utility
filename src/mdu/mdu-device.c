/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-device.c
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

#include "config.h"
#include <glib/gi18n-lib.h>

#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

#include "mdu-private.h"
#include "mdu-pool.h"
#include "mdu-device.h"
#include "udisks-device-glue.h"

/* --- SUCKY CODE BEGIN --- */

/* This totally sucks; dbus-bindings-tool and dbus-glib should be able
 * to do this for us.
 *
 * TODO: keep in sync with code in tools/udisks in udisks.
 */

typedef struct
{
  char *native_path;

  guint64 device_detection_time;
  guint64 device_media_detection_time;
  gint64 device_major;
  gint64 device_minor;
  char *device_file;
  char *device_file_presentation;
  char **device_file_by_id;
  char **device_file_by_path;
  gboolean device_is_system_internal;
  gboolean device_is_partition;
  gboolean device_is_partition_table;
  gboolean device_is_removable;
  gboolean device_is_media_available;
  gboolean device_is_media_change_detected;
  gboolean device_is_media_change_detection_polling;
  gboolean device_is_media_change_detection_inhibitable;
  gboolean device_is_media_change_detection_inhibited;
  gboolean device_is_read_only;
  gboolean device_is_drive;
  gboolean device_is_optical_disc;
  gboolean device_is_luks;
  gboolean device_is_luks_cleartext;
  gboolean device_is_mounted;
  gboolean device_is_linux_md_component;
  gboolean device_is_linux_md;
  gboolean device_is_linux_lvm2_lv;
  gboolean device_is_linux_lvm2_pv;
  gboolean device_is_linux_dmmp;
  gboolean device_is_linux_dmmp_component;
  gboolean device_is_linux_loop;
  char **device_mount_paths;
  uid_t device_mounted_by_uid;
  gboolean device_presentation_hide;
  gboolean device_presentation_nopolicy;
  char *device_presentation_name;
  char *device_presentation_icon_name;
  guint64 device_size;
  guint64 device_block_size;

  gboolean job_in_progress;
  char *job_id;
  uid_t job_initiated_by_uid;
  gboolean job_is_cancellable;
  double job_percentage;

  char *id_usage;
  char *id_type;
  char *id_version;
  char *id_uuid;
  char *id_label;

  char *partition_slave;
  char *partition_scheme;
  int partition_number;
  char *partition_type;
  char *partition_label;
  char *partition_uuid;
  char **partition_flags;
  guint64 partition_offset;
  guint64 partition_size;
  guint64 partition_alignment_offset;

  char *partition_table_scheme;
  int partition_table_count;

  char *luks_holder;

  char *luks_cleartext_slave;
  uid_t luks_cleartext_unlocked_by_uid;

  char *drive_vendor;
  char *drive_model;
  char *drive_revision;
  char *drive_serial;
  char *drive_wwn;
  char *drive_connection_interface;
  guint64 drive_connection_speed;
  char **drive_media_compatibility;
  char *drive_media;
  gboolean drive_is_media_ejectable;
  gboolean drive_can_detach;
  gboolean drive_can_spindown;
  gboolean drive_is_rotational;
  guint drive_rotation_rate;
  char *drive_write_cache;
  char *drive_adapter;
  char **drive_ports;
  char **drive_similar_devices;

  gboolean optical_disc_is_blank;
  gboolean optical_disc_is_appendable;
  gboolean optical_disc_is_closed;
  guint optical_disc_num_tracks;
  guint optical_disc_num_audio_tracks;
  guint optical_disc_num_sessions;

  gboolean drive_ata_smart_is_available;
  guint64 drive_ata_smart_time_collected;
  gchar *drive_ata_smart_status;
  gchar *drive_ata_smart_blob;
  gsize drive_ata_smart_blob_size;

  char *linux_md_component_level;
  int linux_md_component_position;
  int linux_md_component_num_raid_devices;
  char *linux_md_component_uuid;
  char *linux_md_component_home_host;
  char *linux_md_component_name;
  char *linux_md_component_version;
  char *linux_md_component_holder;
  char **linux_md_component_state;

  char *linux_md_state;
  char *linux_md_level;
  int linux_md_num_raid_devices;
  char *linux_md_uuid;
  char *linux_md_home_host;
  char *linux_md_name;
  char *linux_md_version;
  char **linux_md_slaves;
  gboolean linux_md_is_degraded;
  char *linux_md_sync_action;
  double linux_md_sync_percentage;
  guint64 linux_md_sync_speed;

  gchar *linux_lvm2_lv_name;
  gchar *linux_lvm2_lv_uuid;
  gchar *linux_lvm2_lv_group_name;
  gchar *linux_lvm2_lv_group_uuid;

  gchar *linux_lvm2_pv_uuid;
  guint  linux_lvm2_pv_num_metadata_areas;
  gchar *linux_lvm2_pv_group_name;
  gchar *linux_lvm2_pv_group_uuid;
  guint64 linux_lvm2_pv_group_size;
  guint64 linux_lvm2_pv_group_unallocated_size;
  guint64 linux_lvm2_pv_group_sequence_number;
  guint64 linux_lvm2_pv_group_extent_size;
  char **linux_lvm2_pv_group_physical_volumes;
  char **linux_lvm2_pv_group_logical_volumes;

  gchar *linux_dmmp_component_holder;

  gchar *linux_dmmp_name;
  gchar **linux_dmmp_slaves;
  gchar *linux_dmmp_parameters;

  gchar *linux_loop_filename;

} DeviceProperties;

static void
collect_props (const char *key,
               const GValue *value,
               DeviceProperties *props)
{
  gboolean handled = TRUE;

  if (strcmp (key, "NativePath") == 0)
    props->native_path = g_strdup (g_value_get_string (value));

  else if (strcmp (key, "DeviceDetectionTime") == 0)
    props->device_detection_time = g_value_get_uint64 (value);
  else if (strcmp (key, "DeviceMediaDetectionTime") == 0)
    props->device_media_detection_time = g_value_get_uint64 (value);
  else if (strcmp (key, "DeviceMajor") == 0)
    props->device_major = g_value_get_int64 (value);
  else if (strcmp (key, "DeviceMinor") == 0)
    props->device_minor = g_value_get_int64 (value);
  else if (strcmp (key, "DeviceFile") == 0)
    props->device_file = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DeviceFilePresentation") == 0)
    props->device_file_presentation = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DeviceFileById") == 0)
    props->device_file_by_id = g_strdupv (g_value_get_boxed (value));
  else if (strcmp (key, "DeviceFileByPath") == 0)
    props->device_file_by_path = g_strdupv (g_value_get_boxed (value));
  else if (strcmp (key, "DeviceIsSystemInternal") == 0)
    props->device_is_system_internal = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsPartition") == 0)
    props->device_is_partition = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsPartitionTable") == 0)
    props->device_is_partition_table = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsRemovable") == 0)
    props->device_is_removable = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsMediaAvailable") == 0)
    props->device_is_media_available = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsMediaChangeDetected") == 0)
    props->device_is_media_change_detected = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsMediaChangeDetectionPolling") == 0)
    props->device_is_media_change_detection_polling = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsMediaChangeDetectionInhibitable") == 0)
    props->device_is_media_change_detection_inhibitable = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsMediaChangeDetectionInhibited") == 0)
    props->device_is_media_change_detection_inhibited = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsReadOnly") == 0)
    props->device_is_read_only = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsDrive") == 0)
    props->device_is_drive = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsOpticalDisc") == 0)
    props->device_is_optical_disc = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLuks") == 0)
    props->device_is_luks = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLuksCleartext") == 0)
    props->device_is_luks_cleartext = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLinuxMdComponent") == 0)
    props->device_is_linux_md_component = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLinuxMd") == 0)
    props->device_is_linux_md = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLinuxLvm2LV") == 0)
    props->device_is_linux_lvm2_lv = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLinuxLvm2PV") == 0)
    props->device_is_linux_lvm2_pv = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLinuxDmmp") == 0)
    props->device_is_linux_dmmp = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLinuxDmmpComponent") == 0)
    props->device_is_linux_dmmp_component = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsLinuxLoop") == 0)
    props->device_is_linux_loop = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceIsMounted") == 0)
    props->device_is_mounted = g_value_get_boolean (value);
  else if (strcmp (key, "DeviceMountPaths") == 0)
    props->device_mount_paths = g_strdupv (g_value_get_boxed (value));
  else if (strcmp (key, "DeviceMountedByUid") == 0)
    props->device_mounted_by_uid = g_value_get_uint (value);
  else if (strcmp (key, "DevicePresentationHide") == 0)
    props->device_presentation_hide = g_value_get_boolean (value);
  else if (strcmp (key, "DevicePresentationNopolicy") == 0)
    props->device_presentation_nopolicy = g_value_get_boolean (value);
  else if (strcmp (key, "DevicePresentationName") == 0)
    props->device_presentation_name = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DevicePresentationIconName") == 0)
    props->device_presentation_icon_name = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DeviceSize") == 0)
    props->device_size = g_value_get_uint64 (value);
  else if (strcmp (key, "DeviceBlockSize") == 0)
    props->device_block_size = g_value_get_uint64 (value);

  else if (strcmp (key, "JobInProgress") == 0)
    props->job_in_progress = g_value_get_boolean (value);
  else if (strcmp (key, "JobId") == 0)
    props->job_id = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "JobInitiatedByUid") == 0)
    props->job_initiated_by_uid = g_value_get_uint (value);
  else if (strcmp (key, "JobIsCancellable") == 0)
    props->job_is_cancellable = g_value_get_boolean (value);
  else if (strcmp (key, "JobPercentage") == 0)
    props->job_percentage = g_value_get_double (value);

  else if (strcmp (key, "IdUsage") == 0)
    props->id_usage = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "IdType") == 0)
    props->id_type = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "IdVersion") == 0)
    props->id_version = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "IdUuid") == 0)
    props->id_uuid = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "IdLabel") == 0)
    props->id_label = g_strdup (g_value_get_string (value));

  else if (strcmp (key, "PartitionSlave") == 0)
    props->partition_slave = g_strdup (g_value_get_boxed (value));
  else if (strcmp (key, "PartitionScheme") == 0)
    props->partition_scheme = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "PartitionNumber") == 0)
    props->partition_number = g_value_get_int (value);
  else if (strcmp (key, "PartitionType") == 0)
    props->partition_type = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "PartitionLabel") == 0)
    props->partition_label = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "PartitionUuid") == 0)
    props->partition_uuid = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "PartitionFlags") == 0)
    props->partition_flags = g_strdupv (g_value_get_boxed (value));
  else if (strcmp (key, "PartitionOffset") == 0)
    props->partition_offset = g_value_get_uint64 (value);
  else if (strcmp (key, "PartitionSize") == 0)
    props->partition_size = g_value_get_uint64 (value);
  else if (strcmp (key, "PartitionAlignmentOffset") == 0)
    props->partition_alignment_offset = g_value_get_uint64 (value);

  else if (strcmp (key, "PartitionTableScheme") == 0)
    props->partition_table_scheme = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "PartitionTableCount") == 0)
    props->partition_table_count = g_value_get_int (value);

  else if (strcmp (key, "LuksHolder") == 0)
    props->luks_holder = g_strdup (g_value_get_boxed (value));

  else if (strcmp (key, "LuksCleartextSlave") == 0)
    props->luks_cleartext_slave = g_strdup (g_value_get_boxed (value));
  else if (strcmp (key, "LuksCleartextUnlockedByUid") == 0)
    props->luks_cleartext_unlocked_by_uid = g_value_get_uint (value);

  else if (strcmp (key, "DriveVendor") == 0)
    props->drive_vendor = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveModel") == 0)
    props->drive_model = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveRevision") == 0)
    props->drive_revision = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveSerial") == 0)
    props->drive_serial = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveWwn") == 0)
    props->drive_wwn = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveConnectionInterface") == 0)
    props->drive_connection_interface = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveConnectionSpeed") == 0)
    props->drive_connection_speed = g_value_get_uint64 (value);
  else if (strcmp (key, "DriveMediaCompatibility") == 0)
    props->drive_media_compatibility = g_strdupv (g_value_get_boxed (value));
  else if (strcmp (key, "DriveMedia") == 0)
    props->drive_media = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveIsMediaEjectable") == 0)
    props->drive_is_media_ejectable = g_value_get_boolean (value);
  else if (strcmp (key, "DriveCanDetach") == 0)
    props->drive_can_detach = g_value_get_boolean (value);
  else if (strcmp (key, "DriveCanSpindown") == 0)
    props->drive_can_spindown = g_value_get_boolean (value);
  else if (strcmp (key, "DriveIsRotational") == 0)
    props->drive_is_rotational = g_value_get_boolean (value);
  else if (strcmp (key, "DriveRotationRate") == 0)
    props->drive_rotation_rate = g_value_get_uint (value);
  else if (strcmp (key, "DriveWriteCache") == 0)
    props->drive_write_cache = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveAdapter") == 0)
    props->drive_adapter = g_strdup (g_value_get_boxed (value));
  else if (strcmp (key, "DrivePorts") == 0)
    {
      guint n;
      GPtrArray *object_paths;

      object_paths = g_value_get_boxed (value);

      props->drive_ports = g_new0 (char *, object_paths->len + 1);
      for (n = 0; n < object_paths->len; n++)
        props->drive_ports[n] = g_strdup (object_paths->pdata[n]);
      props->drive_ports[n] = NULL;
    }
  else if (strcmp (key, "DriveSimilarDevices") == 0)
    {
      guint n;
      GPtrArray *object_paths;

      object_paths = g_value_get_boxed (value);

      props->drive_similar_devices = g_new0 (char *, object_paths->len + 1);
      for (n = 0; n < object_paths->len; n++)
        props->drive_similar_devices[n] = g_strdup (object_paths->pdata[n]);
      props->drive_similar_devices[n] = NULL;
    }

  else if (strcmp (key, "OpticalDiscIsBlank") == 0)
    props->optical_disc_is_blank = g_value_get_boolean (value);
  else if (strcmp (key, "OpticalDiscIsAppendable") == 0)
    props->optical_disc_is_appendable = g_value_get_boolean (value);
  else if (strcmp (key, "OpticalDiscIsClosed") == 0)
    props->optical_disc_is_closed = g_value_get_boolean (value);
  else if (strcmp (key, "OpticalDiscNumTracks") == 0)
    props->optical_disc_num_tracks = g_value_get_uint (value);
  else if (strcmp (key, "OpticalDiscNumAudioTracks") == 0)
    props->optical_disc_num_audio_tracks = g_value_get_uint (value);
  else if (strcmp (key, "OpticalDiscNumSessions") == 0)
    props->optical_disc_num_sessions = g_value_get_uint (value);

  else if (strcmp (key, "DriveAtaSmartIsAvailable") == 0)
    props->drive_ata_smart_is_available = g_value_get_boolean (value);
  else if (strcmp (key, "DriveAtaSmartTimeCollected") == 0)
    props->drive_ata_smart_time_collected = g_value_get_uint64 (value);
  else if (strcmp (key, "DriveAtaSmartStatus") == 0)
    props->drive_ata_smart_status = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "DriveAtaSmartBlob") == 0)
    {
      GArray *a = g_value_get_boxed (value);
      g_free (props->drive_ata_smart_blob);
      props->drive_ata_smart_blob = g_memdup (a->data, a->len);
      props->drive_ata_smart_blob_size = a->len;
    }

  else if (strcmp (key, "LinuxMdComponentLevel") == 0)
    props->linux_md_component_level = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdComponentPosition") == 0)
    props->linux_md_component_position = g_value_get_int (value);
  else if (strcmp (key, "LinuxMdComponentNumRaidDevices") == 0)
    props->linux_md_component_num_raid_devices = g_value_get_int (value);
  else if (strcmp (key, "LinuxMdComponentUuid") == 0)
    props->linux_md_component_uuid = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdComponentHomeHost") == 0)
    props->linux_md_component_home_host = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdComponentName") == 0)
    props->linux_md_component_name = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdComponentVersion") == 0)
    props->linux_md_component_version = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdComponentHolder") == 0)
    props->linux_md_component_holder = g_strdup (g_value_get_boxed (value));
  else if (strcmp (key, "LinuxMdComponentState") == 0)
    props->linux_md_component_state = g_strdupv (g_value_get_boxed (value));

  else if (strcmp (key, "LinuxMdState") == 0)
    props->linux_md_state = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdLevel") == 0)
    props->linux_md_level = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdNumRaidDevices") == 0)
    props->linux_md_num_raid_devices = g_value_get_int (value);
  else if (strcmp (key, "LinuxMdUuid") == 0)
    props->linux_md_uuid = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdHomeHost") == 0)
    props->linux_md_home_host = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdName") == 0)
    props->linux_md_name = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdVersion") == 0)
    props->linux_md_version = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdSlaves") == 0)
    {
      guint n;
      GPtrArray *object_paths;

      object_paths = g_value_get_boxed (value);

      props->linux_md_slaves = g_new0 (char *, object_paths->len + 1);
      for (n = 0; n < object_paths->len; n++)
        props->linux_md_slaves[n] = g_strdup (object_paths->pdata[n]);
      props->linux_md_slaves[n] = NULL;
    }
  else if (strcmp (key, "LinuxMdIsDegraded") == 0)
    props->linux_md_is_degraded = g_value_get_boolean (value);
  else if (strcmp (key, "LinuxMdSyncAction") == 0)
    props->linux_md_sync_action = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxMdSyncPercentage") == 0)
    props->linux_md_sync_percentage = g_value_get_double (value);
  else if (strcmp (key, "LinuxMdSyncSpeed") == 0)
    props->linux_md_sync_speed = g_value_get_uint64 (value);

  else if (strcmp (key, "LinuxLvm2LVName") == 0)
    props->linux_lvm2_lv_name = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxLvm2LVUuid") == 0)
    props->linux_lvm2_lv_uuid = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxLvm2LVGroupName") == 0)
    props->linux_lvm2_lv_group_name = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxLvm2LVGroupUuid") == 0)
    props->linux_lvm2_lv_group_uuid = g_strdup (g_value_get_string (value));

  else if (strcmp (key, "LinuxLvm2PVUuid") == 0)
    props->linux_lvm2_pv_uuid = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxLvm2PVNumMetadataAreas") == 0)
    props->linux_lvm2_pv_num_metadata_areas = g_value_get_uint (value);
  else if (strcmp (key, "LinuxLvm2PVGroupName") == 0)
    props->linux_lvm2_pv_group_name = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxLvm2PVGroupUuid") == 0)
    props->linux_lvm2_pv_group_uuid = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxLvm2PVGroupSize") == 0)
    props->linux_lvm2_pv_group_size = g_value_get_uint64 (value);
  else if (strcmp (key, "LinuxLvm2PVGroupUnallocatedSize") == 0)
    props->linux_lvm2_pv_group_unallocated_size = g_value_get_uint64 (value);
  else if (strcmp (key, "LinuxLvm2PVGroupSequenceNumber") == 0)
    props->linux_lvm2_pv_group_sequence_number = g_value_get_uint64 (value);
  else if (strcmp (key, "LinuxLvm2PVGroupExtentSize") == 0)
    props->linux_lvm2_pv_group_extent_size = g_value_get_uint64 (value);
  else if (strcmp (key, "LinuxLvm2PVGroupPhysicalVolumes") == 0)
    props->linux_lvm2_pv_group_physical_volumes = g_strdupv (g_value_get_boxed (value));
  else if (strcmp (key, "LinuxLvm2PVGroupLogicalVolumes") == 0)
    props->linux_lvm2_pv_group_logical_volumes = g_strdupv (g_value_get_boxed (value));

  else if (strcmp (key, "LinuxDmmpComponentHolder") == 0)
    props->linux_dmmp_component_holder = g_strdup (g_value_get_boxed (value));

  else if (strcmp (key, "LinuxDmmpName") == 0)
    props->linux_dmmp_name = g_strdup (g_value_get_string (value));
  else if (strcmp (key, "LinuxDmmpSlaves") == 0)
    {
      guint n;
      GPtrArray *object_paths;

      object_paths = g_value_get_boxed (value);

      props->linux_dmmp_slaves = g_new0 (char *, object_paths->len + 1);
      for (n = 0; n < object_paths->len; n++)
        props->linux_dmmp_slaves[n] = g_strdup (object_paths->pdata[n]);
      props->linux_dmmp_slaves[n] = NULL;
    }
  else if (strcmp (key, "LinuxDmmpParameters") == 0)
    props->linux_dmmp_parameters = g_strdup (g_value_get_string (value));

  else if (strcmp (key, "LinuxLoopFilename") == 0)
    props->linux_loop_filename = g_strdup (g_value_get_string (value));

  else
    handled = FALSE;

  if (!handled)
    g_warning ("unhandled property '%s'", key);
}

static void
device_properties_free (DeviceProperties *props)
{
  g_free (props->native_path);
  g_free (props->device_file);
  g_free (props->device_file_presentation);
  g_strfreev (props->device_file_by_id);
  g_strfreev (props->device_file_by_path);
  g_strfreev (props->device_mount_paths);
  g_free (props->device_presentation_name);
  g_free (props->device_presentation_icon_name);
  g_free (props->job_id);
  g_free (props->id_usage);
  g_free (props->id_type);
  g_free (props->id_version);
  g_free (props->id_uuid);
  g_free (props->id_label);
  g_free (props->partition_slave);
  g_free (props->partition_type);
  g_free (props->partition_label);
  g_free (props->partition_uuid);
  g_strfreev (props->partition_flags);
  g_free (props->partition_table_scheme);
  g_free (props->luks_holder);
  g_free (props->luks_cleartext_slave);
  g_free (props->drive_model);
  g_free (props->drive_vendor);
  g_free (props->drive_revision);
  g_free (props->drive_serial);
  g_free (props->drive_wwn);
  g_free (props->drive_connection_interface);
  g_strfreev (props->drive_media_compatibility);
  g_free (props->drive_media);
  g_free (props->drive_write_cache);
  g_free (props->drive_adapter);
  g_strfreev (props->drive_ports);
  g_strfreev (props->drive_similar_devices);

  g_free (props->drive_ata_smart_status);
  g_free (props->drive_ata_smart_blob);

  g_free (props->linux_md_component_level);
  g_free (props->linux_md_component_uuid);
  g_free (props->linux_md_component_home_host);
  g_free (props->linux_md_component_name);
  g_free (props->linux_md_component_version);
  g_free (props->linux_md_component_holder);
  g_strfreev (props->linux_md_component_state);

  g_free (props->linux_md_state);
  g_free (props->linux_md_level);
  g_free (props->linux_md_uuid);
  g_free (props->linux_md_home_host);
  g_free (props->linux_md_name);
  g_free (props->linux_md_version);
  g_strfreev (props->linux_md_slaves);
  g_free (props->linux_md_sync_action);

  g_free (props->linux_lvm2_lv_name);
  g_free (props->linux_lvm2_lv_uuid);
  g_free (props->linux_lvm2_lv_group_name);
  g_free (props->linux_lvm2_lv_group_uuid);

  g_free (props->linux_lvm2_pv_uuid);
  g_free (props->linux_lvm2_pv_group_name);
  g_free (props->linux_lvm2_pv_group_uuid);
  g_strfreev (props->linux_lvm2_pv_group_physical_volumes);
  g_strfreev (props->linux_lvm2_pv_group_logical_volumes);

  g_free (props->linux_dmmp_component_holder);

  g_free (props->linux_dmmp_name);
  g_strfreev (props->linux_dmmp_slaves);
  g_free (props->linux_dmmp_parameters);

  g_free (props->linux_loop_filename);

  g_free (props);
}

static DeviceProperties *
device_properties_get (DBusGConnection *bus,
                       const char *object_path)
{
  DeviceProperties *props;
  GError *error;
  GHashTable *hash_table;
  DBusGProxy *prop_proxy;
  const char *ifname = "org.freedesktop.UDisks.Device";

  props = g_new0 (DeviceProperties, 1);

  prop_proxy
    = dbus_g_proxy_new_for_name (bus, "org.freedesktop.UDisks", object_path, "org.freedesktop.DBus.Properties");
  error = NULL;
  if (!dbus_g_proxy_call (prop_proxy,
                          "GetAll",
                          &error,
                          G_TYPE_STRING,
                          ifname,
                          G_TYPE_INVALID,
                          dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE),
                          &hash_table,
                          G_TYPE_INVALID))
    {
      g_warning ("Couldn't call GetAll() to get properties for %s: %s", object_path, error->message);
      g_error_free (error);

      device_properties_free (props);
      props = NULL;
      goto out;
    }

  g_hash_table_foreach (hash_table, (GHFunc) collect_props, props);

  g_hash_table_unref (hash_table);

 out:
  g_object_unref (prop_proxy);
  return props;
}

/* --- SUCKY CODE END --- */

struct _MduDevicePrivate
{
        DBusGProxy *proxy;
        MduPool *pool;

        char *object_path;

        DeviceProperties *props;
};

enum {
        JOB_CHANGED,
        CHANGED,
        REMOVED,
        LAST_SIGNAL,
};

static GObjectClass *parent_class = NULL;
static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (MduDevice, mdu_device, G_TYPE_OBJECT);

MduPool *
mdu_device_get_pool (MduDevice *device)
{
        return g_object_ref (device->priv->pool);
}

static void
mdu_device_finalize (MduDevice *device)
{
        g_debug ("##### finalized device %s",
                 device->priv->props != NULL ? device->priv->props->device_file : device->priv->object_path);

        g_free (device->priv->object_path);
        if (device->priv->proxy != NULL)
                g_object_unref (device->priv->proxy);
        if (device->priv->pool != NULL)
                g_object_unref (device->priv->pool);
        if (device->priv->props != NULL)
                device_properties_free (device->priv->props);

        if (G_OBJECT_CLASS (parent_class)->finalize)
                (* G_OBJECT_CLASS (parent_class)->finalize) (G_OBJECT (device));
}

static void
mdu_device_class_init (MduDeviceClass *klass)
{
        GObjectClass *obj_class = (GObjectClass *) klass;

        parent_class = g_type_class_peek_parent (klass);

        obj_class->finalize = (GObjectFinalizeFunc) mdu_device_finalize;

        g_type_class_add_private (klass, sizeof (MduDevicePrivate));

        signals[CHANGED] =
                g_signal_new ("changed",
                              G_TYPE_FROM_CLASS (klass),
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (MduDeviceClass, changed),
                              NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);
        signals[JOB_CHANGED] =
                g_signal_new ("job-changed",
                              G_TYPE_FROM_CLASS (klass),
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (MduDeviceClass, job_changed),
                              NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);
        signals[REMOVED] =
                g_signal_new ("removed",
                              G_TYPE_FROM_CLASS (klass),
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (MduDeviceClass, removed),
                              NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);
}

static void
mdu_device_init (MduDevice *device)
{
        device->priv = G_TYPE_INSTANCE_GET_PRIVATE (device, MDU_TYPE_DEVICE, MduDevicePrivate);
}

static gboolean
update_info (MduDevice *device)
{
        DeviceProperties *new_properties;

        new_properties = device_properties_get (_mdu_pool_get_connection (device->priv->pool),
                                                device->priv->object_path);
        if (new_properties != NULL) {
                if (device->priv->props != NULL)
                        device_properties_free (device->priv->props);
                device->priv->props = new_properties;
                return TRUE;
        } else {
                return FALSE;
        }
}


MduDevice *
_mdu_device_new_from_object_path (MduPool *pool, const char *object_path)
{
        MduDevice *device;

        device = MDU_DEVICE (g_object_new (MDU_TYPE_DEVICE, NULL));
        device->priv->object_path = g_strdup (object_path);
        device->priv->pool = g_object_ref (pool);

	device->priv->proxy = dbus_g_proxy_new_for_name (_mdu_pool_get_connection (device->priv->pool),
                                                         "org.freedesktop.UDisks",
                                                         device->priv->object_path,
                                                         "org.freedesktop.UDisks.Device");
        dbus_g_proxy_set_default_timeout (device->priv->proxy, INT_MAX);
        dbus_g_proxy_add_signal (device->priv->proxy, "Changed", G_TYPE_INVALID);

        /* TODO: connect signals */

        if (!update_info (device))
                goto error;

        g_debug ("_mdu_device_new_from_object_path: %s", device->priv->props->device_file);

        return device;
error:
        g_object_unref (device);
        return NULL;
}

gboolean
_mdu_device_changed (MduDevice *device)
{
        g_debug ("_mdu_device_changed: %s", device->priv->props->device_file);
        if (update_info (device)) {
                g_signal_emit (device, signals[CHANGED], 0);
                return TRUE;
        } else {
                return FALSE;
        }
}

void
_mdu_device_job_changed (MduDevice   *device,
                         gboolean     job_in_progress,
                         const char  *job_id,
                         uid_t        job_initiated_by_uid,
                         gboolean     job_is_cancellable,
                         double       job_percentage)
{
        g_debug ("_mdu_device_job_changed: %s: %s", device->priv->props->device_file, job_id);

        device->priv->props->job_in_progress = job_in_progress;
        g_free (device->priv->props->job_id);
        device->priv->props->job_id = g_strdup (job_id);
        device->priv->props->job_initiated_by_uid = job_initiated_by_uid;
        device->priv->props->job_is_cancellable = job_is_cancellable;
        device->priv->props->job_percentage = job_percentage;

        g_signal_emit (device, signals[JOB_CHANGED], 0);
}

const char *
mdu_device_get_object_path (MduDevice *device)
{
        return device->priv->object_path;
}

/* ---------------------------------------------------------------------------------------------------- */

/* TODO:
 *
 *  - for now this is a local method - we might want it to be a D-Bus property instead
 *  - we might want to convey the reason why the device should be ignored (see current call-sites)
 */
gboolean
mdu_device_should_ignore (MduDevice *device)
{
        gboolean ret;

        ret = FALSE;

        if (device->priv->props->device_is_drive) {
                if (device->priv->props->device_is_linux_dmmp_component) {
                        ret = TRUE;
                } else if (g_strv_length (device->priv->props->drive_similar_devices) > 0 &&
                           !device->priv->props->device_is_linux_dmmp) {
                        ret = TRUE;
                }
        } else if (device->priv->props->device_is_partition) {
                MduDevice *drive_device;
                drive_device = mdu_pool_get_by_object_path (device->priv->pool, device->priv->props->partition_slave);
                if (drive_device != NULL) {
                        ret = mdu_device_should_ignore (drive_device);
                        g_object_unref (drive_device);
                }
        }

        return ret;
}

/* ---------------------------------------------------------------------------------------------------- */


/**
 * mdu_device_find_parent:
 * @device: the device
 *
 * Finds a parent device for the given @device. Note that this is only
 * useful for presentation purposes; the device tree may be a lot more
 * complex.
 *
 * Returns: The parent of @device if one could be found, otherwise
 * #NULL. Caller must unref this object using g_object_unref().
 **/
MduDevice *
mdu_device_find_parent (MduDevice *device)
{
        MduDevice *parent;

        parent = NULL;

        /* partitioning relationship */
        if (device->priv->props->device_is_partition &&
            device->priv->props->partition_slave != NULL &&
            strlen (device->priv->props->partition_slave) > 0) {
                parent = mdu_pool_get_by_object_path (device->priv->pool,
                                                      device->priv->props->partition_slave);
        }

        return parent;
}

guint64
mdu_device_get_detection_time (MduDevice *device)
{
        return device->priv->props->device_detection_time;
}

guint64
mdu_device_get_media_detection_time (MduDevice *device)
{
        return device->priv->props->device_media_detection_time;
}

dev_t
mdu_device_get_dev (MduDevice *device)
{
        return makedev (device->priv->props->device_major, device->priv->props->device_minor);
}

const char *
mdu_device_get_device_file (MduDevice *device)
{
        return device->priv->props->device_file;
}

const char *
mdu_device_get_device_file_presentation (MduDevice *device)
{
        return device->priv->props->device_file_presentation;
}

guint64
mdu_device_get_size (MduDevice *device)
{
        return device->priv->props->device_size;
}

guint64
mdu_device_get_block_size (MduDevice *device)
{
        return device->priv->props->device_block_size;
}

gboolean
mdu_device_is_removable (MduDevice *device)
{
        return device->priv->props->device_is_removable;
}

gboolean
mdu_device_is_media_available (MduDevice *device)
{
        return device->priv->props->device_is_media_available;
}

gboolean
mdu_device_is_media_change_detected (MduDevice *device)
{
        return device->priv->props->device_is_media_change_detected;
}

gboolean
mdu_device_is_media_change_detection_polling (MduDevice *device)
{
        return device->priv->props->device_is_media_change_detection_polling;
}

gboolean
mdu_device_is_media_change_detection_inhibitable (MduDevice *device)
{
        return device->priv->props->device_is_media_change_detection_inhibitable;
}

gboolean
mdu_device_is_media_change_detection_inhibited (MduDevice *device)
{
        return device->priv->props->device_is_media_change_detection_inhibited;
}

gboolean
mdu_device_is_read_only (MduDevice *device)
{
        return device->priv->props->device_is_read_only;
}

gboolean
mdu_device_is_system_internal (MduDevice *device)
{
        return device->priv->props->device_is_system_internal;
}

gboolean
mdu_device_is_partition (MduDevice *device)
{
        return device->priv->props->device_is_partition;
}

gboolean
mdu_device_is_partition_table (MduDevice *device)
{
        return device->priv->props->device_is_partition_table;
}

gboolean
mdu_device_is_luks (MduDevice *device)
{
        return device->priv->props->device_is_luks;
}

gboolean
mdu_device_is_luks_cleartext (MduDevice *device)
{
        return device->priv->props->device_is_luks_cleartext;
}

gboolean
mdu_device_is_linux_md_component (MduDevice *device)
{
        return device->priv->props->device_is_linux_md_component;
}

gboolean
mdu_device_is_linux_md (MduDevice *device)
{
        return device->priv->props->device_is_linux_md;
}

gboolean
mdu_device_is_linux_lvm2_lv (MduDevice *device)
{
        return device->priv->props->device_is_linux_lvm2_lv;
}

gboolean
mdu_device_is_linux_lvm2_pv (MduDevice *device)
{
        return device->priv->props->device_is_linux_lvm2_pv;
}

gboolean
mdu_device_is_linux_dmmp (MduDevice *device)
{
        return device->priv->props->device_is_linux_dmmp;
}

gboolean
mdu_device_is_linux_dmmp_component (MduDevice *device)
{
        return device->priv->props->device_is_linux_dmmp_component;
}

gboolean
mdu_device_is_linux_loop (MduDevice *device)
{
        return device->priv->props->device_is_linux_loop;
}

gboolean
mdu_device_is_mounted (MduDevice *device)
{
        return device->priv->props->device_is_mounted;
}

/* keep this around for a while to avoid breaking ABI */
const char *
mdu_device_get_mount_path (MduDevice *device)
{
        if (device->priv->props->device_mount_paths == NULL || device->priv->props->device_mount_paths[0] == NULL)
                return NULL;
        return (const char *) device->priv->props->device_mount_paths[0];
}

char **
mdu_device_get_mount_paths (MduDevice *device)
{
        return device->priv->props->device_mount_paths;
}

gboolean
mdu_device_get_presentation_hide (MduDevice *device)
{
        return device->priv->props->device_presentation_hide;
}

gboolean
mdu_device_get_presentation_nopolicy (MduDevice *device)
{
        return device->priv->props->device_presentation_nopolicy;
}

const char *
mdu_device_get_presentation_name (MduDevice *device)
{
        return device->priv->props->device_presentation_name;
}

const char *
mdu_device_get_presentation_icon_name (MduDevice *device)
{
        return device->priv->props->device_presentation_icon_name;
}

uid_t
mdu_device_get_mounted_by_uid (MduDevice *device)
{
        return device->priv->props->device_mounted_by_uid;
}


const char *
mdu_device_id_get_usage (MduDevice *device)
{
        return device->priv->props->id_usage;
}

const char *
mdu_device_id_get_type (MduDevice *device)
{
        return device->priv->props->id_type;
}

const char *
mdu_device_id_get_version (MduDevice *device)
{
        return device->priv->props->id_version;
}

const char *
mdu_device_id_get_label (MduDevice *device)
{
        return device->priv->props->id_label;
}

const char *
mdu_device_id_get_uuid (MduDevice *device)
{
        return device->priv->props->id_uuid;
}



const char *
mdu_device_partition_get_slave (MduDevice *device)
{
        return device->priv->props->partition_slave;
}

const char *
mdu_device_partition_get_scheme (MduDevice *device)
{
        return device->priv->props->partition_scheme;
}

const char *
mdu_device_partition_get_type (MduDevice *device)
{
        return device->priv->props->partition_type;
}

const char *
mdu_device_partition_get_label (MduDevice *device)
{
        return device->priv->props->partition_label;
}

const char *
mdu_device_partition_get_uuid (MduDevice *device)
{
        return device->priv->props->partition_uuid;
}

char **
mdu_device_partition_get_flags (MduDevice *device)
{
        return device->priv->props->partition_flags;
}

int
mdu_device_partition_get_number (MduDevice *device)
{
        return device->priv->props->partition_number;
}

guint64
mdu_device_partition_get_offset (MduDevice *device)
{
        return device->priv->props->partition_offset;
}

guint64
mdu_device_partition_get_size (MduDevice *device)
{
        return device->priv->props->partition_size;
}

guint64
mdu_device_partition_get_alignment_offset (MduDevice *device)
{
        return device->priv->props->partition_alignment_offset;
}


const char *
mdu_device_partition_table_get_scheme (MduDevice *device)
{
        return device->priv->props->partition_table_scheme;
}

int
mdu_device_partition_table_get_count (MduDevice *device)
{
        return device->priv->props->partition_table_count;
}

const char *
mdu_device_luks_get_holder (MduDevice *device)
{
        return device->priv->props->luks_holder;
}

const char *
mdu_device_luks_cleartext_get_slave (MduDevice *device)
{
        return device->priv->props->luks_cleartext_slave;
}

uid_t
mdu_device_luks_cleartext_unlocked_by_uid (MduDevice *device)
{
        return device->priv->props->luks_cleartext_unlocked_by_uid;
}


gboolean
mdu_device_is_drive (MduDevice *device)
{
        return device->priv->props->device_is_drive;
}

gboolean
mdu_device_is_optical_disc (MduDevice *device)
{
        return device->priv->props->device_is_optical_disc;
}

const char *
mdu_device_drive_get_vendor (MduDevice *device)
{
        return device->priv->props->drive_vendor;
}

const char *
mdu_device_drive_get_model (MduDevice *device)
{
        return device->priv->props->drive_model;
}

const char *
mdu_device_drive_get_revision (MduDevice *device)
{
        return device->priv->props->drive_revision;
}

const char *
mdu_device_drive_get_serial (MduDevice *device)
{
        return device->priv->props->drive_serial;
}

const char *
mdu_device_drive_get_wwn (MduDevice *device)
{
        return device->priv->props->drive_wwn;
}

const char *
mdu_device_drive_get_connection_interface (MduDevice *device)
{
        return device->priv->props->drive_connection_interface;
}

guint64
mdu_device_drive_get_connection_speed (MduDevice *device)
{
        return device->priv->props->drive_connection_speed;
}

char **
mdu_device_drive_get_media_compatibility (MduDevice *device)
{
        return device->priv->props->drive_media_compatibility;
}

const char *
mdu_device_drive_get_media (MduDevice *device)
{
        return device->priv->props->drive_media;
}

const char *
mdu_device_drive_get_write_cache (MduDevice *device)
{
        return device->priv->props->drive_write_cache;
}

const char *
mdu_device_drive_get_adapter (MduDevice *device)
{
        return device->priv->props->drive_adapter;
}

char **
mdu_device_drive_get_ports (MduDevice *device)
{
        return device->priv->props->drive_ports;
}

char **
mdu_device_drive_get_similar_devices (MduDevice *device)
{
        return device->priv->props->drive_similar_devices;
}

gboolean
mdu_device_drive_get_is_media_ejectable (MduDevice *device)
{
        return device->priv->props->drive_is_media_ejectable;
}

gboolean
mdu_device_drive_get_requires_eject (MduDevice *device)
{
        /* TODO: remove once GVfs isn't using this anymore */
        return FALSE;
}

gboolean
mdu_device_drive_get_can_detach (MduDevice *device)
{
        return device->priv->props->drive_can_detach;
}

gboolean
mdu_device_drive_get_can_spindown (MduDevice *device)
{
        return device->priv->props->drive_can_spindown;
}

gboolean
mdu_device_drive_get_is_rotational (MduDevice *device)
{
        return device->priv->props->drive_is_rotational;
}

guint
mdu_device_drive_get_rotation_rate (MduDevice *device)
{
        return device->priv->props->drive_rotation_rate;
}

gboolean
mdu_device_optical_disc_get_is_blank (MduDevice *device)
{
        return device->priv->props->optical_disc_is_blank;
}

gboolean
mdu_device_optical_disc_get_is_appendable (MduDevice *device)
{
        return device->priv->props->optical_disc_is_appendable;
}

gboolean
mdu_device_optical_disc_get_is_closed (MduDevice *device)
{
        return device->priv->props->optical_disc_is_closed;
}

guint
mdu_device_optical_disc_get_num_tracks (MduDevice *device)
{
        return device->priv->props->optical_disc_num_tracks;
}

guint
mdu_device_optical_disc_get_num_audio_tracks (MduDevice *device)
{
        return device->priv->props->optical_disc_num_audio_tracks;
}

guint
mdu_device_optical_disc_get_num_sessions (MduDevice *device)
{
        return device->priv->props->optical_disc_num_sessions;
}

const char *
mdu_device_linux_md_component_get_level (MduDevice *device)
{
        return device->priv->props->linux_md_component_level;
}

int
mdu_device_linux_md_component_get_position (MduDevice *device)
{
        return device->priv->props->linux_md_component_position;
}

int
mdu_device_linux_md_component_get_num_raid_devices (MduDevice *device)
{
        return device->priv->props->linux_md_component_num_raid_devices;
}

const char *
mdu_device_linux_md_component_get_uuid (MduDevice *device)
{
        return device->priv->props->linux_md_component_uuid;
}

const char *
mdu_device_linux_md_component_get_home_host (MduDevice *device)
{
        return device->priv->props->linux_md_component_home_host;
}

const char *
mdu_device_linux_md_component_get_name (MduDevice *device)
{
        return device->priv->props->linux_md_component_name;
}

const char *
mdu_device_linux_md_component_get_version (MduDevice *device)
{
        return device->priv->props->linux_md_component_version;
}

const char *
mdu_device_linux_md_component_get_holder (MduDevice *device)
{
        return device->priv->props->linux_md_component_holder;
}

char **
mdu_device_linux_md_component_get_state (MduDevice *device)
{
        return device->priv->props->linux_md_component_state;
}

const char *
mdu_device_linux_md_get_state (MduDevice *device)
{
        return device->priv->props->linux_md_state;
}

const char *
mdu_device_linux_md_get_level (MduDevice *device)
{
        return device->priv->props->linux_md_level;
}

int
mdu_device_linux_md_get_num_raid_devices (MduDevice *device)
{
        return device->priv->props->linux_md_num_raid_devices;
}

const char *
mdu_device_linux_md_get_uuid (MduDevice *device)
{
        return device->priv->props->linux_md_uuid;
}

const char *
mdu_device_linux_md_get_home_host (MduDevice *device)
{
        return device->priv->props->linux_md_home_host;
}

const char *
mdu_device_linux_md_get_name (MduDevice *device)
{
        return device->priv->props->linux_md_name;
}

const char *
mdu_device_linux_md_get_version (MduDevice *device)
{
        return device->priv->props->linux_md_version;
}

char **
mdu_device_linux_md_get_slaves (MduDevice *device)
{
        return device->priv->props->linux_md_slaves;
}

gboolean
mdu_device_linux_md_is_degraded (MduDevice *device)
{
        return device->priv->props->linux_md_is_degraded;
}

const char *
mdu_device_linux_md_get_sync_action (MduDevice *device)
{
        return device->priv->props->linux_md_sync_action;
}

double
mdu_device_linux_md_get_sync_percentage (MduDevice *device)
{
        return device->priv->props->linux_md_sync_percentage;
}

guint64
mdu_device_linux_md_get_sync_speed (MduDevice *device)
{
        return device->priv->props->linux_md_sync_speed;
}

const char *
mdu_device_linux_lvm2_lv_get_name (MduDevice *device)
{
        return device->priv->props->linux_lvm2_lv_name;
}

const char *
mdu_device_linux_lvm2_lv_get_uuid (MduDevice *device)
{
        return device->priv->props->linux_lvm2_lv_uuid;
}

const char *
mdu_device_linux_lvm2_lv_get_group_name (MduDevice *device)
{
        return device->priv->props->linux_lvm2_lv_group_name;
}

const char *
mdu_device_linux_lvm2_lv_get_group_uuid (MduDevice *device)
{
        return device->priv->props->linux_lvm2_lv_group_uuid;
}


const char *
mdu_device_linux_lvm2_pv_get_uuid (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_uuid;
}

guint
mdu_device_linux_lvm2_pv_get_num_metadata_areas (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_num_metadata_areas;
}

const char *
mdu_device_linux_lvm2_pv_get_group_name (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_group_name;
}

const char *
mdu_device_linux_lvm2_pv_get_group_uuid (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_group_uuid;
}

guint64
mdu_device_linux_lvm2_pv_get_group_size (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_group_size;
}

guint64
mdu_device_linux_lvm2_pv_get_group_unallocated_size (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_group_unallocated_size;
}

guint64
mdu_device_linux_lvm2_pv_get_group_extent_size (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_group_extent_size;
}

guint64
mdu_device_linux_lvm2_pv_get_group_sequence_number (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_group_sequence_number;
}

gchar **
mdu_device_linux_lvm2_pv_get_group_physical_volumes (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_group_physical_volumes;
}

gchar **
mdu_device_linux_lvm2_pv_get_group_logical_volumes (MduDevice *device)
{
        return device->priv->props->linux_lvm2_pv_group_logical_volumes;
}

/* ---------------------------------------------------------------------------------------------------- */

const char *
mdu_device_linux_dmmp_component_get_holder (MduDevice *device)
{
        return device->priv->props->linux_dmmp_component_holder;
}

const char *
mdu_device_linux_dmmp_get_name (MduDevice *device)
{
        return device->priv->props->linux_dmmp_name;
}

char **
mdu_device_linux_dmmp_get_slaves (MduDevice *device)
{
        return device->priv->props->linux_dmmp_slaves;
}

const char *
mdu_device_linux_dmmp_get_parameters (MduDevice *device)
{
        return device->priv->props->linux_dmmp_parameters;
}

const char *
mdu_device_linux_loop_get_filename (MduDevice *device)
{
        return device->priv->props->linux_loop_filename;
}

/* ---------------------------------------------------------------------------------------------------- */

gboolean
mdu_device_drive_ata_smart_get_is_available (MduDevice *device)
{
        return device->priv->props->drive_ata_smart_is_available;
}

guint64
mdu_device_drive_ata_smart_get_time_collected (MduDevice *device)
{
        return device->priv->props->drive_ata_smart_time_collected;
}

const gchar *
mdu_device_drive_ata_smart_get_status (MduDevice *device)
{
        return device->priv->props->drive_ata_smart_status;
}

gconstpointer
mdu_device_drive_ata_smart_get_blob (MduDevice *device, gsize *out_size)
{
        gconstpointer ret;

        ret = device->priv->props->drive_ata_smart_blob;
        if (out_size != NULL)
                *out_size = device->priv->props->drive_ata_smart_blob_size;

        return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

gboolean
mdu_device_job_in_progress (MduDevice *device)
{
        return device->priv->props->job_in_progress;
}

const char *
mdu_device_job_get_id (MduDevice *device)
{
        return device->priv->props->job_id;
}

uid_t
mdu_device_job_get_initiated_by_uid (MduDevice *device)
{
        return device->priv->props->job_initiated_by_uid;
}

gboolean
mdu_device_job_is_cancellable (MduDevice *device)
{
        return device->priv->props->job_is_cancellable;
}

double
mdu_device_job_get_percentage (MduDevice *device)
{
        return device->priv->props->job_percentage;
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceFilesystemCreateCompletedFunc callback;
        gpointer user_data;
} FilesystemCreateData;

static void
op_mkfs_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        FilesystemCreateData *data = user_data;
        _mdu_error_fixup (error);
        data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_filesystem_create (MduDevice                              *device,
                                 const char                             *fstype,
                                 const char                             *fslabel,
                                 const char                             *encrypt_passphrase,
                                 gboolean                                fs_take_ownership,
                                 MduDeviceFilesystemCreateCompletedFunc  callback,
                                 gpointer                                user_data)
{
        int n;
        FilesystemCreateData *data;
        char *options[16];

        data = g_new0 (FilesystemCreateData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        n = 0;
        if (fslabel != NULL && strlen (fslabel) > 0) {
                options[n++] = g_strdup_printf ("label=%s", fslabel);
        }
        if (encrypt_passphrase != NULL && strlen (encrypt_passphrase) > 0) {
                options[n++] = g_strdup_printf ("luks_encrypt=%s", encrypt_passphrase);
        }
        if (fs_take_ownership) {
                options[n++] = g_strdup_printf ("take_ownership_uid=%d", getuid ());
                options[n++] = g_strdup_printf ("take_ownership_gid=%d", getgid ());
        }
        options[n] = NULL;

        org_freedesktop_UDisks_Device_filesystem_create_async (device->priv->proxy,
                                                               fstype,
                                                               (const char **) options,
                                                               op_mkfs_cb,
                                                               data);
        while (n >= 0)
                g_free (options[n--]);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceFilesystemMountCompletedFunc callback;
        gpointer user_data;
} FilesystemMountData;

static void
op_mount_cb (DBusGProxy *proxy, char *mount_path, GError *error, gpointer user_data)
{
        FilesystemMountData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, mount_path, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_filesystem_mount (MduDevice                   *device,
                                gchar                      **options,
                                MduDeviceFilesystemMountCompletedFunc  callback,
                                gpointer                     user_data)
{
        const char *fstype;
        gchar *null_options[16];
        FilesystemMountData *data;

        data = g_new0 (FilesystemMountData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        fstype = NULL;

        null_options[0] = NULL;
        if (options == NULL)
                options = null_options;

        org_freedesktop_UDisks_Device_filesystem_mount_async (device->priv->proxy,
                                                              fstype,
                                                              (const char **) options,
                                                              op_mount_cb,
                                                              data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceFilesystemUnmountCompletedFunc callback;
        gpointer user_data;
} FilesystemUnmountData;

static void
op_unmount_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        FilesystemUnmountData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_filesystem_unmount (MduDevice                     *device,
                                  MduDeviceFilesystemUnmountCompletedFunc  callback,
                                  gpointer                       user_data)
{
        char *options[16];
        FilesystemUnmountData *data;

        data = g_new0 (FilesystemUnmountData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;
        options[0] = NULL;

        org_freedesktop_UDisks_Device_filesystem_unmount_async (device->priv->proxy,
                                                                (const char **) options,
                                                                op_unmount_cb,
                                                                data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceFilesystemCheckCompletedFunc callback;
        gpointer user_data;
} FilesystemCheckData;

static void
op_check_cb (DBusGProxy *proxy, gboolean is_clean, GError *error, gpointer user_data)
{
        FilesystemCheckData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, is_clean, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_filesystem_check (MduDevice                             *device,
                                MduDeviceFilesystemCheckCompletedFunc  callback,
                                gpointer                               user_data)
{
        char *options[16];
        FilesystemCheckData *data;

        data = g_new0 (FilesystemCheckData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;
        options[0] = NULL;

        org_freedesktop_UDisks_Device_filesystem_check_async (device->priv->proxy,
                                                              (const char **) options,
                                                              op_check_cb,
                                                              data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDevicePartitionDeleteCompletedFunc callback;
        gpointer user_data;
} PartitionDeleteData;

static void
op_partition_delete_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        PartitionDeleteData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_partition_delete (MduDevice                             *device,
                                MduDevicePartitionDeleteCompletedFunc  callback,
                                gpointer                               user_data)
{
        int n;
        char *options[16];
        PartitionDeleteData *data;

        data = g_new0 (PartitionDeleteData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        n = 0;
        options[n] = NULL;

        org_freedesktop_UDisks_Device_partition_delete_async (device->priv->proxy,
                                                              (const char **) options,
                                                              op_partition_delete_cb,
                                                              data);

        while (n >= 0)
                g_free (options[n--]);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDevicePartitionCreateCompletedFunc callback;
        gpointer user_data;
} PartitionCreateData;

static void
op_create_partition_cb (DBusGProxy *proxy, char *created_device_object_path, GError *error, gpointer user_data)
{
        PartitionCreateData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, created_device_object_path, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_partition_create (MduDevice   *device,
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
                                gpointer user_data)
{
        int n;
        char *fsoptions[16];
        char *options[16];
        PartitionCreateData *data;

        data = g_new0 (PartitionCreateData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        options[0] = NULL;

        n = 0;
        if (fslabel != NULL && strlen (fslabel) > 0) {
                fsoptions[n++] = g_strdup_printf ("label=%s", fslabel);
        }
        if (encrypt_passphrase != NULL && strlen (encrypt_passphrase) > 0) {
                fsoptions[n++] = g_strdup_printf ("luks_encrypt=%s", encrypt_passphrase);
        }
        if (fs_take_ownership) {
                fsoptions[n++] = g_strdup_printf ("take_ownership_uid=%d", getuid ());
                fsoptions[n++] = g_strdup_printf ("take_ownership_gid=%d", getgid ());
        }
        fsoptions[n] = NULL;

        org_freedesktop_UDisks_Device_partition_create_async (device->priv->proxy,
                                                              offset,
                                                              size,
                                                              type,
                                                              label,
                                                              (const char **) flags,
                                                              (const char **) options,
                                                              fstype,
                                                              (const char **) fsoptions,
                                                              op_create_partition_cb,
                                                              data);

        while (n >= 0)
                g_free (fsoptions[n--]);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDevicePartitionModifyCompletedFunc callback;
        gpointer user_data;
} PartitionModifyData;

static void
op_partition_modify_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        PartitionModifyData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_partition_modify (MduDevice                             *device,
                                const char                            *type,
                                const char                            *label,
                                char                                 **flags,
                                MduDevicePartitionModifyCompletedFunc  callback,
                                gpointer                               user_data)
{
        PartitionModifyData *data;

        data = g_new0 (PartitionModifyData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_partition_modify_async (device->priv->proxy,
                                                              type,
                                                              label,
                                                              (const char **) flags,
                                                              op_partition_modify_cb,
                                                              data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDevicePartitionTableCreateCompletedFunc callback;
        gpointer user_data;
} CreatePartitionTableData;

static void
op_create_partition_table_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        CreatePartitionTableData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_partition_table_create (MduDevice                                  *device,
                                      const char                                 *scheme,
                                      MduDevicePartitionTableCreateCompletedFunc  callback,
                                      gpointer                                    user_data)
{
        int n;
        char *options[16];
        CreatePartitionTableData *data;

        data = g_new0 (CreatePartitionTableData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        n = 0;
        options[n] = NULL;

        org_freedesktop_UDisks_Device_partition_table_create_async (device->priv->proxy,
                                                                    scheme,
                                                                    (const char **) options,
                                                                    op_create_partition_table_cb,
                                                                    data);

        while (n >= 0)
                g_free (options[n--]);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceLuksUnlockCompletedFunc callback;
        gpointer user_data;
} UnlockData;

static void
op_unlock_luks_cb (DBusGProxy *proxy, char *cleartext_object_path, GError *error, gpointer user_data)
{
        UnlockData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, cleartext_object_path, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_luks_unlock (MduDevice *device,
                                const char *secret,
                                MduDeviceLuksUnlockCompletedFunc callback,
                                gpointer user_data)
{
        UnlockData *data;
        char *options[16];
        options[0] = NULL;

        data = g_new0 (UnlockData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_luks_unlock_async (device->priv->proxy,
                                                         secret,
                                                         (const char **) options,
                                                         op_unlock_luks_cb,
                                                         data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;

        MduDeviceLuksChangePassphraseCompletedFunc callback;
        gpointer user_data;
} ChangeSecretData;

static void
op_change_secret_for_luks_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        ChangeSecretData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_luks_change_passphrase (MduDevice   *device,
                                           const char  *old_secret,
                                           const char  *new_secret,
                                           MduDeviceLuksChangePassphraseCompletedFunc callback,
                                           gpointer user_data)
{
        ChangeSecretData *data;

        data = g_new0 (ChangeSecretData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_luks_change_passphrase_async (device->priv->proxy,
                                                                    old_secret,
                                                                    new_secret,
                                                                    op_change_secret_for_luks_cb,
                                                                    data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceLuksLockCompletedFunc callback;
        gpointer user_data;
} LockLuksData;

static void
op_lock_luks_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        LockLuksData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_luks_lock (MduDevice                           *device,
                              MduDeviceLuksLockCompletedFunc  callback,
                              gpointer                             user_data)
{
        char *options[16];
        LockLuksData *data;

        data = g_new0 (LockLuksData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        options[0] = NULL;
        org_freedesktop_UDisks_Device_luks_lock_async (device->priv->proxy,
                                                       (const char **) options,
                                                       op_lock_luks_cb,
                                                       data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceFilesystemSetLabelCompletedFunc callback;
        gpointer user_data;
} FilesystemSetLabelData;

static void
op_change_filesystem_label_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        FilesystemSetLabelData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_filesystem_set_label (MduDevice                                *device,
                                    const char                               *new_label,
                                    MduDeviceFilesystemSetLabelCompletedFunc  callback,
                                    gpointer                                  user_data)
{
        FilesystemSetLabelData *data;

        data = g_new0 (FilesystemSetLabelData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_filesystem_set_label_async (device->priv->proxy,
                                                                  new_label,
                                                                  op_change_filesystem_label_cb,
                                                                  data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceFilesystemListOpenFilesCompletedFunc callback;
        gpointer user_data;
} FilesystemListOpenFilesData;

static GList *
op_filesystem_list_open_files_compute_ret (GPtrArray *processes)
{
        GList *ret;
        int n;

        ret = NULL;
        for (n = 0; n < (int) processes->len; n++) {
                ret = g_list_prepend (ret, _mdu_process_new (processes->pdata[n]));
        }
        ret = g_list_reverse (ret);
        return ret;
}

static void
op_filesystem_list_open_files_cb (DBusGProxy *proxy, GPtrArray *processes, GError *error, gpointer user_data)
{
        FilesystemListOpenFilesData *data = user_data;
        GList *ret;

        _mdu_error_fixup (error);

        ret = NULL;
        if (processes != NULL && error == NULL)
                ret = op_filesystem_list_open_files_compute_ret (processes);

        if (data->callback == NULL)
                data->callback (data->device, ret, error, data->user_data);

        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_filesystem_list_open_files (MduDevice                                     *device,
                                       MduDeviceFilesystemListOpenFilesCompletedFunc  callback,
                                       gpointer                                       user_data)
{
        FilesystemListOpenFilesData *data;

        data = g_new0 (FilesystemListOpenFilesData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_filesystem_list_open_files_async (device->priv->proxy,
                                                                        op_filesystem_list_open_files_cb,
                                                                        data);
}

GList *
mdu_device_filesystem_list_open_files_sync (MduDevice  *device,
                                            GError    **error)
{
        GList *ret;
        GPtrArray *processes;

        ret = NULL;
        if (!org_freedesktop_UDisks_Device_filesystem_list_open_files (device->priv->proxy,
                                                                       &processes,
                                                                       error))
                goto out;

        ret = op_filesystem_list_open_files_compute_ret (processes);
out:
        return ret;
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceDriveAtaSmartRefreshDataCompletedFunc callback;
        gpointer user_data;
} RetrieveAtaSmartDataData;

static void
op_retrieve_ata_smart_data_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        RetrieveAtaSmartDataData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_drive_ata_smart_refresh_data (MduDevice                                  *device,
                                     MduDeviceDriveAtaSmartRefreshDataCompletedFunc callback,
                                     gpointer                                    user_data)
{
        RetrieveAtaSmartDataData *data;
        char *options[16];

        options[0] = NULL;

        data = g_new0 (RetrieveAtaSmartDataData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_drive_ata_smart_refresh_data_async (device->priv->proxy,
                                                                          (const char **) options,
                                                                          op_retrieve_ata_smart_data_cb,
                                                                          data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceDriveAtaSmartInitiateSelftestCompletedFunc callback;
        gpointer user_data;
} DriveAtaSmartInitiateSelftestData;

static void
op_run_ata_smart_selftest_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        DriveAtaSmartInitiateSelftestData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_drive_ata_smart_initiate_selftest (MduDevice                                        *device,
                                                 const char                                       *test,
                                                 MduDeviceDriveAtaSmartInitiateSelftestCompletedFunc  callback,
                                                 gpointer                                          user_data)
{
        DriveAtaSmartInitiateSelftestData *data;
        gchar *options = {NULL};

        data = g_new0 (DriveAtaSmartInitiateSelftestData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_drive_ata_smart_initiate_selftest_async (device->priv->proxy,
                                                                               test,
                                                                               (const gchar **) options,
                                                                               op_run_ata_smart_selftest_cb,
                                                                               data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceLinuxMdStopCompletedFunc callback;
        gpointer user_data;
} LinuxMdStopData;

static void
op_stop_linux_md_array_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        LinuxMdStopData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_linux_md_stop (MduDevice                         *device,
                             MduDeviceLinuxMdStopCompletedFunc  callback,
                             gpointer                           user_data)
{
        char *options[16];
        LinuxMdStopData *data;

        data = g_new0 (LinuxMdStopData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        options[0] = NULL;

        org_freedesktop_UDisks_Device_linux_md_stop_async (device->priv->proxy,
                                                           (const char **) options,
                                                           op_stop_linux_md_array_cb,
                                                           data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceLinuxMdCheckCompletedFunc callback;
        gpointer user_data;
} LinuxMdCheckData;

static void
op_check_linux_md_array_cb (DBusGProxy *proxy, guint64 num_errors, GError *error, gpointer user_data)
{
        LinuxMdCheckData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, num_errors, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_linux_md_check (MduDevice                           *device,
                              gchar                              **options,
                              MduDeviceLinuxMdCheckCompletedFunc   callback,
                              gpointer                             user_data)
{
        gchar *null_options[16];
        LinuxMdCheckData *data;

        data = g_new0 (LinuxMdCheckData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        null_options[0] = NULL;
        if (options == NULL)
                options = null_options;

        org_freedesktop_UDisks_Device_linux_md_check_async (device->priv->proxy,
                                                            (const char **) options,
                                                            op_check_linux_md_array_cb,
                                                            data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceLinuxMdAddSpareCompletedFunc callback;
        gpointer user_data;
} LinuxMdAddSpareData;

static void
op_add_spare_to_linux_md_array_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        LinuxMdAddSpareData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_linux_md_add_spare (MduDevice                                 *device,
                                  const char                                *component_objpath,
                                  MduDeviceLinuxMdAddSpareCompletedFunc  callback,
                                  gpointer                                   user_data)
{
        char *options[16];
        LinuxMdAddSpareData *data;

        data = g_new0 (LinuxMdAddSpareData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        options[0] = NULL;

        org_freedesktop_UDisks_Device_linux_md_add_spare_async (device->priv->proxy,
                                                                component_objpath,
                                                                (const char **) options,
                                                                op_add_spare_to_linux_md_array_cb,
                                                                data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceLinuxMdExpandCompletedFunc callback;
        gpointer user_data;
} LinuxMdExpandData;

static void
op_expand_to_linux_md_array_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        LinuxMdExpandData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_linux_md_expand (MduDevice                            *device,
                               GPtrArray                            *component_objpaths,
                               MduDeviceLinuxMdExpandCompletedFunc   callback,
                               gpointer                              user_data)
{
        char *options[16];
        LinuxMdExpandData *data;

        data = g_new0 (LinuxMdExpandData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        options[0] = NULL;

        org_freedesktop_UDisks_Device_linux_md_expand_async (device->priv->proxy,
                                                             component_objpaths,
                                                             (const char **) options,
                                                             op_expand_to_linux_md_array_cb,
                                                             data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceLinuxMdRemoveComponentCompletedFunc callback;
        gpointer user_data;
} LinuxMdRemoveComponentData;

static void
op_remove_component_from_linux_md_array_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        LinuxMdRemoveComponentData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_linux_md_remove_component (MduDevice                                    *device,
                                         const char                                   *component_objpath,
                                         MduDeviceLinuxMdRemoveComponentCompletedFunc  callback,
                                         gpointer                                      user_data)
{
        int n;
        char *options[16];
        LinuxMdRemoveComponentData *data;

        data = g_new0 (LinuxMdRemoveComponentData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        n = 0;
        options[n] = NULL;

        org_freedesktop_UDisks_Device_linux_md_remove_component_async (device->priv->proxy,
                                                                       component_objpath,
                                                                       (const char **) options,
                                                                       op_remove_component_from_linux_md_array_cb,
                                                                       data);

        while (n >= 0)
                g_free (options[n--]);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceCancelJobCompletedFunc callback;
        gpointer user_data;
} CancelJobData;

static void
op_cancel_job_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        CancelJobData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_cancel_job (MduDevice *device, MduDeviceCancelJobCompletedFunc callback, gpointer user_data)
{
        CancelJobData *data;

        data = g_new0 (CancelJobData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_job_cancel_async (device->priv->proxy,
                                                        op_cancel_job_cb,
                                                        data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceDriveEjectCompletedFunc callback;
        gpointer user_data;
} DriveEjectData;

static void
op_eject_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        DriveEjectData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_drive_eject (MduDevice                        *device,
                           MduDeviceDriveEjectCompletedFunc  callback,
                           gpointer                          user_data)
{
        char *options[16];
        DriveEjectData *data;

        data = g_new0 (DriveEjectData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;
        options[0] = NULL;

        org_freedesktop_UDisks_Device_drive_eject_async (device->priv->proxy,
                                                         (const char **) options,
                                                         op_eject_cb,
                                                         data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceDriveDetachCompletedFunc callback;
        gpointer user_data;
} DriveDetachData;

static void
op_detach_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        DriveDetachData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_drive_detach (MduDevice                        *device,
                           MduDeviceDriveDetachCompletedFunc  callback,
                           gpointer                           user_data)
{
        char *options[16];
        DriveDetachData *data;

        data = g_new0 (DriveDetachData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;
        options[0] = NULL;

        org_freedesktop_UDisks_Device_drive_detach_async (device->priv->proxy,
                                                          (const char **) options,
                                                          op_detach_cb,
                                                          data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceDrivePollMediaCompletedFunc callback;
        gpointer user_data;
} DrivePollMediaData;

static void
op_poll_media_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        DrivePollMediaData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_drive_poll_media (MduDevice                        *device,
                                MduDeviceDrivePollMediaCompletedFunc  callback,
                                gpointer                          user_data)
{
        DrivePollMediaData *data;

        data = g_new0 (DrivePollMediaData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_drive_poll_media_async (device->priv->proxy,
                                                              op_poll_media_cb,
                                                              data);
}

/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceDriveBenchmarkCompletedFunc callback;
        gpointer user_data;
} DriveBenchmarkData;

static void
op_drive_benchmark_cb (DBusGProxy *proxy,
                       GPtrArray *read_transfer_rate_results,
                       GPtrArray *write_transfer_rate_results,
                       GPtrArray *access_time_results,
                       GError *error,
                       gpointer user_data)
{
        DriveBenchmarkData *data = user_data;
        _mdu_error_fixup (error);

        if (data->callback != NULL) {
                data->callback (data->device,
                                read_transfer_rate_results,
                                write_transfer_rate_results,
                                access_time_results,
                                error,
                                data->user_data);
        }
        g_object_unref (data->device);
        g_free (data);
}

void mdu_device_op_drive_benchmark (MduDevice                             *device,
                                    gboolean                               do_write_benchmark,
                                    const gchar* const *                   options,
                                    MduDeviceDriveBenchmarkCompletedFunc   callback,
                                    gpointer                               user_data)
{
        DriveBenchmarkData *data;

        data = g_new0 (DriveBenchmarkData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        org_freedesktop_UDisks_Device_drive_benchmark_async (device->priv->proxy,
                                                             do_write_benchmark,
                                                             (const gchar **) options,
                                                             op_drive_benchmark_cb,
                                                             data);
}


/* -------------------------------------------------------------------------------- */

typedef struct {
        MduDevice *device;
        MduDeviceLinuxLvm2LVStopCompletedFunc callback;
        gpointer user_data;
} LinuxLvm2LVStopData;

static void
op_stop_linux_lvm2_lv_array_cb (DBusGProxy *proxy, GError *error, gpointer user_data)
{
        LinuxLvm2LVStopData *data = user_data;
        _mdu_error_fixup (error);
        if (data->callback != NULL)
                data->callback (data->device, error, data->user_data);
        g_object_unref (data->device);
        g_free (data);
}

void
mdu_device_op_linux_lvm2_lv_stop (MduDevice                             *device,
                                  MduDeviceLinuxLvm2LVStopCompletedFunc  callback,
                                  gpointer                               user_data)
{
        char *options[16];
        LinuxLvm2LVStopData *data;

        data = g_new0 (LinuxLvm2LVStopData, 1);
        data->device = g_object_ref (device);
        data->callback = callback;
        data->user_data = user_data;

        options[0] = NULL;

        org_freedesktop_UDisks_Device_linux_lvm2_lv_stop_async (device->priv->proxy,
                                                                (const char **) options,
                                                                op_stop_linux_lvm2_lv_array_cb,
                                                                data);
}

/* -------------------------------------------------------------------------------- */
