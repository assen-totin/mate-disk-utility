/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-section.c
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
#include <glib/gi18n.h>

#include <string.h>
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include <math.h>

#include <mdu/mdu.h>
#include "mdu-section.h"

struct _MduSectionPrivate
{
        MduShell *shell;
        MduPresentable *presentable;
};

static GObjectClass *parent_class = NULL;

G_DEFINE_TYPE (MduSection, mdu_section, GTK_TYPE_VBOX)

enum {
        PROP_0,
        PROP_SHELL,
        PROP_PRESENTABLE,
};

static void
mdu_section_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
        MduSection *section = MDU_SECTION (object);
        gpointer obj;

        switch (prop_id) {
        case PROP_SHELL:
                if (section->priv->shell != NULL)
                        g_object_unref (section->priv->shell);
                obj = g_value_get_object (value);
                section->priv->shell = (obj == NULL ? NULL : g_object_ref (obj));
                break;

        case PROP_PRESENTABLE:
                if (section->priv->presentable != NULL)
                        g_object_unref (section->priv->presentable);
                obj = g_value_get_object (value);
                section->priv->presentable = (obj == NULL ? NULL : g_object_ref (obj));
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
        }
}

static void
mdu_section_get_property (GObject     *object,
                          guint        prop_id,
                          GValue      *value,
                          GParamSpec  *pspec)
{
        MduSection *section = MDU_SECTION (object);

        switch (prop_id) {
        case PROP_SHELL:
                g_value_set_object (value, section->priv->shell);
                break;

        case PROP_PRESENTABLE:
                g_value_set_object (value, section->priv->presentable);
                break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
                break;
    }
}

static void
mdu_section_finalize (MduSection *section)
{
        if (section->priv->presentable != NULL)
                g_object_unref (section->priv->presentable);

        if (G_OBJECT_CLASS (parent_class)->finalize)
                (* G_OBJECT_CLASS (parent_class)->finalize) (G_OBJECT (section));
}

static void
mdu_section_class_init (MduSectionClass *klass)
{
        GObjectClass *obj_class = (GObjectClass *) klass;

        parent_class = g_type_class_peek_parent (klass);

        obj_class->finalize = (GObjectFinalizeFunc) mdu_section_finalize;
        obj_class->set_property = mdu_section_set_property;
        obj_class->get_property = mdu_section_get_property;

        klass->update = NULL; /* pure virtual */

        g_type_class_add_private (klass, sizeof (MduSectionPrivate));

        /**
         * MduSection:shell:
         *
         * The #MduShell instance we belong to. Cannot be #NULL.
         */
        g_object_class_install_property (obj_class,
                                         PROP_SHELL,
                                         g_param_spec_object ("shell",
                                                              NULL,
                                                              NULL,
                                                              MDU_TYPE_SHELL,
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_READABLE |
                                                              G_PARAM_CONSTRUCT));

        /**
         * MduSection:presentable:
         *
         * The #MduPresentable instance we are displaying. Cannot be #NULL.
         */
        g_object_class_install_property (obj_class,
                                         PROP_PRESENTABLE,
                                         g_param_spec_object ("presentable",
                                                              NULL,
                                                              NULL,
                                                              MDU_TYPE_PRESENTABLE,
                                                              G_PARAM_WRITABLE |
                                                              G_PARAM_READABLE |
                                                              G_PARAM_CONSTRUCT));
}

static void
mdu_section_init (MduSection *section)
{
        section->priv = G_TYPE_INSTANCE_GET_PRIVATE (section, MDU_TYPE_SECTION, MduSectionPrivate);
}

void
mdu_section_update (MduSection *section)
{
        MduSectionClass *klass;

        g_return_if_fail (MDU_IS_SECTION (section));

        klass = MDU_SECTION_GET_CLASS (section);

        (* klass->update) (section);
}


/**
 * mdu_section_get_shell:
 * @section: The #MduSection.
 *
 * Get the #MduShell this section is part of.
 *
 * Returns: The #MduShell for @section. Caller shall not unref this object.
 **/
MduShell *
mdu_section_get_shell (MduSection *section)
{
        return section->priv->shell;
}

/**
 * mdu_section_get_presentable:
 * @section: The #MduSection.
 *
 * Get the #MduPresentable we're presenting information about.
 *
 * Returns: The #MduPresentable for @section. Caller shall not unref this object.
 **/
MduPresentable *
mdu_section_get_presentable (MduSection *section)
{
        return section->priv->presentable;
}
