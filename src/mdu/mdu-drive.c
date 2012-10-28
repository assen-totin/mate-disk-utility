/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-drive.c
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

#include "mdu-private.h"
#include "mdu-util.h"
#include "mdu-pool.h"
#include "mdu-drive.h"
#include "mdu-presentable.h"
#include "mdu-device.h"
#include "mdu-error.h"
#include "mdu-volume.h"
#include "mdu-volume-hole.h"

/**
 * SECTION:mdu-drive
 * @title: MduDrive
 * @short_description: Drives
 *
 * The #MduDrive class represents drives attached to the
 * system. Normally, objects of this class corresponds 1:1 to physical
 * drives (hard disks, optical drives, card readers etc.) attached to
 * the system. However, it can also relate to software abstractions
 * such as a Linux md Software RAID array and similar things.
 *
 * See the documentation for #MduPresentable for the big picture.
 */

struct _MduDrivePrivate
{
        MduDevice *device;
        MduPool *pool;
        MduPresentable *enclosing_presentable;
        gchar *id;
};

static GObjectClass *parent_class = NULL;

static void mdu_drive_presentable_iface_init (MduPresentableIface *iface);
G_DEFINE_TYPE_WITH_CODE (MduDrive, mdu_drive, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (MDU_TYPE_PRESENTABLE,
                                                mdu_drive_presentable_iface_init))

static void device_job_changed (MduDevice *device, gpointer user_data);
static void device_changed (MduDevice *device, gpointer user_data);

static gboolean mdu_drive_can_create_volume_real (MduDrive        *drive,
                                                  gboolean        *out_is_uninitialized,
                                                  guint64         *out_largest_contiguous_free_segment,
                                                  guint64         *out_total_free,
                                                  MduPresentable **out_presentable);

static void mdu_drive_create_volume_real (MduDrive              *drive,
                                          guint64                size,
                                          const gchar           *name,
                                          MduCreateVolumeFlags   flags,
                                          GAsyncReadyCallback    callback,
                                          gpointer               user_data);

static MduVolume *mdu_drive_create_volume_finish_real (MduDrive              *drive,
                                                       GAsyncResult          *res,
                                                       GError               **error);

static void
mdu_drive_finalize (MduDrive *drive)
{
        //g_debug ("##### finalized drive '%s' %p", drive->priv->id, drive);

        if (drive->priv->device != NULL) {
                g_signal_handlers_disconnect_by_func (drive->priv->device, device_changed, drive);
                g_signal_handlers_disconnect_by_func (drive->priv->device, device_job_changed, drive);
                g_object_unref (drive->priv->device);
        }

        if (drive->priv->pool != NULL)
                g_object_unref (drive->priv->pool);

        if (drive->priv->enclosing_presentable != NULL)
                g_object_unref (drive->priv->enclosing_presentable);

        g_free (drive->priv->id);

        if (G_OBJECT_CLASS (parent_class)->finalize)
                (* G_OBJECT_CLASS (parent_class)->finalize) (G_OBJECT (drive));
}

static void
mdu_drive_class_init (MduDriveClass *klass)
{
        GObjectClass *obj_class = (GObjectClass *) klass;

        parent_class = g_type_class_peek_parent (klass);

        obj_class->finalize = (GObjectFinalizeFunc) mdu_drive_finalize;

        klass->can_create_volume = mdu_drive_can_create_volume_real;
        klass->create_volume = mdu_drive_create_volume_real;
        klass->create_volume_finish = mdu_drive_create_volume_finish_real;

        g_type_class_add_private (klass, sizeof (MduDrivePrivate));
}

gboolean
mdu_drive_is_active (MduDrive *drive)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        if (klass->is_active != NULL)
                return klass->is_active (drive);
        else
                return TRUE;
}

gboolean
mdu_drive_is_activatable (MduDrive *drive)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        if (klass->is_activatable != NULL)
                return klass->is_activatable (drive);
        else
                return FALSE;
}

/**
 * mdu_drive_can_activate:
 * @drive: A #MduDrive.
 * @out_degraded: %NULL or return location for whether the drive will be degraded if activated.
 *
 * Checks if @drive can be activated. If this function returns %TRUE,
 * @out_degraded will be set to whether the drive will be started in
 * degraded mode (e.g. starting a mirror RAID array with only one
 * component available).
 *
 * Returns: %TRUE if @drive can be activated (and @out_degraded will be set), %FALSE otherwise.
 **/
gboolean
mdu_drive_can_activate (MduDrive *drive,
                        gboolean  *out_degraded)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        if (klass->can_activate != NULL)
                return klass->can_activate (drive, out_degraded);
        else
                return FALSE;
}

gboolean
mdu_drive_can_deactivate (MduDrive  *drive)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        if (klass->can_deactivate != NULL)
                return klass->can_deactivate (drive);
        else
                return FALSE;
}

void
mdu_drive_activate (MduDrive            *drive,
                    MduDriveActivateFunc    callback,
                    gpointer             user_data)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        if (klass->activate != NULL)
                return klass->activate (drive, callback, user_data);
        else {
                callback (drive,
                          NULL,
                          g_error_new_literal (MDU_ERROR,
                                               MDU_ERROR_NOT_SUPPORTED,
                                               "Drive does not support activate()"),
                          user_data);
        }
}

void
mdu_drive_deactivate (MduDrive                *drive,
                      MduDriveDeactivateFunc   callback,
                      gpointer                 user_data)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        if (klass->deactivate != NULL)
                return klass->deactivate (drive, callback, user_data);
        else {
                callback (drive,
                          g_error_new_literal (MDU_ERROR,
                                               MDU_ERROR_NOT_SUPPORTED,
                                               "Drive does not support deactivate()"),
                          user_data);
        }
}

