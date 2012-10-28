/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-callbacks.h
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

#ifndef __MDU_CALLBACKS_H
#define __MDU_CALLBACKS_H

#include <mdu/mdu-types.h>

G_BEGIN_DECLS

/**
 * SECTION:mdu-callbacks
 * @title: Callbacks
 * @short_description: Callback function types
 *
 * Various callback function signatures.
 **/

/* ---------------------------------------------------------------------------------------------------- */
/* MduDevice */

typedef void (*MduDeviceFilesystemMountCompletedFunc) (MduDevice    *device,
                                                       char         *mount_point,
                                                       GError       *error,
                                                       gpointer      user_data);

typedef void (*MduDeviceFilesystemUnmountCompletedFunc) (MduDevice    *device,
                                                         GError       *error,
                                                         gpointer      user_data);

typedef void (*MduDeviceFilesystemCheckCompletedFunc) (MduDevice    *device,
                                                       gboolean      is_clean,
                                                       GError       *error,
                                                       gpointer      user_data);

typedef void (*MduDevicePartitionDeleteCompletedFunc) (MduDevice    *device,
                                                       GError       *error,
                                                       gpointer      user_data);

typedef void (*MduDevicePartitionModifyCompletedFunc) (MduDevice    *device,
                                                       GError       *error,
                                                       gpointer      user_data);

typedef void (*MduDevicePartitionTableCreateCompletedFunc) (MduDevice    *device,
                                                            GError       *error,
                                                            gpointer      user_data);

typedef void (*MduDeviceLuksUnlockCompletedFunc) (MduDevice  *device,
                                                  char       *object_path_of_cleartext_device,
                                                  GError     *error,
                                                  gpointer    user_data);

typedef void (*MduDeviceLuksLockCompletedFunc) (MduDevice    *device,
                                                GError       *error,
                                                gpointer      user_data);

typedef void (*MduDeviceLuksChangePassphraseCompletedFunc) (MduDevice  *device,
                                                            GError     *error,
                                                            gpointer    user_data);

typedef void (*MduDeviceFilesystemSetLabelCompletedFunc) (MduDevice    *device,
                                                          GError       *error,
                                                          gpointer      user_data);

typedef void (*MduDeviceDriveAtaSmartInitiateSelftestCompletedFunc) (MduDevice    *device,
                                                                     GError       *error,
                                                                     gpointer      user_data);

typedef void (*MduDeviceDriveAtaSmartRefreshDataCompletedFunc) (MduDevice  *device,
                                                                GError     *error,
                                                                gpointer    user_data);

typedef void (*MduDeviceLinuxMdStopCompletedFunc) (MduDevice    *device,
                                                   GError       *error,
                                                   gpointer      user_data);

typedef void (*MduDeviceLinuxMdCheckCompletedFunc) (MduDevice    *device,
                                                    guint         num_errors,
                                                    GError       *error,
                                                    gpointer      user_data);

typedef void (*MduDeviceLinuxMdAddSpareCompletedFunc) (MduDevice    *device,
                                                       GError       *error,
                                                       gpointer      user_data);

typedef void (*MduDeviceLinuxMdExpandCompletedFunc) (MduDevice    *device,
                                                     GError       *error,
                                                     gpointer      user_data);

typedef void (*MduDeviceLinuxMdRemoveComponentCompletedFunc) (MduDevice    *device,
                                                              GError       *error,
                                                              gpointer      user_data);

typedef void (*MduDeviceFilesystemCreateCompletedFunc) (MduDevice  *device,
                                                        GError     *error,
                                                        gpointer    user_data);

typedef void (*MduDevicePartitionCreateCompletedFunc) (MduDevice  *device,
                                                       char       *created_device_object_path,
                                                       GError     *error,
                                                       gpointer    user_data);

typedef void (*MduDeviceCancelJobCompletedFunc) (MduDevice  *device,
                                                 GError     *error,
                                                 gpointer    user_data);

typedef void (*MduDeviceFilesystemListOpenFilesCompletedFunc) (MduDevice    *device,
                                                               GList        *processes,
                                                               GError       *error,
                                                               gpointer      user_data);


typedef void (*MduDeviceDriveEjectCompletedFunc) (MduDevice    *device,
                                                  GError       *error,
                                                  gpointer      user_data);

typedef void (*MduDeviceDriveDetachCompletedFunc) (MduDevice    *device,
                                                   GError       *error,
                                                   gpointer      user_data);

typedef void (*MduDeviceDrivePollMediaCompletedFunc) (MduDevice    *device,
                                                      GError       *error,
                                                      gpointer      user_data);

typedef void (*MduDeviceDriveBenchmarkCompletedFunc) (MduDevice    *device,
                                                      GPtrArray    *read_transfer_rate_results,
                                                      GPtrArray    *write_transfer_rate_results,
                                                      GPtrArray    *access_time_results,
                                                      GError       *error,
                                                      gpointer      user_data);

typedef void (*MduDeviceLinuxLvm2LVStopCompletedFunc) (MduDevice  *device,
                                                       GError     *error,
                                                       gpointer    user_data);

/* ---------------------------------------------------------------------------------------------------- */
/* MduPool */

typedef void (*MduPoolLinuxMdStartCompletedFunc) (MduPool    *pool,
                                                  char       *assembled_array_object_path,
                                                  GError     *error,
                                                  gpointer    user_data);

typedef void (*MduPoolLinuxMdCreateCompletedFunc) (MduPool    *pool,
                                                   char       *array_object_path,
                                                   GError     *error,
                                                   gpointer    user_data);

typedef void (*MduPoolLinuxLvm2VGStartCompletedFunc) (MduPool    *pool,
                                                      GError     *error,
                                                      gpointer    user_data);

typedef void (*MduPoolLinuxLvm2VGStopCompletedFunc) (MduPool    *pool,
                                                     GError     *error,
                                                     gpointer    user_data);

typedef void (*MduPoolLinuxLvm2LVStartCompletedFunc) (MduPool    *pool,
                                                      GError     *error,
                                                      gpointer    user_data);

typedef void (*MduPoolLinuxLvm2VGSetNameCompletedFunc) (MduPool    *pool,
                                                        GError     *error,
                                                        gpointer    user_data);

typedef void (*MduPoolLinuxLvm2LVSetNameCompletedFunc) (MduPool    *pool,
                                                        GError     *error,
                                                        gpointer    user_data);

typedef void (*MduPoolLinuxLvm2LVRemoveCompletedFunc) (MduPool    *pool,
                                                       GError     *error,
                                                       gpointer    user_data);

typedef void (*MduPoolLinuxLvm2LVCreateCompletedFunc) (MduPool    *pool,
                                                       char       *create_logical_volume_object_path,
                                                       GError     *error,
                                                       gpointer    user_data);

typedef void (*MduPoolLinuxLvm2VGAddPVCompletedFunc) (MduPool    *pool,
                                                      GError     *error,
                                                      gpointer    user_data);

typedef void (*MduPoolLinuxLvm2VGRemovePVCompletedFunc) (MduPool    *pool,
                                                         GError     *error,
                                                         gpointer    user_data);

/* ---------------------------------------------------------------------------------------------------- */
/* MduDrive */

typedef void (*MduDriveActivateFunc) (MduDrive  *drive,
                                      char      *assembled_drive_object_path,
                                      GError    *error,
                                      gpointer   user_data);

typedef void (*MduDriveDeactivateFunc) (MduDrive  *drive,
                                        GError    *error,
                                        gpointer   user_data);


/* ---------------------------------------------------------------------------------------------------- */

G_END_DECLS

#endif /* __MDU_CALLBACKS_H */
