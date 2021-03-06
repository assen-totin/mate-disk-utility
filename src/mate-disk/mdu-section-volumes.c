/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-section-volumes.c
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
#include <glib/gi18n.h>

#include <string.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include <math.h>

#include <mdu-gtk/mdu-gtk.h>
#include "mdu-section-volumes.h"

struct _MduSectionVolumesPrivate
{
        GtkWidget *misaligned_warning_info_bar;
        GtkWidget *misaligned_warning_label;

        MduPresentable *cur_volume;

        GtkWidget *main_label;
        GtkWidget *main_vbox;

        GtkWidget *grid;
        GtkWidget *details_table;
        GtkWidget *button_table;

        /* elements for LVM2 Logical Volumes */
        MduDetailsElement *lvm2_name_element;
        MduDetailsElement *lvm2_state_element;

        /* shared between all volume types */
        MduDetailsElement *usage_element;
        MduDetailsElement *capacity_element;
        MduDetailsElement *partition_type_element;
        MduDetailsElement *partition_flags_element;
        MduDetailsElement *partition_label_element;
        MduDetailsElement *device_element;

        /* elements for the 'filesystem' usage */
        MduDetailsElement *fs_type_element;
        MduDetailsElement *fs_available_element;
        MduDetailsElement *fs_label_element;
        MduDetailsElement *fs_mount_point_element;

        MduButtonElement *fs_mount_button;
        MduButtonElement *fs_unmount_button;
        MduButtonElement *fs_check_button;
        MduButtonElement *fs_change_label_button;
        MduButtonElement *format_button;
        MduButtonElement *partition_edit_button;
        MduButtonElement *partition_delete_button;
        MduButtonElement *partition_create_button;
        MduButtonElement *luks_lock_button;
        MduButtonElement *luks_unlock_button;
        MduButtonElement *luks_forget_passphrase_button;
        MduButtonElement *luks_change_passphrase_button;
        MduButtonElement *lvm2_create_lv_button;
        MduButtonElement *lvm2_lv_start_button;
        MduButtonElement *lvm2_lv_stop_button;
        MduButtonElement *lvm2_lv_edit_name_button;
        MduButtonElement *lvm2_lv_delete_button;
};

