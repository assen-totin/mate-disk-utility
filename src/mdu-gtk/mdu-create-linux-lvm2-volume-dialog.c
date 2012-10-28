/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 *  Copyright (C) 2008-2010 Red Hat, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author: Tomas Bzatek <tbzatek@redhat.com>
 *          David Zeuthen <davidz@redhat.com>
 *
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include <mdu/mdu.h>
#include <mdu-gtk/mdu-gtk.h>

#include "mdu-create-linux-lvm2-volume-dialog.h"

struct MduCreateLinuxLvm2VolumeDialogPrivate
{
        guint64 max_size;
        GtkWidget *size_widget;
        MduPresentable *drive;
};

enum
{
        PROP_0,
        PROP_SIZE,
        PROP_MAX_SIZE,
        PROP_DRIVE
};

static void mdu_create_linux_lvm2_volume_dialog_constructed (GObject *object);

G_DEFINE_TYPE (MduCreateLinuxLvm2VolumeDialog, mdu_create_linux_lvm2_volume_dialog, MDU_TYPE_FORMAT_DIALOG)

static void
mdu_create_linux_lvm2_volume_dialog_finalize (GObject *object)
{
        //MduCreateLinuxLvm2VolumeDialog *dialog = MDU_CREATE_LINUX_LVM2_VOLUME_DIALOG (object);

        if (G_OBJECT_CLASS (mdu_create_linux_lvm2_volume_dialog_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_create_linux_lvm2_volume_dialog_parent_class)->finalize (object);
}

static void
mdu_create_linux_lvm2_volume_dialog_get_property (GObject    *object,
                                          guint       property_id,
                                          GValue     *value,
                                          GParamSpec *pspec)
{
        MduCreateLinuxLvm2VolumeDialog *dialog = MDU_CREATE_LINUX_LVM2_VOLUME_DIALOG (object);

        switch (property_id) {
        case PROP_SIZE:
                g_value_set_uint64 (value, mdu_create_linux_lvm2_volume_dialog_get_size (dialog));
                break;

        case PROP_MAX_SIZE:
                g_value_set_uint64 (value, dialog->priv->max_size);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_create_linux_lvm2_volume_dialog_set_property (GObject      *object,
                                                  guint         property_id,
                                                  const GValue *value,
                                                  GParamSpec   *pspec)
{
        MduCreateLinuxLvm2VolumeDialog *dialog = MDU_CREATE_LINUX_LVM2_VOLUME_DIALOG (object);

        switch (property_id) {
        case PROP_MAX_SIZE:
                dialog->priv->max_size = g_value_get_uint64 (value);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_create_linux_lvm2_volume_dialog_class_init (MduCreateLinuxLvm2VolumeDialogClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (MduCreateLinuxLvm2VolumeDialogPrivate));

        object_class->get_property = mdu_create_linux_lvm2_volume_dialog_get_property;
        object_class->set_property = mdu_create_linux_lvm2_volume_dialog_set_property;
        object_class->constructed  = mdu_create_linux_lvm2_volume_dialog_constructed;
        object_class->finalize     = mdu_create_linux_lvm2_volume_dialog_finalize;

        g_object_class_install_property (object_class,
                                         PROP_MAX_SIZE,
                                         g_param_spec_uint64 ("max-size",
                                                              _("Maximum Logical Volume Size"),
                                                              _("The maximum possible logical volume size"),
                                                              0,
                                                              G_MAXUINT64,
                                                              0,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_CONSTRUCT_ONLY |
                                                              G_PARAM_STATIC_NAME |
                                                              G_PARAM_STATIC_NICK |
                                                              G_PARAM_STATIC_BLURB));

        g_object_class_install_property (object_class,
                                         PROP_SIZE,
                                         g_param_spec_uint64 ("size",
                                                              _("Logical Volume Size"),
                                                              _("The requested Logical Volume size"),
                                                              0,
                                                              G_MAXUINT64,
                                                              0,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_STATIC_NAME |
                                                              G_PARAM_STATIC_NICK |
                                                              G_PARAM_STATIC_BLURB));
}

static void
mdu_create_linux_lvm2_volume_dialog_init (MduCreateLinuxLvm2VolumeDialog *dialog)
{
        dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
                                                    MDU_TYPE_CREATE_LINUX_LVM2_VOLUME_DIALOG,
                                                    MduCreateLinuxLvm2VolumeDialogPrivate);
}

GtkWidget *
mdu_create_linux_lvm2_volume_dialog_new (GtkWindow            *parent,
                                         MduPresentable       *presentable,
                                         guint64               max_size,
                                         MduFormatDialogFlags  flags)
{
        return GTK_WIDGET (g_object_new (MDU_TYPE_CREATE_LINUX_LVM2_VOLUME_DIALOG,
                                         "transient-for", parent,
                                         "presentable", presentable,
                                         "max-size", max_size,
                                         "affirmative-button-mnemonic", _("C_reate"),
                                         "flags", flags,
                                         NULL));
}

/* ---------------------------------------------------------------------------------------------------- */

guint64
mdu_create_linux_lvm2_volume_dialog_get_size (MduCreateLinuxLvm2VolumeDialog *dialog)
{
        g_return_val_if_fail (MDU_IS_CREATE_LINUX_LVM2_VOLUME_DIALOG (dialog), 0);
        return mdu_size_widget_get_size (MDU_SIZE_WIDGET (dialog->priv->size_widget));
}

guint64
mdu_create_linux_lvm2_volume_dialog_get_max_size (MduCreateLinuxLvm2VolumeDialog *dialog)
{
        g_return_val_if_fail (MDU_IS_CREATE_LINUX_LVM2_VOLUME_DIALOG (dialog), 0);
        return dialog->priv->max_size;
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_create_linux_lvm2_volume_dialog_constructed (GObject *object)
{
        MduCreateLinuxLvm2VolumeDialog *dialog = MDU_CREATE_LINUX_LVM2_VOLUME_DIALOG (object);
        gchar *s;
        gchar *s2;
        GtkWidget *table;
        GtkWidget *label;
        GtkWidget *size_widget;
        guint row;

        table = mdu_format_dialog_get_table (MDU_FORMAT_DIALOG (dialog));
        g_object_get (table,
                      "n-rows", &row,
                      NULL);

        /*  add Logical Volume size widget before other widgets in the MduFormatDialog  */
        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_label_set_markup_with_mnemonic (GTK_LABEL (label), _("_Size:"));
        gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1,
                          GTK_FILL, GTK_EXPAND | GTK_FILL, 2, 2);
        size_widget = mdu_size_widget_new (0, 0, dialog->priv->max_size);
        mdu_size_widget_set_size (MDU_SIZE_WIDGET (size_widget), dialog->priv->max_size);
        gtk_table_attach (GTK_TABLE (table), size_widget, 1, 2, row, row + 1,
                          GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 2, 2);
        gtk_label_set_mnemonic_widget (GTK_LABEL (label), size_widget);
        dialog->priv->size_widget = size_widget;
        row++;

        /* run constructed() for parent class (MduFormatDialog) */
        if (G_OBJECT_CLASS (mdu_create_linux_lvm2_volume_dialog_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_create_linux_lvm2_volume_dialog_parent_class)->constructed (object);

        /* adjust window title */
        s2 = mdu_presentable_get_vpd_name (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));

        /* Translators: The %s is the name of the volume group */
        s = g_strdup_printf (_("Create Logical Volume on %s"), s2);
        gtk_window_set_title (GTK_WINDOW (dialog), s);
        g_free (s);


        g_free (s2);
}

/* ---------------------------------------------------------------------------------------------------- */
