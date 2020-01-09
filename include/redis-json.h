#ifndef __REDIS_JSON_H__
#define __REDIS_JSON_H__

#include <glib.h>
#include <json-glib/json-glib.h>

#include "obj.h"
#include "str.h"

/**
 * Document object model for mapping call data to storable JSON.
 * Currently used by the Redis driver.
 * 
 * There is some confusion about the correct way to map the call data structures
 * to JSON and the code in redis.h/c uses a a set of "enumerated object collection"
 * to store the hierarchical call data instead of a more traditional object heirarchy.
 * 
 * The model here suggest an object heirarchy where ownership relationships are implied
 * by containment.
 **/

typedef struct redis_call_media_stream_fd {
	struct obj		obj;
	unsigned		unique_id;
	unsigned		stream_unique_id;
	str*			pref_family;
	unsigned		localport;
	str*			logical_intf;
	unsigned		logical_intf_uid;
} redis_call_media_stream_fd_t;

typedef struct redis_call_media_stream {
	struct obj		obj;
	unsigned		unique_id;
	unsigned		media_unique_id;
	unsigned		selected_sfd;
	int			rtp_sink;
	int			rtcp_sink;
	int			rtcp_sibling;
	unsigned		last_packet;
	unsigned		ps_flags;
	unsigned		component;
	str*			endpoint;
	str*			advertised_endpoint;
	unsigned 		stats_packets;
	unsigned		stats_bytes;
	unsigned		stats_errors;
	GQueue*			fds;
} redis_call_media_stream_t;

struct redis_call_media_tag;

typedef struct redis_call_media_tag {
	struct obj		obj;
	unsigned		unique_id;
	unsigned long		created;
	gboolean		active;
	gboolean		deleted;
	gboolean		block_dtmf;
	gboolean		block_media;
	str*			tag;
	str*			viabranch;
	str*			label;
	struct redis_call_media_tag*	other_tag;
} redis_call_media_tag_t;

typedef struct redis_call_media {
	struct obj		obj;
	unsigned		index;
	unsigned		unique_id;
	str*			type;
	str*			protocol;
	str*			desired_family;
	str*			logical_intf;
	unsigned		ptime;
	unsigned		media_flags;
	str*			rtpe_addr;
	redis_call_media_tag_t*	tag;
	GQueue*			streams;
} redis_call_media_t;

typedef struct redis_call {
	struct obj		obj;
	str*			call_id;
	unsigned long long	created;
	unsigned long		last_signal;
	unsigned		tos;
	gboolean		deleted;
	gboolean		ml_deleted;
	str*			created_from;
	str*			created_from_addr;
	unsigned		redis_hosted_db;
	str*			recording_metadata;
	gboolean		block_dtmf;
	gboolean		block_media;
	GQueue*			media;
} redis_call_t;

/**
 * Parse the JSON node into a `redis_call_t` data structure.
 * @param callid the Call's Call-ID that was used as the key for originally storing the call
 * @param json the glib-json parsed JSON data
 * @return loaded call object model. Release using `obj_put()`
 */
redis_call_t* redis_call_create(const str *callid, JsonNode *json);

/**
 * Retrieve a list of references to `redis_call_media_stream_t` across all media in the call.
 * 
 * @param callref a pointer to the `redis_call_t` data
 * @return list of call streams. Release using `q_queue_free_full(list, gdestroy_obj_put)`
 */
GQueue* redis_call_get_streams(redis_call_t *callref);

/**
 * Helper for using obj_put as a (*GDestroyNotify) parameter for glib.
 * 
 * Use it to cleanup `GQueue*`s returned from redis-json calls.
 * @param o gpointerdata that references a struct that extends `struct obj`
 */
void gdestroy_obj_put(void* o);

#endif /* __REDIS_JSON_H__ */