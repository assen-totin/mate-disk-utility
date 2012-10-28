/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-error.c
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
#include <glib/gi18n-lib.h>

#include <dbus/dbus-glib.h>
#include <string.h>

#include "mdu-error.h"
#include "mdu-private.h"

/**
 * SECTION:mdu-error
 * @title: MduError
 * @short_description: Error helper functions
 *
 * Contains helper functions for reporting errors to the user.
 **/

/**
 * mdu_error_quark:
 *
 * Gets the #MduError Quark.
 *
 * Returns: a #GQuark
 **/
GQuark
mdu_error_quark (void)
{
        static GQuark ret = 0;
        if (ret == 0)
                ret = g_quark_from_static_string ("mdu-error-quark");
        return ret;
}

void
_mdu_error_fixup (GError *error)
{
        const char *name;
        gboolean matched;

        if (error == NULL)
                return;

        if (error->domain != DBUS_GERROR ||
            error->code != DBUS_GERROR_REMOTE_EXCEPTION)
                return;

        name = dbus_g_error_get_name (error);
        if (name == NULL)
                return;

        matched = TRUE;
        if (strcmp (name, "org.freedesktop.UDisks.Error.Failed") == 0)
                error->code = MDU_ERROR_FAILED;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.Busy") == 0)
                error->code = MDU_ERROR_BUSY;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.Cancelled") == 0)
                error->code = MDU_ERROR_CANCELLED;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.Inhibited") == 0)
                error->code = MDU_ERROR_INHIBITED;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.InvalidOption") == 0)
                error->code = MDU_ERROR_INVALID_OPTION;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.NotSupported") == 0)
                error->code = MDU_ERROR_NOT_SUPPORTED;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.AtaSmartWouldWakeup") == 0)
                error->code = MDU_ERROR_ATA_SMART_WOULD_WAKEUP;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.PermissionDenied") == 0)
                error->code = MDU_ERROR_PERMISSION_DENIED;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.FilesystemDriverMissing") == 0)
                error->code = MDU_ERROR_FILESYSTEM_DRIVER_MISSING;
        else if (strcmp (name, "org.freedesktop.UDisks.Error.FilesystemToolsMissing") == 0)
                error->code = MDU_ERROR_FILESYSTEM_TOOLS_MISSING;
        else
                matched = FALSE;

        if (matched)
                error->domain = MDU_ERROR;
}