/**
 * mdu_drive_can_create_volume:
 * @drive: A #MduDrive.
 * @out_is_uninitialized: Return location for whether @drive is uninitialized or %NULL.
 * @out_largest_segment: Return location for biggest contigious free block of @drive or %NULL.
 * @out_total_free: Return location for total amount of free space on @drive or %NULL.
 * @out_presentable: Return location for the presentable that represents free space or %NULL. Free
 * with g_object_unref().
 *
 * This method checks if a new volume can be created on @drive.
 *
 * If @drive uses removable media and there is no media inserted,
 * %FALSE is returned.
 *
 * If @drive appears to be completely uninitialized (such as a hard
 * disk full of zeros), @out_is_unitialized is set to %TRUE, the size
 * of the media/disk is returned in @out_largest_segment and %TRUE is
 * returned. Note that this can also happen if @drive contains
 * signatures unknown to the operating system so be careful and always
 * prompt the user.
 *
 * If the disk is partitioned and unallocated space exists but no more
 * partitions can be created (due to e.g. four primary partitions on a
 * MBR partitioned disk), this method returns %FALSE but
 * @out_largest_segment will be set to a non-zero value.
 *
 * Additionally, @out_presentable will be set to either a
 * #MduVolumeHole (if the disk is partitioned and has free space) or
 * the #MduDrive (if the disk is uninitialized).
 *
 * You can use mdu_drive_create_volume() to create a volume.
 *
 * Returns: %TRUE if @drive has unallocated space, %FALSE otherwise.
 */
gboolean
mdu_drive_can_create_volume (MduDrive        *drive,
                             gboolean        *out_is_uninitialized,
                             guint64         *out_largest_contiguous_free_segment,
                             guint64         *out_total_free,
                             MduPresentable **out_presentable)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        return klass->can_create_volume (drive,
                                         out_is_uninitialized,
                                         out_largest_contiguous_free_segment,
                                         out_total_free,
                                         out_presentable);
}

/**
 * mdu_drive_count_mbr_partitions:
 * @drive: A #MduDrive.
 * @out_num_primary_partitions: Return location for number of primary partitions.
 * @out_has_extended_partition: Return location for number of extended partitions.
 *
 * Counts the number of primary partitions and figures out if there's an extended partition.
 *
 * Returns: %TRUE if @out_num_logical_partitions and @out_has_extended_partition is set, %FALSE otherwise.
 */
gboolean
mdu_drive_count_mbr_partitions (MduDrive  *drive,
                                guint     *out_num_primary_partitions,
                                gboolean  *out_has_extended_partition)
{
        guint num_primary_partitions;
        gboolean has_extended_partition;
        gboolean ret;
        MduDevice *device;
        MduPool *pool;
        GList *enclosed_presentables;
        GList *l;

        ret = FALSE;
        num_primary_partitions = 0;
        has_extended_partition = FALSE;
        pool = NULL;
        device = NULL;

        device = mdu_presentable_get_device (MDU_PRESENTABLE (drive));
        if (device == NULL)
                goto out;

        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (drive));

        if (!mdu_device_is_partition_table (device) ||
            g_strcmp0 (mdu_device_partition_table_get_scheme (device), "mbr") != 0) {
                goto out;
        }

        enclosed_presentables = mdu_pool_get_enclosed_presentables (pool, MDU_PRESENTABLE (drive));
        for (l = enclosed_presentables; l != NULL; l = l->next) {
                MduPresentable *ep = MDU_PRESENTABLE (l->data);

                if (MDU_IS_VOLUME (ep)) {
                        gint type;
                        MduDevice *ep_device;

                        ep_device = mdu_presentable_get_device (ep);

                        type = strtol (mdu_device_partition_get_type (ep_device), NULL, 0);
                        if (type == 0x05 || type == 0x0f || type == 0x85) {
                                has_extended_partition = TRUE;
                        }
                        num_primary_partitions++;
                        g_object_unref (ep_device);
                }
        }
        g_list_foreach (enclosed_presentables, (GFunc) g_object_unref, NULL);
        g_list_free (enclosed_presentables);

        ret = TRUE;

 out:
        if (device != NULL)
                g_object_unref (device);
        if (pool != NULL)
                g_object_unref (pool);

        if (out_num_primary_partitions != NULL)
                *out_num_primary_partitions = num_primary_partitions;
        if (out_has_extended_partition != NULL)
                *out_has_extended_partition = has_extended_partition;
        return ret;
}


