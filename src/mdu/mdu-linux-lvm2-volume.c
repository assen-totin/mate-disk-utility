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

#include "config.h"
#include <glib/gi18n-lib.h>

#include <string.h>
#include <dbus/dbus-glib.h>

#include "mdu-private.h"
#include "mdu-util.h"
#include "mdu-pool.h"
#include "mdu-device.h"
#include "mdu-linux-lvm2-volume-group.h"
#include "mdu-linux-lvm2-volume.h"
#include "mdu-presentable.h"

struct _MduLinuxLvm2VolumePrivate
{
        MduPool *pool;

        gchar *name;

        gchar *group_uuid;
        gchar *uuid;

        guint position;
        guint64 size;

        /* the MduDevice for the LV / mapped device (if activated) */
        MduDevice *lv;

        gchar *id;

        MduPresentable *enclosing_presentable;
};

static GObjectClass *parent_class = NULL;

static void mdu_linux_lvm2_volume_presentable_iface_init (MduPresentableIface *iface);

static void on_device_added (MduPool *pool, MduDevice *device, gpointer user_data);
static void on_device_removed (MduPool *pool, MduDevice *device, gpointer user_data);
static void on_device_changed (MduPool *pool, MduDevice *device, gpointer user_data);

static void on_presentable_changed (MduPresentable *presentable, gpointer user_data);

G_DEFINE_TYPE_WITH_CODE (MduLinuxLvm2Volume, mdu_linux_lvm2_volume, MDU_TYPE_VOLUME,
                         G_IMPLEMENT_INTERFACE (MDU_TYPE_PRESENTABLE,
                                                mdu_linux_lvm2_volume_presentable_iface_init))

static void
mdu_linux_lvm2_volume_finalize (GObject *object)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (object);

        //g_debug ("##### finalized linux-lvm2 volume '%s' %p", lv->priv->id, lv);

        if (volume->priv->pool != NULL) {
                g_signal_handlers_disconnect_by_func (volume->priv->pool, on_device_added, volume);
                g_signal_handlers_disconnect_by_func (volume->priv->pool, on_device_removed, volume);
                g_signal_handlers_disconnect_by_func (volume->priv->pool, on_device_changed, volume);
                g_object_unref (volume->priv->pool);
        }

        g_free (volume->priv->id);
        g_free (volume->priv->name);
        g_free (volume->priv->group_uuid);
        g_free (volume->priv->uuid);

        if (volume->priv->enclosing_presentable != NULL) {
                g_signal_handlers_disconnect_by_func (volume->priv->enclosing_presentable,
                                                      on_presentable_changed,
                                                      volume);
                g_object_unref (volume->priv->enclosing_presentable);
        }

        if (volume->priv->lv != NULL)
                g_object_unref (volume->priv->lv);

        if (G_OBJECT_CLASS (parent_class)->finalize)
                (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static void
mdu_linux_lvm2_volume_class_init (MduLinuxLvm2VolumeClass *klass)
{
        GObjectClass *gobject_class = (GObjectClass *) klass;

        parent_class = g_type_class_peek_parent (klass);

        gobject_class->finalize = mdu_linux_lvm2_volume_finalize;

        g_type_class_add_private (klass, sizeof (MduLinuxLvm2VolumePrivate));
}

static void
mdu_linux_lvm2_volume_init (MduLinuxLvm2Volume *volume)
{
        volume->priv = G_TYPE_INSTANCE_GET_PRIVATE (volume, MDU_TYPE_LINUX_LVM2_VOLUME, MduLinuxLvm2VolumePrivate);
}

static void
emit_changed (MduLinuxLvm2Volume *volume)
{
        //g_debug ("emitting changed for uuid '%s'", volume->priv->uuid);
        g_signal_emit_by_name (volume, "changed");
        g_signal_emit_by_name (volume->priv->pool, "presentable-changed", volume);
}

static gboolean
find_lv (MduLinuxLvm2Volume *volume)
{
        GList *devices;
        GList *l;
        MduDevice *lv_to_use;
        gboolean emitted_changed;

        emitted_changed = FALSE;

        /* find all MduDevice objects for LVs that are part of this VG */
        lv_to_use = NULL;
        devices = mdu_pool_get_devices (volume->priv->pool);
        for (l = devices; l != NULL; l = l->next) {
                MduDevice *d = MDU_DEVICE (l->data);
                if (mdu_device_is_linux_lvm2_lv (d) &&
                    g_strcmp0 (mdu_device_linux_lvm2_lv_get_uuid (d), volume->priv->uuid) == 0) {
                        lv_to_use = d;
                        break;
                }
        }
        g_list_foreach (devices, (GFunc) g_object_unref, NULL);
        g_list_free (devices);

        /* Find the PV with the highest sequence number and use that */

        if (volume->priv->lv != lv_to_use) {
                if (volume->priv->lv != NULL)
                        g_object_unref (volume->priv->lv);
                volume->priv->lv = lv_to_use != NULL ? g_object_ref (lv_to_use) : NULL;
                emit_changed (volume);
                emitted_changed = TRUE;
        }

        return emitted_changed;
}

static void
on_device_added (MduPool *pool, MduDevice *device, gpointer user_data)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (user_data);
        find_lv (volume);
}

