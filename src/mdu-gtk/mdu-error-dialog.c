/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-error-dialog.c
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

#include "mdu-error-dialog.h"

/* ---------------------------------------------------------------------------------------------------- */
/* Gah, someone box GError in libgobject already */

#define MDU_TYPE_ERROR (mdu_error_get_type ())

static GError *
mdu_error_copy (const GError *error)
{
        return g_error_copy (error);
}

static void
mdu_error_free (GError *error)
{
        g_error_free (error);
}

static GType
mdu_error_get_type (void)
{
        static volatile gsize type_volatile = 0;

        if (g_once_init_enter (&type_volatile)) {
                GType type = g_boxed_type_register_static (
                                                           g_intern_static_string ("MduGError"),
                                                           (GBoxedCopyFunc) mdu_error_copy,
                                                           (GBoxedFreeFunc) mdu_error_free);
                g_once_init_leave (&type_volatile, type);
        }

        return type_volatile;
}

/* ---------------------------------------------------------------------------------------------------- */

struct MduErrorDialogPrivate
{
        gchar          *message;
        GError         *error;
};

enum
{
        PROP_0,
        PROP_MESSAGE,
        PROP_ERROR
};


G_DEFINE_TYPE (MduErrorDialog, mdu_error_dialog, MDU_TYPE_DIALOG)

static void
mdu_error_dialog_finalize (GObject *object)
{
        MduErrorDialog *dialog = MDU_ERROR_DIALOG (object);

        g_free (dialog->priv->message);
        g_error_free (dialog->priv->error);

        if (G_OBJECT_CLASS (mdu_error_dialog_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_error_dialog_parent_class)->finalize (object);
}

static void
mdu_error_dialog_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
        MduErrorDialog *dialog = MDU_ERROR_DIALOG (object);

        switch (property_id) {
        case PROP_MESSAGE:
                g_value_set_string (value, dialog->priv->message);
                break;

        case PROP_ERROR:
                g_value_set_boxed (value, dialog->priv->error);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_error_dialog_set_property (GObject      *object,
                               guint         property_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
        MduErrorDialog *dialog = MDU_ERROR_DIALOG (object);

        switch (property_id) {
        case PROP_MESSAGE:
                dialog->priv->message = g_value_dup_string (value);
                break;

        case PROP_ERROR:
                dialog->priv->error = g_value_dup_boxed (value);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_error_dialog_constructed (GObject *object)
{
        MduErrorDialog *dialog = MDU_ERROR_DIALOG (object);
        GtkWidget *button;
        GtkWidget *content_area;
        GtkWidget *hbox;
        GtkWidget *vbox;
        GtkWidget *image;
        GtkWidget *label;
        GtkWidget *expander;
        gchar *s;
        const gchar *error_msg;
        GtkWidget *scrolled_window;
        GtkWidget *text_view;
        GtkTextBuffer *buffer;
        MduPresentable *presentable;

        gtk_window_set_title (GTK_WINDOW (dialog), "");
        gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
        gtk_container_set_border_width (GTK_CONTAINER (dialog), 12);

        button = gtk_dialog_add_button (GTK_DIALOG (dialog),
                                        GTK_STOCK_CLOSE,
                                        GTK_RESPONSE_CLOSE);

        content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

        presentable = mdu_dialog_get_presentable (MDU_DIALOG (dialog));
        if (presentable != NULL) {
                GIcon *icon;
                GEmblem *emblem;
                GIcon *error_icon;
                GIcon *emblemed_icon;

                icon = mdu_presentable_get_icon (presentable);
                error_icon = g_themed_icon_new (GTK_STOCK_DIALOG_ERROR);
                emblem = g_emblem_new (icon);
                emblemed_icon = g_emblemed_icon_new (error_icon,
                                                     emblem);
                image = gtk_image_new_from_gicon (emblemed_icon, GTK_ICON_SIZE_DIALOG);

                g_object_unref (icon);
                g_object_unref (emblem);
                g_object_unref (error_icon);
                g_object_unref (emblemed_icon);
        } else {
                image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);
        }

        hbox = gtk_hbox_new (FALSE, 12);
        gtk_box_pack_start (GTK_BOX (content_area), hbox, TRUE, TRUE, 0);

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

        error_msg = NULL;
        if (dialog->priv->error->domain == MDU_ERROR) {
                switch (dialog->priv->error->code) {
                case MDU_ERROR_FAILED:
                        error_msg = _("The operation failed");
                        break;
                case MDU_ERROR_BUSY:
                        error_msg = _("The device is busy");
                        break;
                case MDU_ERROR_CANCELLED:
                        error_msg = _("The operation was canceled");
                        break;
                case MDU_ERROR_INHIBITED:
                        error_msg = _("The daemon is being inhibited");
                        break;
                case MDU_ERROR_INVALID_OPTION:
                        error_msg = _("An invalid option was passed");
                        break;
                case MDU_ERROR_NOT_SUPPORTED:
                        error_msg = _("The operation is not supported");
                        break;
                case MDU_ERROR_ATA_SMART_WOULD_WAKEUP:
                        error_msg = _("Getting ATA SMART data would wake up the device");
                        break;
                case MDU_ERROR_PERMISSION_DENIED:
                        error_msg = _("Permission denied");
                        break;
                case MDU_ERROR_FILESYSTEM_DRIVER_MISSING:
                        error_msg = _("Filesystem driver not installed");
                        break;
                case MDU_ERROR_FILESYSTEM_TOOLS_MISSING:
                        error_msg = _("Filesystem tools not installed");
                        break;
                }
        }
        if (error_msg == NULL)
                error_msg = _("Unknown error");

        if (presentable != NULL) {
                gchar *name;
                gchar *vpd_name;

                name = mdu_presentable_get_name (presentable);
                vpd_name = mdu_presentable_get_vpd_name (presentable);
                s = g_strdup_printf (_("An error occurred while performing an operation on \"%s\" (%s): %s"),
                                     name,
                                     vpd_name,
                                     error_msg);

                g_free (name);
                g_free (vpd_name);
        } else {
                s = g_strdup_printf (_("An error occurred: %s"), error_msg);
        }

        label = gtk_label_new (s);
        g_free (s);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
        gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

        s = g_strdup_printf ("<b>%s</b>", _("_Details"));
        expander = gtk_expander_new (s);
        g_free (s);
        gtk_expander_set_use_markup (GTK_EXPANDER (expander), TRUE);
        gtk_expander_set_use_underline (GTK_EXPANDER (expander), TRUE);
        gtk_box_pack_start (GTK_BOX (vbox), expander, FALSE, FALSE, 0);

        scrolled_window = gtk_scrolled_window_new (NULL, NULL);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                        GTK_POLICY_AUTOMATIC,
                                        GTK_POLICY_AUTOMATIC);
        gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolled_window),
                                             GTK_SHADOW_IN);
        buffer = gtk_text_buffer_new (NULL);
        gtk_text_buffer_set_text (buffer, dialog->priv->error->message, -1);
        text_view = gtk_text_view_new_with_buffer (buffer);
        g_object_unref (buffer);
        gtk_text_view_set_editable (GTK_TEXT_VIEW (text_view), FALSE);
        gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD_CHAR);
        gtk_container_add (GTK_CONTAINER (expander), scrolled_window);
        gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);

        gtk_widget_grab_focus (button);

        if (G_OBJECT_CLASS (mdu_error_dialog_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_error_dialog_parent_class)->constructed (object);
}

