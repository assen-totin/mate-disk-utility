/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-section-linux-md-drive.c
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
#include <glib/gi18n.h>

#include <string.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include <math.h>

#include <mdu/mdu.h>
#include <mdu-gtk/mdu-gtk.h>

#include "mdu-section-drive.h"
#include "mdu-section-linux-lvm2-volume-group.h"

struct _MduSectionLinuxLvm2VolumeGroupPrivate
{
        MduDetailsElement *name_element;
        MduDetailsElement *state_element;
        MduDetailsElement *capacity_element;
        MduDetailsElement *extent_size_element;
        MduDetailsElement *unallocated_size_element;
        MduDetailsElement *num_pvs_element;

        MduButtonElement *vg_start_button;
        MduButtonElement *vg_stop_button;
        MduButtonElement *vg_edit_name_button;
        MduButtonElement *vg_edit_pvs_button;
};

G_DEFINE_TYPE (MduSectionLinuxLvm2VolumeGroup, mdu_section_linux_lvm2_volume_group, MDU_TYPE_SECTION)

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_linux_lvm2_volume_group_finalize (GObject *object)
{
        //MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (object);

        if (G_OBJECT_CLASS (mdu_section_linux_lvm2_volume_group_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_section_linux_lvm2_volume_group_parent_class)->finalize (object);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
lvm2_vg_start_op_callback (MduPool   *pool,
                           GError    *error,
                           gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                               NULL,
                                               _("Error starting Volume Group"),
                                               error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        }
        g_object_unref (shell);
}

static void
on_lvm2_vg_start_button_clicked (MduButtonElement *button_element,
                                 gpointer          user_data)
{
        MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (user_data);
        MduLinuxLvm2VolumeGroup *vg;
        MduPool *pool;
        const gchar *uuid;

        vg = MDU_LINUX_LVM2_VOLUME_GROUP (mdu_section_get_presentable (MDU_SECTION (section)));
        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (vg));

        uuid = mdu_linux_lvm2_volume_group_get_uuid (vg);

        mdu_pool_op_linux_lvm2_vg_start (pool,
                                        uuid,
                                        lvm2_vg_start_op_callback,
                                        g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

        g_object_unref (pool);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
lvm2_vg_stop_op_callback (MduPool   *pool,
                          GError    *error,
                          gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                               NULL,
                                               _("Error stopping Volume Group"),
                                               error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        }
        g_object_unref (shell);
}

static void
on_lvm2_vg_stop_button_clicked (MduButtonElement *button_element,
                                gpointer          user_data)
{
        MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (user_data);
        MduLinuxLvm2VolumeGroup *vg;
        MduPool *pool;
        const gchar *uuid;

        vg = MDU_LINUX_LVM2_VOLUME_GROUP (mdu_section_get_presentable (MDU_SECTION (section)));
        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (vg));

        uuid = mdu_linux_lvm2_volume_group_get_uuid (vg);

        mdu_pool_op_linux_lvm2_vg_stop (pool,
                                        uuid,
                                        lvm2_vg_stop_op_callback,
                                        g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

        g_object_unref (pool);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
lvm2_vg_set_name_op_callback (MduPool   *pool,
                              GError    *error,
                              gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                               NULL,
                                               _("Error setting name for Volume Group"),
                                               error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        }
        g_object_unref (shell);
}

static void
on_vg_edit_name_clicked (MduButtonElement *button_element,
                         gpointer          user_data)
{
        MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (user_data);
        MduLinuxLvm2VolumeGroup *vg;
        MduPool *pool;
        const gchar *uuid;
        gchar *vg_name;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;

        vg = MDU_LINUX_LVM2_VOLUME_GROUP (mdu_section_get_presentable (MDU_SECTION (section)));
        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (vg));
        uuid = mdu_linux_lvm2_volume_group_get_uuid (vg);
        vg_name = mdu_presentable_get_name (MDU_PRESENTABLE (vg));

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_edit_name_dialog_new (toplevel,
                                           MDU_PRESENTABLE (vg),
                                           vg_name,
                                           256,
                                           _("Choose a new Volume Group name."),
                                           _("_Name:"));
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        if (response == GTK_RESPONSE_APPLY) {
                gchar *new_name;
                new_name = mdu_edit_name_dialog_get_name (MDU_EDIT_NAME_DIALOG (dialog));
                mdu_pool_op_linux_lvm2_vg_set_name (pool,
                                                    uuid,
                                                    new_name,
                                                    lvm2_vg_set_name_op_callback,
                                                    g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));
                g_free (new_name);
        }
        gtk_widget_destroy (dialog);

        g_object_unref (pool);
        g_free (vg_name);
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct {
        MduShell *shell;
        MduLinuxLvm2VolumeGroup *vg;
        MduDrive *drive_to_add_to;
        guint64 size;
} AddPvData;

