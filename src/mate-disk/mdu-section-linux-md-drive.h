/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-section-linux-md-drive.h
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

#ifndef MDU_SECTION_LINUX_MD_DRIVE_H
#define MDU_SECTION_LINUX_MD_DRIVE_H

#define MDU_TYPE_SECTION_LINUX_MD_DRIVE             (mdu_section_linux_md_drive_get_type ())
#define MDU_SECTION_LINUX_MD_DRIVE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_SECTION_LINUX_MD_DRIVE, MduSectionLinuxMdDrive))
#define MDU_SECTION_LINUX_MD_DRIVE_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), MDU_SECTION_LINUX_MD_DRIVE,  MduSectionLinuxMdDriveClass))
#define MDU_IS_SECTION_LINUX_MD_DRIVE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_SECTION_LINUX_MD_DRIVE))
#define MDU_IS_SECTION_LINUX_MD_DRIVE_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), MDU_TYPE_SECTION_LINUX_MD_DRIVE))
#define MDU_SECTION_LINUX_MD_DRIVE_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_SECTION_LINUX_MD_DRIVE, MduSectionLinuxMdDriveClass))

typedef struct _MduSectionLinuxMdDriveClass       MduSectionLinuxMdDriveClass;
typedef struct _MduSectionLinuxMdDrive            MduSectionLinuxMdDrive;

struct _MduSectionLinuxMdDrivePrivate;
typedef struct _MduSectionLinuxMdDrivePrivate     MduSectionLinuxMdDrivePrivate;

struct _MduSectionLinuxMdDrive
{
        MduSection parent;

        /* private */
        MduSectionLinuxMdDrivePrivate *priv;
};

struct _MduSectionLinuxMdDriveClass
{
        MduSectionClass parent_class;
};

GType            mdu_section_linux_md_drive_get_type (void);
GtkWidget       *mdu_section_linux_md_drive_new      (MduShell       *shell,
                                                         MduPresentable *presentable);

#endif /* MDU_SECTION_LINUX_MD_DRIVE_H */
