/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-shell.h
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

#ifndef MDU_SHELL_H
#define MDU_SHELL_H

#include <glib-object.h>
#include <gtk/gtk.h>

#include <mdu/mdu.h>

#define MDU_TYPE_SHELL             (mdu_shell_get_type ())
#define MDU_SHELL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), MDU_TYPE_SHELL, MduShell))
#define MDU_SHELL_CLASS(obj)       (G_TYPE_CHECK_CLASS_CAST ((obj), MDU_SHELL,  MduShellClass))
#define MDU_IS_SHELL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MDU_TYPE_SHELL))
#define MDU_IS_SHELL_CLASS(obj)    (G_TYPE_CHECK_CLASS_TYPE ((obj), MDU_TYPE_SHELL))
#define MDU_SHELL_GET_CLASS        (G_TYPE_INSTANCE_GET_CLASS ((obj), MDU_TYPE_SHELL, MduShellClass))

typedef struct _MduShellClass       MduShellClass;
typedef struct _MduShell            MduShell;

struct _MduShellPrivate;
typedef struct _MduShellPrivate     MduShellPrivate;

struct _MduShell
{
        GObject parent;

        /* private */
        MduShellPrivate *priv;
};

struct _MduShellClass
{
        GObjectClass parent_class;
};

GType           mdu_shell_get_type                          (void);
MduShell       *mdu_shell_new                               (const char      *ssh_address);
GtkWidget      *mdu_shell_get_toplevel                      (MduShell       *shell);
MduPool        *mdu_shell_get_pool_for_selected_presentable (MduShell       *shell);
void            mdu_shell_update                            (MduShell       *shell);
MduPresentable *mdu_shell_get_selected_presentable          (MduShell       *shell);
void            mdu_shell_select_presentable                (MduShell       *shell,
                                                             MduPresentable *presentable);
void            mdu_shell_raise_error                       (MduShell       *shell,
                                                             MduPresentable *presentable,
                                                             GError         *error,
                                                             const char     *primary_markup_format,
                                                             ...);

#endif /* MDU_SHELL_H */
