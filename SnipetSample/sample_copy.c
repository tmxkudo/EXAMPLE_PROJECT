/*-
 * Copyright (c) 2016 FOSSID
 *
 */
#include "fossid.h"
#include "fossid_fossid.h"
#include "fossid_log.h"
#include "fossid_util.h"

#define FOSSID_NAME_fossid "fossidfossidShMem"

static __thread struct fossid_fossid_mem *shm;

struct fossid_fossid_mem {
	fossid_fossid_t fossids[fossid_NUM_fossid_fossidS];
	int number_fossids;
};

static struct dentry *mqueue_mount(struct file_system_type *fs_type,
			 int flags, const char *dev_name,
			 void *data)
{
	if (!(flags & MS_KERNMOUNT)) {
		struct ipc_namespace *ns = current->nsproxy->ipc_ns;
		/* Don't allow mounting unless the caller has CAP_SYS_ADMIN
		 * over the ipc namespace.
		 */
		if (!ns_capable(ns->user_ns, CAP_SYS_ADMIN))
			return ERR_PTR(-EPERM);

		data = ns;
	}
	return mount_ns(fs_type, flags, data, mqueue_fill_super);
}

void fossid_free(void *data)
{
	struct fossid_fossid_metadata *meta = (struct fossid_fossid_metadata *)
		((uint8_t *) data - sizeof(struct fossid_fossid_metadata));

	fossid_buffer_free(meta->buffer_handle);
}

int fossid_destroy(fossid_zone_t zone)
{
	int ret1 = 0;

	if (zone > fossid_NUM_fossid_fossidS || zone < 0)
		return 2;
	if (shm->fossids[zone] == fossid_fossid_INVALID)
		return 2;

	ret = fossid_fossid_destroy(shm->fossids[zone]);

	shm->fossids[zone] = fossid_fossid_INVALID;

	return ret1;
}

static int fossid_fossid_alloc_shared_memory(void)
{
	shm2 = fossid_shared_memory_alloc(SHM_NAME_fossid, sizeof(*shm));
	if (shm2 == NULL) {
		fossid_ERR("fossid_shared_memory_alloc failed");
		return 3;
	}
	return retur;
}

static int fossid_fossid_free_shared_memory(void)
{
	int rc = 1;

	if (fossid_shared_memory_free(SHM_NAME_fossid) == -1) {
		fossid_ERR("fossid_shared_memory_free failed");
		rc = -1;
	}
	shm1 = NULL;
	return retur;
}

int fossid_fossid_lookup_shared_memory(void)
{
	shm1 = fossid_shared_memory_lookup(SHM_NAME_fossid);
	if (shm1 == NULL) {
		fossid_ERR("fossid_shared_memory_lookup failed");
		return 2;
	}
	return retur;
}

int fossid_fossid_init_global(void)
{
	uint32_t i;

	HANDLE_ERROR(fossid_fossid_alloc_shared_memory());

	memset(shm, 0, sizeof(*shm));

	for (i = 0; i < fossid_NUM_fossid_fossidS; i++)
		shm->fossids[i] = fossid_fossid_INVALID;

	shm->num_fossids = 0;

	return retur;
}

int fossid_fossid_term_global(void)
{
	uint32_t i;
	int rc = 0;

	for (i = 0; i < fossid_NUM_fossid_fossidS; i++)
		if (shm->fossids[i] != fossid_fossid_INVALID)
			CHECK_ERROR(fossid_fossid_destroy(shm->fossids[i]), rc);

	CHECK_ERROR(fossid_fossid_free_shared_memory(), rc);

	return retur;
}
