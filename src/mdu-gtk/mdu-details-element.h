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

#ifndef __MDU_DETAILS_ELEMENT_H
#define __MDU_DETAILS_ELEMENT_H

#include <mdu-gtk/mdu-gtk.h>

G_BEGIN_DECLS

#define MDU_TYPE_DETAILS_ELEMENT         (mdu_details_element_get_type())
#define MDU_DETAILS_ELEMENT(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), MDU_TYPE_DETAILS_ELEMENT, MduDetailsElement))
#define MDU_DETAILS_ELEMENT_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST ((k), MDU_TYPE_DETAILS_ELEMENT, MduDetailsElementClass))
#define MDU_IS_DETAILS_ELEMENT(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), MDU_TYPE_DETAILS_ELEMENT))
#define MDU_IS_DETAILS_ELEMENT_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), MDU_TYPE_DETAILS_ELEMENT))
#define MDU_DETAILS_ELEMENT_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), MDU_TYPE_DETAILS_ELEMENT, MduDetailsElementClass))

typedef struct MduDetailsElementClass   MduDetailsElementClass;
typedef struct MduDetailsElementPrivate MduDetailsElementPrivate;

struct MduDetailsElement
{
        GObject parent;

        /*< private >*/
        MduDetailsElementPrivate *priv;
};

struct MduDetailsElementClass
{
        GObjectClass parent_class;

        /* signals */
        void (*changed) (MduDetailsElement *element);

        /* signals */
        void (*activated) (MduDetailsElement *element,
                           const gchar       *uri);
};

GType               mdu_details_element_get_type            (void) G_GNUC_CONST;
MduDetailsElement*  mdu_details_element_new                 (const gchar       *heading,
                                                             const gchar       *text,
                                                             const gchar       *tooltip);

const gchar        *mdu_details_element_get_heading        (MduDetailsElement *element);
GIcon              *mdu_details_element_get_icon           (MduDetailsElement *element);
const gchar        *mdu_details_element_get_text           (MduDetailsElement *element);
guint64             mdu_details_element_get_time           (MduDetailsElement *element);
gdouble             mdu_details_element_get_progress       (MduDetailsElement *element);
const gchar        *mdu_details_element_get_tooltip        (MduDetailsElement *element);
const gchar        *mdu_details_element_get_action_text    (MduDetailsElement *element);
const gchar        *mdu_details_element_get_action_uri     (MduDetailsElement *element);
const gchar        *mdu_details_element_get_action_tooltip (MduDetailsElement *element);
gboolean            mdu_details_element_get_is_spinning    (MduDetailsElement *element);
GtkWidget          *mdu_details_element_get_widget         (MduDetailsElement *element);

void                mdu_details_element_set_heading        (MduDetailsElement *element,
                                                            const gchar       *heading);
void                mdu_details_element_set_icon           (MduDetailsElement *element,
                                                            GIcon             *icon);
void                mdu_details_element_set_text           (MduDetailsElement *element,
                                                            const gchar       *text);
void                mdu_details_element_set_time           (MduDetailsElement *element,
                                                            guint64            time);
void                mdu_details_element_set_progress       (MduDetailsElement *element,
                                                            gdouble            progress);
void                mdu_details_element_set_tooltip        (MduDetailsElement *element,
                                                            const gchar       *tooltip);
void                mdu_details_element_set_action_text    (MduDetailsElement *element,
                                                            const gchar       *action_text);
void                mdu_details_element_set_action_uri     (MduDetailsElement *element,
                                                            const gchar       *action_uri);
void                mdu_details_element_set_action_tooltip (MduDetailsElement *element,
                                                            const gchar       *action_tooltip);
void                mdu_details_element_set_is_spinning    (MduDetailsElement *element,
                                                            gboolean           is_spinning);
void                mdu_details_element_set_widget         (MduDetailsElement *element,
                                                            GtkWidget         *widget);

G_END_DECLS

#endif  /* __MDU_DETAILS_ELEMENT_H */

