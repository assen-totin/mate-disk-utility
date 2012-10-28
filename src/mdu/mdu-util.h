/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*- */
/* mdu-util.h
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

#ifndef __MDU_UTIL_H
#define __MDU_UTIL_H

#include <mdu/mdu-types.h>

G_BEGIN_DECLS

gchar *mdu_util_get_size_for_display (guint64  size,
                                      gboolean use_pow2,
                                      gboolean long_string);

char *mdu_util_get_fstype_for_display (const char *fstype, const char *fsversion, gboolean long_string);

char *mdu_util_fstype_get_description (char *fstype);

char *mdu_util_part_table_type_get_description (char *part_type);

char *mdu_util_get_speed_for_display (guint64 size);

char *mdu_util_get_connection_for_display (const char *connection_interface, guint64 connection_speed);

char *mdu_util_get_desc_for_part_type (const char *part_scheme, const char *part_type);

char *mdu_get_job_description (const char *job_id);

char *mdu_linux_md_get_raid_level_for_display (const gchar *linux_md_raid_level,
                                               gboolean long_string);

char *mdu_linux_md_get_raid_level_description (const gchar *linux_md_raid_level);

gchar *mdu_util_ata_smart_status_to_desc (const gchar  *status,
                                          gboolean     *out_highlight,
                                          gchar       **out_action_text,
                                          GIcon       **out_icon);

typedef void (*MduUtilPartTypeForeachFunc) (const char *scheme,
                                            const char *type,
                                            const char *name,
                                            gpointer user_data);

void mdu_util_part_type_foreach (MduUtilPartTypeForeachFunc callback, gpointer user_data);

/* ---------------------------------------------------------------------------------------------------- */

char      *mdu_util_get_default_part_type_for_scheme_and_fstype (const char *scheme, const char *fstype, guint64 size);

/* ---------------------------------------------------------------------------------------------------- */

gboolean mdu_util_save_secret (MduDevice      *device,
                               const char     *secret,
                               gboolean        save_in_keyring_session);

gboolean mdu_util_delete_secret (MduDevice *device);

gboolean mdu_util_have_secret (MduDevice *device);

gchar *mdu_util_get_secret (MduDevice *device);

/* ---------------------------------------------------------------------------------------------------- */

GIcon *mdu_util_get_emblemed_icon (const gchar *name,
                                   const gchar *emblem_name);

G_END_DECLS

#endif /* __MDU_UTIL_H */