G_DEFINE_TYPE (MduSectionVolumes, mdu_section_volumes, MDU_TYPE_SECTION)

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_volumes_finalize (GObject *object)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (object);

        if (section->priv->cur_volume != NULL)
                g_object_unref (section->priv->cur_volume);

        if (G_OBJECT_CLASS (mdu_section_volumes_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_section_volumes_parent_class)->finalize (object);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
unmount_op_callback (MduDevice *device,
                     GError    *error,
                     gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        /* TODO: handle busy mounts using GtkMountOperation */

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error unmounting volume"),
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
on_unmount_button_clicked (MduButtonElement *button_element,
                           gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);

        MduPresentable *v;
        MduDevice *d;

        v = NULL;
        d = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        mdu_device_op_filesystem_unmount (d,
                                          unmount_op_callback,
                                          g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mount_op_callback (MduDevice *device,
                   gchar     *mount_point,
                   GError    *error,
                   gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error mounting volume"),
                                                          error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        } else {
                g_free (mount_point);
        }
        g_object_unref (shell);
}

static void
on_mount_button_clicked (MduButtonElement *button_element,
                         gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduDevice *d;

        v = NULL;
        d = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        mdu_device_op_filesystem_mount (d,
                                        NULL,
                                        mount_op_callback,
                                        g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
partition_delete_op_callback (MduDevice *device,
                              GError    *error,
                              gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error deleting partition"),
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
on_partition_delete_button_clicked (MduButtonElement *button_element,
                                    gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduDevice *d;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;

        v = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_confirmation_dialog_new (toplevel,
                                              v,
                                              _("Are you sure you want to delete the partition?"),
                                              _("_Delete"));
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        if (response == GTK_RESPONSE_OK) {
                mdu_device_op_partition_delete (d,
                                                partition_delete_op_callback,
                                                g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));
        }
        gtk_widget_destroy (dialog);

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */


typedef struct {
        MduShell *shell;
        MduPresentable *presentable;
        char *encrypt_passphrase;
        gboolean save_in_keyring;
        gboolean save_in_keyring_session;
} CreateFilesystemData;

static void
create_filesystem_data_free (CreateFilesystemData *data)
{
        if (data->shell != NULL)
                g_object_unref (data->shell);
        if (data->presentable != NULL)
                g_object_unref (data->presentable);
        if (data->encrypt_passphrase != NULL) {
                memset (data->encrypt_passphrase, '\0', strlen (data->encrypt_passphrase));
                g_free (data->encrypt_passphrase);
        }
        g_free (data);
}

static void
filesystem_create_op_callback (MduDevice  *device,
                               GError     *error,
                               gpointer    user_data)
{
        CreateFilesystemData *data = user_data;

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (data->shell)),
                                                          device,
                                                          _("Error creating filesystem"),
                                                          error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        } else if (data->encrypt_passphrase != NULL) {
                /* now set the passphrase if requested */
                if (data->save_in_keyring || data->save_in_keyring_session) {
                        mdu_util_save_secret (device,
                                              data->encrypt_passphrase,
                                              data->save_in_keyring_session);
                }
        }
        if (data != NULL)
                create_filesystem_data_free (data);
}

static void
on_format_button_clicked (MduButtonElement *button_element,
                          gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduDevice *d;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        GtkWidget *confirmation_dialog;
        gint response;

        v = NULL;
        dialog = NULL;
        confirmation_dialog = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_format_dialog_new (toplevel,
                                        v,
                                        MDU_FORMAT_DIALOG_FLAGS_NONE);
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_hide (dialog);
        if (response == GTK_RESPONSE_OK) {
                confirmation_dialog = mdu_confirmation_dialog_new (toplevel,
                                                                   v,
                                                                   _("Are you sure you want to format the volume?"),
                                                                   _("_Format"));
                gtk_widget_show_all (confirmation_dialog);
                response = gtk_dialog_run (GTK_DIALOG (confirmation_dialog));
                gtk_widget_hide (confirmation_dialog);
                if (response == GTK_RESPONSE_OK) {
                        CreateFilesystemData *data;

                        data = g_new0 (CreateFilesystemData, 1);
                        data->shell = g_object_ref (mdu_section_get_shell (MDU_SECTION (section)));
                        data->presentable = g_object_ref (v);

                        if (mdu_format_dialog_get_encrypt (MDU_FORMAT_DIALOG (dialog))) {
                                data->encrypt_passphrase = mdu_util_dialog_ask_for_new_secret (
                                      mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))),
                                      &data->save_in_keyring,
                                      &data->save_in_keyring_session);
                                if (data->encrypt_passphrase == NULL) {
                                        create_filesystem_data_free (data);
                                        goto out;
                                }
                        }

                        mdu_device_op_filesystem_create (d,
                                                         mdu_format_dialog_get_fs_type (MDU_FORMAT_DIALOG (dialog)),
                                                         mdu_format_dialog_get_fs_label (MDU_FORMAT_DIALOG (dialog)),
                                                         data->encrypt_passphrase,
                                                         mdu_format_dialog_get_take_ownership (MDU_FORMAT_DIALOG (dialog)),
                                                         filesystem_create_op_callback,
                                                         data);

                }
        }
 out:
        if (dialog != NULL)
                gtk_widget_destroy (dialog);
        if (confirmation_dialog != NULL)
                gtk_widget_destroy (confirmation_dialog);

        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
partition_modify_op_callback (MduDevice *device,
                              GError    *error,
                              gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error modifying partition"),
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
on_partition_edit_button_clicked (MduButtonElement *button_element,
                                  gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduDevice *d;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;

        v = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_edit_partition_dialog_new (toplevel, v);
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        if (response == GTK_RESPONSE_APPLY) {
                gchar *partition_type;
                gchar *partition_label;
                gchar **partition_flags;

                g_object_get (dialog,
                              "partition-type", &partition_type,
                              "partition-label", &partition_label,
                              "partition-flags", &partition_flags,
                              NULL);

                mdu_device_op_partition_modify (d,
                                                partition_type,
                                                partition_label,
                                                partition_flags,
                                                partition_modify_op_callback,
                                                g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

                g_free (partition_type);
                g_free (partition_label);
                g_strfreev (partition_flags);
        }
        gtk_widget_destroy (dialog);

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
on_luks_forget_passphrase_button_clicked (MduButtonElement *button_element,
                                          gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        GtkWidget *dialog;
        MduPresentable *v;
        MduDevice *d;
        gint response;

        v = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        dialog = mdu_confirmation_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section)))),
                                              v,
                                              _("Are you sure you want to forget the passphrase?"),
                                              _("_Forget"));
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        if (response == GTK_RESPONSE_OK) {
                mdu_util_delete_secret (d);
        }
        gtk_widget_destroy (dialog);

        mdu_section_update (MDU_SECTION (section));

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
luks_lock_op_callback (MduDevice *device,
                       GError    *error,
                       gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error locking LUKS volume"),
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
on_luks_lock_button_clicked (MduButtonElement *button_element,
                             gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduDevice *d;

        v = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        mdu_device_op_luks_lock (d,
                                 luks_lock_op_callback,
                                 g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void unlock_action_do (MduSectionVolumes *section,
                              MduPresentable *presentable,
                              gboolean bypass_keyring,
                              gboolean indicate_wrong_passphrase);

typedef struct {
        MduSectionVolumes *section;
        MduPresentable *presentable;
        gboolean asked_user;
} UnlockData;

static UnlockData *
unlock_data_new (MduSectionVolumes *section,
                 MduPresentable    *presentable,
                 gboolean           asked_user)
{
        UnlockData *data;
        data = g_new0 (UnlockData, 1);
        data->section = g_object_ref (section);
        data->presentable = g_object_ref (presentable);
        data->asked_user = asked_user;
        return data;
}

static void
unlock_data_free (UnlockData *data)
{
        g_object_unref (data->section);
        g_object_unref (data->presentable);
        g_free (data);
}

static gboolean
unlock_retry (gpointer user_data)
{
        UnlockData *data = user_data;
        MduDevice *device;
        gboolean indicate_wrong_passphrase;

        device = mdu_presentable_get_device (data->presentable);
        if (device != NULL) {
                indicate_wrong_passphrase = FALSE;

                if (!data->asked_user) {
                        /* if we attempted to unlock the device without asking the user
                         * then the password must have come from the keyring.. hence,
                         * since we failed, the password in the keyring is bad. Remove
                         * it.
                         */
                        g_warning ("removing bad password from keyring");
                        mdu_util_delete_secret (device);
                } else {
                        /* we did ask the user on the last try and that passphrase
                         * didn't work.. make sure the new dialog tells him that
                         */
                        indicate_wrong_passphrase = TRUE;
                }

                unlock_action_do (data->section, data->presentable, TRUE, indicate_wrong_passphrase);
                g_object_unref (device);
        }
        unlock_data_free (data);
        return FALSE;
}

static void
unlock_op_cb (MduDevice *device,
              char      *object_path_of_cleartext_device,
              GError    *error,
              gpointer   user_data)
{
        UnlockData *data = user_data;
        MduShell *shell;

        shell = mdu_section_get_shell (MDU_SECTION (data->section));

        if (error != NULL && error->code == MDU_ERROR_INHIBITED) {
                GtkWidget *dialog;

                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error unlocking LUKS volume"),
                                                          error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);

                g_error_free (error);
        } else if (error != NULL) {
                /* retry */
                g_idle_add (unlock_retry, data);
                g_error_free (error);
        } else {
                unlock_data_free (data);
                g_free (object_path_of_cleartext_device);
        }
}

static void
unlock_action_do (MduSectionVolumes *section,
                  MduPresentable    *presentable,
                  gboolean           bypass_keyring,
                  gboolean           indicate_wrong_passphrase)
{
        gchar *secret;
        gboolean asked_user;
        MduDevice *device;

        device = mdu_presentable_get_device (presentable);
        if (device != NULL) {
                MduShell *shell;

                shell = mdu_section_get_shell (MDU_SECTION (section));

                secret = mdu_util_dialog_ask_for_secret (GTK_WIDGET (mdu_shell_get_toplevel (shell)),
                                                         presentable,
                                                         bypass_keyring,
                                                         indicate_wrong_passphrase,
                                                         &asked_user);
                if (secret != NULL) {
                        mdu_device_op_luks_unlock (device,
                                                   secret,
                                                   unlock_op_cb,
                                                   unlock_data_new (section,
                                                                    presentable,
                                                                    asked_user));
                        /* scrub the password */
                        memset (secret, '\0', strlen (secret));
                        g_free (secret);
                }

                g_object_unref (device);
        }
}


static void
on_luks_unlock_button_clicked (MduButtonElement *button_element,
                               gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;

        v = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        unlock_action_do (section, v, FALSE, FALSE);

 out:
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct {
        char *old_secret;
        char *new_secret;
        gboolean save_in_keyring;
        gboolean save_in_keyring_session;
        MduPresentable *presentable;
        MduSectionVolumes *section;
} ChangePassphraseData;

static void
change_passphrase_data_free (ChangePassphraseData *data)
{
        /* scrub the secrets */
        if (data->old_secret != NULL) {
                memset (data->old_secret, '\0', strlen (data->old_secret));
                g_free (data->old_secret);
        }
        if (data->new_secret != NULL) {
                memset (data->new_secret, '\0', strlen (data->new_secret));
                g_free (data->new_secret);
        }
        if (data->presentable != NULL)
                g_object_unref (data->presentable);
        if (data->section != NULL)
                g_object_unref (data->section);
        g_free (data);
}

static void change_passphrase_do (MduSectionVolumes *section,
                                  MduPresentable    *presentable,
                                  gboolean           bypass_keyring,
                                  gboolean           indicate_wrong_passphrase);

static void
change_passphrase_completed (MduDevice  *device,
                             GError     *error,
                             gpointer    user_data)
{
        ChangePassphraseData *data = user_data;

        if (error == NULL) {
                /* It worked! Now update the keyring */

                if (data->save_in_keyring || data->save_in_keyring_session)
                        mdu_util_save_secret (device, data->new_secret, data->save_in_keyring_session);
                else
                        mdu_util_delete_secret (device);

                change_passphrase_data_free (data);
        } else {
                /* It didn't work. Because the given passphrase was wrong. Try again,
                 * this time forcibly bypassing the keyring and telling the user
                 * the given passphrase was wrong.
                 */
                change_passphrase_do (data->section, data->presentable, TRUE, TRUE);
                change_passphrase_data_free (data);
        }
}

static void
change_passphrase_do (MduSectionVolumes *section,
                      MduPresentable    *presentable,
                      gboolean           bypass_keyring,
                      gboolean           indicate_wrong_passphrase)
{
        MduDevice *device;
        ChangePassphraseData *data;

        device = mdu_presentable_get_device (presentable);
        if (device == NULL) {
                goto out;
        }

        data = g_new0 (ChangePassphraseData, 1);
        data->presentable = g_object_ref (presentable);
        data->section = g_object_ref (section);

        if (!mdu_util_dialog_change_secret (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))),
                                            presentable,
                                            &data->old_secret,
                                            &data->new_secret,
                                            &data->save_in_keyring,
                                            &data->save_in_keyring_session,
                                            bypass_keyring,
                                            indicate_wrong_passphrase)) {
                change_passphrase_data_free (data);
                goto out;
        }

        mdu_device_op_luks_change_passphrase (device,
                                              data->old_secret,
                                              data->new_secret,
                                              change_passphrase_completed,
                                              data);

out:
        if (device != NULL) {
                g_object_unref (device);
        }
}

