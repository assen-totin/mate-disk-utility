/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-section.h
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
#include <mdu/mdu.h>
#include "mdu-shell.h"

#ifndef MDU_SECTION_H
#define MDU_SECTION_H

#define MDU_TYPE_SECTION           (mdu_section_get_type ())
#define MDU_SECTION(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_SECTION, MduSection))
#define MDU_SECTION_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_SECTION,  MduSectionClass))
#define MDU_IS_SECTION(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_SECTION))
#define MDU_IS_SECTION_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_SECTION))
#define MDU_SECTION_GET_CLASS(o)   (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_SECTION, MduSectionClass))

typedef struct _MduSectionClass       MduSectionClass;
typedef struct _MduSection            MduSection;

struct _MduSectionPrivate;
typedef struct _MduSectionPrivate     MduSectionPrivate;

struct _MduSection
{
        GtkVBox parent;

        /* private */
        MduSectionPrivate *priv;
};

struct _MduSectionClass
{
        GtkVBoxClass parent_class;

        /* virtual table */
        void (*update) (MduSection *section);
};

GType            mdu_section_get_type         (void);
void             mdu_section_update           (MduSection     *section);

MduShell        *mdu_section_get_shell        (MduSection     *section);
MduPresentable  *mdu_section_get_presentable  (MduSection     *section);


#endif /* MDU_SECTION_H */
