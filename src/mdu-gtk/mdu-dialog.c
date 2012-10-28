/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-dialog.c
 *
 * Copyright (C) 2009 David Zeuthen
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

#include <atasmart.h>
#include <glib/gstdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "mdu-dialog.h"

/* ---------------------------------------------------------------------------------------------------- */

struct MduDialogPrivate
{
        MduPresentable *presentable;
        MduDevice *device;
        MduPool *pool;
};

enum
{
        PROP_0,
        PROP_PRESENTABLE,
        PROP_DRIVE_DEVICE,
        PROP_VOLUME_DEVICE,
};

G_DEFINE_ABSTRACT_TYPE (MduDialog, mdu_dialog, GTK_TYPE_DIALOG)

static void
mdu_dialog_finalize (GObject *object)
{
        MduDialog *dialog = MDU_DIALOG (object);

        if (dialog->priv->presentable != NULL) {
                g_object_unref (dialog->priv->presentable);
        }
        if (dialog->priv->device != NULL) {
                g_object_unref (dialog->priv->device);
        }
        if (dialog->priv->pool != NULL) {
                g_object_unref (dialog->priv->pool);
        }

        if (G_OBJECT_CLASS (mdu_dialog_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_dialog_parent_class)->finalize (object);
}

static void
mdu_dialog_get_property (GObject    *object,
                         guint       property_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
        MduDialog *dialog = MDU_DIALOG (object);

        switch (property_id) {
        case PROP_PRESENTABLE:
                g_value_set_object (value, dialog->priv->presentable);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_dialog_set_property (GObject      *object,
                         guint         property_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
        MduDialog *dialog = MDU_DIALOG (object);
        MduDevice *device;
        MduPool *pool;

        switch (property_id) {
        case PROP_PRESENTABLE:
                if (g_value_get_object (value) != NULL) {
                        g_warn_if_fail (dialog->priv->presentable == NULL);
                        dialog->priv->presentable = g_value_dup_object (value);
                        dialog->priv->device = mdu_presentable_get_device (dialog->priv->presentable);
                        dialog->priv->pool = mdu_presentable_get_pool (dialog->priv->presentable);
                }
                break;

        case PROP_DRIVE_DEVICE:
                device = MDU_DEVICE (g_value_get_object (value));
                if (device != NULL) {
                        pool = mdu_device_get_pool (device);
                        g_warn_if_fail (dialog->priv->presentable == NULL);
                        dialog->priv->presentable = mdu_pool_get_drive_by_device (pool, device);
                        dialog->priv->device = mdu_presentable_get_device (dialog->priv->presentable);
                        dialog->priv->pool = mdu_presentable_get_pool (dialog->priv->presentable);
                        g_object_unref (pool);
                }
                break;

        case PROP_VOLUME_DEVICE:
                device = MDU_DEVICE (g_value_get_object (value));
                if (device != NULL) {
                        pool = mdu_device_get_pool (device);
                        g_warn_if_fail (dialog->priv->presentable == NULL);
                        dialog->priv->presentable = mdu_pool_get_volume_by_device (pool, device);
                        dialog->priv->device = mdu_presentable_get_device (dialog->priv->presentable);
                        dialog->priv->pool = mdu_presentable_get_pool (dialog->priv->presentable);
                        g_object_unref (pool);
                }
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_dialog_class_init (MduDialogClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (MduDialogPrivate));

        object_class->get_property = mdu_dialog_get_property;
        object_class->set_property = mdu_dialog_set_property;
        object_class->finalize     = mdu_dialog_finalize;

        g_object_class_install_property (object_class,
                                         PROP_PRESENTABLE,
                                         g_param_spec_object ("presentable",
                                                              NULL,
                                                              NULL,
                                                              MDU_TYPE_PRESENTABLE,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_CONSTRUCT_ONLY));

        g_object_class_install_property (object_class,
                                         PROP_DRIVE_DEVICE,
                                         g_param_spec_object ("drive-device",
                                                              NULL,
                                                              NULL,
                                                              MDU_TYPE_DEVICE,
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_CONSTRUCT_ONLY));

        g_object_class_install_property (object_class,
                                         PROP_VOLUME_DEVICE,
                                         g_param_spec_object ("volume-device",
                                                              NULL,
                                                              NULL,
                                                              MDU_TYPE_DEVICE,
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_CONSTRUCT_ONLY));
}

static void
mdu_dialog_init (MduDialog *dialog)
{
        dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog, MDU_TYPE_DIALOG, MduDialogPrivate);
}

MduPresentable *
mdu_dialog_get_presentable (MduDialog *dialog)
{
        return dialog->priv->presentable;
}

MduDevice *
mdu_dialog_get_device (MduDialog *dialog)
{
        return dialog->priv->device;
}

MduPool *
mdu_dialog_get_pool (MduDialog *dialog)
{
        return dialog->priv->pool;
}

/* ---------------------------------------------------------------------------------------------------- */
