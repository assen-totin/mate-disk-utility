/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 *  Copyright (C) 2008-2009 Red Hat, Inc.
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
 *  Author: David Zeuthen <davidz@redhat.com>
 *
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include <mdu/mdu.h>
#include <mdu-gtk/mdu-gtk.h>

#include "mdu-partition-dialog.h"

struct MduPartitionDialogPrivate
{
        GtkWidget *scheme_combo_box;
        GtkWidget *scheme_desc_label;

        gchar *scheme;

        gulong removed_signal_handler_id;
};

enum
{
        PROP_0,
        PROP_SCHEME,
};

static void mdu_partition_dialog_constructed (GObject *object);

G_DEFINE_TYPE (MduPartitionDialog, mdu_partition_dialog, MDU_TYPE_DIALOG)

static void
mdu_partition_dialog_finalize (GObject *object)
{
        MduPartitionDialog *dialog = MDU_PARTITION_DIALOG (object);

        g_free (dialog->priv->scheme);

        if (dialog->priv->removed_signal_handler_id > 0) {
                g_signal_handler_disconnect (mdu_dialog_get_presentable (MDU_DIALOG (dialog)),
                                             dialog->priv->removed_signal_handler_id);
        }

        if (G_OBJECT_CLASS (mdu_partition_dialog_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_partition_dialog_parent_class)->finalize (object);
}

static void
mdu_partition_dialog_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
        MduPartitionDialog *dialog = MDU_PARTITION_DIALOG (object);

        switch (property_id) {
        case PROP_SCHEME:
                g_value_take_string (value, dialog->priv->scheme);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_partition_dialog_class_init (MduPartitionDialogClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (MduPartitionDialogPrivate));

        object_class->get_property = mdu_partition_dialog_get_property;
        object_class->constructed  = mdu_partition_dialog_constructed;
        object_class->finalize     = mdu_partition_dialog_finalize;

        g_object_class_install_property (object_class,
                                         PROP_SCHEME,
                                         g_param_spec_string ("scheme",
                                                              _("Partitioning Scheme"),
                                                              _("The selected partitioning scheme"),
                                                              NULL,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_STATIC_NAME |
                                                              G_PARAM_STATIC_NICK |
                                                              G_PARAM_STATIC_BLURB));
}

static void
mdu_partition_dialog_init (MduPartitionDialog *dialog)
{
        dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog, MDU_TYPE_PARTITION_DIALOG, MduPartitionDialogPrivate);
}

GtkWidget *
mdu_partition_dialog_new (GtkWindow            *parent,
                          MduPresentable       *presentable)
{
        return GTK_WIDGET (g_object_new (MDU_TYPE_PARTITION_DIALOG,
                                         "transient-for", parent,
                                         "presentable", presentable,
                                         NULL));
}

GtkWidget *
mdu_partition_dialog_new_for_drive (GtkWindow            *parent,
                                    MduDevice            *device)
{
        return GTK_WIDGET (g_object_new (MDU_TYPE_PARTITION_DIALOG,
                                         "transient-for", parent,
                                         "drive-device", device,
                                         NULL));
}

/* ---------------------------------------------------------------------------------------------------- */

