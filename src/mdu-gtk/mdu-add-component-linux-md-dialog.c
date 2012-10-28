/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* Copyright (C) 2009 David Zeuthen
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
#include "mdu-add-component-linux-md-dialog.h"

/* ---------------------------------------------------------------------------------------------------- */

struct MduAddComponentLinuxMdDialogPrivate
{
        GtkWidget *disk_selection_widget;
        MduAddComponentLinuxMdFlags flags;
};

enum {
        PROP_0,
        PROP_DRIVES,
        PROP_SIZE,
        PROP_FLAGS,
};

G_DEFINE_TYPE (MduAddComponentLinuxMdDialog, mdu_add_component_linux_md_dialog, MDU_TYPE_DIALOG)

static void
mdu_add_component_linux_md_dialog_finalize (GObject *object)
{
        /*MduAddComponentLinuxMdDialog *dialog = MDU_ADD_COMPONENT_LINUX_MD_DIALOG (object);*/

        if (G_OBJECT_CLASS (mdu_add_component_linux_md_dialog_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_add_component_linux_md_dialog_parent_class)->finalize (object);
}

static void
mdu_add_component_linux_md_dialog_set_property (GObject          *object,
                                                guint             property_id,
                                                const GValue     *value,
                                                GParamSpec       *pspec)
{
        MduAddComponentLinuxMdDialog *dialog = MDU_ADD_COMPONENT_LINUX_MD_DIALOG (object);

        switch (property_id) {

        case PROP_FLAGS:
                dialog->priv->flags = g_value_get_flags (value);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_add_component_linux_md_dialog_get_property (GObject    *object,
                                                guint       property_id,
                                                GValue     *value,
                                                GParamSpec *pspec)
{
        MduAddComponentLinuxMdDialog *dialog = MDU_ADD_COMPONENT_LINUX_MD_DIALOG (object);

        switch (property_id) {
        case PROP_DRIVES:
                g_value_take_boxed (value, mdu_add_component_linux_md_dialog_get_drives (dialog));
                break;

        case PROP_SIZE:
                g_value_set_uint64 (value, mdu_add_component_linux_md_dialog_get_size (dialog));
                break;

        case PROP_FLAGS:
                g_value_set_flags (value, dialog->priv->flags);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

/* ---------------------------------------------------------------------------------------------------- */

static void
update_add_sensitivity (MduAddComponentLinuxMdDialog *dialog)
{
        GPtrArray *drives;
        gboolean add_is_sensitive;

        drives = mdu_disk_selection_widget_get_selected_drives (MDU_DISK_SELECTION_WIDGET (dialog->priv->disk_selection_widget));
        add_is_sensitive = (drives->len > 0);
        g_ptr_array_unref (drives);

        gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog),
                                           GTK_RESPONSE_APPLY,
                                           add_is_sensitive);
}

static void
update (MduAddComponentLinuxMdDialog *dialog)
{
        update_add_sensitivity (dialog);
}


/* ---------------------------------------------------------------------------------------------------- */

static void
on_disk_selection_widget_changed (MduDiskSelectionWidget *widget,
                                  gpointer                user_data)
{
        MduAddComponentLinuxMdDialog *dialog = MDU_ADD_COMPONENT_LINUX_MD_DIALOG (user_data);
        update_add_sensitivity (dialog);
}

/* ---------------------------------------------------------------------------------------------------- */

static gchar *
on_is_drive_ignored (MduDiskSelectionWidget *widget,
                     MduDrive               *drive,
                     gpointer                user_data)
{
        MduAddComponentLinuxMdDialog *dialog = MDU_ADD_COMPONENT_LINUX_MD_DIALOG (user_data);
        gchar *ignored_reason;
        MduLinuxMdDrive *md_drive;

        ignored_reason = NULL;

        md_drive = MDU_LINUX_MD_DRIVE (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));

        if (MDU_PRESENTABLE (drive) == MDU_PRESENTABLE (md_drive)) {
                ignored_reason = g_strdup (_("The RAID Array to add a component to."));
                goto out;
        }

        /* TODO: check if drive has one or more components for our array - if so, return something
         * like "Device is already part of the array".
         */

 out:
        return ignored_reason;
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_add_component_linux_md_dialog_constructed (GObject *object)
{
        MduAddComponentLinuxMdDialog *dialog = MDU_ADD_COMPONENT_LINUX_MD_DIALOG (object);
        GtkWidget *content_area;
        GtkWidget *hbox;
        GtkWidget *vbox;
        GtkWidget *image;
        GtkWidget *label;
        gchar *s;
        gchar *s2;
        GIcon *icon;
        MduPresentable *p;
        MduDevice *d;
        MduPool *pool;
        GtkWidget *disk_selection_widget;
        GList *slaves;
        MduDevice *slave;
        guint64 component_size;
        gchar *component_size_str;
        gchar *array_name;
        gchar *array_name_vpd;
        MduDiskSelectionWidgetFlags flags;

        slaves = mdu_linux_md_drive_get_slaves (MDU_LINUX_MD_DRIVE (mdu_dialog_get_presentable (MDU_DIALOG (dialog))));
        slave = MDU_DEVICE (slaves->data);
        /* TODO: need size slider for e.g. linear arrays */
        component_size = mdu_device_get_size (slave);
        component_size_str = mdu_util_get_size_for_display (component_size, FALSE, FALSE);
        array_name = mdu_presentable_get_name (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));
        array_name_vpd = mdu_presentable_get_vpd_name (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));

        gtk_container_set_border_width (GTK_CONTAINER (dialog), 12);

        gtk_dialog_add_button (GTK_DIALOG (dialog),
                               GTK_STOCK_CANCEL,
                               GTK_RESPONSE_CANCEL);

        content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

        icon = mdu_presentable_get_icon (mdu_dialog_get_presentable (MDU_DIALOG (dialog)));

        hbox = gtk_hbox_new (FALSE, 12);
        gtk_box_pack_start (GTK_BOX (content_area), hbox, TRUE, TRUE, 0);

        image = gtk_image_new_from_gicon (icon, GTK_ICON_SIZE_DIALOG);
        gtk_misc_set_alignment (GTK_MISC (image), 0.5, 0.0);
        gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

        vbox = gtk_vbox_new (FALSE, 12);
        gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);

        p = mdu_dialog_get_presentable (MDU_DIALOG (dialog));
        d = mdu_presentable_get_device (p);
        pool = mdu_presentable_get_pool (p);

        flags = MDU_DISK_SELECTION_WIDGET_FLAGS_NONE;
        if (dialog->priv->flags & MDU_ADD_COMPONENT_LINUX_MD_FLAGS_SPARE) {
                s = g_strdup_printf (_("Add spare to %s"), array_name);
                s2 = g_strdup_printf (_("Select a device to create a %s spare on for the RAID Array \"%s\" (%s)"),
                                     component_size_str,
                                     array_name,
                                     array_name_vpd);
                gtk_dialog_add_button (GTK_DIALOG (dialog),
                                       GTK_STOCK_ADD,
                                       GTK_RESPONSE_APPLY);
        } else if (dialog->priv->flags & MDU_ADD_COMPONENT_LINUX_MD_FLAGS_EXPANSION) {
                s = g_strdup_printf (_("Expand %s"), array_name);
                s2 = g_strdup_printf (_("Select one or more devices to use %s on for expanding the RAID Array \"%s\" (%s)"),
                                     component_size_str,
                                     array_name,
                                     array_name_vpd);
                flags |= MDU_DISK_SELECTION_WIDGET_FLAGS_ALLOW_MULTIPLE;
                gtk_dialog_add_button (GTK_DIALOG (dialog),
                                       _("_Expand"),
                                       GTK_RESPONSE_APPLY);
        } else {
                g_assert_not_reached ();
        }
        gtk_window_set_title (GTK_WINDOW (dialog), s);
        g_free (s);

        /* --- */

        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
        gtk_label_set_width_chars (GTK_LABEL (label), 70); /* TODO: hate */
        gtk_label_set_markup (GTK_LABEL (label), s2);
        g_free (s2);
        gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

        /* --- */

        disk_selection_widget = mdu_disk_selection_widget_new (pool, flags);
        g_signal_connect (disk_selection_widget,
                          "is-drive-ignored",
                          G_CALLBACK (on_is_drive_ignored),
                          dialog);
        mdu_disk_selection_widget_set_component_size (MDU_DISK_SELECTION_WIDGET (disk_selection_widget),
                                                      component_size);
        dialog->priv->disk_selection_widget = disk_selection_widget;
        gtk_box_pack_start (GTK_BOX (vbox), disk_selection_widget, TRUE, TRUE, 0);

        /* --- */


        g_signal_connect (dialog->priv->disk_selection_widget,
                          "changed",
                          G_CALLBACK (on_disk_selection_widget_changed),
                          dialog);

        g_object_unref (icon);
        g_object_unref (d);
        g_object_unref (pool);

        /* select a sane size for the widget and allow resizing */
        gtk_widget_set_size_request (GTK_WIDGET (dialog), 550, 450);
        gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);

        update (dialog);

        g_list_foreach (slaves, (GFunc) g_object_unref, NULL);
        g_list_free (slaves);
        g_free (component_size_str);
        g_free (array_name);
        g_free (array_name_vpd);

        if (G_OBJECT_CLASS (mdu_add_component_linux_md_dialog_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_add_component_linux_md_dialog_parent_class)->constructed (object);
}

