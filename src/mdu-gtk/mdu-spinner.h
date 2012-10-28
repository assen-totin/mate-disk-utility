/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Based on BlingSpinner: Copyright (C) 2007 John Stowers, Neil Jagdish Patel.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


#ifndef _MDU_SPINNER_H_
#define _MDU_SPINNER_H_

#include <mdu-gtk/mdu-gtk-types.h>

G_BEGIN_DECLS

#define MDU_TYPE_SPINNER         (mdu_spinner_get_type ())
#define MDU_SPINNER(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_SPINNER, MduSpinner))
#define MDU_SPINNER_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_SPINNER,  MduSpinnerClass))
#define MDU_IS_SPINNER(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_SPINNER))
#define MDU_IS_SPINNER_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_SPINNER))
#define MDU_SPINNER_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_SPINNER, MduSpinnerClass))

typedef struct MduSpinnerClass    MduSpinnerClass;
typedef struct MduSpinnerPrivate  MduSpinnerPrivate;

struct MduSpinner
{
	GtkDrawingArea parent;
};

struct MduSpinnerClass
{
	GtkDrawingAreaClass parent_class;
	MduSpinnerPrivate *priv;
};

GType mdu_spinner_get_type (void) G_GNUC_CONST;

GtkWidget *mdu_spinner_new (void);

void mdu_spinner_start (MduSpinner *spinner);
void mdu_spinner_stop  (MduSpinner *spinner);

G_END_DECLS

#endif