static void
mdu_error_dialog_class_init (MduErrorDialogClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (MduErrorDialogPrivate));

        object_class->get_property = mdu_error_dialog_get_property;
        object_class->set_property = mdu_error_dialog_set_property;
        object_class->constructed  = mdu_error_dialog_constructed;
        object_class->finalize     = mdu_error_dialog_finalize;

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
                                         PROP_ERROR,
                                         g_param_spec_boxed ("error",
                                                             NULL,
                                                             NULL,
                                                             MDU_TYPE_ERROR,
                                                             G_PARAM_READABLE |
                                                             G_PARAM_WRITABLE |
                                                             G_PARAM_CONSTRUCT_ONLY));
}

static void
mdu_error_dialog_init (MduErrorDialog *dialog)
{
        dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog, MDU_TYPE_ERROR_DIALOG, MduErrorDialogPrivate);
}

GtkWidget *
mdu_error_dialog_new (GtkWindow      *parent,
                      MduPresentable *presentable,
                      const gchar    *message,
                      const GError   *error)
{
        g_return_val_if_fail (presentable == NULL || MDU_IS_PRESENTABLE (presentable), NULL);
        return GTK_WIDGET (g_object_new (MDU_TYPE_ERROR_DIALOG,
                                         "transient-for", parent,
                                         "presentable", presentable,
                                         "message", message,
                                         "error", error,
                                         NULL));
}

GtkWidget *
mdu_error_dialog_new_for_drive (GtkWindow      *parent,
                                MduDevice      *device,
                                const gchar    *message,
                                const GError   *error)
{
        g_return_val_if_fail (MDU_IS_DEVICE (device), NULL);
        return GTK_WIDGET (g_object_new (MDU_TYPE_ERROR_DIALOG,
                                         "transient-for", parent,
                                         "drive-device", device,
                                         "message", message,
                                         "error", error,
                                         NULL));
}

GtkWidget *
mdu_error_dialog_new_for_volume (GtkWindow      *parent,
                                 MduDevice      *device,
                                 const gchar    *message,
                                 const GError   *error)
{
        g_return_val_if_fail (MDU_IS_DEVICE (device), NULL);
        return GTK_WIDGET (g_object_new (MDU_TYPE_ERROR_DIALOG,
                                         "transient-for", parent,
                                         "volume-device", device,
                                         "message", message,
                                         "error", error,
                                         NULL));
}

/* ---------------------------------------------------------------------------------------------------- */
