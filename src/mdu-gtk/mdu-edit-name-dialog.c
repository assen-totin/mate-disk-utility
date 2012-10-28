/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-edit-name-dialog.c
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

#include "mdu-gtk.h"
#include "mdu-edit-name-dialog.h"

/* ---------------------------------------------------------------------------------------------------- */

struct MduEditNameDialogPrivate
{
        gchar *orig_name;
        GtkWidget *name_entry;
        guint name_max_bytes;

        gchar *message;
        gchar *entry_mnemonic;
};

enum {
        PROP_0,
        PROP_NAME,
        PROP_NAME_MAX_BYTES,
        PROP_MESSAGE,
        PROP_ENTRY_MNEMONIC,
};

G_DEFINE_TYPE (MduEditNameDialog, mdu_edit_name_dialog, MDU_TYPE_DIALOG)

static void
mdu_edit_name_dialog_finalize (GObject *object)
{
        MduEditNameDialog *dialog = MDU_EDIT_NAME_DIALOG (object);

        g_free (dialog->priv->orig_name);
        g_free (dialog->priv->message);
        g_free (dialog->priv->entry_mnemonic);

        if (G_OBJECT_CLASS (mdu_edit_name_dialog_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_edit_name_dialog_parent_class)->finalize (object);
}

static void
mdu_edit_name_dialog_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
        MduEditNameDialog *dialog = MDU_EDIT_NAME_DIALOG (object);

        switch (property_id) {
        case PROP_NAME:
                g_value_take_string (value, mdu_edit_name_dialog_get_name (dialog));
                break;

        case PROP_NAME_MAX_BYTES:
                g_value_set_uint (value, dialog->priv->name_max_bytes);
                break;

        case PROP_MESSAGE:
                g_value_set_string (value, dialog->priv->message);
                break;

        case PROP_ENTRY_MNEMONIC:
                g_value_set_string (value, dialog->priv->entry_mnemonic);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_edit_name_dialog_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
        MduEditNameDialog *dialog = MDU_EDIT_NAME_DIALOG (object);

        switch (property_id) {
        case PROP_NAME:
                mdu_edit_name_dialog_set_name (dialog, g_value_get_string (value));
                break;

        case PROP_NAME_MAX_BYTES:
                dialog->priv->name_max_bytes = g_value_get_uint (value);
                gtk_entry_set_max_length (GTK_ENTRY (dialog->priv->name_entry), dialog->priv->name_max_bytes);
                break;

        case PROP_MESSAGE:
                dialog->priv->message = g_value_dup_string (value);
                break;

        case PROP_ENTRY_MNEMONIC:
                dialog->priv->entry_mnemonic = g_value_dup_string (value);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

/* ---------------------------------------------------------------------------------------------------- */

static void
update_apply_sensitivity (MduEditNameDialog *dialog)
{
        gboolean name_differ;

        name_differ = FALSE;

        if (g_strcmp0 (dialog->priv->orig_name, gtk_entry_get_text (GTK_ENTRY (dialog->priv->name_entry))) != 0) {
                name_differ = TRUE;
        }

        if (name_differ) {
                gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_APPLY, TRUE);
        } else {
                gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog), GTK_RESPONSE_APPLY, FALSE);
        }
}

static void
update (MduEditNameDialog *dialog)
{
        update_apply_sensitivity (dialog);
}


/* ---------------------------------------------------------------------------------------------------- */

static void
name_entry_changed (GtkWidget *combo_box,
                     gpointer   user_data)
{
        MduEditNameDialog *dialog = MDU_EDIT_NAME_DIALOG (user_data);
        update_apply_sensitivity (dialog);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_edit_name_dialog_constructed (GObject *object)
{
        MduEditNameDialog *dialog = MDU_EDIT_NAME_DIALOG (object);
        GtkWidget *content_area;
        GtkWidget *hbox;
        GtkWidget *vbox;
        GtkWidget *image;
        GtkWidget *label;
        gchar *s;
        gchar *s2;
        GIcon *icon;
        GtkWidget *table;
        gint row;

        gtk_container_set_border_width (GTK_CONTAINER (dialog), 12);

        gtk_dialog_add_button (GTK_DIALOG (dialog),
                               GTK_STOCK_CANCEL,
                               GTK_RESPONSE_CANCEL);

        gtk_dialog_add_button (GTK_DIALOG (dialog),
                               GTK_STOCK_APPLY,
                               GTK_RESPONSE_APPLY);

        gtk_dialog_set_default_response (GTK_DIALOG (dialog),
                                         GTK_RESPONSE_APPLY);
        gtk_entry_set_activates_default (GTK_ENTRY (dialog->priv->name_entry),
                                         TRUE);

        content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

        icon = mdu_presentable_get_icon (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));

        hbox = gtk_hbox_new (FALSE, 12);
        gtk_box_pack_start (GTK_BOX (content_area), hbox, TRUE, TRUE, 0);

        image = gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_DIALOG);
        gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
        gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

        vbox = gtk_vbox_new (FALSE, 12);
        gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, FALSE, 0);

        s2 = mdu_presentable_get_vpd_name (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));
        s = g_strdup_printf (_("Edit %s"), s2);
        gtk_window_set_title (GTK_WINDOW (dialog), s);
        g_free (s);
        g_free (s2);

        /* --- */

        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_label_set_markup (GTK_LABEL (label), dialog->priv->message);
        gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

        /* --- */

        table = gtk_table_new (2, 2, FALSE);
        gtk_table_set_col_spacings (GTK_TABLE (table), 12);
        gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);

        row = 0;

        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_label_set_markup_with_mnemonic (GTK_LABEL (label), dialog->priv->entry_mnemonic);
        gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row + 1,
                          GTK_FILL, GTK_EXPAND | GTK_FILL, 2, 2);
        gtk_table_attach (GTK_TABLE (table), dialog->priv->name_entry, 1, 2, row, row + 1,
                          GTK_FILL, GTK_EXPAND | GTK_FILL, 2, 2);
        gtk_label_set_mnemonic_widget (GTK_LABEL (label), dialog->priv->name_entry);
        row++;

        /* --- */


        g_signal_connect (dialog->priv->name_entry,
                          "changed",
                          G_CALLBACK (name_entry_changed),
                          dialog);

        g_object_unref (icon);

        update (dialog);

        if (G_OBJECT_CLASS (mdu_edit_name_dialog_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_edit_name_dialog_parent_class)->constructed (object);
}

