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

#ifndef __MDU_DISK_SELECTION_WIDGET_H
#define __MDU_DISK_SELECTION_WIDGET_H

#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_DISK_SELECTION_WIDGET         (mdu_disk_selection_widget_get_type())
#define MDU_DISK_SELECTION_WIDGET(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_DISK_SELECTION_WIDGET, MduDiskSelectionWidget))
#define MDU_DISK_SELECTION_WIDGET_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_DISK_SELECTION_WIDGET, MduDiskSelectionWidgetClass))
#define MDU_IS_DISK_SELECTION_WIDGET(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_DISK_SELECTION_WIDGET))
#define MDU_IS_DISK_SELECTION_WIDGET_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_DISK_SELECTION_WIDGET))
#define MDU_DISK_SELECTION_WIDGET_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_DISK_SELECTION_WIDGET, MduDiskSelectionWidgetClass))

typedef struct MduDiskSelectionWidgetClass   MduDiskSelectionWidgetClass;
typedef struct MduDiskSelectionWidgetPrivate MduDiskSelectionWidgetPrivate;

struct MduDiskSelectionWidget
{
        GtkVBox parent;

        /*< private >*/
        MduDiskSelectionWidgetPrivate *priv;
};

struct MduDiskSelectionWidgetClass
{
        GtkVBoxClass parent_class;

        void (*changed) (MduDiskSelectionWidget *widget);

        gchar *(*is_drive_ignored) (MduDiskSelectionWidget *widget,
                                    MduDrive               *drive);
};

GType       mdu_disk_selection_widget_get_type                         (void) G_GNUC_CONST;
GtkWidget  *mdu_disk_selection_widget_new                              (MduPool                     *pool,
                                                                        MduDiskSelectionWidgetFlags  flags);
GPtrArray  *mdu_disk_selection_widget_get_selected_drives              (MduDiskSelectionWidget      *widget);
guint64     mdu_disk_selection_widget_get_component_size               (MduDiskSelectionWidget      *widget);
void        mdu_disk_selection_widget_set_component_size               (MduDiskSelectionWidget      *widget,
                                                                        guint64                      component_size);
guint       mdu_disk_selection_widget_get_num_available_disks          (MduDiskSelectionWidget      *widget);
guint64     mdu_disk_selection_widget_get_largest_segment_for_all      (MduDiskSelectionWidget      *widget);
guint64     mdu_disk_selection_widget_get_largest_segment_for_selected (MduDiskSelectionWidget      *widget);

G_END_DECLS

#endif  /* __MDU_DISK_SELECTION_WIDGET_H */