static gboolean
mdu_drive_can_create_volume_real (MduDrive        *drive,
                                  gboolean        *out_is_unitialized,
                                  guint64         *out_largest_contiguous_free_segment,
                                  guint64         *out_total_free,
                                  MduPresentable **out_presentable)
{
        MduDevice *device;
        MduPool *pool;
        guint64 largest_contiguous_free_segment;
        guint64 total_free;
        gboolean whole_disk_uninitialized;
        GList *enclosed_presentables;
        GList *l;
        gboolean has_extended_partition;
        gboolean ret;
        guint64 size;
        MduPresentable *pres;

        largest_contiguous_free_segment = 0;
        total_free = 0;
        whole_disk_uninitialized = FALSE;
        ret = FALSE;
        device = NULL;
        pool = NULL;
        pres = NULL;

        device = mdu_presentable_get_device (MDU_PRESENTABLE (drive));
        if (device == NULL)
                goto out;

        if (mdu_device_is_read_only (device))
                goto out;

        if (mdu_device_is_removable (device) && !mdu_device_is_media_available (device))
                goto out;

        /* now figure out @whole_disk_is_uninitialized... this is to be set to %TRUE exactly
         * when we _think_ the disk has never been used - we define this to happen when
         *
         *  1. The disk has no partition table; and
         *
         *  2. the whole-disk device is unrecognized
         */
        if (!mdu_device_is_partition_table (device) && strlen (mdu_device_id_get_usage (device)) == 0) {
                whole_disk_uninitialized = TRUE;
                largest_contiguous_free_segment = mdu_device_get_size (device);
                total_free = mdu_device_get_size (device);
                ret = TRUE;
                pres = MDU_PRESENTABLE (drive);
                goto out;
        }

        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (drive));

        has_extended_partition = FALSE;
        enclosed_presentables = mdu_pool_get_enclosed_presentables (pool,
                                                                    MDU_PRESENTABLE (drive));
        for (l = enclosed_presentables; l != NULL; l = l->next) {
                MduPresentable *ep = MDU_PRESENTABLE (l->data);

                if (MDU_IS_VOLUME_HOLE (ep)) {
                        size = mdu_presentable_get_size (ep);

                        if (size > largest_contiguous_free_segment) {
                                largest_contiguous_free_segment = size;
                                pres = ep;
                        }

                        total_free += size;

                } else if (MDU_IS_VOLUME (ep)) {
                        gint type;
                        MduDevice *ep_device;

                        ep_device = mdu_presentable_get_device (ep);

                        type = strtol (mdu_device_partition_get_type (ep_device), NULL, 0);
                        if (type == 0x05 || type == 0x0f || type == 0x85) {
                                GList *logical_partitions;
                                GList *ll;

                                has_extended_partition = TRUE;

                                /* This is MS-DOS extended partition, count volume holes inside */
                                logical_partitions = mdu_pool_get_enclosed_presentables (pool, ep);
                                for (ll = logical_partitions; ll != NULL; ll = ll->next) {
                                        MduPresentable *lep = MDU_PRESENTABLE (ll->data);

                                        if (MDU_IS_VOLUME_HOLE (lep)) {
                                                size = mdu_presentable_get_size (lep);
                                                if (size > largest_contiguous_free_segment) {
                                                        largest_contiguous_free_segment = size;
                                                        pres = lep;
                                                }

                                                total_free += size;
                                        }
                                }
                                g_list_foreach (logical_partitions, (GFunc) g_object_unref, NULL);
                                g_list_free (logical_partitions);
                        }
                        g_object_unref (ep_device);
                }
        }
        g_list_foreach (enclosed_presentables, (GFunc) g_object_unref, NULL);
        g_list_free (enclosed_presentables);

        ret = (largest_contiguous_free_segment > 0);

        /* Now igure out if the partition table is full (e.g. four primary partitions already) and
         * return %FALSE and non-zero @out_largest_contiguous_free_segment
         */
        if (g_strcmp0 (mdu_device_partition_table_get_scheme (device), "mbr") == 0 &&
            mdu_device_partition_table_get_count (device) == 4 &&
            !has_extended_partition) {
                ret = FALSE;
        }

 out:
        if (device != NULL)
                g_object_unref (device);
        if (pool != NULL)
                g_object_unref (pool);

        if (out_largest_contiguous_free_segment != NULL)
                *out_largest_contiguous_free_segment = largest_contiguous_free_segment;

        if (out_total_free != NULL)
                *out_total_free = total_free;

        if (out_is_unitialized != NULL)
                *out_is_unitialized = whole_disk_uninitialized;

        if (out_presentable != NULL) {
                *out_presentable = (pres != NULL ? g_object_ref (pres) : NULL);
        }

        return ret;
}

static void
mdu_drive_init (MduDrive *drive)
{
        drive->priv = G_TYPE_INSTANCE_GET_PRIVATE (drive, MDU_TYPE_DRIVE, MduDrivePrivate);
}

static void
device_changed (MduDevice *device, gpointer user_data)
{
        MduDrive *drive = MDU_DRIVE (user_data);
        g_signal_emit_by_name (drive, "changed");
        g_signal_emit_by_name (drive->priv->pool, "presentable-changed", drive);
}

static void
device_job_changed (MduDevice *device, gpointer user_data)
{
        MduDrive *drive = MDU_DRIVE (user_data);
        g_signal_emit_by_name (drive, "job-changed");
        g_signal_emit_by_name (drive->priv->pool, "presentable-job-changed", drive);
}

MduDrive *
_mdu_drive_new_from_device (MduPool *pool, MduDevice *device, MduPresentable *enclosing_presentable)
{
        MduDrive *drive;

        drive = MDU_DRIVE (g_object_new (MDU_TYPE_DRIVE, NULL));
        drive->priv->device = g_object_ref (device);
        drive->priv->pool = g_object_ref (pool);
        drive->priv->enclosing_presentable =
                enclosing_presentable != NULL ? g_object_ref (enclosing_presentable) : NULL;
        drive->priv->id = g_strdup_printf ("drive_%s_enclosed_by_%s",
                                           mdu_device_get_device_file (drive->priv->device),
                                           enclosing_presentable != NULL ? mdu_presentable_get_id (enclosing_presentable) : "(none)");

        g_signal_connect (device, "changed", (GCallback) device_changed, drive);
        g_signal_connect (device, "job-changed", (GCallback) device_job_changed, drive);

        return drive;
}

static const gchar *
mdu_drive_get_id (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        return drive->priv->id;
}

static MduDevice *
mdu_drive_get_device (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        return g_object_ref (drive->priv->device);
}

static MduPresentable *
mdu_drive_get_enclosing_presentable (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        if (drive->priv->enclosing_presentable != NULL)
                return g_object_ref (drive->priv->enclosing_presentable);
        return NULL;
}