static void
mdu_edit_name_dialog_class_init (MduEditNameDialogClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (MduEditNameDialogPrivate));

        object_class->get_property = mdu_edit_name_dialog_get_property;
        object_class->set_property = mdu_edit_name_dialog_set_property;
        object_class->constructed  = mdu_edit_name_dialog_constructed;
        object_class->finalize     = mdu_edit_name_dialog_finalize;

        g_object_class_install_property (object_class,
                                         PROP_NAME,
                                         g_param_spec_string ("name",
                                                              NULL,
                                                              NULL,
                                                              NULL,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_STATIC_STRINGS));

        g_object_class_install_property (object_class,
                                         PROP_NAME_MAX_BYTES,
                                         g_param_spec_uint ("name-max-bytes",
                                                            NULL,
                                                            NULL,
                                                            0,
                                                            G_MAXUINT,
                                                            G_MAXUINT,
                                                            G_PARAM_READABLE |
                                                            G_PARAM_WRITABLE |
                                                            G_PARAM_CONSTRUCT_ONLY |
                                                            G_PARAM_STATIC_STRINGS));

        g_object_class_install_property (object_class,
                                         PROP_MESSAGE,
                                         g_param_spec_string ("message",
                                                              NULL,
                                                              NULL,
                                                              NULL,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_CONSTRUCT_ONLY |
                                                              G_PARAM_STATIC_STRINGS));

        g_object_class_install_property (object_class,
                                         PROP_ENTRY_MNEMONIC,
                                         g_param_spec_string ("entry-mnemonic",
                                                              NULL,
                                                              NULL,
                                                              NULL,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_CONSTRUCT_ONLY |
                                                              G_PARAM_STATIC_STRINGS));
}

static void
mdu_edit_name_dialog_init (MduEditNameDialog *dialog)
{
        dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
                                                    MDU_TYPE_EDIT_NAME_DIALOG,
                                                    MduEditNameDialogPrivate);

        dialog->priv->name_entry = gtk_entry_new ();
}

GtkWidget *
mdu_edit_name_dialog_new (GtkWindow        *parent,
                          MduPresentable   *presentable,
                          const gchar      *name,
                          guint             name_max_bytes,
                          const gchar      *message,
                          const gchar      *entry_mnemonic)
{
        g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);
        return GTK_WIDGET (g_object_new (MDU_TYPE_EDIT_NAME_DIALOG,
                                         "transient-for", parent,
                                         "presentable", presentable,
                                         "name", name,
                                         "name-max-bytes", name_max_bytes,
                                         "message", message,
                                         "entry-mnemonic", entry_mnemonic,
                                         NULL));
}

gchar *
mdu_edit_name_dialog_get_name (MduEditNameDialog *dialog)
{
        g_return_val_if_fail (MDU_IS_EDIT_NAME_DIALOG (dialog), NULL);
        return g_strdup (gtk_entry_get_text (GTK_ENTRY (dialog->priv->name_entry)));
}

void
mdu_edit_name_dialog_set_name (MduEditNameDialog *dialog,
                               const gchar       *name)
{
        g_return_if_fail (MDU_IS_EDIT_NAME_DIALOG (dialog));
        gtk_entry_set_text (GTK_ENTRY (dialog->priv->name_entry), name);
        g_free (dialog->priv->orig_name);
        dialog->priv->orig_name = g_strdup (name);
        update_apply_sensitivity (dialog);
}

