/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2008 John Crispin <blogic@openwrt.org> 
 */

#ifndef _UCI_H__
#define _UCI_H__
#include <uci.h>
#include "list.h"

struct ucilist {
	struct list_head list;
	char *val;
};

struct uci_context* ucix_init(const char *config_file);
struct uci_context* ucix_init_path(const char *path, const char *config_file);
void ucix_cleanup(struct uci_context *ctx);
int ucix_save(struct uci_context *ctx, const char *p);
int ucix_save_state(struct uci_context *ctx, const char *p);
char* ucix_get_option(struct uci_context *ctx,
	const char *p, const char *s, const char *o);
int ucix_get_option_list(struct uci_context *ctx, const char *p,
	const char *s, const char *o, struct list_head *l);
int ucix_get_option_int(struct uci_context *ctx,
	const char *p, const char *s, const char *o, int def);
void ucix_add_section(struct uci_context *ctx,
	const char *p, const char *s, const char *t);
void ucix_add_option(struct uci_context *ctx,
	const char *p, const char *s, const char *o, const char *t);
void ucix_add_option_int(struct uci_context *ctx,
	const char *p, const char *s, const char *o, int t);
void ucix_for_each_section_type(struct uci_context *ctx,
	const char *p, const char *t,
	void (*cb)(const char*, void*), void *priv);
void ucix_for_each_section_option(struct uci_context *ctx,
	const char *p, const char *s,
	void (*cb)(const char*, const char*, void*), void *priv);
int ucix_commit(struct uci_context *ctx, const char *p);
void ucix_revert(struct uci_context *ctx,
	const char *p, const char *s, const char *o);
void ucix_del(struct uci_context *ctx, const char *p,
	const char *s, const char *o);
#endif
