/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-expander.h
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

#ifndef __MDU_EXPANDER_H
#define __MDU_EXPANDER_H

#include <unistd.h>
#include <sys/types.h>

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>

G_BEGIN_DECLS

#define MDU_TYPE_EXPANDER           (mdu_expander_get_type ())
#define MDU_EXPANDER(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_EXPANDER, MduExpander))
#define MDU_EXPANDER_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_EXPANDER,  MduExpanderClass))
#define MDU_IS_EXPANDER(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_EXPANDER))
#define MDU_IS_EXPANDER_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_EXPANDER))
#define MDU_EXPANDER_GET_CLASS(k)   (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_EXPANDER, MduExpanderClass))

typedef struct _MduExpanderClass    MduExpanderClass;
typedef struct _MduExpanderPrivate  MduExpanderPrivate;

struct _MduExpander
{
        GObject parent;

        /* private */
        MduExpanderPrivate *priv;
};

struct _MduExpanderClass
{
        GObjectClass parent_class;

        /* signals */
        void (*changed)     (MduExpander *expander);
        void (*removed)     (MduExpander *expander);
};

GType        mdu_expander_get_type              (void);
const char  *mdu_expander_get_object_path       (MduExpander   *expander);
MduPool     *mdu_expander_get_pool              (MduExpander   *expander);

const gchar *mdu_expander_get_native_path       (MduExpander   *expander);
const gchar *mdu_expander_get_vendor            (MduExpander   *expander);
const gchar *mdu_expander_get_model             (MduExpander   *expander);
const gchar *mdu_expander_get_revision          (MduExpander   *expander);
guint        mdu_expander_get_num_ports         (MduExpander   *expander);
gchar      **mdu_expander_get_upstream_ports    (MduExpander   *expander);
const gchar *mdu_expander_get_adapter           (MduExpander   *expander);

G_END_DECLS

#endif /* __MDU_EXPANDER_H */
