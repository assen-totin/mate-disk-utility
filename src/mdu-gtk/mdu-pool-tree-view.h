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

#if !defined (__MDU_GTK_INSIDE_MDU_GTK_H) && !defined (MDU_GTK_COMPILATION)
#error "Only <mdu-gtk/mdu-gtk.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef MDU_POOL_TREE_VIEW_H
#define MDU_POOL_TREE_VIEW_H

#include <mdu-gtk/mdu-gtk-types.h>

#define MDU_TYPE_POOL_TREE_VIEW             (mdu_pool_tree_view_get_type ())
#define MDU_POOL_TREE_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_POOL_TREE_VIEW, MduPoolTreeView))
#define MDU_POOL_TREE_VIEW_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), MDU_POOL_TREE_VIEW,  MduPoolTreeViewClass))
#define MDU_IS_POOL_TREE_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_POOL_TREE_VIEW))
#define MDU_IS_POOL_TREE_VIEW_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), MDU_TYPE_POOL_TREE_VIEW))
#define MDU_POOL_TREE_VIEW_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_POOL_TREE_VIEW, MduPoolTreeViewClass))

typedef struct MduPoolTreeViewClass       MduPoolTreeViewClass;
typedef struct MduPoolTreeViewPrivate     MduPoolTreeViewPrivate;

struct MduPoolTreeView
{
        GtkTreeView parent;

        /*< private >*/
        MduPoolTreeViewPrivate *priv;
};

struct MduPoolTreeViewClass
{
        GtkTreeViewClass parent_class;
};

GType            mdu_pool_tree_view_get_type                 (void) G_GNUC_CONST;
GtkWidget       *mdu_pool_tree_view_new                      (MduPoolTreeModel     *model,
                                                              MduPoolTreeViewFlags  flags);
MduPresentable  *mdu_pool_tree_view_get_selected_presentable (MduPoolTreeView      *view);
void             mdu_pool_tree_view_select_presentable       (MduPoolTreeView      *view,
                                                              MduPresentable       *presentable);
void             mdu_pool_tree_view_select_first_presentable (MduPoolTreeView      *view);

#endif /* MDU_POOL_TREE_VIEW_H */
