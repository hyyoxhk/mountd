#include <string.h>
#include <stdlib.h>

#include "include/ucix.h"

static struct uci_ptr ptr;

static inline int ucix_get_ptr(struct uci_context *ctx, const char *p, const char *s, const char *o, const char *t)
{
	memset(&ptr, 0, sizeof(ptr));
	ptr.package = p;
	ptr.section = s;
	ptr.option = o;
	ptr.value = t;
	return uci_lookup_ptr(ctx, &ptr, NULL, true);
}

struct uci_context* ucix_init(const char *config_file)
{
	struct uci_context *ctx = uci_alloc_context();
	uci_add_delta_path(ctx, "/var/state");
	if(uci_load(ctx, config_file, NULL) != UCI_OK)
	{
		printf("%s/%s is missing or corrupt\n", ctx->savedir, config_file);
		return NULL;
	}
	return ctx;
}

struct uci_context* ucix_init_path(const char *path, const char *config_file)
{
	struct uci_context *ctx = uci_alloc_context();
	if(path)
	{
		uci_set_savedir(ctx, path);
	}
	if(uci_load(ctx, config_file, NULL) != UCI_OK)
	{
		printf("%s/%s is missing or corrupt\n", ctx->savedir, config_file);
		return NULL;
	}
	return ctx;
}

void ucix_cleanup(struct uci_context *ctx)
{
	uci_free_context(ctx);
}

int ucix_save(struct uci_context *ctx, const char *p)
{
	if(ucix_get_ptr(ctx, p, NULL, NULL, NULL))
		return 1;
	uci_set_savedir(ctx, "/tmp/.uci/");
	uci_save(ctx, ptr.p);
	return 0;
}

int ucix_save_state(struct uci_context *ctx, const char *p)
{
	if(ucix_get_ptr(ctx, p, NULL, NULL, NULL))
		return 1;
	uci_set_savedir(ctx, "/var/state/");
	uci_save(ctx, ptr.p);
	return 0;
}

int ucix_get_option_list(struct uci_context *ctx, const char *p,
	const char *s, const char *o, struct list_head *l)
{
	struct uci_element *e = NULL;
	if(ucix_get_ptr(ctx, p, s, o, NULL))
		return 1;
	if (!(ptr.flags & UCI_LOOKUP_COMPLETE))
		return 1;
	e = ptr.last;
	switch (e->type)
	{
	case UCI_TYPE_OPTION:
		switch(ptr.o->type) {
			case UCI_TYPE_LIST:
				uci_foreach_element(&ptr.o->v.list, e)
				{
					struct ucilist *ul = malloc(sizeof(struct ucilist));
					ul->val = strdup((e->name)?(e->name):(""));
					INIT_LIST_HEAD(&ul->list);
					list_add(&ul->list, l);
				}
				break;
			default:
				break;
		}
		break;
	default:
		return 1;
	}

	return 0;
}

char* ucix_get_option(struct uci_context *ctx, const char *p, const char *s, const char *o)
{
	struct uci_element *e = NULL;
	const char *value = NULL;
	if(ucix_get_ptr(ctx, p, s, o, NULL))
		return NULL;
	if (!(ptr.flags & UCI_LOOKUP_COMPLETE))
		return NULL;
	e = ptr.last;
	switch (e->type)
	{
	case UCI_TYPE_SECTION:
		value = uci_to_section(e)->type;
		break;
	case UCI_TYPE_OPTION:
		switch(ptr.o->type) {
			case UCI_TYPE_STRING:
				value = ptr.o->v.string;
				break;
			default:
				value = NULL;
				break;
		}
		break;
	default:
		return 0;
	}

	return (value) ? (strdup(value)):(NULL);
}

int ucix_get_option_int(struct uci_context *ctx, const char *p, const char *s, const char *o, int def)
{
	char *tmp = ucix_get_option(ctx, p, s, o);
	int ret = def;

	if (tmp)
	{
		ret = atoi(tmp);
		free(tmp);
	}
	return ret;
}

void ucix_add_section(struct uci_context *ctx, const char *p, const char *s, const char *t)
{
	if(ucix_get_ptr(ctx, p, s, NULL, t))
		return;
	uci_set(ctx, &ptr);
}

void ucix_add_option(struct uci_context *ctx, const char *p, const char *s, const char *o, const char *t)
{
	if(ucix_get_ptr(ctx, p, s, o, (t)?(t):("")))
		return;
	uci_set(ctx, &ptr);
}

void ucix_add_option_int(struct uci_context *ctx, const char *p, const char *s, const char *o, int t)
{
	char tmp[64];
	snprintf(tmp, 64, "%d", t);
	ucix_add_option(ctx, p, s, o, tmp);
}

void ucix_del(struct uci_context *ctx, const char *p, const char *s, const char *o)
{
	if(!ucix_get_ptr(ctx, p, s, o, NULL))
		uci_delete(ctx, &ptr);
}

void ucix_revert(struct uci_context *ctx, const char *p, const char *s, const char *o)
{
	if(!ucix_get_ptr(ctx, p, s, o, NULL))
		uci_revert(ctx, &ptr);
}

void ucix_for_each_section_type(struct uci_context *ctx,
	const char *p, const char *t,
	void (*cb)(const char*, void*), void *priv)
{
	struct uci_element *e;
	if(ucix_get_ptr(ctx, p, NULL, NULL, NULL))
		return;
	uci_foreach_element(&ptr.p->sections, e)
		if (!strcmp(t, uci_to_section(e)->type))
			cb(e->name, priv);
}

void ucix_for_each_section_option(struct uci_context *ctx,
	const char *p, const char *s,
	void (*cb)(const char*, const char*, void*), void *priv)
{
	struct uci_element *e;
	if(ucix_get_ptr(ctx, p, s, NULL, NULL))
		return;
	uci_foreach_element(&ptr.s->options, e)
	{
		struct uci_option *o = uci_to_option(e);
		cb(o->e.name, o->v.string, priv);
	}
}

int ucix_commit(struct uci_context *ctx, const char *p)
{
	if(ucix_get_ptr(ctx, p, NULL, NULL, NULL))
		return 1;
	return uci_commit(ctx, &ptr.p, false);
}
