/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-gtk-enums.h
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

#if !defined (__MDU_GTK_INSIDE_MDU_GTK_H) && !defined (MDU_GTK_COMPILATION)
#error "Only <mdu-gtk/mdu-gtk.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef MDU_GTK_ENUMS_H
#define MDU_GTK_ENUMS_H

#include <glib-object.h>

/**
 * MduPoolTreeModelColumn:
 * @MDU_POOL_TREE_MODEL_COLUMN_ICON: The #GIcon for the presentable.
 * @MDU_POOL_TREE_MODEL_COLUMN_NAME: Human readable name of the presentable, e.g. "80 GB Solid-state Disk" or
 * "Fedora (Rawhide)".
 * @MDU_POOL_TREE_MODEL_COLUMN_VPD_NAME: Name for the presentable derived from Vital Product Data,
 * e.g. "ATA INTEL SSDSA2MH080G1GC".
 * @MDU_POOL_TREE_MODEL_COLUMN_DESCRIPTION: Human readable description of the presentable, e.g. "MBR Partition Table"
 * or "32GB Linux ext3".
 * @MDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE: The #MduPresentable object.
 * @MDU_POOL_TREE_MODEL_COLUMN_VISIBLE: Whether the item is visible.
 * @MDU_POOL_TREE_MODEL_COLUMN_TOGGLED: Whether the item is toggled.
 * @MDU_POOL_TREE_MODEL_COLUMN_CAN_BE_TOGGLED: Whether the item can be toggled.
 *
 * Columns used in #MduPoolTreeModel.
 */
typedef enum {
        MDU_POOL_TREE_MODEL_COLUMN_ICON,
        MDU_POOL_TREE_MODEL_COLUMN_NAME,
        MDU_POOL_TREE_MODEL_COLUMN_VPD_NAME,
        MDU_POOL_TREE_MODEL_COLUMN_DESCRIPTION,
        MDU_POOL_TREE_MODEL_COLUMN_PRESENTABLE,
        MDU_POOL_TREE_MODEL_COLUMN_VISIBLE,
        MDU_POOL_TREE_MODEL_COLUMN_TOGGLED,
        MDU_POOL_TREE_MODEL_COLUMN_CAN_BE_TOGGLED,
} MduPoolTreeModelColumn;

typedef enum {
        MDU_POOL_TREE_VIEW_FLAGS_NONE        = 0,
        MDU_POOL_TREE_VIEW_FLAGS_SHOW_TOGGLE = (1<<0),
} MduPoolTreeViewFlags;

/**
 * MduPoolTreeModelFlags:
 * @MDU_POOL_TREE_MODEL_FLAGS_NONE: No flags set.
 * @MDU_POOL_TREE_MODEL_FLAGS_NO_VOLUMES: Don't include presentables representing volumes or holes.
 * @MDU_POOL_TREE_MODEL_FLAGS_NO_UNALLOCATABLE_DRIVES: Don't include drives that cannot be allocated to e.g. a RAID array.
 *
 * Flags used when creating a #MduPoolTreeModel.
 */
typedef enum {
        MDU_POOL_TREE_MODEL_FLAGS_NONE                      = 0,
        MDU_POOL_TREE_MODEL_FLAGS_NO_VOLUMES                = (1<<0),
        MDU_POOL_TREE_MODEL_FLAGS_NO_UNALLOCATABLE_DRIVES   = (1<<2),
} MduPoolTreeModelFlags;

/**
 * MduFormatDialogFlags:
 * @MDU_FORMAT_DIALOG_FLAGS_NONE: No flags set.
 * @MDU_FORMAT_DIALOG_FLAGS_SIMPLE: Show a simple form of the dialog.
 * @MDU_FORMAT_DIALOG_FLAGS_DISK_UTILITY_BUTTON: Show a "Disk Utility" button.
 * @MDU_FORMAT_DIALOG_FLAGS_ALLOW_MSDOS_EXTENDED: Allow selecting a MS-DOS extended partition instead of a file system.
 *
 * Flags used when creating a #MduFormatDialog.
 */
typedef enum {
        MDU_FORMAT_DIALOG_FLAGS_NONE = 0,
        MDU_FORMAT_DIALOG_FLAGS_SIMPLE = (1<<0),
        MDU_FORMAT_DIALOG_FLAGS_DISK_UTILITY_BUTTON = (1<<1),
        MDU_FORMAT_DIALOG_FLAGS_ALLOW_MSDOS_EXTENDED = (1<<2)
} MduFormatDialogFlags;

/**
 * MduDiskSelectionWidgetFlags:
 * @MDU_DISK_SELECTION_WIDGET_FLAGS_NONE: No flags set.
 * @MDU_DISK_SELECTION_WIDGET_FLAGS_ALLOW_MULTIPLE: Allow multiple disks to be selected.
 * @MDU_DISK_SELECTION_WIDGET_FLAGS_ALLOW_DISKS_WITH_INSUFFICIENT_SPACE: If set, disks with less
 * space than #MduDiskSelectionWidget:component-size will be eligible for selection.
 *
 * Flags used when creating a #MduDiskSelectionWidget.
 */
typedef enum {
        MDU_DISK_SELECTION_WIDGET_FLAGS_NONE = 0,
        MDU_DISK_SELECTION_WIDGET_FLAGS_ALLOW_MULTIPLE = (1<<0),
        MDU_DISK_SELECTION_WIDGET_FLAGS_ALLOW_DISKS_WITH_INSUFFICIENT_SPACE = (1<<1)
} MduDiskSelectionWidgetFlags;

/**
 * MduAddComponentLinuxMdFlags:
 * @MDU_ADD_COMPONENT_LINUX_MD_FLAGS_NONE: No flags set.
 * @MDU_ADD_COMPONENT_LINUX_MD_FLAGS_SPARE: The dialog is for selecting a spare.
 * @MDU_ADD_COMPONENT_LINUX_MD_FLAGS_EXPANSION: The dialog is for selecting a device used for expansion.
 *
 * Flags used when creating a #MduAddComponentLinuxMdDialog.
 */
typedef enum {
        MDU_ADD_COMPONENT_LINUX_MD_FLAGS_NONE = 0,
        MDU_ADD_COMPONENT_LINUX_MD_FLAGS_SPARE = (1<<0),
        MDU_ADD_COMPONENT_LINUX_MD_FLAGS_EXPANSION = (1<<1)
} MduAddComponentLinuxMdFlags;

#endif /* MDU_GTK_ENUMS_H */
