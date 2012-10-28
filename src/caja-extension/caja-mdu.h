/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 *  caja-mdu.h
 *
 *  Copyright (C) 2008-2009 Red Hat, Inc.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Author: Tomas Bzatek <tbzatek@redhat.com>
 *
 */

#ifndef CAJA_MDU_H
#define CAJA_MDU_H

#include <glib-object.h>
#include <libcaja-extension/caja-menu-provider.h>
#include <mdu/mdu.h>

G_BEGIN_DECLS

#define CAJA_TYPE_MDU	  (caja_mdu_get_type ())
#define CAJA_MDU(o)		  (G_TYPE_CHECK_INSTANCE_CAST ((o), CAJA_TYPE_MDU, CajaMdu))
#define CAJA_IS_MDU(o)	  (G_TYPE_CHECK_INSTANCE_TYPE ((o), CAJA_TYPE_MDU))

typedef struct _CajaMdu      CajaMdu;
typedef struct _CajaMduClass CajaMduClass;

struct _CajaMdu
{
	GObject parent;
};

struct _CajaMduClass
{
	GObjectClass parent_class;
};

GType caja_mdu_get_type      (void) G_GNUC_CONST;
void  caja_mdu_register_type (GTypeModule *module);

G_END_DECLS

#endif  /* CAJA_MDU_H */

