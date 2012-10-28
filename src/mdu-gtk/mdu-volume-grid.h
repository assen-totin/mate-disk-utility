/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-grid-element.h
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

#if !defined (__MDU_GTK_INSIDE_MDU_GTK_H) && !defined (MDU_GTK_COMPILATION)
#error "Only <mdu-gtk/mdu-gtk.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_VOLUME_GRID_H
#define __MDU_VOLUME_GRID_H

#include <mdu-gtk/mdu-gtk-types.h>

G_BEGIN_DECLS

#define MDU_TYPE_VOLUME_GRID         mdu_volume_grid_get_type()
#define MDU_VOLUME_GRID(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_VOLUME_GRID, MduVolumeGrid))
#define MDU_VOLUME_GRID_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_VOLUME_GRID, MduVolumeGridClass))
#define MDU_IS_VOLUME_GRID(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_VOLUME_GRID))
#define MDU_IS_VOLUME_GRID_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_VOLUME_GRID))
#define MDU_VOLUME_GRID_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_VOLUME_GRID, MduVolumeGridClass))

typedef struct MduVolumeGridClass   MduVolumeGridClass;
typedef struct MduVolumeGridPrivate MduVolumeGridPrivate;

struct MduVolumeGrid
{
        GtkDrawingArea parent;

        /*< private >*/
        MduVolumeGridPrivate *priv;
};

struct MduVolumeGridClass
{
        GtkDrawingAreaClass parent_class;

        /* signals */
        void (*changed) (MduVolumeGrid *grid);
};

GType           mdu_volume_grid_get_type         (void) G_GNUC_CONST;
GtkWidget*      mdu_volume_grid_new              (MduDrive            *drive);
MduPresentable *mdu_volume_grid_get_selected     (MduVolumeGrid       *grid);
gboolean        mdu_volume_grid_select           (MduVolumeGrid       *grid,
                                                  MduPresentable      *volume);

G_END_DECLS



#endif /* __MDU_VOLUME_GRID_H */