/* TODO: should move to mdu-util.c */
static void
get_drive_name_from_media_compat (MduDevice *device,
                                  GString   *result)
{
        guint n;
        gboolean optical_cd;
        gboolean optical_dvd;
        gboolean optical_bd;
        gboolean optical_hddvd;
        const gchar* const *media_compat;

        media_compat = (const gchar* const *) mdu_device_drive_get_media_compatibility (device);

        optical_cd = FALSE;
        optical_dvd = FALSE;
        optical_bd = FALSE;
        optical_hddvd = FALSE;
        for (n = 0; media_compat != NULL && media_compat[n] != NULL; n++) {
                const gchar *media_name;
                const gchar *media;

                media = media_compat[n];
                media_name = NULL;
                if (g_strcmp0 (media, "flash_cf") == 0) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("CompactFlash");
                } else if (g_strcmp0 (media, "flash_ms") == 0) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("MemoryStick");
                } else if (g_strcmp0 (media, "flash_sm") == 0) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("SmartMedia");
                } else if (g_strcmp0 (media, "flash_sd") == 0) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("SecureDigital");
                } else if (g_strcmp0 (media, "flash_sdhc") == 0) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("SD High Capacity");
                } else if (g_strcmp0 (media, "floppy") == 0) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("Floppy");
                } else if (g_strcmp0 (media, "floppy_zip") == 0) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("Zip");
                } else if (g_strcmp0 (media, "floppy_jaz") == 0) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("Jaz");
                } else if (g_str_has_prefix (media, "flash")) {
                        /* Translators: This word is used to describe the media inserted into a device */
                        media_name = _("Flash");
                } else if (g_str_has_prefix (media, "optical_cd")) {
                        optical_cd = TRUE;
                } else if (g_str_has_prefix (media, "optical_dvd")) {
                        optical_dvd = TRUE;
                } else if (g_str_has_prefix (media, "optical_bd")) {
                        optical_bd = TRUE;
                } else if (g_str_has_prefix (media, "optical_hddvd")) {
                        optical_hddvd = TRUE;
                }

                if (media_name != NULL) {
                        if (result->len > 0)
                                g_string_append_c (result, '/');
                        g_string_append (result, media_name);
                }
        }
        if (optical_cd) {
                if (result->len > 0)
                        g_string_append_c (result, '/');
                /* Translators: This word is used to describe the optical disc type, it may appear
                 * in a slash-separated list e.g. 'CD/DVD/Blu-Ray'
                 */
                g_string_append (result, _("CD"));
        }
        if (optical_dvd) {
                if (result->len > 0)
                        g_string_append_c (result, '/');
                /* Translators: This word is used to describe the optical disc type, it may appear
                 * in a slash-separated list e.g. 'CD/DVD/Blu-Ray'
                 */
                g_string_append (result, _("DVD"));
        }
        if (optical_bd) {
                if (result->len > 0)
                        g_string_append_c (result, '/');
                /* Translators: This word is used to describe the optical disc type, it may appear
                 * in a slash-separated list e.g. 'CD/DVD/Blu-Ray'
                 */
                g_string_append (result, _("Blu-Ray"));
        }
        if (optical_hddvd) {
                if (result->len > 0)
                        g_string_append_c (result, '/');
                /* Translators: This word is used to describe the optical disc type, it may appear
                 * in a slash-separated list e.g. 'CD/DVD/Blu-Ray'
                 */
                g_string_append (result, _("HDDVD"));
        }

}

static gchar *
mdu_drive_get_name (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        const gchar *vendor;
        const gchar *model;
        const char *presentation_name;
        guint64 size;
        gboolean is_removable;
        GString *result;
        gboolean is_rotational;
        gboolean has_media;
        gchar *strsize;

        strsize = NULL;
        result = g_string_new (NULL);

        presentation_name = mdu_device_get_presentation_name (drive->priv->device);
        if (presentation_name != NULL && strlen (presentation_name) > 0) {
                g_string_append (result, presentation_name);
                goto out;
        }

        vendor = mdu_device_drive_get_vendor (drive->priv->device);
        model = mdu_device_drive_get_model (drive->priv->device);
        size = mdu_device_get_size (drive->priv->device);
        is_removable = mdu_device_is_removable (drive->priv->device);
        has_media = mdu_device_is_media_available (drive->priv->device);
        is_rotational = mdu_device_drive_get_is_rotational (drive->priv->device);

        if (has_media && size > 0) {
                strsize = mdu_util_get_size_for_display (size, FALSE, FALSE);
        }

        if (mdu_device_is_linux_loop (drive->priv->device)) {
                /* Translators: This is the name of a "Drive" backed by a file.
                 * The %s is the size of the file (e.g. "42 GB" or "5 KB").
                 *
                 * See e.g. http://people.freedesktop.org/~david/mate-loopback-2.png
                 */
                g_string_append_printf (result, _("%s File"), strsize);
                goto out;
        }

        if (is_removable) {

                get_drive_name_from_media_compat (drive->priv->device, result);

                /* If we know the media type, just append Drive */
                if (result->len > 0) {
                        GString *new_result;

                        new_result = g_string_new (NULL);
                        /* Translators: %s is the media type e.g. 'CD/DVD' or 'CompactFlash' */
                        g_string_append_printf (new_result, _("%s Drive"),
                                                result->str);
                        g_string_free (result, TRUE);
                        result = new_result;
                } else {
                        /* Otherwise use Vendor/Model */
                        if (vendor != NULL && strlen (vendor) == 0)
                                vendor = NULL;

                        if (model != NULL && strlen (model) == 0)
                                model = NULL;

                        g_string_append_printf (result,
                                                "%s%s%s",
                                                vendor != NULL ? vendor : "",
                                                vendor != NULL ? " " : "",
                                                model != NULL ? model : "");
                }

        } else {
                /* Media is not removable, use "Hard Disk" resp. "Solid-State Disk"
                 * unless we actually know what media types the drive is compatible with
                 */

                get_drive_name_from_media_compat (drive->priv->device, result);

                if (result->len > 0) {
                        GString *new_result;

                        new_result = g_string_new (NULL);
                        if (strsize != NULL) {
                                /* Translators: first %s is the size, second %s is the media type
                                 * e.g. 'CD/DVD' or 'CompactFlash'
                                 */
                                g_string_append_printf (new_result, _("%s %s Drive"),
                                                        strsize,
                                                        result->str);
                        } else {
                                /* Translators: %s is the media type e.g. 'CD/DVD' or 'CompactFlash' */
                                g_string_append_printf (new_result, _("%s Drive"),
                                                        result->str);
                        }
                        g_string_free (result, TRUE);
                        result = new_result;
                } else {
                        if (is_rotational) {
                                if (strsize != NULL) {
                                        /* Translators: This string is used to describe a hard disk.
                                         * The first %s is the size of the drive e.g. '45 GB'.
                                         */
                                        g_string_append_printf (result, _("%s Hard Disk"), strsize);
                                } else {
                                        /* Translators: This string is used to describe a hard disk where the size
                                         * is not known.
                                         */
                                        g_string_append (result, _("Hard Disk"));
                                }
                        } else {
                                if (strsize != NULL) {
                                        /* Translators: This string is used to describe a SSD. The first %s is
                                         * the size of the drive e.g. '45 GB'.
                                         */
                                        g_string_append_printf (result, _("%s Solid-State Disk"),
                                                                strsize);
                                } else {
                                        /* Translators: This string is used to describe a SSD where the size
                                         * is not known.
                                         */
                                        g_string_append (result, _("Solid-State Disk"));
                                }
                        }
                }
        }

 out:
        g_free (strsize);
        return g_string_free (result, FALSE);
}