gchar *
mdu_partition_dialog_get_scheme (MduPartitionDialog *dialog)
{
        g_return_val_if_fail (MDU_IS_PARTITION_DIALOG (dialog), NULL);
        return g_strdup (dialog->priv->scheme);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
update (MduPartitionDialog *dialog)
{
        /* keep in sync with where combo box is constructed in constructed() */
        g_free (dialog->priv->scheme);
        dialog->priv->scheme = mdu_util_part_table_type_combo_box_get_selected (dialog->priv->scheme_combo_box);
}

static void
on_combo_box_changed (GtkWidget *combo_box,
                      gpointer   user_data)
{
        MduPartitionDialog *dialog = MDU_PARTITION_DIALOG (user_data);
        update (dialog);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
on_presentable_removed (MduPresentable *presentable,
                        gpointer        user_data)
{
        MduPartitionDialog *dialog = MDU_PARTITION_DIALOG (user_data);

        gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_CANCEL);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_partition_dialog_constructed (GObject *object)
{
        MduPartitionDialog *dialog = MDU_PARTITION_DIALOG (object);
        GtkWidget *content_area;
        GtkWidget *action_area;
        GtkWidget *label;
        GtkWidget *hbox;
        GtkWidget *image;
        GtkWidget *table;
        GtkWidget *combo_box;
        GtkWidget *vbox2;
        GdkPixbuf *pixbuf;
        gint row;
        gboolean ret;
        GtkWidget *align;
        gchar *s;
        gchar *s2;

        ret = FALSE;

        pixbuf = mdu_util_get_pixbuf_for_presentable (mdu_dialog_get_presentable (MDU_DIALOG (dialog)),
                                                      GTK_ICON_SIZE_DIALOG);

        content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
        action_area = gtk_dialog_get_action_area (GTK_DIALOG (dialog));

        gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
        gtk_box_set_spacing (GTK_BOX (content_area), 0);
        gtk_container_set_border_width (GTK_CONTAINER (action_area), 5);
        gtk_box_set_spacing (GTK_BOX (action_area), 6);

        gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

        s2 = mdu_presentable_get_vpd_name (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));
        /* Translators: Format is a verb here. The %s is the name of the volume */
        s = g_strdup_printf (_("Format %s"), s2);
        gtk_window_set_title (GTK_WINDOW (dialog), s);
        g_free (s);
        g_free (s2);

        gtk_dialog_add_button (GTK_DIALOG (dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
        gtk_dialog_add_button (GTK_DIALOG (dialog),
                               /* Translators: Format is used as a verb here */
                               _("_Format"),
                               GTK_RESPONSE_OK);

        gtk_container_set_border_width (GTK_CONTAINER (content_area), 10);

        /*  icon and text labels  */
        hbox = gtk_hbox_new (FALSE, 0);
        gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, TRUE, 0);

        image = gtk_image_new_from_pixbuf (pixbuf);
        gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
        gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, TRUE, 12);

        align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
        gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 12, 0, 0);
        gtk_box_pack_start (GTK_BOX (hbox), align, TRUE, TRUE, 0);

        vbox2 = gtk_vbox_new (FALSE, 6);
        gtk_container_add (GTK_CONTAINER (align), vbox2);

        row = 0;

        table = gtk_table_new (2, 2, FALSE);
        gtk_table_set_col_spacings (GTK_TABLE (table), 12);
        gtk_table_set_row_spacings (GTK_TABLE (table), 6);
        gtk_box_pack_start (GTK_BOX (vbox2), table, FALSE, FALSE, 0);

        /*  partitioning scheme  */
        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        /* Translators: 'scheme' means 'partitioning scheme' here. */
        gtk_label_set_markup_with_mnemonic (GTK_LABEL (label), _("_Scheme:"));
        gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1,
                          GTK_FILL, GTK_EXPAND | GTK_FILL, 2, 2);

        combo_box = mdu_util_part_table_type_combo_box_create ();
        mdu_util_part_table_type_combo_box_select (combo_box, "mbr");
        dialog->priv->scheme = g_strdup ("mbr");
        gtk_table_attach (GTK_TABLE (table), combo_box, 1, 2, row, row +1,
                          GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 2, 2);
        gtk_label_set_mnemonic_widget (GTK_LABEL (label), combo_box);
        dialog->priv->scheme_combo_box = combo_box;
        row++;

        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
        gtk_table_attach (GTK_TABLE (table), label, 1, 2, row, row +1,
                          GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 2, 2);
        dialog->priv->scheme_desc_label = label;
        row++;
        mdu_util_part_table_type_combo_box_set_desc_label (dialog->priv->scheme_combo_box,
                                                           dialog->priv->scheme_desc_label);

        g_signal_connect (dialog->priv->scheme_combo_box,
                          "changed",
                          G_CALLBACK (on_combo_box_changed),
                          dialog);

        update (dialog);

        /* nuke dialog if device is yanked */
        dialog->priv->removed_signal_handler_id = g_signal_connect (mdu_dialog_get_presentable (MDU_DIALOG (dialog)),
                                                                    "removed",
                                                                    G_CALLBACK (on_presentable_removed),
                                                                    dialog);

        if (G_OBJECT_CLASS (mdu_partition_dialog_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_partition_dialog_parent_class)->constructed (object);
}

/* ---------------------------------------------------------------------------------------------------- */
