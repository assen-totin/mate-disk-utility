/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-section-hub.c
 *
 * Copyright (C) 2009 David Zeuthen
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
#include <gio/gdesktopappinfo.h>

#include <mdu-gtk/mdu-gtk.h>
#include "mdu-section-hub.h"

struct _MduSectionHubPrivate
{
        GtkWidget *heading_label;
        MduDetailsElement *vendor_element;
        MduDetailsElement *model_element;
        MduDetailsElement *revision_element;
        MduDetailsElement *driver_element;
        MduDetailsElement *fabric_element;
        MduDetailsElement *num_ports_element;
};

G_DEFINE_TYPE (MduSectionHub, mdu_section_hub, MDU_TYPE_SECTION)

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_hub_finalize (GObject *object)
{
        //MduSectionHub *section = MDU_SECTION_HUB (object);

        if (G_OBJECT_CLASS (mdu_section_hub_parent_class)->finalize != NULL)
                G_OBJECT_CLASS (mdu_section_hub_parent_class)->finalize (object);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_hub_update (MduSection *_section)
{
        MduSectionHub *section = MDU_SECTION_HUB (_section);
        MduPresentable *p;
        MduAdapter *a;
        MduExpander *e;
        MduPool *pool;
        const gchar *vendor;
        const gchar *model;
        const gchar *revision;
        const gchar *fabric;
        const gchar *driver;
        guint num_ports;
        gchar *num_ports_str;
        gchar *fabric_str;
        gchar *s;
        gchar *s2;

        a = NULL;
        e = NULL;
        num_ports_str = NULL;

        p = mdu_section_get_presentable (_section);

        a = mdu_hub_get_adapter (MDU_HUB (p));
        e = mdu_hub_get_expander (MDU_HUB (p));

        pool = mdu_presentable_get_pool (p);

        fabric = "";
        if (e != NULL) {
                vendor = mdu_expander_get_vendor (e);
                model = mdu_expander_get_model (e);
                revision = mdu_expander_get_revision (e);
                if (a != NULL)
                        fabric = mdu_adapter_get_fabric (a);
                num_ports = mdu_expander_get_num_ports (e);

                /* TODO: maybe move these blocks of code to mdu-util.c as util functions */

                if (num_ports > 0) {
                        if (g_strcmp0 (fabric, "scsi_sas") == 0) {
                                /* Translators: Used for SAS to convey the number of PHYs in the
                                 * "Number of Ports" element. You should probably not translate PHY.
                                 */
                                num_ports_str = g_strdup_printf (_("%d PHYs"), num_ports);
                        } else {
                                num_ports_str = g_strdup_printf ("%d", num_ports);
                        }
                } else {
                        num_ports_str = g_strdup ("–");
                }

                if (revision == NULL || strlen (revision) == 0)
                        revision = "–";

                driver = "–";
        } else if (a != NULL) {

                vendor = mdu_adapter_get_vendor (a);
                model = mdu_adapter_get_model (a);
                driver = mdu_adapter_get_driver (a);
                fabric = mdu_adapter_get_fabric (a);
                num_ports = mdu_adapter_get_num_ports (a);

                revision = "–";
        } else {
                vendor = "–";
                model = "–";
                driver = "–";
                fabric = "–";
                revision = "–";
                num_ports = 0;
        }

        if (g_str_has_prefix (fabric, "ata_pata")) {
                fabric_str = g_strdup (_("Parallel ATA"));
        } else if (g_str_has_prefix (fabric, "ata_sata")) {
                fabric_str = g_strdup (_("Serial ATA"));
        } else if (g_str_has_prefix (fabric, "ata")) {
                fabric_str = g_strdup (_("ATA"));
        } else if (g_str_has_prefix (fabric, "scsi_sas")) {
                fabric_str = g_strdup (_("Serial Attached SCSI"));
        } else if (g_str_has_prefix (fabric, "scsi")) {
                fabric_str = g_strdup (_("SCSI"));
        } else {
                fabric_str = g_strdup ("–");
        }

        s = mdu_presentable_get_name (p);
        s2 = g_strconcat ("<b>", s, "</b>", NULL);
        gtk_label_set_markup (GTK_LABEL (section->priv->heading_label), s2);
        g_free (s);
        g_free (s2);

        if (num_ports > 0) {
                if (g_strcmp0 (fabric, "scsi_sas") == 0) {
                        /* Translators: Used for SAS to convey the number of PHYs in the
                         * "Number of Ports" element. You should probably not translate PHY.
                         */
                        num_ports_str = g_strdup_printf (_("%d PHYs"), num_ports);
                } else {
                        num_ports_str = g_strdup_printf ("%d", num_ports);
                }
        } else {
                num_ports_str = g_strdup ("–");
        }

        mdu_details_element_set_text (section->priv->vendor_element, vendor);
        mdu_details_element_set_text (section->priv->model_element, model);
        mdu_details_element_set_text (section->priv->revision_element, revision);
        mdu_details_element_set_text (section->priv->driver_element, driver);
        mdu_details_element_set_text (section->priv->fabric_element, fabric_str);
        mdu_details_element_set_text (section->priv->num_ports_element, num_ports_str);

        g_free (num_ports_str);
        if (pool != NULL)
                g_object_unref (pool);
        if (a != NULL)
                g_object_unref (a);
        if (e != NULL)
                g_object_unref (e);
}

/* ---------------------------------------------------------------------------------------------------- */

static void
mdu_section_hub_constructed (GObject *object)
{
        MduSectionHub *section = MDU_SECTION_HUB (object);
        GtkWidget *align;
        GtkWidget *label;
        GtkWidget *table;
        GtkWidget *vbox;
        MduPresentable *p;
        MduDevice *d;
        GPtrArray *elements;
        MduDetailsElement *element;

        p = mdu_section_get_presentable (MDU_SECTION (section));
        d = mdu_presentable_get_device (p);

        gtk_box_set_spacing (GTK_BOX (section), 12);

        /*------------------------------------- */

        label = gtk_label_new (NULL);
        gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
        gtk_box_pack_start (GTK_BOX (section), label, FALSE, FALSE, 0);
        section->priv->heading_label = label;

        align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
        gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 12, 0);
        gtk_box_pack_start (GTK_BOX (section), align, FALSE, FALSE, 0);

        vbox = gtk_vbox_new (FALSE, 6);
        gtk_container_add (GTK_CONTAINER (align), vbox);

        elements = g_ptr_array_new_with_free_func (g_object_unref);

        element = mdu_details_element_new (_("Vendor:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->vendor_element = element;

        element = mdu_details_element_new (_("Model:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->model_element = element;

        element = mdu_details_element_new (_("Revision:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->revision_element = element;

        element = mdu_details_element_new (_("Driver:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->driver_element = element;

        element = mdu_details_element_new (_("Fabric:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->fabric_element = element;

        element = mdu_details_element_new (_("Number of Ports:"), NULL, NULL);
        g_ptr_array_add (elements, element);
        section->priv->num_ports_element = element;

        table = mdu_details_table_new (1, elements);
        g_ptr_array_unref (elements);
        gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);

        /* -------------------------------------------------------------------------------- */

        gtk_widget_show_all (GTK_WIDGET (section));

        if (d != NULL)
                g_object_unref (d);

        if (G_OBJECT_CLASS (mdu_section_hub_parent_class)->constructed != NULL)
                G_OBJECT_CLASS (mdu_section_hub_parent_class)->constructed (object);
}

static void
mdu_section_hub_class_init (MduSectionHubClass *klass)
{
        GObjectClass *gobject_class;
        MduSectionClass *section_class;

        gobject_class = G_OBJECT_CLASS (klass);
        section_class = MDU_SECTION_CLASS (klass);

        gobject_class->finalize    = mdu_section_hub_finalize;
        gobject_class->constructed = mdu_section_hub_constructed;
        section_class->update      = mdu_section_hub_update;

        g_type_class_add_private (klass, sizeof (MduSectionHubPrivate));
}

static void
mdu_section_hub_init (MduSectionHub *section)
{
        section->priv = G_TYPE_INSTANCE_GET_PRIVATE (section, MDU_TYPE_SECTION_HUB, MduSectionHubPrivate);
}

GtkWidget *
mdu_section_hub_new (MduShell       *shell,
                     MduPresentable *presentable)
{
        return GTK_WIDGET (g_object_new (MDU_TYPE_SECTION_HUB,
                                         "shell", shell,
                                         "presentable", presentable,
                                         NULL));
}
