/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2009 Red Hat, Inc.
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
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include <string.h>

#include "mdu-gtk.h"
#include "mdu-pool-tree-model.h"

struct MduPoolTreeModelPrivate
{
        GPtrArray *pools;
        MduPresentable *root;
        MduPoolTreeModelFlags flags;
        gboolean constructed;
};

G_DEFINE_TYPE (MduPoolTreeModel, mdu_pool_tree_model, GTK_TYPE_TREE_STORE)

enum
{
        PROP_0,
        PROP_ROOT,
        PROP_POOLS,
        PROP_FLAGS,
};

/* ---------------------------------------------------------------------------------------------------- */

static void on_presentable_added   (MduPool          *pool,
                                    MduPresentable   *presentable,
                                    gpointer          user_data);
static void on_presentable_removed (MduPool          *pool,
                                    MduPresentable   *presentable,
                                    gpointer          user_data);
static void on_presentable_changed (MduPool          *pool,
                                    MduPresentable   *presentable,
                                    gpointer          user_data);
static void add_presentable        (MduPoolTreeModel *model,
                                    MduPresentable   *presentable,
                                    GtkTreeIter      *iter_out);

/* ---------------------------------------------------------------------------------------------------- */

static void
disconnect_from_pool_cb (MduPool  *pool,
                         gpointer  user_data)
{
        MduPoolTreeModel *model = MDU_POOL_TREE_MODEL (user_data);
        g_signal_handlers_disconnect_by_func (pool, on_presentable_added, model);
        g_signal_handlers_disconnect_by_func (pool, on_presentable_removed, model);
        g_signal_handlers_disconnect_by_func (pool, on_presentable_changed, model);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_pool_tree_model_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
        MduPoolTreeModel *model = MDU_POOL_TREE_MODEL (object);

        switch (prop_id) {
        case PROP_POOLS:
                mdu_pool_tree_model_set_pools (model, g_value_get_boxed (value));
                break;

        case PROP_ROOT:
                if (g_value_get_object (value) != NULL)
                        model->priv->root = g_value_dup_object (value);
                break;

        case PROP_FLAGS:
                model->priv->flags = g_value_get_flags (value);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
mdu_pool_tree_model_get_property (GObject     *object,
                                  guint        prop_id,
                                  GValue      *value,
                                  GParamSpec  *pspec)
{
        MduPoolTreeModel *model = MDU_POOL_TREE_MODEL (object);

        switch (prop_id) {
        case PROP_POOLS:
                g_value_set_boxed (value, model->priv->pools);
                break;

        case PROP_ROOT:
                g_value_set_object (value, model->priv->root);
                break;

        case PROP_FLAGS:
                g_value_set_flags (value, model->priv->flags);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
    }
}

static void
mdu_pool_tree_model_finalize (GObject *object)
{
        MduPoolTreeModel *model = MDU_POOL_TREE_MODEL (object);

        if (model->priv->pools != NULL) {
                g_ptr_array_foreach (model->priv->pools, (GFunc) disconnect_from_pool_cb, model);
                g_ptr_array_unref (model->priv->pools);
        }

        if (G_OBJECT_CLASS (mdu_pool_tree_model_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_pool_tree_model_parent_class)->finalize (object);
}

static gint
presentable_sort_func (GtkTreeModel *model,
                       GtkTreeIter  *a,
                       GtkTreeIter  *b,
                       gpointer      userdata)
{
        MduPresentable *p1;
        MduPresentable *p2;
        gint result;

        result = 0;

        gtk_tree_model_get (model, a, MDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE, &p1, -1);
        gtk_tree_model_get (model, b, MDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE, &p2, -1);
        if (p1 == NULL || p2 == NULL)
                goto out;

        result = mdu_presentable_compare (p1, p2);

 out:
        if (p1 != NULL)
                g_object_unref (p1);
        if (p2 != NULL)
                g_object_unref (p2);

        return result;
}

static void
do_coldplug (MduPoolTreeModel *model)
{
        guint n;

        /* remove all.. */
        gtk_tree_store_clear (GTK_TREE_STORE (model));

        /* then coldplug it back */
        for (n = 0; n < model->priv->pools->len; n++) {
                MduPool *pool = MDU_POOL (model->priv->pools->pdata[n]);
                GList *presentables;
                GList *l;

                presentables = mdu_pool_get_presentables (pool);
                for (l = presentables; l != NULL; l = l->next) {
                        MduPresentable *presentable = MDU_PRESENTABLE (l->data);

                        add_presentable (model, presentable, NULL);
                        g_object_unref (presentable);
                }
                g_list_free (presentables);
        }
}

static void
mdu_pool_tree_model_constructed (GObject *object)
{
        MduPoolTreeModel *model = MDU_POOL_TREE_MODEL (object);
        GType column_types[8];

        column_types[0] = G_TYPE_ICON;
        column_types[1] = G_TYPE_STRING;
        column_types[2] = G_TYPE_STRING;
        column_types[3] = G_TYPE_STRING;
        column_types[4] = MDU_TYPE_PRESENTABLE;
        column_types[5] = G_TYPE_BOOLEAN;
        column_types[6] = G_TYPE_BOOLEAN;
        column_types[7] = G_TYPE_BOOLEAN;

        gtk_tree_store_set_column_types (GTK_TREE_STORE (model),
                                         G_N_ELEMENTS (column_types),
                                         column_types);

        gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (model),
                                         MDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE,
                                         presentable_sort_func,
                                         NULL,
                                         NULL);

        gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (model),
                                              MDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE,
                                              GTK_SORT_ASCENDING);

        model->priv->constructed = TRUE;

        do_coldplug (model);

        if (G_OBJECT_CLASS (mdu_pool_tree_model_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_pool_tree_model_parent_class)->constructed (object);
}

void
mdu_pool_tree_model_set_pools (MduPoolTreeModel      *model,
                               GPtrArray             *pools)
{
        guint n;

        /* Do a deep copy here */
        if (model->priv->pools != NULL) {
                g_ptr_array_foreach (model->priv->pools, (GFunc) disconnect_from_pool_cb, model);
                g_ptr_array_unref (model->priv->pools);
        }

        model->priv->pools = g_ptr_array_new_with_free_func (g_object_unref);
        for (n = 0; pools != NULL && n < pools->len; n++) {
                MduPool *pool = MDU_POOL (pools->pdata[n]);

                g_signal_connect (pool,
                                  "presentable-added",
                                  G_CALLBACK (on_presentable_added),
                                  model);
                g_signal_connect (pool,
                                  "presentable-removed",
                                  G_CALLBACK (on_presentable_removed),
                                  model);
                g_signal_connect (pool,
                                  "presentable-changed",
                                  G_CALLBACK (on_presentable_changed),
                                  model);

                g_ptr_array_add (model->priv->pools, g_object_ref (pool));
        }

        /* only do coldplug if we have been constructed as the result depends on the value of the flags */
        if (model->priv->constructed)
                do_coldplug (model);
}

static void
mdu_pool_tree_model_class_init (MduPoolTreeModelClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

        gobject_class->finalize     = mdu_pool_tree_model_finalize;
        gobject_class->constructed  = mdu_pool_tree_model_constructed;
        gobject_class->set_property = mdu_pool_tree_model_set_property;
        gobject_class->get_property = mdu_pool_tree_model_get_property;

        g_type_class_add_private (klass, sizeof (MduPoolTreeModelPrivate));

        /**
         * MduPoolTreeModel:pools:
         *
         * The pools displayed - this must be a #GPtrArray of #MduPool objects.
         */
        g_object_class_install_property (gobject_class,
                                         PROP_POOLS,
                                         g_param_spec_boxed ("pools",
                                                             NULL,
                                                             NULL,
                                                             G_TYPE_PTR_ARRAY,
                                                             G_PARAM_WRITABLE |
                                                             G_PARAM_READABLE |
                                                             G_PARAM_CONSTRUCT));

        /**
         * MduPoolTreeModel:root:
         *
         * %NULL to include all #MduPresentable objects in #MduPoolTreeModel:pool, otherwise only
         * include presentables that are descendents of this #MduPresentable.
         */
        g_object_class_install_property (gobject_class,
                                         PROP_ROOT,
                                         g_param_spec_object ("root",
                                                              NULL,
                                                              NULL,
                                                              MDU_TYPE_PRESENTABLE,
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_READABLE |
                                                              G_PARAM_CONSTRUCT_ONLY));

        /**
         * MduPoolTreeModel:flags:
         *
         * The flags for the model.
         */
        g_object_class_install_property (gobject_class,
                                         PROP_FLAGS,
                                         g_param_spec_flags ("flags",
                                                             NULL,
                                                             NULL,
                                                             MDU_TYPE_POOL_TREE_MODEL_FLAGS,
                                                             MDU_POOL_TREE_MODEL_FLAGS_NONE,
                                                             G_PARAM_WRITABLE |
                                                             G_PARAM_READABLE |
                                                             G_PARAM_CONSTRUCT_ONLY));
}

static void
mdu_pool_tree_model_init (MduPoolTreeModel *model)
{
        model->priv = G_TYPE_INSTANCE_GET_PRIVATE (model,
                                                   MDU_TYPE_POOL_TREE_MODEL,
                                                   MduPoolTreeModelPrivate);
}

MduPoolTreeModel *
mdu_pool_tree_model_new (GPtrArray             *pools,
                         MduPresentable        *root,
                         MduPoolTreeModelFlags  flags)
{
        return MDU_POOL_TREE_MODEL (g_object_new (MDU_TYPE_POOL_TREE_MODEL,
                                                  "pools", pools,
                                                  "root", root,
                                                  "flags", flags,
                                                  NULL));
}

/* ---------------------------------------------------------------------------------------------------- */

typedef struct {
        MduPresentable *presentable;
        gboolean found;
        GtkTreeIter iter;
} FIBDData;

static gboolean
find_iter_by_presentable_foreach (GtkTreeModel *model,
                                  GtkTreePath  *path,
                                  GtkTreeIter  *iter,
                                  gpointer      data)
{
        gboolean ret;
        MduPresentable *presentable = NULL;
        FIBDData *fibd_data = (FIBDData *) data;

        ret = FALSE;
        gtk_tree_model_get (model,
                            iter,
                            MDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE, &presentable,
                            -1);
        if (g_strcmp0 (mdu_presentable_get_id (presentable), mdu_presentable_get_id (fibd_data->presentable)) == 0) {
                fibd_data->found = TRUE;
                fibd_data->iter = *iter;
                ret = TRUE;
        }
        if (presentable != NULL)
                g_object_unref (presentable);

        return ret;
}


gboolean
mdu_pool_tree_model_get_iter_for_presentable (MduPoolTreeModel *model,
                                              MduPresentable   *presentable,
                                              GtkTreeIter      *out_iter)
{
        FIBDData fibd_data;
        gboolean ret;

        fibd_data.presentable = presentable;
        fibd_data.found = FALSE;
        gtk_tree_model_foreach (GTK_TREE_MODEL (model),
                                find_iter_by_presentable_foreach,
                                &fibd_data);
        if (fibd_data.found) {
                if (out_iter != NULL)
                        *out_iter = fibd_data.iter;
                ret = TRUE;
        } else {
                ret = FALSE;
        }

        return ret;
}

/* ---------------------------------------------------------------------------------------------------- */

static void
set_data_for_presentable (MduPoolTreeModel *model,
                          GtkTreeIter      *iter,
                          MduPresentable   *presentable)
{
        MduDevice *device;
        GIcon *icon;
        gchar *vpd_name;
        gchar *name;
        gchar *desc;

        device = mdu_presentable_get_device (presentable);

        name = mdu_presentable_get_name (presentable);
        desc = mdu_presentable_get_description (presentable);
        vpd_name = mdu_presentable_get_vpd_name (presentable);

        icon = mdu_presentable_get_icon (presentable);

        /* TODO: insert NAME */
        gtk_tree_store_set (GTK_TREE_STORE (model),
                            iter,
                            MDU_POOL_TREE_MODEL_COLUMN_ICON, icon,
                            MDU_POOL_TREE_MODEL_COLUMN_VPD_NAME, vpd_name,
                            MDU_POOL_TREE_MODEL_COLUMN_NAME, name,
                            MDU_POOL_TREE_MODEL_COLUMN_DESCRIPTION, desc,
                            MDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE, presentable,
                            MDU_POOL_TREE_MODEL_COLUMN_VISIBLE, TRUE,
                            MDU_POOL_TREE_MODEL_COLUMN_TOGGLED, FALSE,
                            MDU_POOL_TREE_MODEL_COLUMN_CAN_BE_TOGGLED, FALSE,
                            -1);

        g_object_unref (icon);
        g_free (vpd_name);
        g_free (name);
        g_free (desc);
        if (device != NULL)
                g_object_unref (device);
}

static gboolean
should_include_presentable (MduPoolTreeModel *model,
                            MduPresentable   *presentable)
{
        gboolean ret;

        ret = FALSE;

        /* see if it should be ignored because it is a volume */
        if ((model->priv->flags & MDU_POOL_TREE_MODEL_FLAGS_NO_VOLUMES) &&
            (MDU_IS_VOLUME (presentable) || MDU_IS_VOLUME_HOLE (presentable)))
                goto out;

        if (model->priv->root != NULL) {
                gboolean is_enclosed_by_root;
                MduPresentable *p_iter;

                is_enclosed_by_root = FALSE;
                p_iter = g_object_ref (presentable);
                do {
                        MduPresentable *p;

                        p = mdu_presentable_get_enclosing_presentable (p_iter);
                        g_object_unref (p_iter);
                        if (p == NULL)
                                break;

                        if (p == model->priv->root) {
                                is_enclosed_by_root = TRUE;
                                g_object_unref (p);
                                break;
                        }

                        p_iter = p;

                } while (TRUE);

                if (!is_enclosed_by_root)
                        goto out;
        }

        if (MDU_IS_DRIVE (presentable)) {
                if ((model->priv->flags & MDU_POOL_TREE_MODEL_FLAGS_NO_UNALLOCATABLE_DRIVES) &&
                    (!mdu_drive_can_create_volume (MDU_DRIVE (presentable), NULL, NULL, NULL, NULL)))
                        goto out;
        }

        ret = TRUE;

 out:
        return ret;
}


static void
add_presentable (MduPoolTreeModel *model,
                 MduPresentable   *presentable,
                 GtkTreeIter      *iter_out)
{
        GtkTreeIter  iter;
        GtkTreeIter  iter2;
        GtkTreeIter *parent_iter;
        MduPresentable *enclosing_presentable;

        /* check to see if presentable is already added */
        if (mdu_pool_tree_model_get_iter_for_presentable (model, presentable, NULL))
                goto out;

        if (!should_include_presentable (model, presentable))
                goto out;

        /* set up parent relationship */
        parent_iter = NULL;
        enclosing_presentable = mdu_presentable_get_enclosing_presentable (presentable);
        if (enclosing_presentable != NULL) {
                if (mdu_pool_tree_model_get_iter_for_presentable (model, enclosing_presentable, &iter2)) {
                        parent_iter = &iter2;
                } else {
                        if (should_include_presentable (model, enclosing_presentable)) {
                                /* add parent if it's not already added */
                                g_warning ("No parent for %s", mdu_presentable_get_id (enclosing_presentable));
                                add_presentable (model, enclosing_presentable, &iter2);
                                parent_iter = &iter2;
                        } else {
                                /* parent explicitly excluded */
                        }
                }
                g_object_unref (enclosing_presentable);
        }


        /* g_debug ("adding %s (%p)", mdu_presentable_get_id (presentable), presentable); */

        gtk_tree_store_append (GTK_TREE_STORE (model),
                               &iter,
                               parent_iter);

        set_data_for_presentable (model,
                                  &iter,
                                  presentable);


        if (iter_out != NULL)
                *iter_out = iter;

out:
        ;
}

static void
on_presentable_added (MduPool          *pool,
                      MduPresentable   *presentable,
                      gpointer          user_data)
{
        MduPoolTreeModel *model = MDU_POOL_TREE_MODEL (user_data);

        /* g_debug ("on_added `%s' (%p)", mdu_presentable_get_id (presentable), presentable); */

        add_presentable (model, presentable, NULL);
}

static void
on_presentable_removed (MduPool          *pool,
                        MduPresentable   *presentable,
                        gpointer          user_data)
{
        MduPoolTreeModel *model = MDU_POOL_TREE_MODEL (user_data);
        GtkTreeIter iter;

        /* g_debug ("on_removed `%s' (%p)", mdu_presentable_get_id (presentable), presentable); */

        if (mdu_pool_tree_model_get_iter_for_presentable (model, presentable, &iter)) {
                /* g_debug ("removed row for `%s' (%p)", mdu_presentable_get_id (presentable), presentable); */
                gtk_tree_store_remove (GTK_TREE_STORE (model), &iter);
        }
}

static void
on_presentable_changed (MduPool          *pool,
                        MduPresentable   *presentable,
                        gpointer          user_data)
{
        MduPoolTreeModel *model = MDU_POOL_TREE_MODEL (user_data);
        GtkTreeIter iter;

        if (mdu_pool_has_presentable (pool, presentable)) {
                /* g_debug ("on_changed `%s' (%p)", mdu_presentable_get_id (presentable), presentable); */

                /* will do NOP if presentable has already been added */
                add_presentable (model, presentable, NULL);

                /* update name and icon */
                if (mdu_pool_tree_model_get_iter_for_presentable (model, presentable, &iter)) {
                        set_data_for_presentable (model,
                                                  &iter,
                                                  presentable);
                }
        }
}
