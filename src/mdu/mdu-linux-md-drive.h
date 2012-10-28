/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-linux-md-drive.h
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

#if !defined (__MDU_INSIDE_MDU_H) && !defined (MDU_COMPILATION)
#error "Only <mdu/mdu.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __MDU_LINUX_MD_DRIVE_H
#define __MDU_LINUX_MD_DRIVE_H

#include <mdu/mdu-types.h>
#include <mdu/mdu-callbacks.h>
#include <mdu/mdu-drive.h>

G_BEGIN_DECLS

#define MDU_TYPE_LINUX_MD_DRIVE         (mdu_linux_md_drive_get_type ())
#define MDU_LINUX_MD_DRIVE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_LINUX_MD_DRIVE, MduLinuxMdDrive))
#define MDU_LINUX_MD_DRIVE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_LINUX_MD_DRIVE,  MduLinuxMdDriveClass))
#define MDU_IS_LINUX_MD_DRIVE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_LINUX_MD_DRIVE))
#define MDU_IS_LINUX_MD_DRIVE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_LINUX_MD_DRIVE))
#define MDU_LINUX_MD_DRIVE_GET_CLASS(k) (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_LINUX_MD_DRIVE, MduLinuxMdDriveClass))

typedef struct _MduLinuxMdDriveClass    MduLinuxMdDriveClass;
typedef struct _MduLinuxMdDrivePrivate  MduLinuxMdDrivePrivate;

struct _MduLinuxMdDrive
{
        MduDrive parent;

        /*< private >*/
        MduLinuxMdDrivePrivate *priv;
};

struct _MduLinuxMdDriveClass
{
        MduDriveClass parent_class;
};

/**
 * MduLinuxMdDriveSlaveFlags:
 * @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_NONE: No flags are set.
 * @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_NOT_ATTACHED: If set, the slave is
 * not part of the array but appears as a child only because the UUID
 * on the device matches that of the array. Is also set if the array
 * does not exist.
 * @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_FAULTY: Device has been kick from
 * active use due to a detected fault.
 * @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_IN_SYNC: Device is a fully in-sync
 * member of the array.
 * @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_WRITEMOSTLY: Device will only be
 * subject to read requests if there are no other options. This
 * applies only to RAID1 arrays.
 * @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_BLOCKED: Device has failed,
 * metadata is "external", and the failure hasn't been acknowledged
 * yet. Writes that would write to this device if it were not faulty
 * are blocked.
 * @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_SPARE: Device is working, but not a
 * full member. This includes spares that in the process of being
 * recovered to.
 *
 * State for slaves of an Linux MD software raid drive. Everything but @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_NONE
 * and @MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_NOT_ATTACHED corresponds to the comma-separated strings in
 * <literal>/sys/block/mdXXX/md/dev-YYY/state</literal> in sysfs. See Documentation/md.txt in the Linux
 * kernel for more information.
 **/
typedef enum {
        MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_NONE          = 0,
        MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_NOT_ATTACHED  = (1<<0),
        MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_FAULTY        = (1<<1),
        MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_IN_SYNC       = (1<<2),
        MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_WRITEMOSTLY   = (1<<3),
        MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_BLOCKED       = (1<<4),
        MDU_LINUX_MD_DRIVE_SLAVE_FLAGS_SPARE         = (1<<5),
} MduLinuxMdDriveSlaveFlags;

GType                      mdu_linux_md_drive_get_type               (void);
const gchar               *mdu_linux_md_drive_get_uuid               (MduLinuxMdDrive  *drive);
gboolean                   mdu_linux_md_drive_has_slave              (MduLinuxMdDrive  *drive,
                                                                      MduDevice        *device);
GList                     *mdu_linux_md_drive_get_slaves             (MduLinuxMdDrive  *drive);
MduLinuxMdDriveSlaveFlags  mdu_linux_md_drive_get_slave_flags        (MduLinuxMdDrive  *drive,
                                                                      MduDevice        *slave);
gchar                     *mdu_linux_md_drive_get_slave_state_markup (MduLinuxMdDrive  *drive,
                                                                      MduDevice        *slave);

G_END_DECLS

#endif /* __MDU_LINUX_MD_DRIVE_H */