static gchar *
mdu_drive_get_description (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        const gchar *vendor;
        const gchar *model;
        const gchar *part_table_scheme;
        GString *result;
        guint64 size;
        gboolean is_removable;
        gboolean has_media;

        result = g_string_new (NULL);

        vendor = mdu_device_drive_get_vendor (drive->priv->device);
        model = mdu_device_drive_get_model (drive->priv->device);

        size = mdu_device_get_size (drive->priv->device);
        is_removable = mdu_device_is_removable (drive->priv->device);
        has_media = mdu_device_is_media_available (drive->priv->device);
        part_table_scheme = mdu_device_partition_table_get_scheme (drive->priv->device);

        /* If removable, include size of media or the fact there is no media */
        if (is_removable) {
                if (has_media && size > 0) {
                        gchar *strsize;
                        strsize = mdu_util_get_size_for_display (size, FALSE, FALSE);
                        /* Translators: This string is the description of a drive. The first %s is the
                         * size of the inserted media, for example '45 GB'.
                         */
                        g_string_append_printf (result, _("%s Media"),
                                                strsize);
                        g_free (strsize);
                } else {
                        /* Translators: This string is used as a description text when no media has
                         * been detected for a drive
                         */
                        g_string_append_printf (result, _("No Media Detected"));
                }
        }

        /* If we have media, include whether partitioned or not */
        if (has_media && size > 0) {
                if (result->len > 0)
                        g_string_append (result, ", ");
                if (mdu_device_is_partition_table (drive->priv->device)) {
                        if (g_strcmp0 (part_table_scheme, "mbr") == 0) {
                                /* Translators: This string is used for conveying the partition table format */
                                g_string_append (result, _("MBR Partition Table"));
                        } else if (g_strcmp0 (part_table_scheme, "gpt") == 0) {
                                /* Translators: This string is used for conveying the partition table format */
                                g_string_append (result, _("GUID Partition Table"));
                        } else if (g_strcmp0 (part_table_scheme, "apm") == 0) {
                                /* Translators: This string is used for conveying the partition table format */
                                g_string_append (result, _("Apple Partition Table"));
                        } else {
                                /* Translators: This string is used for conveying the partition table format when
                                 * the format is unknown
                                 */
                                g_string_append (result, _("Partitioned"));
                        }
                } else {
                        /* Translators: This string is used for conveying a device is not partitioned.
                         */
                        g_string_append (result, _("Not Partitioned"));
                }
        }


        return g_string_free (result, FALSE);
}

static gchar *
mdu_drive_get_vpd_name (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        const gchar *vendor;
        const gchar *model;
        GString *result;

        result = g_string_new (NULL);

        if (mdu_device_is_linux_loop (drive->priv->device)) {
                gchar *s;
                s = g_path_get_basename (mdu_device_linux_loop_get_filename (drive->priv->device));
                g_string_append (result, s);
                g_free (s);
                goto out;
        }

        vendor = mdu_device_drive_get_vendor (drive->priv->device);
        model = mdu_device_drive_get_model (drive->priv->device);

        if (vendor != NULL && strlen (vendor) == 0)
                vendor = NULL;

        if (model != NULL && strlen (model) == 0)
                model = NULL;

        g_string_append_printf (result,
                                "%s%s%s",
                                vendor != NULL ? vendor : "",
                                vendor != NULL ? " " : "",
                                model != NULL ? model : "");

 out:
        return g_string_free (result, FALSE);
}

static gboolean
strv_has (char **strv, const gchar *str)
{
        gboolean ret;
        guint n;

        ret = FALSE;

        for (n = 0; strv != NULL && strv[n] != NULL; n++) {
                if (g_strcmp0 (strv[n], str) == 0) {
                        ret = TRUE;
                        goto out;
                }
        }

 out:
        return ret;
}

static gboolean
strv_has_prefix (char **strv, const gchar *str)
{
        gboolean ret;
        guint n;

        ret = FALSE;

        for (n = 0; strv != NULL && strv[n] != NULL; n++) {
                if (g_str_has_prefix (strv[n], str)) {
                        ret = TRUE;
                        goto out;
                }
        }

 out:
        return ret;
}

