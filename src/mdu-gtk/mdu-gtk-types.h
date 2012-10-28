/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-drive.h
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

#ifndef __MDU_GTK_TYPES_H
#define __MDU_GTK_TYPES_H

#include <glib-object.h>
#include <mdu/mdu.h>
#include <gtk/gtk.h>
#include <mdu-gtk/mdu-gtk-enums.h>

G_BEGIN_DECLS

typedef struct MduSample                      MduSample;
typedef struct MduColor                       MduColor;
typedef struct MduCurve                       MduCurve;
typedef struct MduGraph                       MduGraph;
typedef struct MduTimeLabel                   MduTimeLabel;
typedef struct MduAtaSmartDialog              MduAtaSmartDialog;
typedef struct MduSpinner                     MduSpinner;
typedef struct MduPoolTreeModel               MduPoolTreeModel;
typedef struct MduPoolTreeView                MduPoolTreeView;
typedef struct MduCreateLinuxMdDialog         MduCreateLinuxMdDialog;
typedef struct MduSizeWidget                  MduSizeWidget;
typedef struct MduVolumeGrid                  MduVolumeGrid;
typedef struct MduDetailsTable                MduDetailsTable;
typedef struct MduDetailsElement              MduDetailsElement;
typedef struct MduErrorDialog                 MduErrorDialog;
typedef struct MduConfirmationDialog          MduConfirmationDialog;
typedef struct MduButtonElement               MduButtonElement;
typedef struct MduButtonTable                 MduButtonTable;
typedef struct MduDialog                      MduDialog;
typedef struct MduEditPartitionDialog         MduEditPartitionDialog;
typedef struct MduFormatDialog                MduFormatDialog;
typedef struct MduPartitionDialog             MduPartitionDialog;
typedef struct MduCreatePartitionDialog       MduCreatePartitionDialog;
typedef struct MduCreateLinuxLvm2VolumeDialog MduCreateLinuxLvm2VolumeDialog;
typedef struct MduEditNameDialog              MduEditNameDialog;
typedef struct MduDiskSelectionWidget         MduDiskSelectionWidget;
typedef struct MduAddComponentLinuxMdDialog   MduAddComponentLinuxMdDialog;
typedef struct MduEditLinuxMdDialog           MduEditLinuxMdDialog;
typedef struct MduDriveBenchmarkDialog        MduDriveBenchmarkDialog;
typedef struct MduConnectToServerDialog       MduConnectToServerDialog;
typedef struct MduEditLinuxLvm2Dialog         MduEditLinuxLvm2Dialog;
typedef struct MduAddPvLinuxLvm2Dialog        MduAddPvLinuxLvm2Dialog;

G_END_DECLS

#endif /* __MDU_GTK_TYPES_H */
