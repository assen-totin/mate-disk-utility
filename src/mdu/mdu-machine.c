/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-machine.c
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
#include "mdu-machine.h"
#include "mdu-presentable.h"
#include "mdu-linux-md-drive.h"

/**
 * SECTION:mdu-machine
 * @title: MduMachine
 * @short_description: Machines
 *
 * #MduMachine objects are used to represent connections to a udisk
 * instance on a local or remote machine.
 *
 * See the documentation for #MduPresentable for the big picture.
 */

struct _MduMachinePrivate
{
        MduPool *pool;
        gchar *id;
};

static GObjectClass *parent_class = NULL;

static void mdu_machine_presentable_iface_init (MduPresentableIface *iface);
G_DEFINE_TYPE_WITH_CODE (MduMachine, mdu_machine, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (MDU_TYPE_PRESENTABLE,
                                                mdu_machine_presentable_iface_init))


static void
mdu_machine_finalize (GObject *object)
{
        MduMachine *machine = MDU_MACHINE (object);

        g_free (machine->priv->id);

        if (machine->priv->pool != NULL)
                g_object_unref (machine->priv->pool);

        if (G_OBJECT_CLASS (parent_class)->finalize != NULL)
                (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static void
mdu_machine_class_init (MduMachineClass *klass)
{
        GObjectClass *obj_class = (GObjectClass *) klass;

        parent_class = g_type_class_peek_parent (klass);

        obj_class->finalize = mdu_machine_finalize;

        g_type_class_add_private (klass, sizeof (MduMachinePrivate));
}

static void
mdu_machine_init (MduMachine *machine)
{
        machine->priv = G_TYPE_INSTANCE_GET_PRIVATE (machine, MDU_TYPE_MACHINE, MduMachinePrivate);
}

MduMachine *
_mdu_machine_new (MduPool *pool)
{
        MduMachine *machine;
        const gchar *ssh_user_name;
        const gchar *ssh_address;
        static guint count = 0;

        machine = MDU_MACHINE (g_object_new (MDU_TYPE_MACHINE, NULL));
        machine->priv->pool = g_object_ref (pool);

        ssh_user_name = mdu_pool_get_ssh_user_name (machine->priv->pool);
        if (ssh_user_name == NULL)
                ssh_user_name = g_get_user_name ();
        ssh_address = mdu_pool_get_ssh_address (machine->priv->pool);

        if (ssh_address != NULL) {
                machine->priv->id = g_strdup_printf ("__machine_root_%d_for_%s@%s__",
                                                     count++,
                                                     ssh_user_name,
                                                     ssh_address);
        } else {
                machine->priv->id = g_strdup_printf ("__machine_root_%d__", count++);
        }
        return machine;
}

static const gchar *
mdu_machine_get_id (MduPresentable *presentable)
{
        MduMachine *machine = MDU_MACHINE (presentable);
        return machine->priv->id;
}

static MduDevice *
mdu_machine_get_device (MduPresentable *presentable)
{
        return NULL;
}

static MduPresentable *
mdu_machine_get_enclosing_presentable (MduPresentable *presentable)
{
        return NULL;
}

static char *
mdu_machine_get_name (MduPresentable *presentable)
{
        MduMachine *machine = MDU_MACHINE (presentable);
        const gchar *ssh_address;
        gchar *ret;

        ssh_address = mdu_pool_get_ssh_address (machine->priv->pool);

        if (ssh_address == NULL) {
                ret = g_strdup (_("Local Storage"));
        } else {
                /* TODO: use display-hostname */
                ret = g_strdup_printf (_("Storage on %s"), ssh_address);
        }

        return ret;
}

static gchar *
mdu_machine_get_vpd_name (MduPresentable *presentable)
{
        MduMachine *machine = MDU_MACHINE (presentable);
        const gchar *ssh_user_name;
        const gchar *ssh_address;
        gchar *ret;

        ssh_user_name = mdu_pool_get_ssh_user_name (machine->priv->pool);
        if (ssh_user_name == NULL)
                ssh_user_name = g_get_user_name ();
        ssh_address = mdu_pool_get_ssh_address (machine->priv->pool);

        if (ssh_address == NULL) {
                ret = g_strdup_printf ("%s@localhost", ssh_user_name);
        } else {
                /* TODO: include IP address */
                ret = g_strdup_printf ("%s@%s", ssh_user_name, ssh_address);
        }

        /* TODO: include udisks and OS version numbers? */

        return ret;

}

static gchar *
mdu_machine_get_description (MduPresentable *presentable)
{
        return mdu_machine_get_vpd_name (presentable);
}

static GIcon *
mdu_machine_get_icon (MduPresentable *presentable)
{
        GIcon *icon;
        icon = g_themed_icon_new_with_default_fallbacks ("computer"); /* TODO? */
        return icon;
}

static guint64
mdu_machine_get_offset (MduPresentable *presentable)
{
        return 0;
}

static guint64
mdu_machine_get_size (MduPresentable *presentable)
{
        return 0;
}

static MduPool *
mdu_machine_get_pool (MduPresentable *presentable)
{
        MduMachine *machine = MDU_MACHINE (presentable);
        return g_object_ref (machine->priv->pool);
}

static gboolean
mdu_machine_is_allocated (MduPresentable *presentable)
{
        return FALSE;
}

static gboolean
mdu_machine_is_recognized (MduPresentable *presentable)
{
        return FALSE;
}

static void
mdu_machine_presentable_iface_init (MduPresentableIface *iface)
{
        iface->get_id                    = mdu_machine_get_id;
        iface->get_device                = mdu_machine_get_device;
        iface->get_enclosing_presentable = mdu_machine_get_enclosing_presentable;
        iface->get_name                  = mdu_machine_get_name;
        iface->get_description           = mdu_machine_get_description;
        iface->get_vpd_name              = mdu_machine_get_vpd_name;
        iface->get_icon                  = mdu_machine_get_icon;
        iface->get_offset                = mdu_machine_get_offset;
        iface->get_size                  = mdu_machine_get_size;
        iface->get_pool                  = mdu_machine_get_pool;
        iface->is_allocated              = mdu_machine_is_allocated;
        iface->is_recognized             = mdu_machine_is_recognized;
}
