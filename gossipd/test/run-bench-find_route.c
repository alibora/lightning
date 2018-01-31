#include <assert.h>
#include <bitcoin/pubkey.h>
#include <ccan/err/err.h>
#include <ccan/opt/opt.h>
#include <ccan/tal/str/str.h>
#include <ccan/time/time.h>
#include <common/pseudorand.h>
#include <common/status.h>
#include <common/type_to_string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#define status_trace(fmt, ...) \
	do { printf((fmt) ,##__VA_ARGS__); printf("\n"); } while(0)

static bool in_bench = 0;

/* We use made-up pubkeys, so don't try to format them. */
static char *fake_type_to_string_(const tal_t *ctx, const char *typename,
				  union printable_types u)
{
	/* We *do* call this at end of route setup. */
	if (streq(typename, "struct pubkey")) {
		size_t n;
		memcpy(&n, u.pubkey, sizeof(n));
		return tal_fmt(ctx, "pubkey-#%zu", n);
	}
	return type_to_string_(ctx, typename, u);
}

/* Only used on setup: if it was in benchmark run, we'd need the real one */
static int fake_pubkey_cmp(const struct pubkey *a, const struct pubkey *b)
{
	assert(!in_bench);
	return memcmp(a, b, sizeof(*a));
}

#define pubkey_cmp fake_pubkey_cmp
#define type_to_string_ fake_type_to_string_
#include "../routing.c"
#undef type_to_string_

struct broadcast_state *new_broadcast_state(tal_t *ctx UNNEEDED)
{
	return NULL;
}

/* AUTOGENERATED MOCKS START */
/* Generated stub for fromwire_channel_announcement */
bool fromwire_channel_announcement(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, size_t *plen UNNEEDED, secp256k1_ecdsa_signature *node_signature_1 UNNEEDED, secp256k1_ecdsa_signature *node_signature_2 UNNEEDED, secp256k1_ecdsa_signature *bitcoin_signature_1 UNNEEDED, secp256k1_ecdsa_signature *bitcoin_signature_2 UNNEEDED, u8 **features UNNEEDED, struct bitcoin_blkid *chain_hash UNNEEDED, struct short_channel_id *short_channel_id UNNEEDED, struct pubkey *node_id_1 UNNEEDED, struct pubkey *node_id_2 UNNEEDED, struct pubkey *bitcoin_key_1 UNNEEDED, struct pubkey *bitcoin_key_2 UNNEEDED)
{ fprintf(stderr, "fromwire_channel_announcement called!\n"); abort(); }
/* Generated stub for fromwire_channel_update */
bool fromwire_channel_update(const void *p UNNEEDED, size_t *plen UNNEEDED, secp256k1_ecdsa_signature *signature UNNEEDED, struct bitcoin_blkid *chain_hash UNNEEDED, struct short_channel_id *short_channel_id UNNEEDED, u32 *timestamp UNNEEDED, u16 *flags UNNEEDED, u16 *cltv_expiry_delta UNNEEDED, u64 *htlc_minimum_msat UNNEEDED, u32 *fee_base_msat UNNEEDED, u32 *fee_proportional_millionths UNNEEDED)
{ fprintf(stderr, "fromwire_channel_update called!\n"); abort(); }
/* Generated stub for fromwire_node_announcement */
bool fromwire_node_announcement(const tal_t *ctx UNNEEDED, const void *p UNNEEDED, size_t *plen UNNEEDED, secp256k1_ecdsa_signature *signature UNNEEDED, u8 **features UNNEEDED, u32 *timestamp UNNEEDED, struct pubkey *node_id UNNEEDED, u8 rgb_color[3] UNNEEDED, u8 alias[32] UNNEEDED, u8 **addresses UNNEEDED)
{ fprintf(stderr, "fromwire_node_announcement called!\n"); abort(); }
/* Generated stub for fromwire_peektype */
int fromwire_peektype(const u8 *cursor UNNEEDED)
{ fprintf(stderr, "fromwire_peektype called!\n"); abort(); }
/* Generated stub for fromwire_u8 */
u8 fromwire_u8(const u8 **cursor UNNEEDED, size_t *max UNNEEDED)
{ fprintf(stderr, "fromwire_u8 called!\n"); abort(); }
/* Generated stub for fromwire_wireaddr */
bool fromwire_wireaddr(const u8 **cursor UNNEEDED, size_t *max UNNEEDED, struct wireaddr *addr UNNEEDED)
{ fprintf(stderr, "fromwire_wireaddr called!\n"); abort(); }
/* Generated stub for onion_type_name */
const char *onion_type_name(int e UNNEEDED)
{ fprintf(stderr, "onion_type_name called!\n"); abort(); }
/* Generated stub for queue_broadcast */
bool queue_broadcast(struct broadcast_state *bstate UNNEEDED,
			     const int type UNNEEDED,
			     const u8 *tag UNNEEDED,
			     const u8 *payload UNNEEDED)
{ fprintf(stderr, "queue_broadcast called!\n"); abort(); }
/* Generated stub for replace_broadcast */
bool replace_broadcast(struct broadcast_state *bstate UNNEEDED,
		       u64 *index UNNEEDED,
		       const int type UNNEEDED,
		       const u8 *tag UNNEEDED,
		       const u8 *payload UNNEEDED)
{ fprintf(stderr, "replace_broadcast called!\n"); abort(); }
/* Generated stub for status_failed */
void status_failed(enum status_fail code UNNEEDED, const char *fmt UNNEEDED, ...)
{ fprintf(stderr, "status_failed called!\n"); abort(); }
/* Generated stub for towire_pubkey */
void towire_pubkey(u8 **pptr UNNEEDED, const struct pubkey *pubkey UNNEEDED)
{ fprintf(stderr, "towire_pubkey called!\n"); abort(); }
/* Generated stub for towire_short_channel_id */
void towire_short_channel_id(u8 **pptr UNNEEDED,
			     const struct short_channel_id *short_channel_id UNNEEDED)
{ fprintf(stderr, "towire_short_channel_id called!\n"); abort(); }
/* Generated stub for towire_u16 */
void towire_u16(u8 **pptr UNNEEDED, u16 v UNNEEDED)
{ fprintf(stderr, "towire_u16 called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

const void *trc;

/* Updates existing route if required. */
static struct node_connection *add_connection(struct routing_state *rstate,
					      const struct pubkey *from,
					      const struct pubkey *to,
					      u32 base_fee, s32 proportional_fee,
					      u32 delay)
{
	struct node_connection *c = get_or_make_connection(rstate, from, to);
	c->base_fee = base_fee;
	c->proportional_fee = proportional_fee;
	c->delay = delay;
	c->active = true;
	memset(&c->short_channel_id, 0, sizeof(c->short_channel_id));
	c->flags = get_channel_direction(from, to);
	return c;
}

static struct pubkey nodeid(size_t n)
{
	struct pubkey id;

	memset(&id, 0, sizeof(id));
	memcpy(&id, &n, sizeof(n));
	return id;
}

static void populate_random_node(struct routing_state *rstate, u64 n)
{
	struct pubkey id = nodeid(n);

	/* Create 2 random channels. */
	if (n < 1)
		return;

	for (size_t i = 0; i < 2; i++) {
		struct pubkey randnode = nodeid(pseudorand(n));

		add_connection(rstate, &id, &randnode,
			       pseudorand(100),
			       pseudorand(100),
			       pseudorand(144));
		add_connection(rstate, &randnode, &id,
			       pseudorand(100),
			       pseudorand(100),
			       pseudorand(144));
	}
}

static void run(const char *name)
{
	int status;

	switch (fork()) {
	case 0:
		execlp(name, name, NULL);
		exit(127);
	case -1:
		err(1, "forking %s", name);
	default:
		wait(&status);
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			errx(1, "%s failed", name);
	}
}

int main(int argc, char *argv[])
{
	static const struct bitcoin_blkid zerohash;
	const tal_t *ctx = trc = tal_tmpctx(NULL);
	struct routing_state *rstate;
	size_t num_nodes = 100, num_runs = 1;
	struct timemono start, end;
	size_t num_success;
	struct pubkey me = nodeid(0);
	bool perfme = false;
	const double riskfactor = 0.01 / BLOCKS_PER_YEAR / 10000;

	secp256k1_ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY
						 | SECP256K1_CONTEXT_SIGN);

	rstate = new_routing_state(ctx, &zerohash, &me);
	opt_register_noarg("--perfme", opt_set_bool, &perfme,
			   "Run perfme-start and perfme-stop around benchmark");

	opt_parse(&argc, argv, opt_log_stderr_exit);

	if (argc > 1)
		num_nodes = atoi(argv[1]);
	if (argc > 2)
		num_runs = atoi(argv[2]);
	if (argc > 3)
		opt_usage_and_exit("[num_nodes [num_runs]]");

	for (size_t i = 0; i < num_nodes; i++)
		populate_random_node(rstate, i);

	in_bench = true;
	if (perfme)
		run("perfme-start");

	start = time_mono();
	num_success = 0;
	for (size_t i = 0; i < num_runs; i++) {
		struct pubkey from = nodeid(pseudorand(num_nodes));
		struct pubkey to = nodeid(pseudorand(num_nodes));
		u64 fee;
		struct node_connection **route = NULL, *nc;

		nc = find_route(ctx, rstate, &from, &to,
				pseudorand(100000),
				riskfactor,
				&fee, &route);
		num_success += (nc != NULL);
		tal_free(route);
	}
	end = time_mono();

	if (perfme)
		run("perfme-stop");

	printf("%zu (%zu succeeded) routes in %zu nodes in %"PRIu64" msec (%"PRIu64" nanoseconds per route)",
	       num_runs, num_success, num_nodes,
	       time_to_msec(timemono_between(end, start)),
	       time_to_nsec(time_divide(timemono_between(end, start), num_runs)));

	tal_free(ctx);
	secp256k1_context_destroy(secp256k1_ctx);
	opt_free_table();
	return 0;
}
