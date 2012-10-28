/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-presentable.h
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

#ifndef __MDU_PRESENTABLE_H
#define __MDU_PRESENTABLE_H

#include <mdu/mdu-types.h>

G_BEGIN_DECLS

#define MDU_TYPE_PRESENTABLE         (mdu_presentable_get_type ())
#define MDU_PRESENTABLE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_PRESENTABLE, MduPresentable))
#define MDU_IS_PRESENTABLE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_PRESENTABLE))
#define MDU_PRESENTABLE_GET_IFACE(o) (G_TYPE_INSTANCE_GET_INTERFACE ((o), MDU_TYPE_PRESENTABLE, MduPresentableIface))

typedef struct _MduPresentableIface    MduPresentableIface;

/**
 * MduPresentableIface:
 * @g_iface: The parent interface.
 * @changed: Signal emitted when the presentable is changed.
 * @removed: Signal emitted when the presentable is removed. Recipients should release all references to the object.
 * @job_changed: Signal emitted when the job state on the underlying #MduDevice changes.
 * @get_id: Returns a unique id for the presentable.
 * @get_device: Returns the underlying #MduDevice.
 * @get_enclosing_presentable: Returns the #MduPresentable that is the parent or #NULL if there is no parent.
 * @get_name: Returns a name for the presentable suitable for presentation in an user interface.
 * @get_description: Returns a description of the presentable suitable for presentation in an user interface.
 * @get_vpd_name: Returns a name for the presentable suitable for UI that includes Vital Product Pata.
 * @get_icon: Returns an icon suitable for display in an user interface.
 * @get_offset: Returns where the data represented by the presentable starts on the underlying main block device.
 * @get_size: Returns the size of the presentable or zero if not allocated.
 * @get_pool: Returns the #MduPool object that the presentable was obtained from.
 * @is_allocated: Returns whether the presentable is allocated or whether it represents free space.
 * @is_recognized: Returns whether the contents of the presentable are recognized (e.g. well-known file system type).
 *
 * Interface for #MduPresentable implementations.
 */
struct _MduPresentableIface
{
        GTypeInterface g_iface;

        /* signals */
        void (*changed)     (MduPresentable *presentable);
        void (*removed)     (MduPresentable *presentable);
        void (*job_changed) (MduPresentable *presentable);

        /* virtual table */
        const gchar *    (*get_id)                    (MduPresentable *presentable);
        MduDevice *      (*get_device)                (MduPresentable *presentable);
        MduPresentable * (*get_enclosing_presentable) (MduPresentable *presentable);
        gchar *          (*get_name)                  (MduPresentable *presentable);
        gchar *          (*get_description)           (MduPresentable *presentable);
        gchar *          (*get_vpd_name)              (MduPresentable *presentable);
        GIcon *          (*get_icon)                  (MduPresentable *presentable);
        guint64          (*get_offset)                (MduPresentable *presentable);
        guint64          (*get_size)                  (MduPresentable *presentable);
        MduPool *        (*get_pool)                  (MduPresentable *presentable);
        gboolean         (*is_allocated)              (MduPresentable *presentable);
        gboolean         (*is_recognized)             (MduPresentable *presentable);
};

GType           mdu_presentable_get_type                  (void) G_GNUC_CONST;
const gchar    *mdu_presentable_get_id                    (MduPresentable *presentable);
MduDevice      *mdu_presentable_get_device                (MduPresentable *presentable);
MduPresentable *mdu_presentable_get_enclosing_presentable (MduPresentable *presentable);
gchar          *mdu_presentable_get_name                  (MduPresentable *presentable);
gchar          *mdu_presentable_get_description           (MduPresentable *presentable);
gchar          *mdu_presentable_get_vpd_name              (MduPresentable *presentable);
GIcon          *mdu_presentable_get_icon                  (MduPresentable *presentable);
guint64         mdu_presentable_get_offset                (MduPresentable *presentable);
guint64         mdu_presentable_get_size                  (MduPresentable *presentable);
MduPool        *mdu_presentable_get_pool                  (MduPresentable *presentable);
gboolean        mdu_presentable_is_allocated              (MduPresentable *presentable);
gboolean        mdu_presentable_is_recognized             (MduPresentable *presentable);

MduPresentable *mdu_presentable_get_toplevel              (MduPresentable *presentable);
guint           mdu_presentable_hash                      (MduPresentable *presentable);
gboolean        mdu_presentable_equals                    (MduPresentable *a,
                                                           MduPresentable *b);
gint            mdu_presentable_compare                   (MduPresentable *a,
                                                           MduPresentable *b);

GList          *mdu_presentable_get_enclosed              (MduPresentable *presentable);
gboolean        mdu_presentable_encloses                  (MduPresentable *a,
                                                           MduPresentable *b);


G_END_DECLS

#endif /* __MDU_PRESENTABLE_H */