static void
on_device_removed (MduPool *pool, MduDevice *device, gpointer user_data)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (user_data);
        find_lv (volume);
}

static void
on_device_changed (MduPool *pool, MduDevice *device, gpointer user_data)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (user_data);
        gboolean emitted_changed;

        emitted_changed = find_lv (volume);

        if (!emitted_changed && device == volume->priv->lv)
                emit_changed (volume);
}

static gboolean
update_lv_info (MduLinuxLvm2Volume *volume)
{
        guint position;
        gchar *name;
        guint64 size;
        gboolean changed;

        changed = FALSE;

        if (mdu_linux_lvm2_volume_group_get_lv_info (MDU_LINUX_LVM2_VOLUME_GROUP (volume->priv->enclosing_presentable),
                                                     volume->priv->uuid,
                                                     &position,
                                                     &name,
                                                     &size)) {
                if (volume->priv->position != position) {
                        volume->priv->position = position;
                        changed = TRUE;
                }
                if (g_strcmp0 (volume->priv->name, name) != 0) {
                        g_free (volume->priv->name);
                        volume->priv->name = name;
                        changed = TRUE;
                }
                if (volume->priv->size != size) {
                        volume->priv->size = size;
                        changed = TRUE;
                }
        }

        return changed;
}

/* Called when the VG changes */
static void
on_presentable_changed (MduPresentable *presentable, gpointer user_data)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (user_data);

        if (update_lv_info (volume)) {
                g_signal_emit_by_name (volume, "changed");
                g_signal_emit_by_name (volume->priv->pool, "presentable-changed", volume);
        }
}

/**
 * _mdu_linux_lvm2_volume_new:
 * @pool: A #MduPool.
 * @group_uuid: The UUID of the group that the logical volume belongs to.
 * @uuid: The UUID of the logical volume.
 * @enclosing_presentable: The enclosing presentable.
 *
 * Creates a new #MduLinuxLvm2Volume.
 */
