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

#define _GNU_SOURCE

#include "config.h"
#include <glib/gi18n-lib.h>

#include <math.h>

#include "mdu-size-widget.h"

struct MduButtonElementPrivate
{
        gchar *primary_text;
        gchar *secondary_text;
        gchar *icon_name;
        gboolean visible;
};

enum
{
        PROP_0,
        PROP_ICON_NAME,
        PROP_PRIMARY_TEXT,
        PROP_SECONDARY_TEXT,
        PROP_VISIBLE,
};

enum
{
        CHANGED_SIGNAL,
        CLICKED_SIGNAL,
        LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = {0,};

G_DEFINE_TYPE (MduButtonElement, mdu_button_element, G_TYPE_OBJECT)

static void
mdu_button_element_finalize (GObject *object)
{
        MduButtonElement *element = MDU_BUTTON_ELEMENT (object);

        g_free (element->priv->icon_name);
        g_free (element->priv->primary_text);
        g_free (element->priv->secondary_text);

        if (G_OBJECT_CLASS (mdu_button_element_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_button_element_parent_class)->finalize (object);
}

static void
mdu_button_element_get_property (GObject    *object,
                                  guint       property_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
        MduButtonElement *element = MDU_BUTTON_ELEMENT (object);

        switch (property_id) {
        case PROP_ICON_NAME:
                g_value_set_string (value, mdu_button_element_get_icon_name (element));
                break;

        case PROP_PRIMARY_TEXT:
                g_value_set_string (value, mdu_button_element_get_primary_text (element));
                break;

        case PROP_SECONDARY_TEXT:
                g_value_set_string (value, mdu_button_element_get_primary_text (element));
                break;

        case PROP_VISIBLE:
                g_value_set_boolean (value, mdu_button_element_get_visible (element));
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_button_element_set_property (GObject      *object,
                                  guint         property_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
        MduButtonElement *element = MDU_BUTTON_ELEMENT (object);

        switch (property_id) {
        case PROP_ICON_NAME:
                mdu_button_element_set_icon_name (element, g_value_get_string (value));
                break;

        case PROP_PRIMARY_TEXT:
                mdu_button_element_set_primary_text (element, g_value_get_string (value));
                break;

        case PROP_SECONDARY_TEXT:
                mdu_button_element_set_secondary_text (element, g_value_get_string (value));
                break;

        case PROP_VISIBLE:
                mdu_button_element_set_visible (element, g_value_get_boolean (value));
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
                break;
        }
}

static void
mdu_button_element_init (MduButtonElement *element)
{
        element->priv = G_TYPE_INSTANCE_GET_PRIVATE (element,
                                                     MDU_TYPE_BUTTON_ELEMENT,
                                                     MduButtonElementPrivate);

}

static void
mdu_button_element_class_init (MduButtonElementClass *klass)
{
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

        g_type_class_add_private (klass, sizeof (MduButtonElementPrivate));

        gobject_class->get_property        = mdu_button_element_get_property;
        gobject_class->set_property        = mdu_button_element_set_property;
        gobject_class->finalize            = mdu_button_element_finalize;

        g_object_class_install_property (gobject_class,
                                         PROP_ICON_NAME,
                                         g_param_spec_string ("icon-name",
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             G_PARAM_READABLE |
                                                             G_PARAM_WRITABLE |
                                                             G_PARAM_CONSTRUCT));

        g_object_class_install_property (gobject_class,
                                         PROP_PRIMARY_TEXT,
                                         g_param_spec_string ("primary-text",
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             G_PARAM_READABLE |
                                                             G_PARAM_WRITABLE |
                                                             G_PARAM_CONSTRUCT));

        g_object_class_install_property (gobject_class,
                                         PROP_SECONDARY_TEXT,
                                         g_param_spec_string ("secondary-text",
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             G_PARAM_READABLE |
                                                             G_PARAM_WRITABLE |
                                                             G_PARAM_CONSTRUCT));

        g_object_class_install_property (gobject_class,
                                         PROP_VISIBLE,
                                         g_param_spec_boolean ("visible",
                                                               NULL,
                                                               NULL,
                                                               TRUE,
                                                               G_PARAM_READABLE |
                                                               G_PARAM_WRITABLE |
                                                               G_PARAM_CONSTRUCT));

        signals[CHANGED_SIGNAL] = g_signal_new ("changed",
                                                MDU_TYPE_BUTTON_ELEMENT,
                                                G_SIGNAL_RUN_LAST,
                                                G_STRUCT_OFFSET (MduButtonElementClass, changed),
                                                NULL,
                                                NULL,
                                                g_cclosure_marshal_VOID__VOID,
                                                G_TYPE_NONE,
                                                0);

        signals[CLICKED_SIGNAL] = g_signal_new ("clicked",
                                                MDU_TYPE_BUTTON_ELEMENT,
                                                G_SIGNAL_RUN_LAST,
                                                G_STRUCT_OFFSET (MduButtonElementClass, clicked),
                                                NULL,
                                                NULL,
                                                g_cclosure_marshal_VOID__VOID,
                                                G_TYPE_NONE,
                                                0);
}

MduButtonElement *
mdu_button_element_new (const gchar       *icon_name,
                        const gchar       *primary_text,
                        const gchar       *secondary_text)
{
        return MDU_BUTTON_ELEMENT (g_object_new (MDU_TYPE_BUTTON_ELEMENT,
                                                 "icon-name", icon_name,
                                                 "primary-text", primary_text,
                                                 "secondary-text", secondary_text,
                                                 NULL));
}

const gchar *
mdu_button_element_get_icon_name (MduButtonElement *element)
{
        g_return_val_if_fail (MDU_IS_BUTTON_ELEMENT (element), NULL);
        return element->priv->icon_name;
}

const gchar *
mdu_button_element_get_primary_text (MduButtonElement *element)
{
        g_return_val_if_fail (MDU_IS_BUTTON_ELEMENT (element), NULL);
        return element->priv->primary_text;
}

const gchar *
mdu_button_element_get_secondary_text (MduButtonElement *element)
{
        g_return_val_if_fail (MDU_IS_BUTTON_ELEMENT (element), NULL);
        return element->priv->secondary_text;
}

gboolean
mdu_button_element_get_visible (MduButtonElement *element)
{
        g_return_val_if_fail (MDU_IS_BUTTON_ELEMENT (element), FALSE);
        return element->priv->visible;
}


void
mdu_button_element_set_icon_name (MduButtonElement *element,
                                  const gchar      *icon_name)
{
        g_return_if_fail (MDU_IS_BUTTON_ELEMENT (element));
        if (g_strcmp0 (element->priv->icon_name, icon_name) != 0) {
                g_free (element->priv->icon_name);
                element->priv->icon_name = g_strdup (icon_name);
                g_object_notify (G_OBJECT (element), "icon-name");
                g_signal_emit (element, signals[CHANGED_SIGNAL], 0);
        }
}

void
mdu_button_element_set_primary_text (MduButtonElement *element,
                                     const gchar      *primary_text)
{
        g_return_if_fail (MDU_IS_BUTTON_ELEMENT (element));
        if (g_strcmp0 (element->priv->primary_text, primary_text) != 0) {
                g_free (element->priv->primary_text);
                element->priv->primary_text = g_strdup (primary_text);
                g_object_notify (G_OBJECT (element), "primary-text");
                g_signal_emit (element, signals[CHANGED_SIGNAL], 0);
        }
}

void
mdu_button_element_set_secondary_text (MduButtonElement *element,
                                       const gchar      *secondary_text)
{
        g_return_if_fail (MDU_IS_BUTTON_ELEMENT (element));
        if (g_strcmp0 (element->priv->secondary_text, secondary_text) != 0) {
                g_free (element->priv->secondary_text);
                element->priv->secondary_text = g_strdup (secondary_text);
                g_object_notify (G_OBJECT (element), "secondary-text");
                g_signal_emit (element, signals[CHANGED_SIGNAL], 0);
        }
}

void
mdu_button_element_set_visible (MduButtonElement *element,
                                gboolean          visible)
{
        g_return_if_fail (MDU_IS_BUTTON_ELEMENT (element));
        if (element->priv->visible != visible) {
                element->priv->visible = visible;
                g_object_notify (G_OBJECT (element), "visible");
                g_signal_emit (element, signals[CHANGED_SIGNAL], 0);
        }
}