static void
on_luks_change_passphrase_button_clicked (MduButtonElement *button_element,
                                          gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;

        v = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        change_passphrase_do (section, v, FALSE, FALSE);

 out:
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static gboolean
can_create_partition (MduSectionVolumes *section,
                      MduVolumeHole     *hole,
                      gboolean          *out_can_create_extended)
{
        MduPresentable *drive;
        MduDevice *drive_device;
        gboolean can_create;
        gboolean can_create_extended;
        guint num_primary;
        gboolean has_extended;
        const gchar *part_scheme;
        MduPresentable *enclosing_presentable_for_hole;

        can_create = FALSE;
        can_create_extended = FALSE;

        enclosing_presentable_for_hole = NULL;
        drive_device = NULL;

        drive = mdu_section_get_presentable (MDU_SECTION (section));
        drive_device = mdu_presentable_get_device (drive);
        if (drive_device == NULL)
                goto out;

        part_scheme = mdu_device_partition_table_get_scheme (drive_device);

        if (g_strcmp0 (part_scheme, "mbr") != 0) {
                can_create = TRUE;
                goto out;
        }

        enclosing_presentable_for_hole = mdu_presentable_get_enclosing_presentable (MDU_PRESENTABLE (hole));
        if (MDU_IS_DRIVE (enclosing_presentable_for_hole)) {
                /* hole is in primary partition space */
                if (mdu_drive_count_mbr_partitions (MDU_DRIVE (drive), &num_primary, &has_extended)) {
                        if (num_primary < 4) {
                                can_create = TRUE;
                                if (!has_extended)
                                        can_create_extended = TRUE;
                        }
                }
        } else {
                /* hole is in an extended partition */
                can_create = TRUE;
        }

 out:
        if (enclosing_presentable_for_hole != NULL)
                g_object_unref (enclosing_presentable_for_hole);

        if (drive_device != NULL)
                g_object_unref (drive_device);

        if (out_can_create_extended)
                *out_can_create_extended = can_create_extended;

        return can_create;
}


typedef struct {
        MduShell *shell;
        MduPresentable *presentable;
        char *encrypt_passphrase;
        gboolean save_in_keyring;
        gboolean save_in_keyring_session;
} CreatePartitionData;

static void
create_partition_data_free (CreatePartitionData *data)
{
        if (data->shell != NULL)
                g_object_unref (data->shell);
        if (data->presentable != NULL)
                g_object_unref (data->presentable);
        if (data->encrypt_passphrase != NULL) {
                memset (data->encrypt_passphrase, '\0', strlen (data->encrypt_passphrase));
                g_free (data->encrypt_passphrase);
        }
        g_free (data);
}

static void
partition_create_op_callback (MduDevice  *device,
                              gchar      *created_device_object_path,
                              GError     *error,
                              gpointer    user_data)
{
        CreatePartitionData *data = user_data;

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_drive (GTK_WINDOW (mdu_shell_get_toplevel (data->shell)),
                                                         device,
                                                         _("Error creating partition"),
                                                         error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        } else {
                if (data->encrypt_passphrase != NULL) {
                        MduDevice *cleartext_device;
                        MduPool *pool;

                        pool = mdu_device_get_pool (device);
                        cleartext_device = mdu_pool_get_by_object_path (pool, created_device_object_path);
                        if (cleartext_device != NULL) {
                                const gchar *cryptotext_device_object_path;

                                cryptotext_device_object_path = mdu_device_luks_cleartext_get_slave (cleartext_device);
                                if (cryptotext_device_object_path != NULL) {
                                        MduDevice *cryptotext_device;

                                        cryptotext_device = mdu_pool_get_by_object_path (pool,
                                                                                         cryptotext_device_object_path);
                                        if (cryptotext_device != NULL) {
                                                /* now set the passphrase if requested */
                                                if (data->save_in_keyring || data->save_in_keyring_session) {
                                                        mdu_util_save_secret (cryptotext_device,
                                                                              data->encrypt_passphrase,
                                                                              data->save_in_keyring_session);
                                                }
                                                g_object_unref (cryptotext_device);
                                        }
                                }
                                g_object_unref (cleartext_device);
                        }
                }

                g_free (created_device_object_path);
        }

        if (data != NULL)
                create_partition_data_free (data);
}

static void
on_partition_create_button_clicked (MduButtonElement *button_element,
                                    gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduPresentable *drive;
        MduDevice *drive_device;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;
        MduFormatDialogFlags flags;
        const gchar *part_scheme;
        gboolean can_create_extended;

        v = NULL;
        drive_device = NULL;
        dialog = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL || !MDU_IS_VOLUME_HOLE (v))
                goto out;

        drive = mdu_section_get_presentable (MDU_SECTION (section));
        drive_device = mdu_presentable_get_device (drive);
        if (drive_device == NULL)
                goto out;

        part_scheme = mdu_device_partition_table_get_scheme (drive_device);

        flags = MDU_FORMAT_DIALOG_FLAGS_NONE;
        if (!can_create_partition (section, MDU_VOLUME_HOLE (v), &can_create_extended))
                goto out;
        if (can_create_extended)
                flags |= MDU_FORMAT_DIALOG_FLAGS_ALLOW_MSDOS_EXTENDED;

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_create_partition_dialog_new_for_drive (toplevel,
                                                            drive_device,
                                                            mdu_presentable_get_size (v),
                                                            flags);
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_hide (dialog);
        if (response == GTK_RESPONSE_OK) {
                CreatePartitionData *data;
                gchar *fs_type;
                gchar *fs_label;
                gboolean fs_take_ownership;
                guint64 offset;
                guint64 size;
                gchar *part_type;

                data = g_new0 (CreatePartitionData, 1);
                data->shell = g_object_ref (mdu_section_get_shell (MDU_SECTION (section)));
                data->presentable = g_object_ref (v);

                if (mdu_format_dialog_get_encrypt (MDU_FORMAT_DIALOG (dialog))) {
                        data->encrypt_passphrase = mdu_util_dialog_ask_for_new_secret (GTK_WIDGET (toplevel),
                                                                                       &data->save_in_keyring,
                                                                                       &data->save_in_keyring_session);
                        if (data->encrypt_passphrase == NULL) {
                                create_partition_data_free (data);
                                goto out;
                        }
                }

                fs_type = mdu_format_dialog_get_fs_type (MDU_FORMAT_DIALOG (dialog));
                fs_label = mdu_format_dialog_get_fs_label (MDU_FORMAT_DIALOG (dialog));
                fs_take_ownership = mdu_format_dialog_get_take_ownership (MDU_FORMAT_DIALOG (dialog));

                offset = mdu_presentable_get_offset (v);
                size = mdu_create_partition_dialog_get_size (MDU_CREATE_PARTITION_DIALOG (dialog));

                if (strcmp (fs_type, "msdos_extended_partition") == 0) {
                        part_type = g_strdup ("0x05");
                        g_free (fs_type);
                        g_free (fs_label);
                        fs_type = g_strdup ("");
                        fs_label = g_strdup ("");
                        fs_take_ownership = FALSE;
                } else {
                        part_type = mdu_util_get_default_part_type_for_scheme_and_fstype (part_scheme,
                                                                                          fs_type,
                                                                                          size);
                }

                mdu_device_op_partition_create (drive_device,
                                                offset,
                                                size,
                                                part_type,
                                                "",   /* empty partition label for now */
                                                NULL, /* no flags for the partition */
                                                fs_type,
                                                fs_label,
                                                data->encrypt_passphrase,
                                                fs_take_ownership,
                                                partition_create_op_callback,
                                                data);

                g_free (fs_type);
                g_free (fs_label);
                g_free (part_type);
        }
 out:
        if (dialog != NULL)
                gtk_widget_destroy (dialog);

        if (drive_device != NULL)
                g_object_unref (drive_device);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
filesystem_set_label_op_callback (MduDevice *device,
                                  GError    *error,
                                  gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error changing label"),
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
on_fs_change_label_button_clicked (MduButtonElement *button_element,
                                   gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduDevice *d;
        MduPool *pool;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;
        MduKnownFilesystem *kfs;
        const gchar *label;
        guint label_max_bytes;

        v = NULL;
        kfs = NULL;
        pool = NULL;
        d = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        pool = mdu_device_get_pool (d);

        kfs = mdu_pool_get_known_filesystem_by_id (pool, mdu_device_id_get_type (d));

        label = mdu_device_id_get_label (d);
        if (kfs != NULL) {
                label_max_bytes = mdu_known_filesystem_get_max_label_len (kfs);
        } else {
                label_max_bytes = G_MAXUINT;
        }

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_edit_name_dialog_new (toplevel,
                                           v,
                                           label,
                                           label_max_bytes,
                                           _("Choose a new filesystem label."),
                                           _("_Label:"));
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        if (response == GTK_RESPONSE_APPLY) {
                gchar *label;

                label = mdu_edit_name_dialog_get_name (MDU_EDIT_NAME_DIALOG (dialog));

                mdu_device_op_filesystem_set_label (d,
                                                    label,
                                                    filesystem_set_label_op_callback,
                                                    g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

                g_free (label);
        }
        gtk_widget_destroy (dialog);

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
        if (pool != NULL)
                g_object_unref (pool);
        if (kfs != NULL)
                g_object_unref (kfs);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
fsck_op_callback (MduDevice *device,
                  gboolean   is_clean,
                  GError    *error,
                  gpointer   user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduShell *shell;

        shell = mdu_section_get_shell (MDU_SECTION (section));

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error checking filesystem on volume"),
                                                          error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        } else {
                GtkWidget *dialog;
                gchar *name;
                gchar *vpd_name;
                MduPool *pool;
                MduPresentable *p;

                /* TODO: would probably be nice with MduInformationDialog or, more
                 * generally, a MduProgressDialog class
                 */
                pool = mdu_device_get_pool (device);
                p = mdu_pool_get_volume_by_device (pool, device);

                name = mdu_presentable_get_name (p);
                vpd_name = mdu_presentable_get_vpd_name (p);

                dialog = gtk_message_dialog_new (GTK_WINDOW (GTK_WINDOW (mdu_shell_get_toplevel (shell))),
                                                 GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 is_clean ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING,
                                                 GTK_BUTTONS_CLOSE,
                                                 _("File system check on \"%s\" (%s) completed"),
                                                 name, vpd_name);
                if (is_clean) {
                        gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
                                                                    _("File system is clean."));
                } else {
                        gtk_message_dialog_format_secondary_markup (GTK_MESSAGE_DIALOG (dialog),
                                                                    _("File system is <b>NOT</b> clean."));
                }
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);

                g_free (vpd_name);
                g_free (name);
                g_object_unref (pool);
                g_object_unref (p);
        }
        g_object_unref (shell);
}

static void
on_fsck_button_clicked (MduButtonElement *button_element,
                        gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);

        MduPresentable *v;
        MduDevice *d;

        v = NULL;
        d = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        mdu_device_op_filesystem_check (d,
                                        fsck_op_callback,
                                        g_object_ref (section));

 out:
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
on_usage_element_activated (MduDetailsElement *element,
                            const gchar       *uri,
                            gpointer           user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduDevice *d;
        MduLinuxMdDrive *linux_md_drive;
        MduPool *pool;

        v = NULL;
        d = NULL;
        linux_md_drive = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL)
                goto out;

        d = mdu_presentable_get_device (v);
        if (d == NULL)
                goto out;

        pool = mdu_device_get_pool (d);

        linux_md_drive = mdu_pool_get_linux_md_drive_by_uuid (pool, mdu_device_linux_md_component_get_uuid (d));
        if (linux_md_drive == NULL)
                goto out;

        mdu_shell_select_presentable (mdu_section_get_shell (MDU_SECTION (section)), MDU_PRESENTABLE (linux_md_drive));

 out:
        if (linux_md_drive != NULL)
                g_object_unref (linux_md_drive);
        if (pool != NULL)
                g_object_unref (pool);
        if (d != NULL)
                g_object_unref (d);
        if (v != NULL)
                g_object_unref (v);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
on_fs_mount_point_element_activated (MduDetailsElement *element,
                                     const gchar       *uri,
                                     gpointer           user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        GError *error;
        gchar *s;

        /* We want to use caja instead of gtk_show_uri() because
         * the latter doesn't handle automatically mounting the mount
         * - maybe gtk_show_uri() should do that though...
         */

        s = g_strdup_printf ("caja \"%s\"", uri);

        error = NULL;
        if (!g_spawn_command_line_async (s, &error)) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section)))),
                                               mdu_section_get_presentable (MDU_SECTION (section)),
                                               _("Error spawning caja: %s"),
                                               error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        }

        g_free (s);
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct {
        MduShell *shell;
        MduPresentable *presentable;
        char *encrypt_passphrase;
        gboolean save_in_keyring;
        gboolean save_in_keyring_session;
} CreateLinuxLvm2LVData;

