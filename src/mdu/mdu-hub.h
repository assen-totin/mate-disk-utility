/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-hub.h
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

#ifndef __MDU_HUB_H
#define __MDU_HUB_H

#include <mdu/mdu-types.h>

G_BEGIN_DECLS

#define MDU_TYPE_HUB           (mdu_hub_get_type ())
#define MDU_HUB(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_HUB, MduHub))
#define MDU_HUB_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_HUB,  MduHubClass))
#define MDU_IS_HUB(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_HUB))
#define MDU_IS_HUB_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_HUB))
#define MDU_HUB_GET_CLASS(k)   (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_HUB, MduHubClass))

typedef struct _MduHubClass       MduHubClass;
typedef struct _MduHubPrivate     MduHubPrivate;

struct _MduHub
{
        GObject parent;

        /* private */
        MduHubPrivate *priv;
};

struct _MduHubClass
{
        GObjectClass parent_class;
};

GType        mdu_hub_get_type     (void);
MduAdapter  *mdu_hub_get_adapter  (MduHub *hub);
MduExpander *mdu_hub_get_expander (MduHub *hub);
MduHubUsage  mdu_hub_get_usage    (MduHub *hub);

G_END_DECLS

#endif /* __MDU_HUB_H */
