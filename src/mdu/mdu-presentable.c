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

#include "config.h"
#include <glib/gi18n-lib.h>

#include <string.h>
#include <dbus/dbus-glib.h>

#include "mdu-presentable.h"
#include "mdu-pool.h"

/**
 * SECTION:mdu-presentable
 * @title: MduPresentable
 * @short_description: Interface for devices presentable to the end user
 *
 * All storage devices in <literal>UNIX</literal> and
 * <literal>UNIX</literal>-like operating systems are mostly
 * represented by so-called <literal>block</literal> devices at the
 * kernel/user-land interface. UNIX block devices, including
 * information and operations are represented by the #MduDevice class.
 *
 * However, from an user-interface point of view, it's useful to make
 * a finer-grained distinction; for example it's useful to make a
 * distinction between drives (e.g. a phyiscal hard disk, optical
 * drives) and volumes (e.g. a mountable file system).
 *
 * As such, classes encapsulating aspects of a UNIX block device (such
 * as it being drive, volume, empty space) that are interesting to
 * present in the user interface all implement the #MduPresentable
 * interface. This interface provides lowest-common denominator
 * functionality assisting in the creation of user interfaces; name
 * and icons are easily available as well as hierarchical grouping
 * in terms of parent/child relationships. Thus, several classes
 * such as #MduVolume, #MduDrive and others implement the
 * #MduPresentable interface
 *
 * For example, if a device (<literal>/dev/sda</literal>) is
 * partitioned into two partitions (<literal>/dev/sda1</literal> and
 * <literal>/dev/sda2</literal>), the parent/child relation look looks
 * like this
 *
 * <programlisting>
 * MduDrive     (/dev/sda)
 *   MduVolume  (/dev/sda1)
 *   MduVolume  (/dev/sda2)
 * </programlisting>
 *
 * Some partitioning schemes (notably Master Boot Record) have a
 * concept of nested partition tables. Supposed
 * <literal>/dev/sda2</literal> is an extended partition and
 * <literal>/dev/sda5</literal> and <literal>/dev/sda6</literal> are
 * logical partitions:
 *
 * <programlisting>
 * MduDrive       (/dev/sda)
 *   MduVolume    (/dev/sda1)
 *   MduVolume    (/dev/sda2)
 *     MduVolume  (/dev/sda5)
 *     MduVolume  (/dev/sda6)
 * </programlisting>
 *
 * The mdu_presentable_get_offset() function can be used to
 * determine the ordering; this function will return the offset
 * of a #MduPresentable relative to the topmost enclosing device.
 *
 * Now, consider the case where there are "holes", e.g. where
 * there exists one or more regions on the partitioned device
 * not occupied by any partitions. In that case, the #MduPool
 * object will create #MduVolumeHole objects to patch the holes:
 *
 * <programlisting>
 * MduDrive           (/dev/sda)
 *   MduVolume        (/dev/sda1)
 *   MduVolume        (/dev/sda2)
 *     MduVolume      (/dev/sda5)
 *     MduVolumeHole  (no device)
 *     MduVolume      (/dev/sda6)
 *   MduVolumeHole    (no device)
 * </programlisting>
 *
 * Also, some devices are not partitioned. For example, the UNIX
 * block device <literal>/dev/sr0</literal> refers to both the
 * optical drive and (if any medium is present) the contents of
 * the optical disc inserted into the disc. In that case, the
 * following structure will be created:
 *
 * <programlisting>
 * MduDrive     (/dev/sr0)
 *   MduVolume  (/dev/sr0)
 * </programlisting>
 *
 * If no media is available, only a single #MduDrive object will
 * exist:
 *
 * <programlisting>
 * MduDrive     (/dev/sr0)
 * </programlisting>
 *
 * Finally, unlocked LUKS Encrypted devices are represented as
 * children of their encrypted counter parts, for example:
 *
 * <programlisting>
 * MduDrive       (/dev/sda)
 *   MduVolume    (/dev/sda1)
 *   MduVolume    (/dev/sda2)
 *     MduVolume  (/dev/dm-0)
 * </programlisting>
 *
 * Some devices, like RAID and LVM devices, needs to be assembled from
 * components (e.g. "activated" or "started". This is encapsulated in
 * the #MduActivatableDrive class; this is not much different from
 * #MduDrive except that there only is a #MduDevice assoicated with
 * the object when the device itself is started. For example:
 *
 * <programlisting>
 * MduActivatableDrive     (no device)
 * </programlisting>
 *
 * will be created (e.g. synthesized) as soon as the first component
 * of the activatable drive is available. When activated, the
 * #MduActivatableDrive will gain a #MduDevice and the hierarchy looks
 * somewhat like this
 *
 * <programlisting>
 * MduActivatableDrive     (/dev/md0)
 *   MduVolume             (/dev/md0)
 * </programlisting>
 *
 * To sum up, the #MduPresentable interface (and classes implementing
 * it such as #MduDrive and #MduVolume) describe how a drive / medium
 * is organized such that it's easy to compose an user interface. To
 * perform operations on devices, use mdu_presentable_get_device() and
 * the functions on #MduDevice.
 **/