static void
mdu_add_component_linux_md_dialog_class_init (MduAddComponentLinuxMdDialogClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (MduAddComponentLinuxMdDialogPrivate));

        object_class->set_property = mdu_add_component_linux_md_dialog_set_property;
        object_class->get_property = mdu_add_component_linux_md_dialog_get_property;
        object_class->constructed  = mdu_add_component_linux_md_dialog_constructed;
        object_class->finalize     = mdu_add_component_linux_md_dialog_finalize;

        g_object_class_install_property (object_class,
                                         PROP_DRIVES,
                                         g_param_spec_boxed ("drives",
                                                             NULL,
                                                             NULL,
                                                             G_TYPE_PTR_ARRAY,
                                                             G_PARAM_READABLE));

        g_object_class_install_property (object_class,
                                         PROP_SIZE,
                                         g_param_spec_uint64 ("size",
                                                              NULL,
                                                              NULL,
                                                              0,
                                                              G_MAXUINT64,
                                                              0,
                                                              G_PARAM_READABLE));

        g_object_class_install_property (object_class,
                                         PROP_FLAGS,
                                         g_param_spec_flags ("flags",
                                                             NULL,
                                                             NULL,
                                                             MDU_TYPE_ADD_COMPONENT_LINUX_MD_FLAGS,
                                                             MDU_ADD_COMPONENT_LINUX_MD_FLAGS_NONE,
                                                             G_PARAM_READABLE|
                                                             G_PARAM_WRITABLE|
                                                             G_PARAM_CONSTRUCT_ONLY));
}

