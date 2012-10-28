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
#include "mdu-linux-lvm2-volume-hole.h"
#include "mdu-presentable.h"

struct _MduLinuxLvm2VolumeHolePrivate
{
        MduPool *pool;

        gchar *id;

        MduPresentable *enclosing_presentable;
};

static GObjectClass *parent_class = NULL;

static void mdu_linux_lvm2_volume_hole_presentable_iface_init (MduPresentableIface *iface);

G_DEFINE_TYPE_WITH_CODE (MduLinuxLvm2VolumeHole, mdu_linux_lvm2_volume_hole, MDU_TYPE_VOLUME_HOLE,
                         G_IMPLEMENT_INTERFACE (MDU_TYPE_PRESENTABLE,
                                                mdu_linux_lvm2_volume_hole_presentable_iface_init))

static void on_presentable_changed (MduPresentable *presentable, gpointer user_data);

static void
mdu_linux_lvm2_volume_hole_finalize (GObject *object)
{
        MduLinuxLvm2VolumeHole *volume_hole = MDU_LINUX_LVM2_VOLUME_HOLE (object);

        //g_debug ("##### finalized linux-lvm2 volume_hole '%s' %p", volume_hole->priv->id, lv);

        g_free (volume_hole->priv->id);

        if (volume_hole->priv->enclosing_presentable != NULL) {
                g_signal_handlers_disconnect_by_func (volume_hole->priv->enclosing_presentable,
                                                      on_presentable_changed,
                                                      volume_hole);
                g_object_unref (volume_hole->priv->enclosing_presentable);
        }

        if (G_OBJECT_CLASS (parent_class)->finalize)
                (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static void
mdu_linux_lvm2_volume_hole_class_init (MduLinuxLvm2VolumeHoleClass *klass)
{
        GObjectClass *gobject_class = (GObjectClass *) klass;

        parent_class = g_type_class_peek_parent (klass);

        gobject_class->finalize = mdu_linux_lvm2_volume_hole_finalize;

        g_type_class_add_private (klass, sizeof (MduLinuxLvm2VolumeHolePrivate));
}

static void
mdu_linux_lvm2_volume_hole_init (MduLinuxLvm2VolumeHole *volume_hole)
{
        volume_hole->priv = G_TYPE_INSTANCE_GET_PRIVATE (volume_hole, MDU_TYPE_LINUX_LVM2_VOLUME_HOLE, MduLinuxLvm2VolumeHolePrivate);
}

/**
 * _mdu_linux_lvm2_volume_hole_new:
 * @pool: A #MduPool.
 * @enclosing_presentable: The enclosing presentable.
 *
 * Creates a new #MduLinuxLvm2VolumeHole.
 */
MduLinuxLvm2VolumeHole *
_mdu_linux_lvm2_volume_hole_new (MduPool        *pool,
                                 MduPresentable *enclosing_presentable)
{
        MduLinuxLvm2VolumeHole *volume_hole;

        volume_hole = MDU_LINUX_LVM2_VOLUME_HOLE (g_object_new (MDU_TYPE_LINUX_LVM2_VOLUME_HOLE, NULL));
        volume_hole->priv->pool = g_object_ref (pool);

        volume_hole->priv->id = g_strdup_printf ("linux_lvm2_volume_hole_enclosed_by_%s",
                                                 enclosing_presentable != NULL ? mdu_presentable_get_id (enclosing_presentable) : "(none)");

        volume_hole->priv->enclosing_presentable =
                enclosing_presentable != NULL ? g_object_ref (enclosing_presentable) : NULL;

        /* Track the VG since we get the amount of free space from there */
        if (volume_hole->priv->enclosing_presentable != NULL) {
                g_signal_connect (volume_hole->priv->enclosing_presentable,
                                  "changed",
                                  G_CALLBACK (on_presentable_changed),
                                  volume_hole);
        }

        return volume_hole;
}

static void
on_presentable_changed (MduPresentable *presentable, gpointer user_data)
{
        MduLinuxLvm2VolumeHole *volume_hole = MDU_LINUX_LVM2_VOLUME_HOLE (user_data);

        g_signal_emit_by_name (volume_hole, "changed");
        g_signal_emit_by_name (volume_hole->priv->pool, "presentable-changed", volume_hole);
}

/* ---------------------------------------------------------------------------------------------------- */

/* MduPresentable methods */

static const gchar *
mdu_linux_lvm2_volume_hole_get_id (MduPresentable *presentable)
{
        MduLinuxLvm2VolumeHole *volume_hole = MDU_LINUX_LVM2_VOLUME_HOLE (presentable);

        return volume_hole->priv->id;
}

static MduDevice *
mdu_linux_lvm2_volume_hole_get_device (MduPresentable *presentable)
{
        return NULL;
}

static MduPresentable *
mdu_linux_lvm2_volume_hole_get_enclosing_presentable (MduPresentable *presentable)
{
        MduLinuxLvm2VolumeHole *volume_hole = MDU_LINUX_LVM2_VOLUME_HOLE (presentable);

        if (volume_hole->priv->enclosing_presentable != NULL)
                return g_object_ref (volume_hole->priv->enclosing_presentable);
        return NULL;
}

static guint64
mdu_linux_lvm2_volume_hole_get_size (MduPresentable *presentable)
{
        MduLinuxLvm2VolumeHole *volume_hole = MDU_LINUX_LVM2_VOLUME_HOLE (presentable);
        MduDevice *pv_device;
        guint64 ret;

        ret = 0;

        pv_device = mdu_linux_lvm2_volume_group_get_pv_device (MDU_LINUX_LVM2_VOLUME_GROUP (volume_hole->priv->enclosing_presentable));
        if (pv_device != NULL) {
                ret = mdu_device_linux_lvm2_pv_get_group_unallocated_size (pv_device);
                g_object_unref (pv_device);
        }

        return ret;
}

static gchar *
get_names_and_desc (MduPresentable  *presentable,
                    gchar          **out_vpd_name,
                    gchar          **out_desc)
{
        gchar *ret;
        gchar *ret_desc;
        gchar *ret_vpd;
        guint64 size;
        gchar *strsize;

        ret = NULL;
        ret_desc = NULL;
        ret_vpd = NULL;

        size = mdu_linux_lvm2_volume_hole_get_size (presentable);
        strsize = mdu_util_get_size_for_display (size, FALSE, FALSE);

        /* Translators: label for an unallocated space in a LVM2 volume group.
         * %s is the size, formatted like '45 GB'
         */
        ret = g_strdup_printf (_("%s Free"), strsize);
        g_free (strsize);

        /* Translators: Description */
        ret_desc = g_strdup (_("Unallocated Space"));

        /* Translators: VPD name */
        ret_vpd = g_strdup (_("LVM2 VG Unallocated Space"));

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
mdu_linux_lvm2_volume_hole_get_name (MduPresentable *presentable)
{
        return get_names_and_desc (presentable, NULL, NULL);
}

static gchar *
mdu_linux_lvm2_volume_hole_get_description (MduPresentable *presentable)
{
        gchar *desc;
        gchar *name;

        name = get_names_and_desc (presentable, NULL, &desc);
        g_free (name);

        return desc;
}

static gchar *
mdu_linux_lvm2_volume_hole_get_vpd_name (MduPresentable *presentable)
{
        gchar *vpd_name;
        gchar *name;

        name = get_names_and_desc (presentable, &vpd_name, NULL);
        g_free (name);

        return vpd_name;
}

static GIcon *
mdu_linux_lvm2_volume_hole_get_icon (MduPresentable *presentable)
{
        return g_themed_icon_new_with_default_fallbacks ("mdu-multidisk-drive");
}

static guint64
mdu_linux_lvm2_volume_hole_get_offset (MduPresentable *presentable)
{
        /* Halfway to G_MAXUINT64 - should guarantee that we're always at the end... */
        return G_MAXINT64;
}

static MduPool *
mdu_linux_lvm2_volume_hole_get_pool (MduPresentable *presentable)
{
        MduLinuxLvm2VolumeHole *volume_hole = MDU_LINUX_LVM2_VOLUME_HOLE (presentable);
        return g_object_ref (volume_hole->priv->pool);
}

static gboolean
mdu_linux_lvm2_volume_hole_is_allocated (MduPresentable *presentable)
{
        return FALSE;
}

static gboolean
mdu_linux_lvm2_volume_hole_is_recognized (MduPresentable *presentable)
{
        /* TODO: maybe we need to return FALSE sometimes */
        return TRUE;
}

static void
mdu_linux_lvm2_volume_hole_presentable_iface_init (MduPresentableIface *iface)
{
        iface->get_id                    = mdu_linux_lvm2_volume_hole_get_id;
        iface->get_device                = mdu_linux_lvm2_volume_hole_get_device;
        iface->get_enclosing_presentable = mdu_linux_lvm2_volume_hole_get_enclosing_presentable;
        iface->get_name                  = mdu_linux_lvm2_volume_hole_get_name;
        iface->get_description           = mdu_linux_lvm2_volume_hole_get_description;
        iface->get_vpd_name              = mdu_linux_lvm2_volume_hole_get_vpd_name;
        iface->get_icon                  = mdu_linux_lvm2_volume_hole_get_icon;
        iface->get_offset                = mdu_linux_lvm2_volume_hole_get_offset;
        iface->get_size                  = mdu_linux_lvm2_volume_hole_get_size;
        iface->get_pool                  = mdu_linux_lvm2_volume_hole_get_pool;
        iface->is_allocated              = mdu_linux_lvm2_volume_hole_is_allocated;
        iface->is_recognized             = mdu_linux_lvm2_volume_hole_is_recognized;
}

/* ---------------------------------------------------------------------------------------------------- */

void
_mdu_linux_lvm2_volume_hole_rewrite_enclosing_presentable (MduLinuxLvm2VolumeHole *volume_hole)
{
        if (volume_hole->priv->enclosing_presentable != NULL) {
                const gchar *enclosing_presentable_id;
                MduPresentable *new_enclosing_presentable;

                enclosing_presentable_id = mdu_presentable_get_id (volume_hole->priv->enclosing_presentable);

                new_enclosing_presentable = mdu_pool_get_presentable_by_id (volume_hole->priv->pool,
                                                                            enclosing_presentable_id);
                if (new_enclosing_presentable == NULL) {
                        g_warning ("Error rewriting enclosing_presentable for %s, no such id %s",
                                   volume_hole->priv->id,
                                   enclosing_presentable_id);
                        goto out;
                }

                g_object_unref (volume_hole->priv->enclosing_presentable);
                volume_hole->priv->enclosing_presentable = new_enclosing_presentable;
        }

 out:
        ;
}

/* ---------------------------------------------------------------------------------------------------- */
