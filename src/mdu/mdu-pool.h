/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-pool.h
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

#if !defined (__MDU_INSIDE_MDU_H) && !defined (MDU_COMPILATION)
#error "Only <mdu/mdu.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_POOL_H
#define __MDU_POOL_H

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>

G_BEGIN_DECLS

#define MDU_TYPE_POOL         (mdu_pool_get_type ())
#define MDU_POOL(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_POOL, MduPool))
#define MDU_POOL_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_POOL,  MduPoolClass))
#define MDU_IS_POOL(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_POOL))
#define MDU_IS_POOL_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_POOL))
#define MDU_POOL_GET_CLASS(k) (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_POOL, MduPoolClass))

typedef struct _MduPoolClass       MduPoolClass;
typedef struct _MduPoolPrivate     MduPoolPrivate;

struct _MduPool
{
        GObject parent;

        /* private */
        MduPoolPrivate *priv;
};

struct _MduPoolClass
{
        GObjectClass parent_class;

        /* signals */
        void (*disconnected) (MduPool *pool);

        void (*device_added) (MduPool *pool, MduDevice *device);
        void (*device_removed) (MduPool *pool, MduDevice *device);
        void (*device_changed) (MduPool *pool, MduDevice *device);
        void (*device_job_changed) (MduPool *pool, MduDevice *device);

        void (*adapter_added) (MduPool *pool, MduAdapter *adapter);
        void (*adapter_removed) (MduPool *pool, MduAdapter *adapter);
        void (*adapter_changed) (MduPool *pool, MduAdapter *adapter);

        void (*expander_added) (MduPool *pool, MduExpander *expander);
        void (*expander_removed) (MduPool *pool, MduExpander *expander);
        void (*expander_changed) (MduPool *pool, MduExpander *expander);

        void (*port_added) (MduPool *pool, MduPort *port);
        void (*port_removed) (MduPool *pool, MduPort *port);
        void (*port_changed) (MduPool *pool, MduPort *port);

        void (*presentable_added) (MduPool *pool, MduPresentable *presentable);
        void (*presentable_removed) (MduPool *pool, MduPresentable *presentable);
        void (*presentable_changed) (MduPool *pool, MduPresentable *presentable);
        void (*presentable_job_changed) (MduPool *pool, MduPresentable *presentable);
};

GType       mdu_pool_get_type           (void);
MduPool    *mdu_pool_new                (void);
MduPool    *mdu_pool_new_for_address    (const gchar  *ssh_user_name,
                                         const gchar  *ssh_address,
                                         GError      **error);

const gchar *mdu_pool_get_ssh_user_name (MduPool *pool);
const gchar *mdu_pool_get_ssh_address   (MduPool *pool);

char       *mdu_pool_get_daemon_version (MduPool *pool);
gboolean    mdu_pool_is_daemon_inhibited (MduPool *pool);
gboolean    mdu_pool_supports_luks_devices (MduPool *pool);
GList      *mdu_pool_get_known_filesystems (MduPool *pool);
MduKnownFilesystem *mdu_pool_get_known_filesystem_by_id (MduPool *pool, const char *id);

MduDevice  *mdu_pool_get_by_object_path (MduPool *pool, const char *object_path);
MduDevice  *mdu_pool_get_by_device_file (MduPool *pool, const char *device_file);
MduPresentable *mdu_pool_get_volume_by_device      (MduPool *pool, MduDevice *device);
MduPresentable *mdu_pool_get_drive_by_device       (MduPool *pool, MduDevice *device);
MduLinuxMdDrive *mdu_pool_get_linux_md_drive_by_uuid (MduPool *pool, const gchar *uuid);

MduPresentable *mdu_pool_get_presentable_by_id     (MduPool *pool, const gchar *id);

gboolean    mdu_pool_has_presentable (MduPool *pool, MduPresentable *presentable);


GList      *mdu_pool_get_devices               (MduPool *pool);
GList      *mdu_pool_get_presentables          (MduPool *pool);
GList      *mdu_pool_get_enclosed_presentables (MduPool *pool, MduPresentable *presentable);

