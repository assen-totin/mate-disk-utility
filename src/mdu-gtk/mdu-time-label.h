/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-time-label.h
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

#if !defined (__MDU_GTK_INSIDE_MDU_GTK_H) && !defined (MDU_GTK_COMPILATION)
#error "Only <mdu-gtk/mdu-gtk.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef MDU_TIME_LABEL_H
#define MDU_TIME_LABEL_H

#include <mdu-gtk/mdu-gtk-types.h>

#define MDU_TYPE_TIME_LABEL             (mdu_time_label_get_type ())
#define MDU_TIME_LABEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_TIME_LABEL, MduTimeLabel))
#define MDU_TIME_LABEL_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), MDU_TIME_LABEL,  MduTimeLabelClass))
#define MDU_IS_TIME_LABEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_TIME_LABEL))
#define MDU_IS_TIME_LABEL_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), MDU_TYPE_TIME_LABEL))
#define MDU_TIME_LABEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_TIME_LABEL, MduTimeLabelClass))

typedef struct MduTimeLabelClass       MduTimeLabelClass;
typedef struct MduTimeLabelPrivate     MduTimeLabelPrivate;

struct MduTimeLabel
{
        GtkLabel parent;

        /* private */
        MduTimeLabelPrivate *priv;
};

struct MduTimeLabelClass
{
        GtkLabelClass parent_class;
};


GType      mdu_time_label_get_type     (void);
GtkWidget *mdu_time_label_new          (GTimeVal     *time);
void       mdu_time_label_set_time     (MduTimeLabel *time_label,
                                        GTimeVal     *time);

#endif /* MDU_TIME_LABEL_H */
