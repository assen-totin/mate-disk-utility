/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-confirmation-dialog.c
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

#include "mdu-confirmation-dialog.h"

struct MduConfirmationDialogPrivate
{
        gchar *message;
        gchar *button_text;
};

enum
{
        PROP_0,
        PROP_MESSAGE,
        PROP_BUTTON_TEXT
};


G_DEFINE_TYPE (MduConfirmationDialog, mdu_confirmation_dialog, MDU_TYPE_DIALOG)

static void
mdu_confirmation_dialog_finalize (GObject *object)
{
        MduConfirmationDialog *dialog = MDU_CONFIRMATION_DIALOG (object);

        g_free (dialog->priv->message);

        if (G_OBJECT_CLASS (mdu_confirmation_dialog_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_confirmation_dialog_parent_class)->finalize (object);
}

static void
mdu_confirmation_dialog_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
        MduConfirmationDialog *dialog = MDU_CONFIRMATION_DIALOG (object);

        switch (property_id) {
        case PROP_MESSAGE:
                g_value_set_string (value, dialog->priv->message);
                break;

        case PROP_BUTTON_TEXT:
                g_value_set_string (value, dialog->priv->button_text);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_confirmation_dialog_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
        MduConfirmationDialog *dialog = MDU_CONFIRMATION_DIALOG (object);

        switch (property_id) {
        case PROP_MESSAGE:
                dialog->priv->message = g_value_dup_string (value);
                break;

        case PROP_BUTTON_TEXT:
                dialog->priv->button_text = g_value_dup_string (value);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_confirmation_dialog_constructed (GObject *object)
{
        MduConfirmationDialog *dialog = MDU_CONFIRMATION_DIALOG (object);
        GtkWidget *content_area;
        GtkWidget *hbox;
        GtkWidget *vbox;
        GtkWidget *image;
        GtkWidget *label;
        gchar *s;
        GIcon *icon;
        GEmblem *emblem;
        GIcon *confirmation_icon;
        GIcon *emblemed_icon;
        gchar *name;
        gchar *vpd_name;

        icon = NULL;
        name = NULL;
        vpd_name = NULL;

        gtk_window_set_title (GTK_WINDOW (dialog), "");
        gtk_container_set_border_width (GTK_CONTAINER (dialog), 12);

        gtk_dialog_add_button (GTK_DIALOG (dialog),
                               GTK_STOCK_CANCEL,
                               GTK_RESPONSE_CANCEL);

        gtk_dialog_add_button (GTK_DIALOG (dialog),
                               dialog->priv->button_text,
                               GTK_RESPONSE_OK);

        content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

        icon = mdu_presentable_get_icon (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));
        confirmation_icon = g_themed_icon_new (GTK_STOCK_DIALOG_WARNING);
        emblem = g_emblem_new (icon);
        emblemed_icon = g_emblemed_icon_new (confirmation_icon,
                                             emblem);

        hbox = gtk_hbox_new (FALSE, 12);
        gtk_box_pack_start (GTK_BOX (content_area), hbox, TRUE, TRUE, 0);

        image = gtk_image_new_from_gicon (emblemed_icon,
                                          GTK_ICON_SIZE_DIALOG);
        gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
        gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

        vbox = gtk_vbox_new (FALSE, 12);
        gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, FALSE, 0);

        s = g_strdup_printf ("<big><big><b>%s</b></big></big>",
                             dialog->priv->message);
        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_label_set_markup (GTK_LABEL (label), s);
        g_free (s);
        gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

        name = mdu_presentable_get_name (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));
        vpd_name = mdu_presentable_get_vpd_name (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));
        if (MDU_IS_VOLUME (mdu_dialog_get_presentable (MDU_DIALOG (dialog)))) {
                s = g_strdup_printf (_("This operation concerns the volume \"%s\" (%s)"),
                                     name,
                                     vpd_name);
        } else if (MDU_IS_DRIVE (mdu_dialog_get_presentable (MDU_DIALOG (dialog)))) {
                s = g_strdup_printf (_("This operation concerns the drive \"%s\" (%s)"),
                                     name,
                                     vpd_name);
        } else {
                s = g_strdup_printf (_("This operation concerns \"%s\" (%s)"),
                                     name,
                                     vpd_name);
        }

        label = gtk_label_new (s);
        g_free (s);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
        gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

        g_object_unref (icon);
        g_object_unref (emblem);
        g_object_unref (confirmation_icon);
        g_object_unref (emblemed_icon);

        g_free (name);
        g_free (vpd_name);

        if (G_OBJECT_CLASS (mdu_confirmation_dialog_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_confirmation_dialog_parent_class)->constructed (object);
}

static void
mdu_confirmation_dialog_class_init (MduConfirmationDialogClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (MduConfirmationDialogPrivate));

        object_class->get_property = mdu_confirmation_dialog_get_property;
        object_class->set_property = mdu_confirmation_dialog_set_property;
        object_class->constructed  = mdu_confirmation_dialog_constructed;
        object_class->finalize     = mdu_confirmation_dialog_finalize;

        g_object_class_install_property (object_class,
                                         PROP_MESSAGE,
                                         g_param_spec_string ("message",
                                                              NULL,
                                                              NULL,
                                                              NULL,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_CONSTRUCT_ONLY));

        g_object_class_install_property (object_class,
                                         PROP_BUTTON_TEXT,
                                         g_param_spec_string ("button-text",
                                                              NULL,
                                                              NULL,
                                                              GTK_STOCK_DELETE,
                                                              G_PARAM_READABLE |
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_CONSTRUCT_ONLY));
}

static void
mdu_confirmation_dialog_init (MduConfirmationDialog *dialog)
{
        dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog, MDU_TYPE_CONFIRMATION_DIALOG, MduConfirmationDialogPrivate);
}

GtkWidget *
mdu_confirmation_dialog_new (GtkWindow      *parent,
                             MduPresentable *presentable,
                             const gchar    *message,
                             const gchar    *button_text)
{
        g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);
        return GTK_WIDGET (g_object_new (MDU_TYPE_CONFIRMATION_DIALOG,
                                         "transient-for", parent,
                                         "presentable", presentable,
                                         "message", message,
                                         "button-text", button_text,
                                         NULL));
}

GtkWidget *
mdu_confirmation_dialog_new_for_drive (GtkWindow      *parent,
                                       MduDevice      *device,
                                       const gchar    *message,
                                       const gchar    *button_text)
{
        g_return_val_if_fail (MDU_IS_DEVICE (device), NULL);
        return GTK_WIDGET (g_object_new (MDU_TYPE_CONFIRMATION_DIALOG,
                                         "transient-for", parent,
                                         "drive-device", device,
                                         "message", message,
                                         "button-text", button_text,
                                         NULL));
}

GtkWidget *
mdu_confirmation_dialog_new_for_volume (GtkWindow      *parent,
                                        MduDevice      *device,
                                        const gchar    *message,
                                        const gchar    *button_text)
{
        g_return_val_if_fail (MDU_IS_DEVICE (device), NULL);
        return GTK_WIDGET (g_object_new (MDU_TYPE_CONFIRMATION_DIALOG,
                                         "transient-for", parent,
                                         "volume-device", device,
                                         "message", message,
                                         "button-text", button_text,
                                         NULL));
}

/* ---------------------------------------------------------------------------------------------------- */
