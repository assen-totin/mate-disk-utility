/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2009 Red Hat, Inc.
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
 *
 * Author: David Zeuthen <davidz@redhat.com>
 */

#ifndef __MDU_BUTTON_ELEMENT_H
#define __MDU_BUTTON_ELEMENT_H

#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_BUTTON_ELEMENT         (mdu_button_element_get_type())
#define MDU_BUTTON_ELEMENT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_BUTTON_ELEMENT, MduButtonElement))
#define MDU_BUTTON_ELEMENT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_BUTTON_ELEMENT, MduButtonElementClass))
#define MDU_IS_BUTTON_ELEMENT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_BUTTON_ELEMENT))
#define MDU_IS_BUTTON_ELEMENT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_BUTTON_ELEMENT))
#define MDU_BUTTON_ELEMENT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_BUTTON_ELEMENT, MduButtonElementClass))

typedef struct MduButtonElementClass   MduButtonElementClass;
typedef struct MduButtonElementPrivate MduButtonElementPrivate;

struct MduButtonElement
{
        GObject parent;

        /*< private >*/
        MduButtonElementPrivate *priv;
};

struct MduButtonElementClass
{
        GObjectClass parent_class;

        /* signals */
        void (*changed) (MduButtonElement *element);

        /* signals */
        void (*clicked) (MduButtonElement *element);
};

GType              mdu_button_element_get_type             (void) G_GNUC_CONST;
MduButtonElement*  mdu_button_element_new                  (const gchar       *icon_name,
                                                            const gchar       *primary_text,
                                                            const gchar       *secondary_text);
const gchar       *mdu_button_element_get_icon_name        (MduButtonElement *element);
const gchar       *mdu_button_element_get_primary_text     (MduButtonElement *element);
const gchar       *mdu_button_element_get_secondary_text   (MduButtonElement *element);
gboolean           mdu_button_element_get_visible          (MduButtonElement *element);
void               mdu_button_element_set_icon_name        (MduButtonElement *element,
                                                            const gchar      *icon_name);
void               mdu_button_element_set_primary_text     (MduButtonElement  *element,
                                                            const gchar       *primary_text);
void               mdu_button_element_set_secondary_text   (MduButtonElement  *element,
                                                            const gchar       *primary_text);
void               mdu_button_element_set_visible          (MduButtonElement  *element,
                                                            gboolean           visible);

G_END_DECLS

#endif  /* __MDU_BUTTON_ELEMENT_H */

