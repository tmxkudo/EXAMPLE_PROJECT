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

uma_zone_t ofp_uma_pool_create(const char *name, int nitems, int size)
{
	odp_pool_param_t pool_params;
	odp_pool_t pool;
	uma_zone_t zone;

	pool_params.buf.size  = size + sizeof(struct uma_pool_metadata);
	pool_params.buf.align = 0;
	pool_params.buf.num   = nitems;
	pool_params.type      = ODP_POOL_BUFFER;

	OFP_INFO("Creating pool '%s', nitems=%d size=%d total=%d",
		 name, pool_params.buf.num, pool_params.buf.size,
		 pool_params.buf.num * pool_params.buf.size);

	if (shm->num_pools >= OFP_NUM_UMA_POOLS) {
		OFP_ERR("Exceeded max number (%d) of pools",
			OFP_NUM_UMA_POOLS);
		return OFP_UMA_ZONE_INVALID;
	}
	pool = ofp_pool_create(name, &pool_params);
	if (pool == ODP_POOL_INVALID) {
		OFP_ERR("odp_pool_create failed");
		return OFP_UMA_ZONE_INVALID;
	}

	zone = shm->num_pools++;
	shm->pools[zone] = pool;

	return zone;
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
