/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-adapter.h
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

#if !defined (__MDU_INSIDE_MDU_H) && !defined (MDU_COMPILATION)
#error "Only <mdu/mdu.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_ADAPTER_H
#define __MDU_ADAPTER_H

#include <unistd.h>
#include <sys/types.h>

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>

G_BEGIN_DECLS

#define MDU_TYPE_ADAPTER           (mdu_adapter_get_type ())
#define MDU_ADAPTER(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_ADAPTER, MduAdapter))
#define MDU_ADAPTER_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_ADAPTER,  MduAdapterClass))
#define MDU_IS_ADAPTER(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_ADAPTER))
#define MDU_IS_ADAPTER_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_ADAPTER))
#define MDU_ADAPTER_GET_CLASS(k)   (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_ADAPTER, MduAdapterClass))

typedef struct _MduAdapterClass    MduAdapterClass;
typedef struct _MduAdapterPrivate  MduAdapterPrivate;

struct _MduAdapter
{
        GObject parent;

        /* private */
        MduAdapterPrivate *priv;
};

struct _MduAdapterClass
{
        GObjectClass parent_class;

        /* signals */
        void (*changed)     (MduAdapter *adapter);
        void (*removed)     (MduAdapter *adapter);
};

GType        mdu_adapter_get_type              (void);
const char  *mdu_adapter_get_object_path       (MduAdapter   *adapter);
MduPool     *mdu_adapter_get_pool              (MduAdapter   *adapter);

const gchar *mdu_adapter_get_native_path       (MduAdapter   *adapter);
const gchar *mdu_adapter_get_vendor            (MduAdapter   *adapter);
const gchar *mdu_adapter_get_model             (MduAdapter   *adapter);
const gchar *mdu_adapter_get_driver            (MduAdapter   *adapter);
const gchar *mdu_adapter_get_fabric            (MduAdapter   *adapter);
guint        mdu_adapter_get_num_ports         (MduAdapter   *adapter);

G_END_DECLS

#endif /* __MDU_ADAPTER_H */