static void
create_linux_lvm2_lv_data_free (CreateLinuxLvm2LVData *data)
{
        if (data->shell != NULL)
                g_object_unref (data->shell);
        if (data->presentable != NULL)
                g_object_unref (data->presentable);
        if (data->encrypt_passphrase != NULL) {
                memset (data->encrypt_passphrase, '\0', strlen (data->encrypt_passphrase));
                g_free (data->encrypt_passphrase);
        }
        g_free (data);
}

static void
lvm2_lv_create_op_callback (MduPool    *pool,
                            gchar      *created_device_object_path,
                            GError     *error,
                            gpointer    user_data)
{
        CreateLinuxLvm2LVData *data = user_data;

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (data->shell)),
                                               data->presentable,
                                               _("Error creating Logical Volume"),
                                               error);
                gtk_widget_show_all (dialog);
                gtk_window_present (GTK_WINDOW (dialog));
                gtk_dialog_run (GTK_DIALOG (dialog));
                gtk_widget_destroy (dialog);
                g_error_free (error);
        } else {
                if (data->encrypt_passphrase != NULL) {
                        MduDevice *cleartext_device;

                        cleartext_device = mdu_pool_get_by_object_path (pool, created_device_object_path);
                        if (cleartext_device != NULL) {
                                const gchar *cryptotext_device_object_path;

                                cryptotext_device_object_path = mdu_device_luks_cleartext_get_slave (cleartext_device);
                                if (cryptotext_device_object_path != NULL) {
                                        MduDevice *cryptotext_device;

                                        cryptotext_device = mdu_pool_get_by_object_path (pool,
                                                                                         cryptotext_device_object_path);
                                        if (cryptotext_device != NULL) {
                                                /* now set the passphrase if requested */
                                                if (data->save_in_keyring || data->save_in_keyring_session) {
                                                        mdu_util_save_secret (cryptotext_device,
                                                                              data->encrypt_passphrase,
                                                                              data->save_in_keyring_session);
                                                }
                                                g_object_unref (cryptotext_device);
                                        }
                                }
                                g_object_unref (cleartext_device);
                        }
                }
                g_free (created_device_object_path);
        }

        if (data != NULL)
                create_linux_lvm2_lv_data_free (data);
}

static void
on_lvm2_create_lv_button_clicked (MduButtonElement *button_element,
                                  gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduPresentable *v;
        MduPresentable *vg;
        MduPool *pool;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;
        MduFormatDialogFlags flags;
        const gchar *vg_uuid;

        v = NULL;
        pool = NULL;
        dialog = NULL;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));
        if (v == NULL || !MDU_IS_VOLUME_HOLE (v))
                goto out;

        vg = mdu_section_get_presentable (MDU_SECTION (section));
        vg_uuid = mdu_linux_lvm2_volume_group_get_uuid (MDU_LINUX_LVM2_VOLUME_GROUP (vg));

        pool = mdu_presentable_get_pool (v);

        flags = MDU_FORMAT_DIALOG_FLAGS_NONE;

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_create_linux_lvm2_volume_dialog_new (toplevel,
                                                          vg,
                                                          mdu_presentable_get_size (v),
                                                          flags);
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_hide (dialog);
        if (response == GTK_RESPONSE_OK) {
                CreateLinuxLvm2LVData *data;
                gchar *volume_name;
                gchar *fs_type;
                gchar *fs_label;
                gboolean fs_take_ownership;
                guint64 size;

                data = g_new0 (CreateLinuxLvm2LVData, 1);
                data->shell = g_object_ref (mdu_section_get_shell (MDU_SECTION (section)));
                data->presentable = g_object_ref (vg);

                if (mdu_format_dialog_get_encrypt (MDU_FORMAT_DIALOG (dialog))) {
                        data->encrypt_passphrase = mdu_util_dialog_ask_for_new_secret (GTK_WIDGET (toplevel),
                                                                                       &data->save_in_keyring,
                                                                                       &data->save_in_keyring_session);
                        if (data->encrypt_passphrase == NULL) {
                                create_linux_lvm2_lv_data_free (data);
                                goto out;
                        }
                }

                /* For now, just use a generic LV name - maybe we should include a GtkEntry widget in
                 * the MduCreateLinuxLvm2LVDialog, maybe not.
                 */
                volume_name = mdu_linux_lvm2_volume_group_get_compute_new_lv_name (MDU_LINUX_LVM2_VOLUME_GROUP (vg));

                fs_type = mdu_format_dialog_get_fs_type (MDU_FORMAT_DIALOG (dialog));
                fs_label = mdu_format_dialog_get_fs_label (MDU_FORMAT_DIALOG (dialog));
                fs_take_ownership = mdu_format_dialog_get_take_ownership (MDU_FORMAT_DIALOG (dialog));

                size = mdu_create_linux_lvm2_volume_dialog_get_size (MDU_CREATE_LINUX_LVM2_VOLUME_DIALOG (dialog));

                /* TODO: include widgets for configuring striping and mirroring */

                mdu_pool_op_linux_lvm2_lv_create (pool,
                                                  vg_uuid,
                                                  volume_name,
                                                  size,
                                                  0, /* num_stripes */
                                                  0, /* stripe_size */
                                                  0, /* num_mirrors */
                                                  fs_type,
                                                  fs_label,
                                                  data->encrypt_passphrase,
                                                  fs_take_ownership,
                                                  lvm2_lv_create_op_callback,
                                                  data);

                g_free (fs_type);
                g_free (fs_label);
                g_free (volume_name);
        }
 out:
        if (dialog != NULL)
                gtk_widget_destroy (dialog);
        if (v != NULL)
                g_object_unref (v);
        if (pool != NULL)
                g_object_unref (pool);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