static void mdu_presentable_base_init (gpointer g_class);
static void mdu_presentable_class_init (gpointer g_class,
                                        gpointer class_data);

GType
mdu_presentable_get_type (void)
{
  static GType presentable_type = 0;

  if (! presentable_type)
    {
      static const GTypeInfo presentable_info =
      {
        sizeof (MduPresentableIface), /* class_size */
	mdu_presentable_base_init,   /* base_init */
	NULL,		/* base_finalize */
	mdu_presentable_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
	0,
	0,              /* n_preallocs */
	NULL
      };

      presentable_type =
	g_type_register_static (G_TYPE_INTERFACE, "MduPresentable",
				&presentable_info, 0);

      g_type_interface_add_prerequisite (presentable_type, G_TYPE_OBJECT);
    }

  return presentable_type;
}

static void
mdu_presentable_class_init (gpointer g_class,
                     gpointer class_data)
{
}

static void
mdu_presentable_base_init (gpointer g_class)
{
        static gboolean initialized = FALSE;

        if (! initialized)
        {
                /**
                 * MduPresentable::changed
                 * @presentable: A #MduPresentable.
                 *
                 * Emitted when @presentable changes.
                 **/
                g_signal_new ("changed",
                              MDU_TYPE_PRESENTABLE,
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (MduPresentableIface, changed),
                              NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);

                /**
                 * MduPresentable::job-changed
                 * @presentable: A #MduPresentable.
                 *
                 * Emitted when job status on @presentable changes.
                 **/
                g_signal_new ("job-changed",
                              MDU_TYPE_PRESENTABLE,
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (MduPresentableIface, job_changed),
                              NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);

                /**
                 * MduPresentable::removed
                 * @presentable: The #MduPresentable that was removed.
                 *
                 * Emitted when @presentable is removed. Recipients
                 * should release references to @presentable.
                 **/
                g_signal_new ("removed",
                              MDU_TYPE_PRESENTABLE,
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET (MduPresentableIface, removed),
                              NULL, NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);

                initialized = TRUE;
        }
}

/**
 * mdu_presentable_get_id:
 * @presentable: A #MduPresentable.
 *
 * Gets a stable identifier for @presentable.
 *
 * Returns: An stable identifier for @presentable. Do not free, the string is
 * owned by @presentable.
 **/
const gchar *
mdu_presentable_get_id (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_id) (presentable);
}

/**
 * mdu_presentable_get_device:
 * @presentable: A #MduPresentable.
 *
 * Gets the underlying device for @presentable if one is available.
 *
 * Returns: A #MduDevice or #NULL if there are no underlying device of
 * @presentable. Caller must unref the object when done with it.
 **/
MduDevice *
mdu_presentable_get_device (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_device) (presentable);
}

/**
 * mdu_presentable_get_enclosing_presentable:
 * @presentable: A #MduPresentable.
 *
 * Gets the #MduPresentable that is the parent of @presentable or
 * #NULL if there is no parent.
 *
 * Returns: The #MduPresentable that is a parent of @presentable or
 * #NULL if @presentable is the top-most presentable already. Caller
 * must unref the object.
 **/
MduPresentable *
mdu_presentable_get_enclosing_presentable (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_enclosing_presentable) (presentable);
}

