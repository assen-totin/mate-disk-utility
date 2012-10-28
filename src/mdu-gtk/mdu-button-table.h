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

#ifndef __MDU_BUTTON_TABLE_H
#define __MDU_BUTTON_TABLE_H

#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_BUTTON_TABLE         (mdu_button_table_get_type())
#define MDU_BUTTON_TABLE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_BUTTON_TABLE, MduButtonTable))
#define MDU_BUTTON_TABLE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_BUTTON_TABLE, MduButtonTableClass))
#define MDU_IS_BUTTON_TABLE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_BUTTON_TABLE))
#define MDU_IS_BUTTON_TABLE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_BUTTON_TABLE))
#define MDU_BUTTON_TABLE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_BUTTON_TABLE, MduButtonTableClass))

typedef struct MduButtonTableClass   MduButtonTableClass;
typedef struct MduButtonTablePrivate MduButtonTablePrivate;

struct MduButtonTable
{
        GtkHBox parent;

        /*< private >*/
        MduButtonTablePrivate *priv;
};

struct MduButtonTableClass
{
        GtkHBoxClass parent_class;
};

GType       mdu_button_table_get_type        (void) G_GNUC_CONST;
GtkWidget*  mdu_button_table_new             (guint            num_columns,
                                              GPtrArray       *elements);
guint       mdu_button_table_get_num_columns (MduButtonTable *table);
GPtrArray  *mdu_button_table_get_elements    (MduButtonTable *table);
void        mdu_button_table_set_num_columns (MduButtonTable *table,
                                              guint            num_columns);
void        mdu_button_table_set_elements    (MduButtonTable *table,
                                              GPtrArray       *elements);

G_END_DECLS

#endif  /* __MDU_BUTTON_TABLE_H */