static void
mdu_add_component_linux_md_dialog_init (MduAddComponentLinuxMdDialog *dialog)
{
        dialog->priv = G_TYPE_INSTANCE_GET_PRIVATE (dialog,
                                                    MDU_TYPE_ADD_COMPONENT_LINUX_MD_DIALOG,
                                                    MduAddComponentLinuxMdDialogPrivate);
}

GtkWidget *
mdu_add_component_linux_md_dialog_new (GtkWindow                    *parent,
                                       MduAddComponentLinuxMdFlags   flags,
                                       MduLinuxMdDrive              *linux_md_drive)
{
        g_return_val_if_fail (MDU_IS_LINUX_MD_DRIVE (linux_md_drive), NULL);
        return GTK_WIDGET (g_object_new (MDU_TYPE_ADD_COMPONENT_LINUX_MD_DIALOG,
                                         "transient-for", parent,
                                         "presentable", linux_md_drive,
                                         "flags", flags,
                                         NULL));
}

GPtrArray *
mdu_add_component_linux_md_dialog_get_drives (MduAddComponentLinuxMdDialog *dialog)
{
        GPtrArray *drives;

        g_return_val_if_fail (MDU_IS_ADD_COMPONENT_LINUX_MD_DIALOG (dialog), NULL);

        drives = mdu_disk_selection_widget_get_selected_drives (MDU_DISK_SELECTION_WIDGET (dialog->priv->disk_selection_widget));

        return drives;
}

guint64
mdu_add_component_linux_md_dialog_get_size  (MduAddComponentLinuxMdDialog *dialog)
{
        g_return_val_if_fail (MDU_IS_ADD_COMPONENT_LINUX_MD_DIALOG (dialog), 0);
        return mdu_disk_selection_widget_get_component_size (MDU_DISK_SELECTION_WIDGET (dialog->priv->disk_selection_widget));
}