lvm2_lv_stop_op_callback (MduDevice *device,
                          GError    *error,
                          gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new_for_volume (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                                          device,
                                                          _("Error stopping Logical Volume"),
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
on_lvm2_lv_stop_button_clicked (MduButtonElement *button_element,
                                gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduLinuxLvm2Volume *volume;
        MduDevice *d;

        volume = MDU_LINUX_LVM2_VOLUME (mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid)));
        d = mdu_presentable_get_device (MDU_PRESENTABLE (volume));
        if (d == NULL)
                goto out;

        mdu_device_op_linux_lvm2_lv_stop (d,
                                          lvm2_lv_stop_op_callback,
                                          g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

 out:
        if (d != NULL)
                g_object_unref (d);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
lvm2_lv_start_op_callback (MduPool   *pool,
                           GError    *error,
                           gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                               NULL,
                                               _("Error starting Logical Volume"),
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
on_lvm2_lv_start_button_clicked (MduButtonElement *button_element,
                                 gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduLinuxLvm2Volume *volume;
        MduPool *pool;
        const gchar *group_uuid;
        const gchar *uuid;

        volume = MDU_LINUX_LVM2_VOLUME (mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid)));
        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (volume));

        group_uuid = mdu_linux_lvm2_volume_get_group_uuid (volume);
        uuid = mdu_linux_lvm2_volume_get_uuid (volume);

        mdu_pool_op_linux_lvm2_lv_start (pool,
                                         group_uuid,
                                         uuid,
                                         lvm2_lv_start_op_callback,
                                         g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));

        g_object_unref (pool);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
lvm2_lv_set_name_op_callback (MduPool   *pool,
                              GError    *error,
                              gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                               NULL,
                                               _("Error setting name for Logical Volume"),
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
on_lvm2_lv_edit_name_button_clicked (MduButtonElement *button_element,
                                     gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduLinuxLvm2Volume *volume;
        MduPool *pool;
        const gchar *group_uuid;
        const gchar *uuid;
        const gchar *lv_name;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;


        volume = MDU_LINUX_LVM2_VOLUME (mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid)));
        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (volume));

        lv_name = mdu_linux_lvm2_volume_get_name (volume);
        group_uuid = mdu_linux_lvm2_volume_get_group_uuid (volume);
        uuid = mdu_linux_lvm2_volume_get_uuid (volume);

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_edit_name_dialog_new (toplevel,
                                           MDU_PRESENTABLE (volume),
                                           lv_name,
                                           256,
                                           _("Choose a new name for the Logical Volume."),
                                           _("_Name:"));
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        if (response == GTK_RESPONSE_APPLY) {
                gchar *new_name;
                new_name = mdu_edit_name_dialog_get_name (MDU_EDIT_NAME_DIALOG (dialog));
                mdu_pool_op_linux_lvm2_lv_set_name (pool,
                                                    group_uuid,
                                                    uuid,
                                                    new_name,
                                                    lvm2_lv_set_name_op_callback,
                                                    g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));
                g_free (new_name);
        }
        gtk_widget_destroy (dialog);

        g_object_unref (pool);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
