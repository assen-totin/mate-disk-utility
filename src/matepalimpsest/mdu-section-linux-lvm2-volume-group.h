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

#ifndef MDU_SECTION_LINUX_LVM2_VOLUME_GROUP_H
#define MDU_SECTION_LINUX_LVM2_VOLUME_GROUP_H

#define MDU_TYPE_SECTION_LINUX_LVM2_VOLUME_GROUP             (mdu_section_linux_lvm2_volume_group_get_type ())
#define MDU_SECTION_LINUX_LVM2_VOLUME_GROUP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_SECTION_LINUX_LVM2_VOLUME_GROUP, MduSectionLinuxLvm2VolumeGroup))
#define MDU_SECTION_LINUX_LVM2_VOLUME_GROUP_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), MDU_SECTION_LINUX_LVM2_VOLUME_GROUP,  MduSectionLinuxLvm2VolumeGroupClass))
#define MDU_IS_SECTION_LINUX_LVM2_VOLUME_GROUP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_SECTION_LINUX_LVM2_VOLUME_GROUP))
#define MDU_IS_SECTION_LINUX_LVM2_VOLUME_GROUP_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), MDU_TYPE_SECTION_LINUX_LVM2_VOLUME_GROUP))
#define MDU_SECTION_LINUX_LVM2_VOLUME_GROUP_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_SECTION_LINUX_LVM2_VOLUME_GROUP, MduSectionLinuxLvm2VolumeGroupClass))

typedef struct _MduSectionLinuxLvm2VolumeGroupClass       MduSectionLinuxLvm2VolumeGroupClass;
typedef struct _MduSectionLinuxLvm2VolumeGroup            MduSectionLinuxLvm2VolumeGroup;

struct _MduSectionLinuxLvm2VolumeGroupPrivate;
typedef struct _MduSectionLinuxLvm2VolumeGroupPrivate     MduSectionLinuxLvm2VolumeGroupPrivate;

struct _MduSectionLinuxLvm2VolumeGroup
{
        MduSection parent;

        /* private */
        MduSectionLinuxLvm2VolumeGroupPrivate *priv;
};

struct _MduSectionLinuxLvm2VolumeGroupClass
{
        MduSectionClass parent_class;
};

GType            mdu_section_linux_lvm2_volume_group_get_type (void);
GtkWidget       *mdu_section_linux_lvm2_volume_group_new      (MduShell       *shell,
                                                               MduPresentable *presentable);

#endif /* MDU_SECTION_LINUX_LVM2_VOLUME_GROUP_H */