/**
 * mdu_presentable_get_name:
 * @presentable: A #MduPresentable.
 *
 * Gets a name for @presentable suitable for presentation in an user
 * interface.
 *
 * For drives (e.g. #MduDrive) this could be "80G Solid-State Disk",
 * "500G Hard Disk", "CompactFlash Drive", "CD/DVD Drive"
 *
 * For volumes (e.g. #MduVolume) this string could be "Fedora
 * (Rawhide)" (for filesystems with labels), "48G Free" (for
 * unallocated space), "2GB Swap Space" (for swap), "430GB RAID
 * Component" (for RAID components) and so on.
 *
 * Constrast with mdu_presentable_get_vpd_name() that returns a name
 * based on Vital Product Data including things such as the name of
 * the vendor, the model name and so on.
 *
 * Returns: The name. Caller must free the string with g_free().
 **/
gchar *
mdu_presentable_get_name (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_name) (presentable);
}

/**
 * mdu_presentable_get_vpd_name:
 * @presentable: A #MduPresentable.
 *
 * Gets a name for @presentable suitable for presentation in an user
 * interface that includes Vital Product Data details such as the name
 * of the vendor, the model name and so on.
 *
 * For drives (e.g. #MduDrive) this typically includes the
 * vendor/model strings obtained from the hardware, for example
 * "MATSHITA DVD/CDRW UJDA775" or "ATA INTEL SSDSA2MH080G1GC".
 *
 * For volumes (e.g. #MduVolume) this includes information about e.g.
 * partition infomation for example "Whole-disk volume on ATA INTEL
 * SSDSA2MH080G1GC", "Partition 2 of ATA INTEL SSDSA2MH080G1GC".
 *
 * Contrast with mdu_presentable_get_name() that may not include this
 * information.
 *
 * Returns: The name. Caller must free the string with g_free().
 **/
gchar *
mdu_presentable_get_vpd_name (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_vpd_name) (presentable);
}

/**
 * mdu_presentable_get_description:
 * @presentable: A #MduPresentable.
 *
 * Gets a description for @presentable suitable for presentation in an user
 * interface.
 *
 * Returns: The description. Caller must free the string with g_free().
 */
gchar *
mdu_presentable_get_description (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_description) (presentable);
}

/**
 * mdu_presentable_get_icon:
 * @presentable: A #MduPresentable.
 *
 * Gets a #GIcon suitable for display in an user interface.
 *
 * Returns: The icon. Caller must free this with g_object_unref().
 **/
GIcon *
mdu_presentable_get_icon (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_icon) (presentable);
}

/**
 * mdu_presentable_get_offset:
 * @presentable: A #MduPresentable.
 *
 * Gets where the data represented by the presentable starts on the
 * underlying main block device
 *
 * Returns: Offset of @presentable or 0 if @presentable has no underlying device.
 **/
guint64
mdu_presentable_get_offset (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), 0);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_offset) (presentable);
}

/**
 * mdu_presentable_get_size:
 * @presentable: A #MduPresentable.
 *
 * Gets the size of @presentable.
 *
 * Returns: The size of @presentable or 0 if @presentable has no underlying device.
 **/
guint64
mdu_presentable_get_size (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), 0);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_size) (presentable);
}

/**
 * mdu_presentable_get_pool:
 * @presentable: A #MduPresentable.
 *
 * Gets the #MduPool that @presentable stems from.
 *
 * Returns: A #MduPool. Caller must unref object when done with it.
 **/
MduPool *
mdu_presentable_get_pool (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), NULL);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->get_pool) (presentable);
}

/**
 * mdu_presentable_is_allocated:
 * @presentable: A #MduPresentable.
 *
 * Determines if @presentable represents an underlying block device with data.
 *
 * Returns: Whether @presentable is allocated.
 **/
gboolean
mdu_presentable_is_allocated (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), FALSE);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->is_allocated) (presentable);
}

/**
 * mdu_presentable_is_recognized:
 * @presentable: A #MduPresentable.
 *
 * Gets whether the contents of @presentable are recognized; e.g. if
 * it's a file system, encrypted data or swap space.
 *
 * Returns: Whether @presentable is recognized.
 **/
