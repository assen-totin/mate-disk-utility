/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-known-filesystem.h
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

#ifndef __MDU_KNOWN_FILESYSTEM_H
#define __MDU_KNOWN_FILESYSTEM_H

#include <mdu/mdu-types.h>

G_BEGIN_DECLS

#define MDU_TYPE_KNOWN_FILESYSTEM         (mdu_known_filesystem_get_type ())
#define MDU_KNOWN_FILESYSTEM(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_KNOWN_FILESYSTEM, MduKnownFilesystem))
#define MDU_KNOWN_FILESYSTEM_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_KNOWN_FILESYSTEM,  MduKnownFilesystemClass))
#define MDU_IS_KNOWN_FILESYSTEM(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_KNOWN_FILESYSTEM))
#define MDU_IS_KNOWN_FILESYSTEM_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_KNOWN_FILESYSTEM))
#define MDU_KNOWN_FILESYSTEM_GET_CLASS(k) (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_KNOWN_FILESYSTEM, MduKnownFilesystemClass))

typedef struct _MduKnownFilesystemClass       MduKnownFilesystemClass;
typedef struct _MduKnownFilesystemPrivate     MduKnownFilesystemPrivate;

struct _MduKnownFilesystem
{
        GObject parent;

        /* private */
        MduKnownFilesystemPrivate *priv;
};

struct _MduKnownFilesystemClass
{
        GObjectClass parent_class;
};

GType       mdu_known_filesystem_get_type                           (void);
const char *mdu_known_filesystem_get_id                             (MduKnownFilesystem *known_filesystem);
const char *mdu_known_filesystem_get_name                           (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_unix_owners           (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_can_mount                      (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_can_create                     (MduKnownFilesystem *known_filesystem);
guint       mdu_known_filesystem_get_max_label_len                  (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_label_rename          (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_online_label_rename   (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_fsck                  (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_online_fsck           (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_resize_enlarge        (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_online_resize_enlarge (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_resize_shrink         (MduKnownFilesystem *known_filesystem);
gboolean    mdu_known_filesystem_get_supports_online_resize_shrink  (MduKnownFilesystem *known_filesystem);

G_END_DECLS

#endif /* __MDU_KNOWN_FILESYSTEM_H */