MduLinuxLvm2Volume *
_mdu_linux_lvm2_volume_new (MduPool        *pool,
                            const gchar    *group_uuid,
                            const gchar    *uuid,
                            MduPresentable *enclosing_presentable)
{
        MduLinuxLvm2Volume *volume;

        volume = MDU_LINUX_LVM2_VOLUME (g_object_new (MDU_TYPE_LINUX_LVM2_VOLUME, NULL));
        volume->priv->pool = g_object_ref (pool);
        volume->priv->group_uuid = g_strdup (group_uuid);
        volume->priv->uuid = g_strdup (uuid);

        volume->priv->id = g_strdup_printf ("linux_lvm2_volume_%s_enclosed_by_%s",
                                        uuid,
                                        enclosing_presentable != NULL ? mdu_presentable_get_id (enclosing_presentable) : "(none)");

        volume->priv->enclosing_presentable =
                enclosing_presentable != NULL ? g_object_ref (enclosing_presentable) : NULL;

        update_lv_info (volume);
        /* Track the VG since we get the data like name and size from there */
        if (volume->priv->enclosing_presentable != NULL) {
                g_signal_connect (volume->priv->enclosing_presentable,
                                  "changed",
                                  G_CALLBACK (on_presentable_changed),
                                  volume);
        }

        g_signal_connect (volume->priv->pool, "device-added", G_CALLBACK (on_device_added), volume);
        g_signal_connect (volume->priv->pool, "device-removed", G_CALLBACK (on_device_removed), volume);
        g_signal_connect (volume->priv->pool, "device-changed", G_CALLBACK (on_device_changed), volume);
        find_lv (volume);

        return volume;
}

/* ---------------------------------------------------------------------------------------------------- */

/* MduPresentable methods */

static const gchar *
mdu_linux_lvm2_volume_get_id (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);

        return volume->priv->id;
}

static MduDevice *
mdu_linux_lvm2_volume_get_device (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);

        if (volume->priv->lv != NULL)
                return g_object_ref (volume->priv->lv);
        return NULL;
}

static MduPresentable *
mdu_linux_lvm2_volume_get_enclosing_presentable (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);

        if (volume->priv->enclosing_presentable != NULL)
                return g_object_ref (volume->priv->enclosing_presentable);
        return NULL;
}

static gchar *
get_names_and_desc (MduLinuxLvm2Volume  *volume,
                    gchar              **out_vpd_name,
                    gchar              **out_desc)
{
        gchar *ret;
        gchar *ret_desc;
        gchar *ret_vpd;

        ret = NULL;
        ret_desc = NULL;
        ret_vpd = NULL;

        ret = g_strdup (volume->priv->name);

        /* Translators: Description */
        ret_desc = g_strdup (_("Logical Volume"));

        /* Translators: VPD name */
        ret_vpd = g_strdup (_("LVM2 Logical Volume"));

        if (out_desc != NULL)
                *out_desc = ret_desc;
        else
                g_free (ret_desc);

        if (out_vpd_name != NULL)
                *out_vpd_name = ret_vpd;
        else
                g_free (ret_vpd);

        return ret;
}

static char *
_mdu_linux_lvm2_volume_get_name (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);

        if (volume->priv->lv != NULL)
                return _mdu_volume_get_names_and_desc (presentable, NULL, NULL);
        else
                return get_names_and_desc (volume, NULL, NULL);
}

static gchar *
mdu_linux_lvm2_volume_get_description (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);
        gchar *desc;
        gchar *name;

        if (volume->priv->lv != NULL)
                name = _mdu_volume_get_names_and_desc (presentable, NULL, &desc);
        else
                name = get_names_and_desc (volume, NULL, &desc);
        g_free (name);

        return desc;
}

static gchar *
mdu_linux_lvm2_volume_get_vpd_name (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);
        gchar *vpd_name;
        gchar *name;

        if (volume->priv->lv != NULL)
                name = _mdu_volume_get_names_and_desc (presentable, &vpd_name, NULL);
        else
                name = get_names_and_desc (volume, &vpd_name, NULL);
        g_free (name);

        return vpd_name;
}

