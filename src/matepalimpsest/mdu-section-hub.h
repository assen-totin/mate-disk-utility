/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-section-hub.h
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

#include <gtk/gtk.h>
#include "mdu-section.h"

#ifndef MDU_SECTION_HUB_H
#define MDU_SECTION_HUB_H

#define MDU_TYPE_SECTION_HUB           (mdu_section_hub_get_type ())
#define MDU_SECTION_HUB(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_SECTION_HUB, MduSectionHub))
#define MDU_SECTION_HUB_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_SECTION_HUB,  MduSectionHubClass))
#define MDU_IS_SECTION_HUB(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_SECTION_HUB))
#define MDU_IS_SECTION_HUB_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_SECTION_HUB))
#define MDU_SECTION_HUB_GET_CLASS(o)   (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_SECTION_HUB, MduSectionHubClass))

typedef struct _MduSectionHubClass       MduSectionHubClass;
typedef struct _MduSectionHub            MduSectionHub;

struct _MduSectionHubPrivate;
typedef struct _MduSectionHubPrivate     MduSectionHubPrivate;

struct _MduSectionHub
{
        MduSection parent;

        /* private */
        MduSectionHubPrivate *priv;
};

struct _MduSectionHubClass
{
        MduSectionClass parent_class;
};

GType            mdu_section_hub_get_type (void);
GtkWidget       *mdu_section_hub_new      (MduShell       *shell,
                                           MduPresentable *presentable);

#endif /* MDU_SECTION_HUB_H */
