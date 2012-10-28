/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-hub.c
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

#include <string.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>

#include "mdu-private.h"
#include "mdu-util.h"
#include "mdu-pool.h"
#include "mdu-adapter.h"
#include "mdu-expander.h"
#include "mdu-hub.h"
#include "mdu-presentable.h"
#include "mdu-linux-md-drive.h"

/**
 * SECTION:mdu-hub
 * @title: MduHub
 * @short_description: HUBs
 *
 * #MduHub objects are used to represent Host Adapters and Expanders
 * (e.g. SAS Expanders and SATA Port Multipliers).
 *
 * See the documentation for #MduPresentable for the big picture.
 */

struct _MduHubPrivate
{
        MduHubUsage usage;
        MduAdapter *adapter;
        MduExpander *expander;
        MduPool *pool;
        MduPresentable *enclosing_presentable;
        gchar *id;

        gchar *given_name;
        gchar *given_vpd_name;
        GIcon *given_icon;
};

static GObjectClass *parent_class = NULL;

static void mdu_hub_presentable_iface_init (MduPresentableIface *iface);
G_DEFINE_TYPE_WITH_CODE (MduHub, mdu_hub, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (MDU_TYPE_PRESENTABLE,
                                                mdu_hub_presentable_iface_init))

static void adapter_changed (MduAdapter *adapter, gpointer user_data);
static void expander_changed (MduExpander *expander, gpointer user_data);