lvm2_lv_remove_op_callback (MduPool   *pool,
                            GError    *error,
                            gpointer   user_data)
{
        MduShell *shell = MDU_SHELL (user_data);

        if (error != NULL) {
                GtkWidget *dialog;
                dialog = mdu_error_dialog_new (GTK_WINDOW (mdu_shell_get_toplevel (shell)),
                                               NULL,
                                               _("Error deleting Logical Volume"),
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
on_lvm2_lv_delete_button_clicked (MduButtonElement *button_element,
                                  gpointer          user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);
        MduLinuxLvm2Volume *volume;
        MduPool *pool;
        const gchar *group_uuid;
        const gchar *uuid;
        GtkWindow *toplevel;
        GtkWidget *dialog;
        gint response;

        volume = MDU_LINUX_LVM2_VOLUME (mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid)));
        pool = mdu_presentable_get_pool (MDU_PRESENTABLE (volume));

        group_uuid = mdu_linux_lvm2_volume_get_group_uuid (volume);
        uuid = mdu_linux_lvm2_volume_get_uuid (volume);

        toplevel = GTK_WINDOW (mdu_shell_get_toplevel (mdu_section_get_shell (MDU_SECTION (section))));
        dialog = mdu_confirmation_dialog_new (toplevel,
                                              MDU_PRESENTABLE (volume),
                                              _("Are you sure you want to delete the logical volume?"),
                                              _("_Delete"));
        gtk_widget_show_all (dialog);
        response = gtk_dialog_run (GTK_DIALOG (dialog));
        if (response == GTK_RESPONSE_OK) {
                mdu_pool_op_linux_lvm2_lv_remove (pool,
                                                  group_uuid,
                                                  uuid,
                                                  lvm2_lv_remove_op_callback,
                                                  g_object_ref (mdu_section_get_shell (MDU_SECTION (section))));
        }
        gtk_widget_destroy (dialog);

        g_object_unref (pool);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_volumes_update (MduSection *_section)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (_section);
        MduPresentable *v;
        MduDevice *d;
        MduDevice *drive_d;
        gchar *s;
        gchar *s2;
        const gchar *id_usage;
        const gchar *id_type;
        gboolean show_fs_mount_button;
        gboolean show_fs_unmount_button;
        gboolean show_fs_check_button;
        gboolean show_fs_change_label_button;
        gboolean show_format_button;
        gboolean show_partition_edit_button;
        gboolean show_partition_delete_button;
        gboolean show_partition_create_button;
        gboolean show_luks_lock_button;
        gboolean show_luks_unlock_button;
        gboolean show_luks_forget_passphrase_button;
        gboolean show_luks_change_passphrase_button;
        gboolean show_lvm2_create_lv_button;
        gboolean show_lvm2_lv_start_button;
        gboolean show_lvm2_lv_stop_button;
        gboolean show_lvm2_lv_edit_name_button;
        gboolean show_lvm2_lv_delete_button;
        gboolean show_misaligned_warning_info_bar;
        MduKnownFilesystem *kfs;
        GPtrArray *elements;
        gboolean make_insensitive;

        v = NULL;
        d = NULL;
        drive_d = NULL;
        kfs = NULL;
        id_usage = "";
        id_type = "";
        show_fs_mount_button = FALSE;
        show_fs_unmount_button = FALSE;
        show_fs_check_button = FALSE;
        show_fs_change_label_button = FALSE;
        show_format_button = FALSE;
        show_partition_edit_button = FALSE;
        show_partition_delete_button = FALSE;
        show_partition_create_button = FALSE;
        show_luks_lock_button = FALSE;
        show_luks_unlock_button = FALSE;
        show_luks_forget_passphrase_button = FALSE;
        show_luks_change_passphrase_button = FALSE;
        show_lvm2_create_lv_button = FALSE;
        show_lvm2_lv_start_button = FALSE;
        show_lvm2_lv_stop_button = FALSE;
        show_lvm2_lv_edit_name_button = FALSE;
        show_lvm2_lv_delete_button = FALSE;
        show_misaligned_warning_info_bar = FALSE;
        make_insensitive = FALSE;

        v = mdu_volume_grid_get_selected (MDU_VOLUME_GRID (section->priv->grid));

        if (v != NULL) {
                d = mdu_presentable_get_device (v);
                if (d != NULL) {
                        MduPool *pool;

                        pool = mdu_device_get_pool (d);

                        id_usage = mdu_device_id_get_usage (d);
                        id_type = mdu_device_id_get_type (d);
                        kfs = mdu_pool_get_known_filesystem_by_id (pool, id_type);

                        g_object_unref (pool);
                }
        }

        drive_d = mdu_presentable_get_device (mdu_section_get_presentable (MDU_SECTION (section)));

        /* ---------------------------------------------------------------------------------------------------- */
        /* rebuild table  */

        if (section->priv->cur_volume != NULL)
                g_object_unref (section->priv->cur_volume);
        section->priv->cur_volume = v != NULL ? g_object_ref (v) : NULL;

        section->priv->lvm2_name_element = NULL;
        section->priv->lvm2_state_element = NULL;
        section->priv->usage_element = NULL;
        section->priv->capacity_element = NULL;
        section->priv->partition_type_element = NULL;
        section->priv->partition_flags_element = NULL;
        section->priv->partition_label_element = NULL;
        section->priv->device_element = NULL;
        section->priv->fs_type_element = NULL;
        section->priv->fs_label_element = NULL;
        section->priv->fs_available_element = NULL;
        section->priv->fs_mount_point_element = NULL;

        elements = g_ptr_array_new_with_free_func (g_object_unref);

        if (MDU_IS_LINUX_LVM2_VOLUME (v)) {
                section->priv->lvm2_name_element = mdu_details_element_new (_("Volume Name:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->lvm2_name_element);
                section->priv->lvm2_state_element = mdu_details_element_new (_("State:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->lvm2_state_element);
        }

        section->priv->usage_element = mdu_details_element_new (_("Usage:"), NULL, NULL);
        g_ptr_array_add (elements, section->priv->usage_element);
        g_signal_connect (section->priv->usage_element,
                          "activated",
                          G_CALLBACK (on_usage_element_activated),
                          section);

        section->priv->device_element = mdu_details_element_new (_("Device:"), NULL, NULL);
        g_ptr_array_add (elements, section->priv->device_element);

        section->priv->partition_type_element = mdu_details_element_new (_("Partition Type:"), NULL, NULL);
        g_ptr_array_add (elements, section->priv->partition_type_element);

        if (d != NULL && mdu_device_is_partition (d)) {
                guint64 alignment_offset;

                section->priv->partition_label_element = mdu_details_element_new (_("Partition Label:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->partition_label_element);

                section->priv->partition_flags_element = mdu_details_element_new (_("Partition Flags:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->partition_flags_element);

                alignment_offset = mdu_device_partition_get_alignment_offset (d);
                if (alignment_offset != 0) {
                        /* Translators: this is the text for an infobar that shown if a partition is misaligned.
                         * First %d is number of bytes.
                         * Also see
                         * http://git.kernel.org/?p=linux/kernel/git/torvalds/linux-2.6.git;a=blob;f=Documentation/ABI/testing/sysfs-block;h=d2f90334bb93f90af2986e96d3cfd9710180eca7;hb=HEAD#l75
                         */
                        s = g_strdup_printf ("<b>WARNING:</b> The partition is misaligned by %d bytes. "
                                             "This may result in very poor performance. "
                                             "Repartitioning is suggested.",
                                             (gint) alignment_offset);
                        gtk_label_set_markup (GTK_LABEL (section->priv->misaligned_warning_label), s);
                        g_free (s);

                        show_misaligned_warning_info_bar = TRUE;
                }
        }

        section->priv->capacity_element = mdu_details_element_new (_("Capacity:"), NULL, NULL);
        g_ptr_array_add (elements, section->priv->capacity_element);

        if (g_strcmp0 (id_usage, "filesystem") == 0) {
                section->priv->fs_type_element = mdu_details_element_new (_("Type:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->fs_type_element);

                section->priv->fs_available_element = mdu_details_element_new (_("Available:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->fs_available_element);

                section->priv->fs_label_element = mdu_details_element_new (_("Label:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->fs_label_element);

                section->priv->fs_mount_point_element = mdu_details_element_new (_("Mount Point:"), NULL, NULL);
                g_ptr_array_add (elements, section->priv->fs_mount_point_element);
                g_signal_connect (section->priv->fs_mount_point_element,
                                  "activated",
                                  G_CALLBACK (on_fs_mount_point_element_activated),
                                  section);
        }

        mdu_details_table_set_elements (MDU_DETAILS_TABLE (section->priv->details_table), elements);
        g_ptr_array_unref (elements);

        /* ---------------------------------------------------------------------------------------------------- */
        /* reset all elements */

        if (MDU_IS_LINUX_LVM2_VOLUME (v)) {
                const gchar *lv_name;
                lv_name = mdu_linux_lvm2_volume_get_name (MDU_LINUX_LVM2_VOLUME (v));
                mdu_details_element_set_text (section->priv->lvm2_name_element, lv_name);

                mdu_details_element_set_text (section->priv->lvm2_state_element,
                                              d != NULL ?
                                              C_("LVM2 LV State", "Running") :
                                              C_("LVM2 LV State", "Not Running"));
                if (d != NULL)
                        show_lvm2_lv_stop_button = TRUE;
                else
                        show_lvm2_lv_start_button = TRUE;
                show_lvm2_lv_edit_name_button = TRUE;
                show_lvm2_lv_delete_button = TRUE;
        }

        if (section->priv->usage_element != NULL) {
                mdu_details_element_set_text (section->priv->usage_element, "–");
                mdu_details_element_set_action_text (section->priv->usage_element, NULL);
        }
        if (section->priv->capacity_element != NULL) {
                if (v != NULL) {
                        s = mdu_util_get_size_for_display (mdu_presentable_get_size (v), FALSE, TRUE);
                        mdu_details_element_set_text (section->priv->capacity_element, s);
                        g_free (s);
                } else {
                        mdu_details_element_set_text (section->priv->capacity_element, "–");
                }
        }
        if (section->priv->partition_type_element != NULL) {
                if (d != NULL && mdu_device_is_partition (d)) {
                        const gchar *partition_label;
                        const gchar * const *partition_flags;
                        guint n;
                        GString *str;

                        str = g_string_new (NULL);
                        partition_flags = (const gchar * const *) mdu_device_partition_get_flags (d);
                        for (n = 0; partition_flags != NULL && partition_flags[n] != NULL; n++) {
                                const gchar *flag = partition_flags[n];

                                if (str->len > 0)
                                        g_string_append (str, ", ");

                                if (g_strcmp0 (flag, "boot") == 0) {
                                        /* Translators: This is for the 'boot' MBR/APM partition flag */
                                        g_string_append (str, _("Bootable"));
                                } else if (g_strcmp0 (flag, "required") == 0) {
                                        /* Translators: This is for the 'required' GPT partition flag */
                                        g_string_append (str, _("Required"));
                                } else if (g_strcmp0 (flag, "allocated") == 0) {
                                        /* Translators: This is for the 'allocated' APM partition flag */
                                        g_string_append (str, _("Allocated"));
                                } else if (g_strcmp0 (flag, "allow_read") == 0) {
                                        /* Translators: This is for the 'allow_read' APM partition flag */
                                        g_string_append (str, _("Allow Read"));
                                } else if (g_strcmp0 (flag, "allow_write") == 0) {
                                        /* Translators: This is for the 'allow_write' APM partition flag */
                                        g_string_append (str, _("Allow Write"));
                                } else if (g_strcmp0 (flag, "boot_code_is_pic") == 0) {
                                        /* Translators: This is for the 'boot_code_is_pic' APM partition flag */
                                        g_string_append (str, _("Boot Code PIC"));
                                } else {
                                        g_string_append (str, flag);
                                }
                        }

                        s = mdu_util_get_desc_for_part_type (mdu_device_partition_get_scheme (d),
                                                             mdu_device_partition_get_type (d));
                        mdu_details_element_set_text (section->priv->partition_type_element, s);
                        g_free (s);
                        if (str->len > 0) {
                                mdu_details_element_set_text (section->priv->partition_flags_element, str->str);
                        } else {
                                mdu_details_element_set_text (section->priv->partition_flags_element, "–");
                        }
                        g_string_free (str, TRUE);

                        show_partition_delete_button = TRUE;

                        partition_label = mdu_device_partition_get_label (d);
                        if (partition_label != NULL && strlen (partition_label) > 0) {
                                mdu_details_element_set_text (section->priv->partition_label_element, partition_label);
                        } else {
                                mdu_details_element_set_text (section->priv->partition_label_element, "–");
                        }

                        /* Don't show partition edit button for extended partitions */
                        show_partition_edit_button = TRUE;
                        if (g_strcmp0 (mdu_device_partition_get_scheme (d), "mbr") == 0) {
                                gint partition_type;
                                const gchar *partition_type_str;

                                partition_type_str = mdu_device_partition_get_type (d);
                                partition_type = strtol (partition_type_str, NULL, 0);
                                if ((partition_type == 0x05 || partition_type == 0x0f || partition_type == 0x85)) {
                                        show_partition_edit_button = FALSE;
                                }
                        }
                } else {
                        mdu_details_element_set_text (section->priv->partition_type_element, "–");
                }
        }
        if (section->priv->device_element != NULL) {
                if (d != NULL) {
                        const char *device_file;
                        device_file = mdu_device_get_device_file_presentation (d);
                        if (device_file == NULL || strlen (device_file) == 0)
                                device_file = mdu_device_get_device_file (d);
                        mdu_details_element_set_text (section->priv->device_element, device_file);
                } else {
                        mdu_details_element_set_text (section->priv->device_element, "–");
                }
        }
        if (section->priv->fs_type_element != NULL)
                mdu_details_element_set_text (section->priv->fs_type_element, "–");
        if (section->priv->fs_available_element != NULL)
                mdu_details_element_set_text (section->priv->fs_available_element, "–");
        if (section->priv->fs_label_element != NULL)
                mdu_details_element_set_text (section->priv->fs_label_element, "–");
        if (section->priv->fs_mount_point_element != NULL)
                mdu_details_element_set_text (section->priv->fs_mount_point_element, "–");

        if (v == NULL)
                goto out;

        /* ---------------------------------------------------------------------------------------------------- */
        /* populate according to usage */

        if (d != NULL)
                show_format_button = TRUE;

        if (g_strcmp0 (id_usage, "filesystem") == 0) {
                const gchar *label;

                mdu_details_element_set_text (section->priv->usage_element, _("Filesystem"));
                s = mdu_util_get_fstype_for_display (mdu_device_id_get_type (d),
                                                     mdu_device_id_get_version (d),
                                                     TRUE);
                mdu_details_element_set_text (section->priv->fs_type_element, s);
                g_free (s);
                label = mdu_device_id_get_label (d);
                if (label != NULL && strlen (label) > 0)
                        mdu_details_element_set_text (section->priv->fs_label_element, label);

                /* TODO: figure out amount of free space */
                mdu_details_element_set_text (section->priv->fs_available_element, "–");


                if (mdu_device_is_mounted (d)) {
                        const gchar* const *mount_paths;
                        MduPool *pool;
                        const gchar *ssh_address;

                        pool = mdu_device_get_pool (d);
                        ssh_address = mdu_pool_get_ssh_address (pool);

                        /* For now we ignore if the device is mounted in multiple places */
                        mount_paths = (const gchar* const *) mdu_device_get_mount_paths (d);

                        if (ssh_address != NULL) {
                                /* We don't use the ssh user name right now - we could do that
                                 * but if the user logs in as root it might be bad...
                                 */
                                s = g_strdup_printf ("<a title=\"%s\" href=\"sftp://%s/%s\">%s</a>",
                                                     /* Translators: this the mount point hyperlink tooltip for a
                                                      * remote server - it uses the sftp:// protocol
                                                      */
                                                     _("View files on the volume using a SFTP network share"),
                                                     ssh_address,
                                                     mount_paths[0],
                                                     mount_paths[0]);
                        } else {
                                s = g_strdup_printf ("<a title=\"%s\" href=\"file://%s\">%s</a>",
                                                     /* Translators: this the mount point hyperlink tooltip */
                                                     _("View files on the volume"),
                                                     mount_paths[0],
                                                     mount_paths[0]);
                        }
                        /* Translators: this the the text for the mount point
                         * item - %s is the mount point, e.g. '/media/disk'
                         */
                        s2 = g_strdup_printf (_("Mounted at %s"), s);
                        mdu_details_element_set_text (section->priv->fs_mount_point_element, s2);
                        g_free (s);
                        g_free (s2);
                        g_object_unref (pool);

                        show_fs_unmount_button = TRUE;
                } else {
                        mdu_details_element_set_text (section->priv->fs_mount_point_element, _("Not Mounted"));
                        show_fs_mount_button = TRUE;
                }

                show_fs_check_button = TRUE;

        } else if (g_strcmp0 (id_usage, "crypto") == 0) {

                if (g_strcmp0 (mdu_device_luks_get_holder (d), "/") == 0) {
                        show_luks_unlock_button = TRUE;
                        mdu_details_element_set_text (section->priv->usage_element, _("Encrypted Volume (Locked)"));
                } else {
                        show_luks_lock_button = TRUE;
                        mdu_details_element_set_text (section->priv->usage_element, _("Encrypted Volume (Unlocked)"));
                }
                if (mdu_util_have_secret (d))
                        show_luks_forget_passphrase_button = TRUE;
                show_luks_change_passphrase_button = TRUE;

        } else if (g_strcmp0 (id_usage, "other") == 0 && g_strcmp0 (id_type, "swap") == 0) {

                mdu_details_element_set_text (section->priv->usage_element, _("Swap Space"));

        } else if (d != NULL && mdu_device_is_linux_md_component (d)) {

                mdu_details_element_set_text (section->priv->usage_element, _("RAID Component"));
                mdu_details_element_set_action_text (section->priv->usage_element, _("Go to array"));

        } else if (g_strcmp0 (id_usage, "") == 0 &&
                   d != NULL && mdu_device_is_partition (d) &&
                   g_strcmp0 (mdu_device_partition_get_scheme (d), "mbr") == 0 &&
                   (g_strcmp0 (mdu_device_partition_get_type (d), "0x05") == 0 ||
                    g_strcmp0 (mdu_device_partition_get_type (d), "0x0f") == 0 ||
                    g_strcmp0 (mdu_device_partition_get_type (d), "0x85") == 0)) {
                mdu_details_element_set_text (section->priv->usage_element, _("Container for Logical Partitions"));

                show_format_button = FALSE;

        } else if (MDU_IS_LINUX_LVM2_VOLUME_HOLE (v)) {
                mdu_details_element_set_text (section->priv->usage_element, _("Unallocated Space"));
                mdu_details_element_set_text (section->priv->device_element, "–");
                show_lvm2_create_lv_button = TRUE;
                show_format_button = FALSE;

        } else if (MDU_IS_VOLUME_HOLE (v)) {
                const char *device_file;

                mdu_details_element_set_text (section->priv->usage_element, _("Unallocated Space"));
                device_file = mdu_device_get_device_file_presentation (drive_d);
                if (device_file == NULL || strlen (device_file) == 0)
                        device_file = mdu_device_get_device_file (drive_d);
                mdu_details_element_set_text (section->priv->device_element, device_file);

                if (can_create_partition (section, MDU_VOLUME_HOLE (v), NULL))
                        show_partition_create_button = TRUE;
                show_format_button = FALSE;
        }

        if (kfs != NULL) {
                if (show_fs_unmount_button) {
                        if (mdu_known_filesystem_get_supports_online_label_rename (kfs)) {
                                show_fs_change_label_button = TRUE;
                        }
                } else {
                        if (mdu_known_filesystem_get_supports_label_rename (kfs)) {
                                show_fs_change_label_button = TRUE;
                        }
                }
        }

        if (drive_d != NULL && mdu_device_is_linux_dmmp_component (drive_d)) {
                make_insensitive = TRUE;
        }

 out:
        mdu_button_element_set_visible (section->priv->fs_mount_button, show_fs_mount_button);
        mdu_button_element_set_visible (section->priv->fs_unmount_button, show_fs_unmount_button);
        mdu_button_element_set_visible (section->priv->fs_check_button, show_fs_check_button);
        mdu_button_element_set_visible (section->priv->fs_change_label_button, show_fs_change_label_button);
        mdu_button_element_set_visible (section->priv->format_button, show_format_button);
        mdu_button_element_set_visible (section->priv->partition_edit_button, show_partition_edit_button);
        mdu_button_element_set_visible (section->priv->partition_delete_button, show_partition_delete_button);
        mdu_button_element_set_visible (section->priv->partition_create_button, show_partition_create_button);
        mdu_button_element_set_visible (section->priv->luks_lock_button, show_luks_lock_button);
        mdu_button_element_set_visible (section->priv->luks_unlock_button, show_luks_unlock_button);
        mdu_button_element_set_visible (section->priv->luks_forget_passphrase_button, show_luks_forget_passphrase_button);
        mdu_button_element_set_visible (section->priv->luks_change_passphrase_button, show_luks_change_passphrase_button);
        mdu_button_element_set_visible (section->priv->lvm2_create_lv_button, show_lvm2_create_lv_button);
        mdu_button_element_set_visible (section->priv->lvm2_lv_start_button, show_lvm2_lv_start_button);
        mdu_button_element_set_visible (section->priv->lvm2_lv_stop_button, show_lvm2_lv_stop_button);
        mdu_button_element_set_visible (section->priv->lvm2_lv_edit_name_button, show_lvm2_lv_edit_name_button);
        mdu_button_element_set_visible (section->priv->lvm2_lv_delete_button, show_lvm2_lv_delete_button);

        gtk_widget_set_sensitive (section->priv->main_label, !make_insensitive);
        gtk_widget_set_sensitive (section->priv->main_vbox, !make_insensitive);

        if (show_misaligned_warning_info_bar) {
                gtk_widget_show_all (section->priv->misaligned_warning_info_bar);
        } else {
                gtk_widget_hide_all (section->priv->misaligned_warning_info_bar);
        }

        if (d != NULL)
                g_object_unref (d);
        if (drive_d != NULL)
                g_object_unref (drive_d);
        if (v != NULL)
                g_object_unref (v);
        if (kfs != NULL)
                g_object_unref (kfs);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
on_grid_changed (MduVolumeGrid *grid,
                 gpointer       user_data)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (user_data);

        mdu_section_volumes_update (MDU_SECTION (section));
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_volumes_constructed (GObject *object)
{
        MduSectionVolumes *section = MDU_SECTION_VOLUMES (object);
        MduPresentable *presentable;
        GPtrArray *button_elements;
        MduButtonElement *button_element;
        GtkWidget *grid;
        GtkWidget *align;
        GtkWidget *label;
        GtkWidget *vbox2;
        GtkWidget *table;
        GtkWidget *info_bar;
        GtkWidget *hbox;
        GtkWidget *image;
        gchar *s;

        gtk_box_set_spacing (GTK_BOX (section), 12);

        /*------------------------------------- */

        presentable = mdu_section_get_presentable (MDU_SECTION (section));

        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        if (MDU_IS_LINUX_LVM2_VOLUME_GROUP (presentable)) {
                s = g_strconcat ("<b>", _("Logical _Volumes"), "</b>", NULL);
        } else {
                s = g_strconcat ("<b>", _("_Volumes"), "</b>", NULL);
        }
        gtk_label_set_markup (GTK_LABEL (label), s);
        gtk_label_set_use_underline (GTK_LABEL (label), TRUE);
        g_free (s);
        gtk_box_pack_start (GTK_BOX (section), label, FALSE, FALSE, 0);
        section->priv->main_label = label;

        align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
        gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 12, 0);
        gtk_box_pack_start (GTK_BOX (section), align, FALSE, FALSE, 0);

        vbox2 = gtk_vbox_new (FALSE, 6);
        gtk_container_add (GTK_CONTAINER (align), vbox2);
        section->priv->main_vbox = vbox2;

        /*------------------------------------- */

        grid = mdu_volume_grid_new (MDU_DRIVE (mdu_section_get_presentable (MDU_SECTION (section))));
        gtk_label_set_mnemonic_widget (GTK_LABEL (label), grid);
        section->priv->grid = grid;
        gtk_box_pack_start (GTK_BOX (vbox2),
                            grid,
                            FALSE,
                            FALSE,
                            0);
        g_signal_connect (grid,
                          "changed",
                          G_CALLBACK (on_grid_changed),
                          section);

        /*------------------------------------- */

        info_bar = gtk_info_bar_new ();
        gtk_info_bar_set_message_type (GTK_INFO_BAR (info_bar), GTK_MESSAGE_WARNING);
        hbox = gtk_hbox_new (FALSE, 6);
        image = gtk_image_new_from_stock (GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_MENU);
        gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
        gtk_label_set_width_chars (GTK_LABEL (label), 80);
        gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
        section->priv->misaligned_warning_label = label;
        gtk_container_add (GTK_CONTAINER (gtk_info_bar_get_content_area (GTK_INFO_BAR (info_bar))), hbox);
        gtk_box_pack_start (GTK_BOX (vbox2), info_bar, FALSE, FALSE, 0);
        section->priv->misaligned_warning_info_bar = info_bar;

        /*------------------------------------- */

        table = mdu_details_table_new (2, NULL);
        section->priv->details_table = table;
        gtk_box_pack_start (GTK_BOX (vbox2), table, FALSE, FALSE, 0);

        align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
        gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 12, 0);
        gtk_box_pack_start (GTK_BOX (vbox2), align, FALSE, FALSE, 0);

        table = mdu_button_table_new (2, NULL);
        section->priv->button_table = table;
        gtk_container_add (GTK_CONTAINER (align), table);
        button_elements = g_ptr_array_new_with_free_func (g_object_unref);

        button_element = mdu_button_element_new ("mdu-mount",
                                                 _("_Mount Volume"),
                                                 _("Mount the volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_mount_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->fs_mount_button = button_element;

        button_element = mdu_button_element_new ("mdu-unmount",
                                                 _("Un_mount Volume"),
                                                 _("Unmount the volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_unmount_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->fs_unmount_button = button_element;

        button_element = mdu_button_element_new ("caja-mdu",
                                                 _("Fo_rmat Volume"),
                                                 _("Erase or format the volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_format_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->format_button = button_element;

        button_element = mdu_button_element_new ("mdu-check-disk",
                                                 _("_Check Filesystem"),
                                                 _("Check and repair the filesystem"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_fsck_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->fs_check_button = button_element;

        /* TODO: better icon */
        button_element = mdu_button_element_new (GTK_STOCK_BOLD,
                                                 _("Edit Filesystem _Label"),
                                                 _("Change the label of the filesystem"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_fs_change_label_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->fs_change_label_button = button_element;

        button_element = mdu_button_element_new (GTK_STOCK_EDIT,
                                                 _("Ed_it Partition"),
                                                 _("Change partition type, label and flags"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_partition_edit_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->partition_edit_button = button_element;

        button_element = mdu_button_element_new (GTK_STOCK_DELETE,
                                                 _("D_elete Partition"),
                                                 _("Delete the partition"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_partition_delete_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->partition_delete_button = button_element;

        button_element = mdu_button_element_new (GTK_STOCK_ADD,
                                                 _("_Create Partition"),
                                                 _("Create a new partition"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_partition_create_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->partition_create_button = button_element;

        button_element = mdu_button_element_new ("mdu-encrypted-lock",
                                                 _("_Lock Volume"),
                                                 _("Make encrypted data unavailable"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_luks_lock_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->luks_lock_button = button_element;

        button_element = mdu_button_element_new ("mdu-encrypted-unlock",
                                                 _("Un_lock Volume"),
                                                 _("Make encrypted data available"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_luks_unlock_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->luks_unlock_button = button_element;

        button_element = mdu_button_element_new (GTK_STOCK_CLEAR,
                                                 _("Forge_t Passphrase"),
                                                 _("Delete passphrase from keyring"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_luks_forget_passphrase_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->luks_forget_passphrase_button = button_element;

        /* TODO: not a great choice for the icon but _EDIT would conflict with "Edit Partition */
        button_element = mdu_button_element_new (GTK_STOCK_FIND_AND_REPLACE,
                                                 _("Change _Passphrase"),
                                                 _("Change passphrase"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_luks_change_passphrase_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->luks_change_passphrase_button = button_element;

        button_element = mdu_button_element_new (GTK_STOCK_ADD,
                                                 _("_Create Logical Volume"),
                                                 _("Create a new logical volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_lvm2_create_lv_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->lvm2_create_lv_button = button_element;

        button_element = mdu_button_element_new ("mdu-raid-array-start",
                                                 _("S_tart Volume"),
                                                 _("Activate the Logical Volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_lvm2_lv_start_button_clicked),
                          section);
        section->priv->lvm2_lv_start_button = button_element;
        g_ptr_array_add (button_elements, button_element);

        /* TODO: better icon */
        button_element = mdu_button_element_new (GTK_STOCK_BOLD,
                                                 _("Edit Vol_ume Name"),
                                                 _("Change the name of the volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_lvm2_lv_edit_name_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->lvm2_lv_edit_name_button = button_element;

        button_element = mdu_button_element_new (GTK_STOCK_DELETE,
                                                 _("D_elete Volume"),
                                                 _("Delete the Logical Volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_lvm2_lv_delete_button_clicked),
                          section);
        g_ptr_array_add (button_elements, button_element);
        section->priv->lvm2_lv_delete_button = button_element;

        button_element = mdu_button_element_new ("mdu-raid-array-stop",
                                                 _("Sto_p Volume"),
                                                 _("Deactivate the Logical Volume"));
        g_signal_connect (button_element,
                          "clicked",
                          G_CALLBACK (on_lvm2_lv_stop_button_clicked),
                          section);
        section->priv->lvm2_lv_stop_button = button_element;
        g_ptr_array_add (button_elements, button_element);

        mdu_button_table_set_elements (MDU_BUTTON_TABLE (section->priv->button_table), button_elements);
        g_ptr_array_unref (button_elements);

        /* -------------------------------------------------------------------------------- */

        gtk_widget_show_all (GTK_WIDGET (section));

        if (G_OBJECT_CLASS (mdu_section_volumes_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_section_volumes_parent_class)->constructed (object);
}

static void
mdu_section_volumes_class_init (MduSectionVolumesClass *klass)
{
        GObjectClass *gobject_class;
        MduSectionClass *section_class;

        gobject_class = G_OBJECT_CLASS (klass);
        section_class = MDU_SECTION_CLASS (klass);

        gobject_class->finalize    = mdu_section_volumes_finalize;
        gobject_class->constructed = mdu_section_volumes_constructed;
        section_class->update      = mdu_section_volumes_update;

        g_type_class_add_private (klass, sizeof (MduSectionVolumesPrivate));
}

static void
mdu_section_volumes_init (MduSectionVolumes *section)
{
        section->priv = G_TYPE_INSTANCE_GET_PRIVATE (section, MDU_TYPE_SECTION_VOLUMES, MduSectionVolumesPrivate);
}

GtkWidget *
mdu_section_volumes_new (MduShell       *shell,
                         MduPresentable *presentable)
{
        return GTK_WIDGET (g_object_new (MDU_TYPE_SECTION_VOLUMES,
                                         "shell", shell,
                                         "presentable", presentable,
                                         NULL));
}

gboolean
mdu_section_volumes_select_volume (MduSectionVolumes *section,
                                   MduPresentable    *volume)
{
        g_return_val_if_fail (MDU_IS_SECTION_VOLUMES (section), FALSE);
        return mdu_volume_grid_select (MDU_VOLUME_GRID (section->priv->grid), volume);
}