static GIcon *
mdu_drive_get_icon (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        const char *name;
        const char *connection_interface;
        const char *drive_media;
        const char *presentation_icon_name;
        gchar **drive_media_compat;
        gboolean is_removable;
        GIcon *icon;

        connection_interface = mdu_device_drive_get_connection_interface (drive->priv->device);
        is_removable = mdu_device_is_removable (drive->priv->device);
        drive_media = mdu_device_drive_get_media (drive->priv->device);
        drive_media_compat = mdu_device_drive_get_media_compatibility (drive->priv->device);

        name = NULL;

        presentation_icon_name = mdu_device_get_presentation_icon_name (drive->priv->device);
        if (presentation_icon_name != NULL && strlen (presentation_icon_name) > 0) {
                name = presentation_icon_name;
                goto out;
        }

        if (mdu_device_is_linux_loop (drive->priv->device)) {
                name = "drive-removable-media-file";
                goto out;
        }

        /* media type */
        if (strv_has (drive_media_compat, "optical_cd")) {
                /* TODO: it would probably be nice to export a property whether this device can
                 *       burn discs etc. so we can use the 'drive-optical-recorder' icon when
                 *       applicable.
                 */
                name = "drive-optical";
        } else if (strv_has (drive_media_compat, "floppy")) {
                name = "drive-removable-media-floppy";
        } else if (strv_has (drive_media_compat, "floppy_zip")) {
                name = "drive-removable-media-floppy-zip";
        } else if (strv_has (drive_media_compat, "floppy_jaz")) {
                name = "drive-removable-media-floppy-jaz";
        } else if (strv_has (drive_media_compat, "flash_cf")) {
                name = "drive-removable-media-flash-cf";
        } else if (strv_has (drive_media_compat, "flash_ms")) {
                name = "drive-removable-media-flash-ms";
        } else if (strv_has (drive_media_compat, "flash_sm")) {
                name = "drive-removable-media-flash-sm";
        } else if (strv_has (drive_media_compat, "flash_sd")) {
                name = "drive-removable-media-flash-sd";
        } else if (strv_has (drive_media_compat, "flash_sdhc")) {
                /* TODO: get icon name for sdhc */
                name = "drive-removable-media-flash-sd";
        } else if (strv_has (drive_media_compat, "flash_mmc")) {
                /* TODO: get icon for mmc */
                name = "drive-removable-media-flash-sd";
        } else if (strv_has_prefix (drive_media_compat, "flash")) {
                name = "drive-removable-media-flash";
        }

        /* else fall back to connection interface */
        if (name == NULL && connection_interface != NULL) {
                if (g_str_has_prefix (connection_interface, "ata")) {
                        if (is_removable)
                                name = "drive-removable-media-ata";
                        else
                                name = "drive-harddisk-ata";
                } else if (g_str_has_prefix (connection_interface, "scsi")) {
                        if (is_removable)
                                name = "drive-removable-media-scsi";
                        else
                                name = "drive-harddisk-scsi";
                } else if (strcmp (connection_interface, "usb") == 0) {
                        if (is_removable)
                                name = "drive-removable-media-usb";
                        else
                                name = "drive-harddisk-usb";
                } else if (strcmp (connection_interface, "firewire") == 0) {
                        if (is_removable)
                                name = "drive-removable-media-ieee1394";
                        else
                                name = "drive-harddisk-ieee1394";
                }
        }

 out:
        /* ultimate fallback */
        if (name == NULL) {
                if (is_removable)
                        name = "drive-removable-media";
                else
                        name = "drive-harddisk";
        }

        /* Attach a MP emblem if it's a multipathed device or a path for a multipathed device */
        icon = g_themed_icon_new_with_default_fallbacks (name);
        if (mdu_device_is_linux_dmmp (drive->priv->device)) {
                GEmblem *emblem;
                GIcon *padlock;
                GIcon *emblemed_icon;

                padlock = g_themed_icon_new ("mdu-emblem-mp");
                emblem = g_emblem_new_with_origin (padlock, G_EMBLEM_ORIGIN_DEVICE);

                emblemed_icon = g_emblemed_icon_new (icon, emblem);
                g_object_unref (icon);
                icon = emblemed_icon;

                g_object_unref (padlock);
                g_object_unref (emblem);
        } else if (mdu_device_is_linux_dmmp_component (drive->priv->device)) {
                GEmblem *emblem;
                GIcon *padlock;
                GIcon *emblemed_icon;

                padlock = g_themed_icon_new ("mdu-emblem-mp-component");
                emblem = g_emblem_new_with_origin (padlock, G_EMBLEM_ORIGIN_DEVICE);

                emblemed_icon = g_emblemed_icon_new (icon, emblem);
                g_object_unref (icon);
                icon = emblemed_icon;

                g_object_unref (padlock);
                g_object_unref (emblem);
        }

        return icon;
}

static guint64
mdu_drive_get_offset (MduPresentable *presentable)
{
        return 0;
}

static guint64
mdu_drive_get_size (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        return mdu_device_get_size (drive->priv->device);
}

static MduPool *
mdu_drive_get_pool (MduPresentable *presentable)
{
        MduDrive *drive = MDU_DRIVE (presentable);
        return mdu_device_get_pool (drive->priv->device);
}

static gboolean
mdu_drive_is_allocated (MduPresentable *presentable)
{
        return TRUE;
}

static gboolean
mdu_drive_is_recognized (MduPresentable *presentable)
{
        /* TODO: maybe we need to return FALSE sometimes */
        return TRUE;
}

static void
mdu_drive_presentable_iface_init (MduPresentableIface *iface)
{
        iface->get_id                    = mdu_drive_get_id;
        iface->get_device                = mdu_drive_get_device;
        iface->get_enclosing_presentable = mdu_drive_get_enclosing_presentable;
        iface->get_name                  = mdu_drive_get_name;
        iface->get_description           = mdu_drive_get_description;
        iface->get_vpd_name              = mdu_drive_get_vpd_name;
        iface->get_icon                  = mdu_drive_get_icon;
        iface->get_offset                = mdu_drive_get_offset;
        iface->get_size                  = mdu_drive_get_size;
        iface->get_pool                  = mdu_drive_get_pool;
        iface->is_allocated              = mdu_drive_is_allocated;
        iface->is_recognized             = mdu_drive_is_recognized;
}