static void
mdu_hub_finalize (GObject *object)
{
        MduHub *hub = MDU_HUB (object);

        //g_debug ("##### finalized hub '%s' %p", hub->priv->id, hub);

        if (hub->priv->expander != NULL) {
                g_signal_handlers_disconnect_by_func (hub->priv->expander, expander_changed, hub);
                g_object_unref (hub->priv->expander);
        }

        if (hub->priv->adapter != NULL) {
                g_signal_handlers_disconnect_by_func (hub->priv->adapter, adapter_changed, hub);
                g_object_unref (hub->priv->adapter);
        }

        if (hub->priv->pool != NULL)
                g_object_unref (hub->priv->pool);

        if (hub->priv->enclosing_presentable != NULL)
                g_object_unref (hub->priv->enclosing_presentable);

        g_free (hub->priv->id);

        g_free (hub->priv->given_name);
        g_free (hub->priv->given_vpd_name);
        if (hub->priv->given_icon != NULL)
                g_object_unref (hub->priv->given_icon);

        if (G_OBJECT_CLASS (parent_class)->finalize != NULL)
                (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static void
mdu_hub_class_init (MduHubClass *klass)
{
        GObjectClass *obj_class = (GObjectClass *) klass;

        parent_class = g_type_class_peek_parent (klass);

        obj_class->finalize = mdu_hub_finalize;

        g_type_class_add_private (klass, sizeof (MduHubPrivate));
}

static void
mdu_hub_init (MduHub *hub)
{
        hub->priv = G_TYPE_INSTANCE_GET_PRIVATE (hub, MDU_TYPE_HUB, MduHubPrivate);
}

static void
adapter_changed (MduAdapter *adapter, gpointer user_data)
{
        MduHub *hub = MDU_HUB (user_data);
        g_signal_emit_by_name (hub, "changed");
        g_signal_emit_by_name (hub->priv->pool, "presentable-changed", hub);
}

static void
expander_changed (MduExpander *expander, gpointer user_data)
{
        MduHub *hub = MDU_HUB (user_data);
        g_signal_emit_by_name (hub, "changed");
        g_signal_emit_by_name (hub->priv->pool, "presentable-changed", hub);
}

/**
 *
 * If @usage is %MDU_HUB_USAGE_ADAPTER the @adapter must not be %NULL and @expander, @name, @vpd_name and @icon must all be %NULL.
 * If @usage is %MDU_HUB_USAGE_EXPANDER the @adapter and @expander must not be %NULL and @name, @vpd_name and @icon must all be %NULL.
 * Otherwise @adapter and @expander must be %NULL and @name, @vpd_name and @icon must not not %NULL.
 */
MduHub *
_mdu_hub_new (MduPool        *pool,
              MduHubUsage     usage,
              MduAdapter     *adapter,
              MduExpander    *expander,
              const gchar    *name,
              const gchar    *vpd_name,
              GIcon          *icon,
              MduPresentable *enclosing_presentable)
{
        MduHub *hub;

        hub = MDU_HUB (g_object_new (MDU_TYPE_HUB, NULL));
        hub->priv->adapter = adapter != NULL ? g_object_ref (adapter) : NULL;
        hub->priv->expander = expander != NULL ? g_object_ref (expander) : NULL;
        hub->priv->pool = g_object_ref (pool);
        hub->priv->enclosing_presentable =
                enclosing_presentable != NULL ? g_object_ref (enclosing_presentable) : NULL;
        if (expander != NULL) {
                hub->priv->id = g_strdup_printf ("%s__enclosed_by_%s",
                                                 mdu_expander_get_native_path (hub->priv->expander),
                                                 enclosing_presentable != NULL ? mdu_presentable_get_id (enclosing_presentable) : "(none)");
        } else if (adapter != NULL) {
                hub->priv->id = g_strdup_printf ("%s__enclosed_by_%s",
                                                 mdu_adapter_get_native_path (hub->priv->adapter),
                                                 enclosing_presentable != NULL ? mdu_presentable_get_id (enclosing_presentable) : "(none)");
        } else {
                hub->priv->id = g_strdup_printf ("%s__enclosed_by_%s",
                                                 name,
                                                 enclosing_presentable != NULL ? mdu_presentable_get_id (enclosing_presentable) : "(none)");
        }
        if (adapter != NULL)
                g_signal_connect (adapter, "changed", (GCallback) adapter_changed, hub);
        if (expander != NULL)
                g_signal_connect (expander, "changed", (GCallback) expander_changed, hub);

        hub->priv->usage = usage;
        hub->priv->given_name = g_strdup (name);
        hub->priv->given_vpd_name = g_strdup (vpd_name);
        hub->priv->given_icon = icon != NULL ? g_object_ref (icon) : NULL;

        return hub;
}

static const gchar *
mdu_hub_get_id (MduPresentable *presentable)
{
        MduHub *hub = MDU_HUB (presentable);
        return hub->priv->id;
}

static MduDevice *
mdu_hub_get_device (MduPresentable *presentable)
{
        return NULL;
}

static MduPresentable *
mdu_hub_get_enclosing_presentable (MduPresentable *presentable)
{
        MduHub *hub = MDU_HUB (presentable);
        if (hub->priv->enclosing_presentable != NULL)
                return g_object_ref (hub->priv->enclosing_presentable);
        return NULL;
}

static char *
mdu_hub_get_name (MduPresentable *presentable)
{
        MduHub *hub = MDU_HUB (presentable);
        gchar *ret;

        if (hub->priv->expander != NULL) {
                /* TODO: include type e.g. SATA Port Multiplier, SAS Expander etc */
                ret = g_strdup (_("SAS Expander"));

        } else if (hub->priv->adapter != NULL) {
                const gchar *fabric;

                fabric = mdu_adapter_get_fabric (hub->priv->adapter);

                if (g_str_has_prefix (fabric, "ata_pata")) {
                        ret = g_strdup (_("PATA Host Adapter"));
                } else if (g_str_has_prefix (fabric, "ata_sata")) {
                        ret = g_strdup (_("SATA Host Adapter"));
                } else if (g_str_has_prefix (fabric, "ata")) {
                        ret = g_strdup (_("ATA Host Adapter"));
                } else if (g_str_has_prefix (fabric, "scsi_sas")) {
                        ret = g_strdup (_("SAS Host Adapter"));
                } else if (g_str_has_prefix (fabric, "scsi")) {
                        ret = g_strdup (_("SCSI Host Adapter"));
                } else {
                        ret = g_strdup (_("Host Adapter"));
                }

        } else {
                ret = g_strdup (hub->priv->given_name);
        }

        return ret;
}

static gchar *
mdu_hub_get_vpd_name (MduPresentable *presentable)
{
        MduHub *hub = MDU_HUB (presentable);
        gchar *s;
        const gchar *vendor;
        const gchar *model;

        if (hub->priv->expander != NULL) {
                vendor = mdu_expander_get_vendor (hub->priv->expander);
                model = mdu_expander_get_model (hub->priv->expander);
                s = g_strdup_printf ("%s %s", vendor, model);
        } else if (hub->priv->adapter != NULL) {
                vendor = mdu_adapter_get_vendor (hub->priv->adapter);
                model = mdu_adapter_get_model (hub->priv->adapter);
                //s = g_strdup_printf ("%s %s", vendor, model);
                s = g_strdup (model);
        } else {
                s = g_strdup (hub->priv->given_vpd_name);
        }

        return s;
}

static gchar *
mdu_hub_get_description (MduPresentable *presentable)
{
        /* TODO: include number of ports, speed, receptable type etc. */
        return mdu_hub_get_vpd_name (presentable);
}

static GIcon *
mdu_hub_get_icon (MduPresentable *presentable)
{
        GIcon *icon;
        MduHub *hub = MDU_HUB (presentable);

        if (hub->priv->given_icon != NULL) {
                icon = g_object_ref (hub->priv->given_icon);
        } else {
                if (hub->priv->usage == MDU_HUB_USAGE_EXPANDER)
                        icon = g_themed_icon_new_with_default_fallbacks ("mdu-expander");
                else
                        icon = g_themed_icon_new_with_default_fallbacks ("mdu-hba");
        }

        return icon;
}

static guint64
mdu_hub_get_offset (MduPresentable *presentable)
{
        return 0;
}

static guint64
mdu_hub_get_size (MduPresentable *presentable)
{
        return 0;
}

static MduPool *
mdu_hub_get_pool (MduPresentable *presentable)
{
        MduHub *hub = MDU_HUB (presentable);
        return g_object_ref (hub->priv->pool);
}

static gboolean
mdu_hub_is_allocated (MduPresentable *presentable)
{
        return FALSE;
}

static gboolean
mdu_hub_is_recognized (MduPresentable *presentable)
{
        return FALSE;
}

MduAdapter *
mdu_hub_get_adapter (MduHub *hub)
{
        return hub->priv->adapter != NULL ? g_object_ref (hub->priv->adapter) : NULL;
}

MduExpander *
mdu_hub_get_expander (MduHub *hub)
{
        return hub->priv->expander != NULL ? g_object_ref (hub->priv->expander) : NULL;
}

static void
mdu_hub_presentable_iface_init (MduPresentableIface *iface)
{
        iface->get_id                    = mdu_hub_get_id;
        iface->get_device                = mdu_hub_get_device;
        iface->get_enclosing_presentable = mdu_hub_get_enclosing_presentable;
        iface->get_name                  = mdu_hub_get_name;
        iface->get_description           = mdu_hub_get_description;
        iface->get_vpd_name              = mdu_hub_get_vpd_name;
        iface->get_icon                  = mdu_hub_get_icon;
        iface->get_offset                = mdu_hub_get_offset;
        iface->get_size                  = mdu_hub_get_size;
        iface->get_pool                  = mdu_hub_get_pool;
        iface->is_allocated              = mdu_hub_is_allocated;
        iface->is_recognized             = mdu_hub_is_recognized;
}

void
_mdu_hub_rewrite_enclosing_presentable (MduHub *hub)
{
        if (hub->priv->enclosing_presentable != NULL) {
                const gchar *enclosing_presentable_id;
                MduPresentable *new_enclosing_presentable;

                enclosing_presentable_id = mdu_presentable_get_id (hub->priv->enclosing_presentable);

                new_enclosing_presentable = mdu_pool_get_presentable_by_id (hub->priv->pool,
                                                                            enclosing_presentable_id);
                if (new_enclosing_presentable == NULL) {
                        g_warning ("Error rewriting enclosing_presentable for %s, no such id %s",
                                   hub->priv->id,
                                   enclosing_presentable_id);
                        goto out;
                }

                g_object_unref (hub->priv->enclosing_presentable);
                hub->priv->enclosing_presentable = new_enclosing_presentable;
        }

 out:
        ;
}
