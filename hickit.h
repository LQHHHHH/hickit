#ifndef HICKIT_H
#define HICKIT_H

#include <stdint.h>
#include <stdio.h>
#include "krng.h"

#define HK_SUB_DELIM '!'

#define HK_OUT_PHASE        0x1
#define HK_OUT_P4           0x2
#define HK_OUT_CNT          0x4
#define HK_OUT_PPROB        0x8

#ifdef __cplusplus
extern "C" {
#endif

struct hk_popt {
	int flag;
	int min_dist, max_seg, min_mapq, min_flt_cnt;
	int min_tad_size;
	float area_weight;
	int min_radius, max_radius, max_nei;
	float pseudo_cnt;
	int n_iter;
};

struct hk_sdict {     // sequence dictionary
	int32_t n, m;
	char **name;
	int32_t *len;
	void *h;
};

struct hk_seg {       // a segment
	int32_t frag_id, chr, st, en;
	int8_t strand, phase;
	int16_t mapq;
};

struct hk_pair {      // a contact pair
	uint64_t chr;     // chr1<<32 | chr2
	uint64_t pos;     // pos1<<32 | pos2
	int8_t strand[2]; // strand
	int8_t phase[2];  // phase
	uint32_t n:31, tad_marked:1;
	union {
		float p4[4];
		float phased_prob;
	} _;
};

struct hk_map {
	struct hk_sdict *d;
	int32_t n_frags, n_segs, n_pairs;
	struct hk_seg  *segs;
	struct hk_pair *pairs;
};

struct hk_bpair {
	int32_t bid[2];
	int32_t n, max_nei;
	int8_t phase[2];
	float p;
};

struct hk_bead {
	int32_t chr, st, en;
};

typedef float fvec3_t[3];

struct hk_bmap {
	int32_t n_beads, n_pairs;
	struct hk_sdict *d;
	struct hk_bead *beads;
	uint64_t *offcnt; // index into beads
	struct hk_bpair *pairs;
	fvec3_t *x;
	float *feat;
};

struct hk_fdg_opt {
	float target_radius;
	float k_rep, r_rep;
	int n_iter;
	float step;
	float max_f;
};

extern int hk_verbose;

void hk_popt_init(struct hk_popt *opt);

int32_t *hk_sd_ploidy_XY(const struct hk_sdict *d, int32_t *sex_flag);
struct hk_sdict *hk_sd_dup(const struct hk_sdict *d);

struct hk_map *hk_map_read(const char *fn);
void hk_map_destroy(struct hk_map *m);
void hk_map_phase_male_XY(struct hk_map *m);

struct hk_pair *hk_seg2pair(int32_t n_segs, const struct hk_seg *segs, int min_dist, int max_seg, int min_mapq, int32_t *n_pairs_);
int32_t hk_pair_dedup(int n_pairs, struct hk_pair *pairs, int min_dist);
int32_t hk_pair_filter(int32_t n_pairs, struct hk_pair *pairs, int32_t max_radius, int32_t min_cnt, float drop_frac);
void hk_pair_count_contained(int32_t n_pairs, struct hk_pair *pairs);
void hk_pair_count_nei(int32_t n_pairs, struct hk_pair *pairs, int radius);
struct hk_map *hk_pair_split_phase(const struct hk_map *m, float phase_thres);

struct hk_pair *hk_pair2tad(const struct hk_sdict *d, int32_t n_pairs, struct hk_pair *pairs, int min_tad_size, float area_weight, int32_t *n_tads_);
void hk_mark_by_tad(int32_t n_tads, const struct hk_pair *tads, int32_t n_pairs, struct hk_pair *pairs);

void hk_impute(int32_t n_pairs, struct hk_pair *pairs, int max_radius, int min_radius, int max_nei, int n_iter, float pseudo_cnt, int use_spacial);

void hk_validate_holdback(krng_t *r, float ratio, int32_t n_pairs, struct hk_pair *pairs);
void hk_validate_roc(int32_t n_pairs, struct hk_pair *pairs);

struct hk_bmap *hk_3dg_read(const char *fn);
struct hk_bmap *hk_bmap_gen(const struct hk_sdict *d, int32_t n_pairs, const struct hk_pair *pairs, int size);
int32_t hk_pair_flt_3d(const struct hk_bmap *m, int32_t n_pairs, struct hk_pair *pairs, float max_factor);
void hk_bmap_destroy(struct hk_bmap *m);

void hk_fdg_opt_init(struct hk_fdg_opt *opt);
void hk_fdg(const struct hk_fdg_opt *opt, struct hk_bmap *m, const struct hk_bmap *src, krng_t *rng);
void hk_check_dist(struct hk_bmap *m);

void hk_print_seg(FILE *fp, const struct hk_sdict *d, int32_t n_segs, const struct hk_seg *segs);
void hk_print_pair(FILE *fp, int flag, const struct hk_sdict *d, int32_t n_pairs, const struct hk_pair *pairs);
void hk_print_bmap(FILE *fp, const struct hk_bmap *m);
void hk_print_3dg(FILE *fp, const struct hk_bmap *m);

void hk_pair_image(const struct hk_sdict *d, int32_t n_pairs, const struct hk_pair *pairs, int w, float phase_thres, int no_grad, const char *fn);

void hk_v3d_prep(int *argc, char *argv[]);
void hk_v3d_view(struct hk_bmap *m, int width, int line_width, int color_seed, const char *hl);
void hk_fdg_normalize(struct hk_bmap *m);

#ifdef __cplusplus
}
#endif

#endif