static void
add_pv_data_free (AddPvData *data)
{
        if (data->shell != NULL)
                g_object_unref (data->shell);
        if (data->vg != NULL)
                g_object_unref (data->vg);
        if (data->drive_to_add_to != NULL)
                g_object_unref (data->drive_to_add_to);
        g_free (data);
}

static void
add_pv_cb (MduPool    *pool,
           GError     *error,
           gpointer    user_data)
{
        AddPvData *data = user_data;

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (data->shell)),
                                               MDU_PRESENTABLE (data->vg),
                                               _("Error adding Physical Volume to Volume Group"),
                                               error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        }

        if (data != NULL)
                add_pv_data_free (data);
}

static void
new_pv_create_volume_cb (MduDrive     *drive,
                         GAsyncResult *res,
                         gpointer      user_data)
{
        AddPvData *data = user_data;
        MduVolume *volume;
        GError *error;

        error = NULL;
        volume = mdu_drive_create_volume_finish (drive,
                                                 res,
                                                 &error);
        if (volume == NULL) {
                mdu_shell_raise_error (data->shell,
                                       NULL,
                                       error,
                                       _("Error creating PV for VG"));
                g_error_free (error);
                add_pv_data_free (data);
        } else {
                MduPool *pool;
                MduDevice *pv_device;

                pv_device = mdu_presentable_get_device (MDU_PRESENTABLE (volume));
                pool = mdu_device_get_pool (pv_device);

                mdu_pool_op_linux_lvm2_vg_add_pv (pool,
                                                  mdu_linux_lvm2_volume_group_get_uuid (data->vg),
                                                  mdu_device_get_object_path (pv_device),
                                                  add_pv_cb,
                                                  data);

                g_object_unref (pool);
                g_object_unref (pv_device);
                g_object_unref (volume);
        }
}

static void
on_pvs_dialog_new_button_clicked (MduEditLinuxMdDialog *_dialog,
                                  gpointer              user_data)
{
        MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (user_data);
        MduLinuxLvm2VolumeGroup *vg;
        GtkWidget *dialog;
        gint response;
        GtkWindow *toplevel;
        AddPvData *data;

        dialog = NULL;

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));

        vg = MDU_LINUX_LVM2_VOLUME_GROUP (mdu_section_get_presentable (MDU_SECTION (section)));

        dialog = mdu_add_pv_linux_lvm2_dialog_new (toplevel, vg);
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_hide (dialog);
        if (response != GTK_RESPONSE_APPLY)
                goto out;

        data = g_new0 (AddPvData, 1);
        data->shell = g_object_ref (mdu_section_get_shell (MDU_SECTION (section)));
        data->vg = g_object_ref (vg);
        data->drive_to_add_to = mdu_add_pv_linux_lvm2_dialog_get_drive (MDU_ADD_PV_LINUX_LVM2_DIALOG (dialog));
        data->size = mdu_add_pv_linux_lvm2_dialog_get_size (MDU_ADD_PV_LINUX_LVM2_DIALOG (dialog));

        mdu_drive_create_volume (data->drive_to_add_to,
                                 data->size,
                                 mdu_linux_lvm2_volume_group_get_uuid (vg), /* TODO: name */
                                 MDU_CREATE_VOLUME_FLAGS_LINUX_LVM2,
                                 (GAsyncReadyCallback) new_pv_create_volume_cb,
                                 data);

 out:
        if (dialog != NULL)
                gtk_widget_destroy (dialog);
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct {
        MduShell *shell;
        MduLinuxLvm2VolumeGroup *vg;
        gchar *pv_uuid;
        MduDevice *pv; /* may be NULL */
} RemovePvData;

static void
remove_pv_data_free (RemovePvData *data)
{
        g_object_unref (data->shell);
        g_object_unref (data->vg);
        g_free (data->pv_uuid);
        if (data->pv != NULL)
                g_object_unref (data->pv);
        g_free (data);
}

