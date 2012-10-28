/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-port.h
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

#ifndef __MDU_PORT_H
#define __MDU_PORT_H

#include <unistd.h>
#include <sys/types.h>

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>

G_BEGIN_DECLS

#define MDU_TYPE_PORT           (mdu_port_get_type ())
#define MDU_PORT(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_PORT, MduPort))
#define MDU_PORT_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_PORT,  MduPortClass))
#define MDU_IS_PORT(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_PORT))
#define MDU_IS_PORT_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_PORT))
#define MDU_PORT_GET_CLASS(k)   (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_PORT, MduPortClass))

typedef struct _MduPortClass    MduPortClass;
typedef struct _MduPortPrivate  MduPortPrivate;

struct _MduPort
{
        GObject parent;

        /* private */
        MduPortPrivate *priv;
};

struct _MduPortClass
{
        GObjectClass parent_class;

        /* signals */
        void (*changed)     (MduPort *port);
        void (*removed)     (MduPort *port);
};

GType        mdu_port_get_type              (void);
const char  *mdu_port_get_object_path       (MduPort   *port);
MduPool     *mdu_port_get_pool              (MduPort   *port);

const gchar *mdu_port_get_native_path       (MduPort   *port);
const gchar *mdu_port_get_adapter           (MduPort   *port);
const gchar *mdu_port_get_parent            (MduPort   *port);
gint         mdu_port_get_number            (MduPort   *port);
const gchar *mdu_port_get_connector_type    (MduPort   *port);

G_END_DECLS

#endif /* __MDU_ADAPTER_H */
