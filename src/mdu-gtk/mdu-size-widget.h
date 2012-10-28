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

#ifndef __MDU_SIZE_WIDGET_H
#define __MDU_SIZE_WIDGET_H

#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_SIZE_WIDGET         (mdu_size_widget_get_type())
#define MDU_SIZE_WIDGET(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_SIZE_WIDGET, MduSizeWidget))
#define MDU_SIZE_WIDGET_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_SIZE_WIDGET, MduSizeWidgetClass))
#define MDU_IS_SIZE_WIDGET(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_SIZE_WIDGET))
#define MDU_IS_SIZE_WIDGET_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_SIZE_WIDGET))
#define MDU_SIZE_WIDGET_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_SIZE_WIDGET, MduSizeWidgetClass))

typedef struct MduSizeWidgetClass   MduSizeWidgetClass;
typedef struct MduSizeWidgetPrivate MduSizeWidgetPrivate;

struct MduSizeWidget
{
        GtkHBox parent;

        /*< private >*/
        MduSizeWidgetPrivate *priv;
};

struct MduSizeWidgetClass
{
        GtkHBoxClass parent_class;

        void (*changed) (MduSizeWidget *widget);
};

GType       mdu_size_widget_get_type     (void) G_GNUC_CONST;
GtkWidget*  mdu_size_widget_new          (guint64 size,
                                          guint64 min_size,
                                          guint64 max_size);
void        mdu_size_widget_set_size     (MduSizeWidget *widget,
                                          guint64        size);
void        mdu_size_widget_set_min_size (MduSizeWidget *widget,
                                          guint64        min_size);
void        mdu_size_widget_set_max_size (MduSizeWidget *widget,
                                          guint64        max_size);
guint64     mdu_size_widget_get_size     (MduSizeWidget *widget);
guint64     mdu_size_widget_get_min_size (MduSizeWidget *widget);
guint64     mdu_size_widget_get_max_size (MduSizeWidget *widget);

G_END_DECLS

#endif  /* __MDU_SIZE_WIDGET_H */

