/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-process.h
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

#ifndef __MDU_PROCESS_H
#define __MDU_PROCESS_H

#include <unistd.h>
#include <sys/types.h>
#include <mdu/mdu-types.h>

G_BEGIN_DECLS

#define MDU_TYPE_PROCESS           (mdu_process_get_type ())
#define MDU_PROCESS(o)             (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_PROCESS, MduProcess))
#define MDU_PROCESS_CLASS(k)       (G_TYPE_CHECK_CLASS_CAST ((k), MDU_PROCESS,  MduProcessClass))
#define MDU_IS_PROCESS(o)          (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_PROCESS))
#define MDU_IS_PROCESS_CLASS(k)    (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_PROCESS))
#define MDU_PROCESS_GET_CLASS(k)   (G_TYPE_INSTANCE_GET_CLASS ((k), MDU_TYPE_PROCESS, MduProcessClass))

typedef struct _MduProcessClass       MduProcessClass;
typedef struct _MduProcessPrivate     MduProcessPrivate;

struct _MduProcess
{
        GObject parent;

        /* private */
        MduProcessPrivate *priv;
};

struct _MduProcessClass
{
        GObjectClass parent_class;

};

GType       mdu_process_get_type         (void);
pid_t       mdu_process_get_id           (MduProcess *process);
uid_t       mdu_process_get_owner        (MduProcess *process);
const char *mdu_process_get_command_line (MduProcess *process);
GAppInfo   *mdu_process_get_app_info     (MduProcess *process);

G_END_DECLS

#endif /* __MDU_PROCESS_H */