MduAdapter *mdu_pool_get_adapter_by_object_path (MduPool *pool, const char *object_path);
GList      *mdu_pool_get_adapters               (MduPool *pool);

MduExpander *mdu_pool_get_expander_by_object_path (MduPool *pool, const char *object_path);
GList      *mdu_pool_get_expanders               (MduPool *pool);

MduPort    *mdu_pool_get_port_by_object_path (MduPool *pool, const char *object_path);
GList      *mdu_pool_get_ports               (MduPool *pool);

MduPresentable *mdu_pool_get_hub_by_object_path (MduPool *pool, const gchar *object_path);

/* ---------------------------------------------------------------------------------------------------- */

void mdu_pool_op_linux_md_start (MduPool *pool,
                                 GPtrArray *component_objpaths,
                                 MduPoolLinuxMdStartCompletedFunc callback,
                                 gpointer user_data);

void mdu_pool_op_linux_md_create (MduPool     *pool,
                                  GPtrArray   *component_objpaths,
                                  const gchar *level,
                                  guint64      stripe_size,
                                  const gchar *name,
                                  MduPoolLinuxMdCreateCompletedFunc callback,
                                  gpointer user_data);

void mdu_pool_op_linux_lvm2_vg_start (MduPool *pool,
                                      const gchar *uuid,
                                      MduPoolLinuxLvm2VGStartCompletedFunc callback,
                                      gpointer user_data);

void mdu_pool_op_linux_lvm2_vg_stop (MduPool *pool,
                                     const gchar *uuid,
                                     MduPoolLinuxLvm2VGStopCompletedFunc callback,
                                     gpointer user_data);

void mdu_pool_op_linux_lvm2_lv_start (MduPool *pool,
                                      const gchar *group_uuid,
                                      const gchar *uuid,
                                      MduPoolLinuxLvm2VGStartCompletedFunc callback,
                                      gpointer user_data);

void mdu_pool_op_linux_lvm2_vg_set_name (MduPool *pool,
                                         const gchar *uuid,
                                         const gchar *new_name,
                                         MduPoolLinuxLvm2VGSetNameCompletedFunc callback,
                                         gpointer user_data);

void mdu_pool_op_linux_lvm2_lv_set_name (MduPool *pool,
                                         const gchar *group_uuid,
                                         const gchar *uuid,
                                         const gchar *new_name,
                                         MduPoolLinuxLvm2LVSetNameCompletedFunc callback,
                                         gpointer user_data);

void mdu_pool_op_linux_lvm2_lv_remove (MduPool *pool,
                                       const gchar *group_uuid,
                                       const gchar *uuid,
                                       MduPoolLinuxLvm2LVRemoveCompletedFunc callback,
                                       gpointer user_data);

void mdu_pool_op_linux_lvm2_lv_create (MduPool *pool,
                                       const gchar *group_uuid,
                                       const gchar *name,
                                       guint64 size,
                                       guint num_stripes,
                                       guint64 stripe_size,
                                       guint num_mirrors,
                                       const char                             *fstype,
                                       const char                             *fslabel,
                                       const char                             *encrypt_passphrase,
                                       gboolean                                fs_take_ownership,
                                       MduPoolLinuxLvm2LVCreateCompletedFunc callback,
                                       gpointer user_data);

void mdu_pool_op_linux_lvm2_vg_add_pv (MduPool *pool,
                                       const gchar *uuid,
                                       const gchar *physical_volume_object_path,
                                       MduPoolLinuxLvm2VGAddPVCompletedFunc callback,
                                       gpointer user_data);

void mdu_pool_op_linux_lvm2_vg_remove_pv (MduPool *pool,
                                          const gchar *vg_uuid,
                                          const gchar *pv_uuid,
                                          MduPoolLinuxLvm2VGRemovePVCompletedFunc callback,
                                          gpointer user_data);

G_END_DECLS

#endif /* __MDU_POOL_H */