gboolean
mdu_presentable_is_recognized (MduPresentable *presentable)
{
  MduPresentableIface *iface;

  g_return_val_if_fail (MDU_IS_PRESENTABLE (presentable), FALSE);

  iface = MDU_PRESENTABLE_GET_IFACE (presentable);

  return (* iface->is_recognized) (presentable);
}

/* ---------------------------------------------------------------------------------------------------- */

/**
 * mdu_presentable_get_toplevel:
 * @presentable: A #MduPresentable.
 *
 * Gets the top-level presentable for a given presentable.
 *
 * Returns: A #MduPresentable or #NULL if @presentable is the top-most presentable. Caller must
 * unref the object when done with it
 **/
MduPresentable *
mdu_presentable_get_toplevel (MduPresentable *presentable)
{
        MduPresentable *parent;
        MduPresentable *maybe_parent;

        parent = presentable;
        do {
                maybe_parent = mdu_presentable_get_enclosing_presentable (parent);
                if (maybe_parent != NULL) {
                        g_object_unref (maybe_parent);
                        parent = maybe_parent;
                }
        } while (maybe_parent != NULL);

        return g_object_ref (parent);
}

/* ---------------------------------------------------------------------------------------------------- */
guint
mdu_presentable_hash (MduPresentable *presentable)
{
        return g_str_hash (mdu_presentable_get_id (presentable));
}

gboolean
mdu_presentable_equals (MduPresentable *a,
                        MduPresentable *b)
{
        return g_strcmp0 (mdu_presentable_get_id (a), mdu_presentable_get_id (b)) == 0;
}

static void
compute_sort_path (MduPresentable *presentable,
                   GString        *s)
{
        MduPresentable *enclosing_presentable;

        enclosing_presentable = mdu_presentable_get_enclosing_presentable (presentable);
        if (enclosing_presentable != NULL) {
                compute_sort_path (enclosing_presentable, s);
                g_object_unref (enclosing_presentable);
        }

        g_string_append_printf (s, "_%s", mdu_presentable_get_id (presentable));
}

gint
mdu_presentable_compare (MduPresentable *a,
                         MduPresentable *b)
{
        GString *sort_a;
        GString *sort_b;
        gint ret;

        sort_a = g_string_new (NULL);
        sort_b = g_string_new (NULL);
        compute_sort_path (a, sort_a);
        compute_sort_path (b, sort_b);
        ret = strcmp (sort_a->str, sort_b->str);
        g_string_free (sort_a, TRUE);
        g_string_free (sort_b, TRUE);

        return ret;
}

GList *
mdu_presentable_get_enclosed (MduPresentable *presentable)
{
        GList *l;
        GList *presentables;
        GList *ret;
        MduPool *pool;

        pool = mdu_presentable_get_pool (presentable);
        presentables = mdu_pool_get_presentables (pool);

        ret = NULL;
        for (l = presentables; l != NULL; l = l->next) {
                MduPresentable *p = l->data;
                MduPresentable *e;

                e = mdu_presentable_get_enclosing_presentable (p);
                if (e != NULL) {
                        if (mdu_presentable_equals (e, presentable)) {
                                GList *enclosed_by_p;

                                ret = g_list_prepend (ret, g_object_ref (p));

                                enclosed_by_p = mdu_presentable_get_enclosed (p);
                                ret = g_list_concat (ret, enclosed_by_p);
                        }
                        g_object_unref (e);
                }
        }

        g_list_foreach (presentables, (GFunc) g_object_unref, NULL);
        g_list_free (presentables);
        g_object_unref (pool);
        return ret;
}

gboolean
mdu_presentable_encloses (MduPresentable *a,
                          MduPresentable *b)
{
        GList *enclosed_by_a;
        GList *l;
        gboolean ret;

        ret = FALSE;
        enclosed_by_a = mdu_presentable_get_enclosed (a);
        for (l = enclosed_by_a; l != NULL; l = l->next) {
                MduPresentable *p = MDU_PRESENTABLE (l->data);
                if (mdu_presentable_equals (b, p)) {
                        ret = TRUE;
                        break;
                }
        }
        g_list_foreach (enclosed_by_a, (GFunc) g_object_unref, NULL);
        g_list_free (enclosed_by_a);

        return ret;
}