void
_mdu_drive_rewrite_enclosing_presentable (MduDrive *drive)
{
        if (drive->priv->enclosing_presentable != NULL) {
                const gchar *enclosing_presentable_id;
                MduPresentable *new_enclosing_presentable;

                enclosing_presentable_id = mdu_presentable_get_id (drive->priv->enclosing_presentable);

                new_enclosing_presentable = mdu_pool_get_presentable_by_id (drive->priv->pool,
                                                                            enclosing_presentable_id);
                if (new_enclosing_presentable == NULL) {
                        g_warning ("Error rewriting enclosing_presentable for %s, no such id %s",
                                   drive->priv->id,
                                   enclosing_presentable_id);
                        goto out;
                }

                g_object_unref (drive->priv->enclosing_presentable);
                drive->priv->enclosing_presentable = new_enclosing_presentable;
        }

 out:
        ;
}

/* ---------------------------------------------------------------------------------------------------- */

static void
create_volume_partition_create_cb (MduDevice  *device,
                                   gchar      *created_device_object_path,
                                   GError     *error,
                                   gpointer    user_data)
{
        GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT (user_data);

        if (error != NULL) {
                g_simple_async_result_set_from_error (simple, error);
                g_error_free (error);
        } else {
                MduDevice *d;
                MduPool *pool;
                MduPresentable *volume;

                pool = mdu_device_get_pool (device);
                d = mdu_pool_get_by_object_path (pool, created_device_object_path);
                g_assert (d != NULL);

                volume = mdu_pool_get_volume_by_device (pool, d);
                g_assert (volume != NULL);

                g_simple_async_result_set_op_res_gpointer (simple, volume, g_object_unref);

                g_object_unref (pool);
                g_object_unref (d);
        }
        g_simple_async_result_complete_in_idle (simple);
}

static void
mdu_drive_create_volume_real_internal (MduDrive              *drive,
                                       guint64                size,
                                       const gchar           *name,
                                       MduCreateVolumeFlags   flags,
                                       GSimpleAsyncResult    *simple);

static void
create_volume_partition_table_create_cb (MduDevice  *device,
                                         GError     *error,
                                         gpointer    user_data)
{
        GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT (user_data);

        if (error != NULL) {
                g_simple_async_result_set_from_error (simple, error);
                g_simple_async_result_complete_in_idle (simple);
                g_error_free (error);
        } else {
                MduDrive              *drive;
                guint64                size;
                const gchar           *name;
                MduCreateVolumeFlags   flags;

                drive = MDU_DRIVE (g_async_result_get_source_object (G_ASYNC_RESULT (simple)));
                size = (* ((guint64 *) g_object_get_data (G_OBJECT (simple), "mdu-size")));
                name = g_object_get_data (G_OBJECT (simple), "mdu-name");
                flags = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (simple), "mdu-flags"));

                /* now that we have a partition table... try creating the volume again */
                mdu_drive_create_volume_real_internal (drive,
                                                       size,
                                                       name,
                                                       flags,
                                                       simple);

                g_object_unref (drive);
        }
}

static void
mdu_drive_create_volume_real (MduDrive              *drive,
                              guint64                size,
                              const gchar           *name,
                              MduCreateVolumeFlags   flags,
                              GAsyncReadyCallback    callback,
                              gpointer               user_data)
{
        GSimpleAsyncResult *simple;

        g_return_if_fail (MDU_IS_DRIVE (drive));

        simple = g_simple_async_result_new (G_OBJECT (drive),
                                            callback,
                                            user_data,
                                            mdu_drive_create_volume);

        g_object_set_data_full (G_OBJECT (simple), "mdu-size", g_memdup (&size, sizeof (guint64)), g_free);
        g_object_set_data_full (G_OBJECT (simple), "mdu-name", g_strdup (name), g_free);
        g_object_set_data (G_OBJECT (simple), "mdu-flags", GINT_TO_POINTER (flags));

        mdu_drive_create_volume_real_internal (drive,
                                               size,
                                               name,
                                               flags,
                                               simple);
}

static void
mdu_drive_create_volume_real_internal (MduDrive              *drive,
                                       guint64                size,
                                       const gchar           *name,
                                       MduCreateVolumeFlags   flags,
                                       GSimpleAsyncResult    *simple)
{
        MduPresentable *p;
        MduDevice *d;
        gboolean whole_disk_is_uninitialized;
        guint64 largest_segment;

        if (!mdu_drive_can_create_volume (drive,
                                          &whole_disk_is_uninitialized,
                                          &largest_segment,
                                          NULL, /* total_free */
                                          &p)) {
                g_simple_async_result_set_error (simple,
                                                 MDU_ERROR,
                                                 MDU_ERROR_FAILED,
                                                 "Insufficient space");
                g_simple_async_result_complete_in_idle (simple);
                g_object_unref (simple);
                goto out;
        }

        g_assert (p != NULL);

        d = mdu_presentable_get_device (MDU_PRESENTABLE (drive));

        if (MDU_IS_VOLUME_HOLE (p)) {
                guint64 offset;
                const gchar *scheme;
                const gchar *type;
                gchar *label;

                offset = mdu_presentable_get_offset (p);

                scheme = mdu_device_partition_table_get_scheme (d);
                type = "";
                label = NULL;
                if (g_strcmp0 (scheme, "mbr") == 0) {
                        if (flags & MDU_CREATE_VOLUME_FLAGS_LINUX_MD) {
                                type = "0xfd";
                        } else if (flags & MDU_CREATE_VOLUME_FLAGS_LINUX_LVM2) {
                                type = "0x8e";
                        }
                } else if (g_strcmp0 (scheme, "gpt") == 0) {
                        if (flags & MDU_CREATE_VOLUME_FLAGS_LINUX_MD) {
                                type = "A19D880F-05FC-4D3B-A006-743F0F84911E";
                                /* Limited to 36 UTF-16LE characters according to on-disk format..
                                 * Since a RAID array name is limited to 32 chars this should fit */
                                if (name != NULL)
                                        label = g_strdup_printf ("RAID: %s", name);
                                else
                                        label = g_strdup_printf ("RAID component");
                        } else if (flags & MDU_CREATE_VOLUME_FLAGS_LINUX_LVM2) {
                                type = "E6D6D379-F507-44C2-A23C-238F2A3DF928";
                                /* Limited to 36 UTF-16LE characters according to on-disk format..
                                 * TODO: ensure name is shorter than or equal to 32 characters */
                                if (name != NULL)
                                        label = g_strdup_printf ("LVM2: %s", name);
                                else
                                        label = g_strdup_printf ("LVM2 component");
                        }
                } else if (g_strcmp0 (scheme, "apt") == 0) {
                        type = "Apple_Unix_SVR2";
                        if (flags & MDU_CREATE_VOLUME_FLAGS_LINUX_MD) {
                                if (name != NULL)
                                        label = g_strdup_printf ("RAID: %s", name);
                                else
                                        label = g_strdup_printf ("RAID component");
                        } else if (flags & MDU_CREATE_VOLUME_FLAGS_LINUX_LVM2) {
                                if (name != NULL)
                                        label = g_strdup_printf ("LVM2: %s", name);
                                else
                                        label = g_strdup_printf ("LVM2 component");
                        }
                }

                mdu_device_op_partition_create (d,
                                                offset,
                                                size,
                                                type,
                                                label != NULL ? label : "",
                                                NULL,
                                                "",
                                                "",
                                                "",
                                                FALSE,
                                                create_volume_partition_create_cb,
                                                simple);
                g_free (label);

        } else {

                /* otherwise the whole disk must be uninitialized... */
                g_assert (whole_disk_is_uninitialized);

                /* so create a partition table...
                 * (TODO: take a flag to determine what kind of partition table to create)
                 */
                mdu_device_op_partition_table_create (d,
                                                      "mbr",
                                                      create_volume_partition_table_create_cb,
                                                      simple);
        }

 out:
        if (d != NULL)
                g_object_unref (d);
        if (p != NULL)
                g_object_unref (p);
}

