/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-error.h
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

#ifndef __MDU_ERROR_H
#define __MDU_ERROR_H

#include <mdu/mdu-types.h>

G_BEGIN_DECLS

/**
 * MduError:
 * @MDU_ERROR_FAILED: The operation failed.
 * @MDU_ERROR_BUSY: The device is busy
 * @MDU_ERROR_CANCELLED: The operation was cancelled
 * @MDU_ERROR_INHIBITED: The daemon is being inhibited.
 * @MDU_ERROR_INVALID_OPTION: An invalid option was passed
 * @MDU_ERROR_NOT_SUPPORTED: Operation not supported.
 * @MDU_ERROR_ATA_SMART_WOULD_WAKEUP: Getting S.M.A.R.T. data for the device would require to spin it up.
 * @MDU_ERROR_PERMISSION_DENIED: Permission denied.
 * @MDU_ERROR_FILESYSTEM_DRIVER_MISSING: The filesystem driver for a filesystem is not installed.
 * @MDU_ERROR_FILESYSTEM_TOOLS_MISSING: User-space tools to carry out the request action on a filesystem is not installed.
 *
 * Error codes in the #MDU_ERROR domain.
 */
typedef enum
{
        MDU_ERROR_FAILED,
        MDU_ERROR_BUSY,
        MDU_ERROR_CANCELLED,
        MDU_ERROR_INHIBITED,
        MDU_ERROR_INVALID_OPTION,
        MDU_ERROR_NOT_SUPPORTED,
        MDU_ERROR_ATA_SMART_WOULD_WAKEUP,
        MDU_ERROR_PERMISSION_DENIED,
        MDU_ERROR_FILESYSTEM_DRIVER_MISSING,
        MDU_ERROR_FILESYSTEM_TOOLS_MISSING
} MduError;

/**
 * MDU_ERROR:
 *
 * Error domain used for errors reported from udisks daemon
 * via D-Bus. Note that not all remote errors are mapped to this
 * domain. Errors in this domain will come from the #MduError
 * enumeration. See #GError for more information on error domains.
 */
#define MDU_ERROR mdu_error_quark ()

GQuark      mdu_error_quark           (void);

G_END_DECLS

#endif /* __MDU_ERROR_H */