static void
remove_pv_delete_partition_op_callback (MduDevice  *device,
                                        GError     *error,
                                        gpointer    user_data)
{
        RemovePvData *data = user_data;

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_drive (GTK_WINDOW (mdu_shell_get_toplevel (data->shell)),
                                                         device,
                                                         _("Error deleting partition for Physical Volume in Volume Group"),
                                                         error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        }

        remove_pv_data_free (data);
}

static void
remove_pv_op_callback (MduPool    *pool,
                       GError     *error,
                       gpointer    user_data)
{
        RemovePvData *data = user_data;

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (data->shell)),
                                               MDU_PRESENTABLE (data->vg),
                                               _("Error removing Physical Volume from Volume Group"),
                                               error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);

                remove_pv_data_free (data);
        } else {
                /* if the device is a partition, also remove the partition */
                if (data->pv != NULL && mdu_device_is_partition (data->pv)) {
                        mdu_device_op_partition_delete (data->pv,
                                                        remove_pv_delete_partition_op_callback,
                                                        data);
                } else {
                        remove_pv_data_free (data);
                }
        }
}

static MduDevice *
find_pv_with_uuid (MduPool     *pool,
                   const gchar *pv_uuid)
{
        MduDevice *ret;
        GList *devices;
        GList *l;

        ret = NULL;
        devices = mdu_pool_get_devices (pool);
        for (l = devices; l != NULL; l = l->next) {
                MduDevice *d = MDU_DEVICE (l->data);

                if (mdu_device_is_linux_lvm2_pv (d) &&
                    g_strcmp0 (mdu_device_linux_lvm2_pv_get_uuid (d), pv_uuid) == 0) {
                        ret = g_object_ref (d);
                        goto out;
                }
        }

 out:
        g_list_foreach (devices, (GFunc) g_object_unref, NULL);
        g_list_free (devices);

        return ret;
}


static void
on_pvs_dialog_remove_button_clicked (MduEditLinuxMdDialog   *_dialog,
                                     const gchar            *pv_uuid,
                                     gpointer                user_data)
{
        MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (user_data);
        MduLinuxLvm2VolumeGroup *vg;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;
        RemovePvData *data;
        MduPool *pool;
        MduDevice *pv;

        pool = NULL;
        pv = NULL;

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));

        vg = MDU_LINUX_LVM2_VOLUME_GROUP (mdu_section_get_presentable (MDU_SECTION (section)));

        /* TODO: more details in this dialog - e.g. "The VG may degrade" etc etc */
        dialog = mdu_confirmation_dialog_new (toplevel,
                                              MDU_PRESENTABLE (vg),
                                              _("Are you sure you want the remove the Physical Volume?"),
                                              _("_Remove"));
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_hide (dialog);
        gtk_widget_destroy (dialog);
        if (response != GTK_RESPONSE_OK)
                goto out;

        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (vg));

        pv = find_pv_with_uuid (pool, pv_uuid);

        data = g_new0 (RemovePvData, 1);
        data->shell = g_object_ref (mdu_section_get_shell (MDU_SECTION (section)));
        data->vg = g_object_ref (vg);
        data->pv_uuid = g_strdup (pv_uuid);

        data->pv = pv != NULL ? g_object_ref (pv) : NULL;

        mdu_pool_op_linux_lvm2_vg_remove_pv (pool,
                                             mdu_linux_lvm2_volume_group_get_uuid (data->vg),
                                             pv_uuid,
                                             remove_pv_op_callback,
                                             data);

 out:
        if (pool != NULL)
                g_object_unref (pool);
        if (pv != NULL)
                g_object_unref (pv);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