static GIcon *
mdu_linux_lvm2_volume_get_icon (MduPresentable *presentable)
{
        GIcon *icon;

        icon = mdu_util_get_emblemed_icon ("mdu-multidisk-drive", "mdu-emblem-lvm-lv");

#if 0
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);
        if (volume->priv->lv != NULL) {
                const gchar *usage;

                usage = mdu_device_id_get_usage (volume->priv->lv);
                if (usage != NULL && strcmp (usage, "crypto") == 0) {
                        GEmblem *emblem;
                        GIcon *padlock;
                        GIcon *emblemed_icon;

                        padlock = g_themed_icon_new ("mdu-encrypted-lock");
                        emblem = g_emblem_new_with_origin (padlock, G_EMBLEM_ORIGIN_DEVICE);

                        emblemed_icon = g_emblemed_icon_new (icon, emblem);
                        g_object_unref (icon);
                        icon = emblemed_icon;

                        g_object_unref (padlock);
                        g_object_unref (emblem);
                }
        }
#endif

        return icon;
}

static guint64
mdu_linux_lvm2_volume_get_offset (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);

        return volume->priv->position;
}

static guint64
mdu_linux_lvm2_volume_get_size (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);
        return volume->priv->size;
}

static MduPool *
mdu_linux_lvm2_volume_get_pool (MduPresentable *presentable)
{
        MduLinuxLvm2Volume *volume = MDU_LINUX_LVM2_VOLUME (presentable);
        return g_object_ref (volume->priv->pool);
}

static gboolean
mdu_linux_lvm2_volume_is_allocated (MduPresentable *presentable)
{
        return TRUE;
}

static gboolean
mdu_linux_lvm2_volume_is_recognized (MduPresentable *presentable)
{
        /* TODO: maybe we need to return FALSE sometimes */
        return TRUE;
}

static void
mdu_linux_lvm2_volume_presentable_iface_init (MduPresentableIface *iface)
{
        iface->get_id                    = mdu_linux_lvm2_volume_get_id;
        iface->get_device                = mdu_linux_lvm2_volume_get_device;
        iface->get_enclosing_presentable = mdu_linux_lvm2_volume_get_enclosing_presentable;
        iface->get_name                  = _mdu_linux_lvm2_volume_get_name;
        iface->get_description           = mdu_linux_lvm2_volume_get_description;
        iface->get_vpd_name              = mdu_linux_lvm2_volume_get_vpd_name;
        iface->get_icon                  = mdu_linux_lvm2_volume_get_icon;
        iface->get_offset                = mdu_linux_lvm2_volume_get_offset;
        iface->get_size                  = mdu_linux_lvm2_volume_get_size;
        iface->get_pool                  = mdu_linux_lvm2_volume_get_pool;
        iface->is_allocated              = mdu_linux_lvm2_volume_is_allocated;
        iface->is_recognized             = mdu_linux_lvm2_volume_is_recognized;
}

/* ---------------------------------------------------------------------------------------------------- */

void
_mdu_linux_lvm2_volume_rewrite_enclosing_presentable (MduLinuxLvm2Volume *volume)
{
        if (volume->priv->enclosing_presentable != NULL) {
                const gchar *enclosing_presentable_id;
                MduPresentable *new_enclosing_presentable;

                enclosing_presentable_id = mdu_presentable_get_id (volume->priv->enclosing_presentable);

                new_enclosing_presentable = mdu_pool_get_presentable_by_id (volume->priv->pool,
                                                                            enclosing_presentable_id);
                if (new_enclosing_presentable == NULL) {
                        g_warning ("Error rewriting enclosing_presentable for %s, no such id %s",
                                   volume->priv->id,
                                   enclosing_presentable_id);
                        goto out;
                }

                g_object_unref (volume->priv->enclosing_presentable);
                volume->priv->enclosing_presentable = new_enclosing_presentable;
        }

 out:
        ;
}

/* ---------------------------------------------------------------------------------------------------- */

const gchar *
mdu_linux_lvm2_volume_get_name (MduLinuxLvm2Volume *volume)
{
        return volume->priv->name;
}

const gchar *
mdu_linux_lvm2_volume_get_uuid (MduLinuxLvm2Volume *volume)
{
        return volume->priv->uuid;
}

const gchar *
mdu_linux_lvm2_volume_get_group_uuid (MduLinuxLvm2Volume *volume)
{
        return volume->priv->group_uuid;
}