static MduVolume *
mdu_drive_create_volume_finish_real (MduDrive              *drive,
                                     GAsyncResult          *res,
                                     GError               **error)
{
        GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT (res);
        MduVolume *ret;

        g_return_val_if_fail (MDU_IS_DRIVE (drive), NULL);
        g_return_val_if_fail (res != NULL, NULL);

        g_warn_if_fail (g_simple_async_result_get_source_tag (simple) == mdu_drive_create_volume);

        ret = NULL;
        if (g_simple_async_result_propagate_error (simple, error))
                goto out;

        ret = MDU_VOLUME (g_simple_async_result_get_op_res_gpointer (simple));

 out:
        return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * mdu_drive_create_volume:
 * @drive: A #MduDrive.
 * @size: The size of the volume to create.
 * @name: A name for the volume.
 * @flags: Flags describing what kind of volume to create
 * @callback: Function to call when the result is ready.
 * @user_data: User data to pass to @callback.
 *
 * High-level method for creating a new volume on @drive of size @size
 * using @name and @flags as influential hints.
 *
 * Depending on the actual type of @drive, different things may happen
 * - if @drive represents a partitioned drive, then a new partition
 * will be created (and if the partitioning scheme supports partition
 * labels @name will be used as the label). If @drive is completely
 * uninitialized, it may (or may not) be partitioned.
 *
 * If @drive represents a LVM2 volume group, a logical volume may be
 * created (with @name being used as LV name).
 *
 * This is an asynchronous operation. When the result of the operation
 * is ready, @callback will be invoked.
 */
void
mdu_drive_create_volume (MduDrive              *drive,
                         guint64                size,
                         const gchar           *name,
                         MduCreateVolumeFlags   flags,
                         GAsyncReadyCallback    callback,
                         gpointer               user_data)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        klass->create_volume (drive,
                              size,
                              name,
                              flags,
                              callback,
                              user_data);
}

/**
 * mdu_drive_create_volume_finish:
 * @drive: A #MduDrive.
 * @res: A #GAsyncResult.
 * @error: A #GError or %NULL.
 *
 * Finishes an operation started with mdu_drive_create_volume().
 *
 * Returns: A #MduVolume for the created volume or %NULL if @error is
 * set. The returned object must be freed with g_object_unref().
 */
MduVolume *
mdu_drive_create_volume_finish (MduDrive              *drive,
                                GAsyncResult          *res,
                                GError               **error)
{
        MduDriveClass *klass = MDU_DRIVE_GET_CLASS (drive);
        return klass->create_volume_finish (drive,
                                            res,
                                            error);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
get_volumes_add_enclosed (MduPresentable  *p,
                          GList          **ret)
{
        GList *enclosed;
        GList *l;
        MduPool *pool;

        pool = mdu_presentable_get_pool (p);
        enclosed = mdu_pool_get_enclosed_presentables (pool, p);

        for (l = enclosed; l != NULL; l = l->next) {
                MduPresentable *ep = MDU_PRESENTABLE (l->data);

                if (!MDU_IS_VOLUME (ep))
                        continue;

                get_volumes_add_enclosed (ep, ret);

                *ret = g_list_prepend (*ret, g_object_ref (p));
        }

        g_list_foreach (enclosed, (GFunc) g_object_unref, NULL);
        g_list_free (enclosed);
        g_object_unref (pool);
}

/**
 * mdu_drive_get_volumes:
 * @drive: A #MduDrive
 *
 * Returns a list of all #MduVolume<!-- -->s for @drive.
 *
 * Returns: A #GList of #MduVolume objects. The caller must free the list and each element.
 */
GList *
mdu_drive_get_volumes (MduDrive *drive)
{
        GList *ret;

        /* TODO: do we need a vfunc for this? */

        ret = NULL;

        get_volumes_add_enclosed (MDU_PRESENTABLE (drive), &ret);

        ret = g_list_reverse (ret);

        return ret;
}

/* ---------------------------------------------------------------------------------------------------- */