on_vg_edit_pvs_button_clicked (MduButtonElement *button_element,
                               gpointer          user_data)
{
        MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (user_data);
        MduPresentable *p;
        GtkWindow *toplevel;
        GtkWidget *dialog;

        p = mdu_section_get_presentable (MDU_SECTION (section));
        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));

        dialog = mdu_edit_linux_lvm2_dialog_new (toplevel, MDU_LINUX_LVM2_VOLUME_GROUP (p));

        g_signal_connect (dialog,
                          "new-button-clicked",
                          G_CALLBACK (on_pvs_dialog_new_button_clicked),
                          section);
        g_signal_connect (dialog,
                          "remove-button-clicked",
                          G_CALLBACK (on_pvs_dialog_remove_button_clicked),
                          section);

        gtk_widget_show_all (dialog);
        gtk_window_present (GTK_WINDOW (dialog));
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_linux_lvm2_volume_group_update (MduSection *_section)
{
        MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (_section);
        MduPresentable *p;
        MduLinuxLvm2VolumeGroup *vg;
        MduDevice *pv_device;
        const gchar *name;
        guint64 size;
        guint64 unallocated_size;
        guint64 extent_size;
        gchar **pvs;
        gchar *s;
        MduLinuxLvm2VolumeGroupState state;
        gchar *state_str;
        gboolean show_vg_start_button;
        gboolean show_vg_stop_button;

        show_vg_start_button = FALSE;
        show_vg_stop_button = FALSE;

        state_str = NULL;

        p = mdu_section_get_presentable (_section);
        vg = MDU_LINUX_LVM2_VOLUME_GROUP (p);

        pv_device = mdu_linux_lvm2_volume_group_get_pv_device (vg);
        if (pv_device == NULL)
                goto out;

        name = mdu_device_linux_lvm2_pv_get_group_name (pv_device);
        size = mdu_device_linux_lvm2_pv_get_group_size (pv_device);
        unallocated_size = mdu_device_linux_lvm2_pv_get_group_unallocated_size (pv_device);
        extent_size = mdu_device_linux_lvm2_pv_get_group_extent_size (pv_device);
        pvs = mdu_device_linux_lvm2_pv_get_group_physical_volumes (pv_device);

        mdu_details_element_set_text (section->priv->name_element, name);
        s = mdu_util_get_size_for_display (size, FALSE, TRUE);
        mdu_details_element_set_text (section->priv->capacity_element, s);
        g_free (s);
        s = mdu_util_get_size_for_display (unallocated_size, FALSE, TRUE);
        mdu_details_element_set_text (section->priv->unallocated_size_element, s);
        g_free (s);
        /* Use the nerd units here (MiB) since that's what LVM defaults to (divisble by sector size etc.) */
        s = mdu_util_get_size_for_display (extent_size, TRUE, TRUE);
        mdu_details_element_set_text (section->priv->extent_size_element, s);
        g_free (s);
        s = g_strdup_printf ("%d", g_strv_length (pvs));
        mdu_details_element_set_text (section->priv->num_pvs_element, s);
        g_free (s);

        state = mdu_linux_lvm2_volume_group_get_state (vg);

        switch (state) {
        case MDU_LINUX_LVM2_VOLUME_GROUP_STATE_NOT_RUNNING:
                state_str = g_strdup (_("Not Running"));
                show_vg_start_button = TRUE;
                break;
        case MDU_LINUX_LVM2_VOLUME_GROUP_STATE_PARTIALLY_RUNNING:
                state_str = g_strdup (_("Partially Running"));
                show_vg_start_button = TRUE;
                show_vg_stop_button = TRUE;
                break;
        case MDU_LINUX_LVM2_VOLUME_GROUP_STATE_RUNNING:
                state_str = g_strdup (_("Running"));
                show_vg_stop_button = TRUE;
                break;
        default:
                state_str = g_strdup_printf (_("Unknown (%d)"), state);
                show_vg_start_button = TRUE;
                show_vg_stop_button = TRUE;
                break;
        }
        mdu_details_element_set_text (section->priv->state_element, state_str);


 out:
        mdu_button_element_set_visible (section->priv->vg_start_button, show_vg_start_button);
        mdu_button_element_set_visible (section->priv->vg_stop_button, show_vg_stop_button);

        if (pv_device != NULL)
                g_object_unref (pv_device);
        g_free (state_str);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_linux_lvm2_volume_group_constructed (GObject *object)
{
        MduSectionLinuxLvm2VolumeGroup *section = MDU_SECTION_LINUX_LVM2_VOLUME_GROUP (object);
        GtkWidget *align;
        GtkWidget *label;
        GtkWidget *table;
        GtkWidget *vbox;
        gchar *s;
        MduPresentable *p;
        MduDevice *d;
        GPtrArray *elements;
        MduDetailsElement *element;
        MduButtonElement *button_element;

        p = mdu_section_get_presentable (MDU_SECTION (section));
        d = mdu_presentable_get_device (p);

        gtk_box_set_spacing (GTK_BOX (section), 12);

        /*------------------------------------- */

        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        s = g_strconcat ("<b>", _("Volume Group"), "</b>", NULL);
        gtk_label_set_markup (GTK_LABEL (label), s);
        g_free (s);
        gtk_box_pack_start (GTK_BOX (section), label, FALSE, FALSE, 0);

        align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
        gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 12, 0);
        gtk_box_pack_start (GTK_BOX (section), align, FALSE, FALSE, 0);

        vbox = gtk_vbox_new (FALSE, 6);
        gtk_container_add (GTK_CONTAINER (align), vbox);

        elements = g_ptr_array_new_with_free_func (g_object_unref);

        element = mdu_details_element_new (_("Name:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->name_element = element;

        element = mdu_details_element_new (_("Extent Size:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->extent_size_element = element;

        element = mdu_details_element_new (_("Physical Volumes:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->num_pvs_element = element;

        element = mdu_details_element_new (_("Capacity:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->capacity_element = element;

        element = mdu_details_element_new (_("State:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->state_element = element;

        element = mdu_details_element_new (_("Unallocated:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->unallocated_size_element = element;

        table = mdu_details_table_new (2, elements);
        g_ptr_array_unref (elements);
        gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);

        /* -------------------------------------------------------------------------------- */

        align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
        gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 12, 0);
        gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 0);

        table = mdu_button_table_new (2, NULL);
        gtk_container_add (GTK_CONTAINER (align), table);
        elements = g_ptr_array_new_with_free_func (g_object_unref);

        button_element = mdu_button_element_new ("mdu-raid-array-start",
                                                 _("St_art Volume Group"),
                                                 _("Activate all LVs in the VG"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_lvm2_vg_start_button_clicked),
                          section);
        section->priv->vg_start_button = button_element;
        g_ptr_array_add (elements, button_element);

        button_element = mdu_button_element_new ("mdu-raid-array-stop",
                                                 _("St_op Volume Group"),
                                                 _("Deactivate all LVs in the VG"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_lvm2_vg_stop_button_clicked),
                          section);
        section->priv->vg_stop_button = button_element;
        g_ptr_array_add (elements, button_element);

        /* TODO: better icon */
        button_element = mdu_button_element_new (GTK_STOCK_BOLD,
                                                 _("Edit _Name"),
                                                 _("Change the Volume Group name"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_vg_edit_name_clicked),
                          section);
        g_ptr_array_add (elements, button_element);
        section->priv->vg_edit_name_button = button_element;

        button_element = mdu_button_element_new (GTK_STOCK_EDIT,
                                                 _("Edit _Physical Volumes"),
                                                 _("Create and remove PVs"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_vg_edit_pvs_button_clicked),
                          section);
        g_ptr_array_add (elements, button_element);
        section->priv->vg_edit_pvs_button = button_element;

        mdu_button_table_set_elements (MDU_BUTTON_TABLE (table), elements);
        g_ptr_array_unref (elements);

        /* -------------------------------------------------------------------------------- */

        gtk_widget_show_all (GTK_WIDGET (section));

        if (d != NULL)
                g_object_unref (d);

        if (G_OBJECT_CLASS (mdu_section_linux_lvm2_volume_group_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_section_linux_lvm2_volume_group_parent_class)->constructed (object);
}

static void
mdu_section_linux_lvm2_volume_group_class_init (MduSectionLinuxLvm2VolumeGroupClass *klass)
{
        GObjectClass *gobject_class;
        MduSectionClass *section_class;

        gobject_class = G_OBJECT_CLASS (klass);
        section_class = MDU_SECTION_CLASS (klass);

        gobject_class->finalize    = mdu_section_linux_lvm2_volume_group_finalize;
        gobject_class->constructed = mdu_section_linux_lvm2_volume_group_constructed;
        section_class->update      = mdu_section_linux_lvm2_volume_group_update;

        g_type_class_add_private (klass, sizeof (MduSectionLinuxLvm2VolumeGroupPrivate));
}

static void
mdu_section_linux_lvm2_volume_group_init (MduSectionLinuxLvm2VolumeGroup *section)
{
        section->priv = G_TYPE_INSTANCE_GET_PRIVATE (section, MDU_TYPE_SECTION_LINUX_LVM2_VOLUME_GROUP, MduSectionLinuxLvm2VolumeGroupPrivate);
}

GtkWidget *
mdu_section_linux_lvm2_volume_group_new (MduShell       *shell,
                                         MduPresentable *presentable)
{
        return GTK_WIDGET (g_object_new (MDU_TYPE_SECTION_LINUX_LVM2_VOLUME_GROUP,
                                         "shell", shell,
                                         "presentable", presentable,
                                         NULL));
}
