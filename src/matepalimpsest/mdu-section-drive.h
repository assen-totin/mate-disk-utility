/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-section-drive.h
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

#ifndef MDU_SECTION_DRIVE_H
#define MDU_SECTION_DRIVE_H

#define MDU_TYPE_SECTION_DRIVE           (mdu_section_drive_get_type ())
#define MDU_SECTION_DRIVE(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_SECTION_DRIVE, MduSectionDrive))
#define MDU_SECTION_DRIVE_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_SECTION_DRIVE,  MduSectionDriveClass))
#define MDU_IS_SECTION_DRIVE(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_SECTION_DRIVE))
#define MDU_IS_SECTION_DRIVE_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_SECTION_DRIVE))
#define MDU_SECTION_DRIVE_GET_CLASS(o)   (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_SECTION_DRIVE, MduSectionDriveClass))

typedef struct _MduSectionDriveClass       MduSectionDriveClass;
typedef struct _MduSectionDrive            MduSectionDrive;

struct _MduSectionDrivePrivate;
typedef struct _MduSectionDrivePrivate     MduSectionDrivePrivate;

struct _MduSectionDrive
{
        MduSection parent;

        /* private */
        MduSectionDrivePrivate *priv;
};

struct _MduSectionDriveClass
{
        MduSectionClass parent_class;
};

GType            mdu_section_drive_get_type (void);
GtkWidget       *mdu_section_drive_new      (MduShell       *shell,
                                             MduPresentable *presentable);

/* these functions are exported for use in MduSectionLinuxMd and other sections - user_data must
 * be a MduSection instance
 */
void
mdu_section_drive_on_format_button_clicked (MduButtonElement *button_element,
                                            gpointer          user_data);
void
mdu_section_drive_on_benchmark_button_clicked (MduButtonElement *button_element,
                                               gpointer          user_data);


#endif /* MDU_SECTION_DRIVE_H */
