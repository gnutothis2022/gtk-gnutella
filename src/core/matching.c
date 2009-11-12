/*
 * $Id$
 *
 * Copyright (c) 2001-2003, Raphael Manfredi
 *
 * Search bins are Copyright (c) 2001-2003, Kenn Brooks Hamm & Raphael Manfredi
 *
 *----------------------------------------------------------------------
 * This file is part of gtk-gnutella.
 *
 *  gtk-gnutella is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  gtk-gnutella is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gtk-gnutella; if not, write to the Free Software
 *  Foundation, Inc.:
 *      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *----------------------------------------------------------------------
 */

#include "common.h"

RCSID("$Id$")

#include "matching.h"
#include "qrp.h"				/* For qhvec_add() */
#include "share.h"

#include "lib/atoms.h"
#include "lib/ascii.h"
#include "lib/halloc.h"
#include "lib/pattern.h"
#include "lib/utf8.h"
#include "lib/wordvec.h"
#include "lib/walloc.h"

#include "if/gnet_property_priv.h"

#include "lib/override.h"		/* Must be the last header included */

#define WOVEC_DFLT	10			/**< Default size of word-vectors */

/*
 * Masks for mask_hash().
 */

#define MASK_LETTER(x)		(1 << (x))		/**< bits 0 to 25 */
#define MASK_DIGIT			0x80000000

/*
 * Search table searching routines.
 *
 * We're building an inverted index of all the file names by linking
 * together all the names having in common sequences of two chars.
 *
 * For instance, given the filenames "foo", "bar", "ar" and "arc", we'll
 * have the following bins:
 *
 *    bin["fo"] = { "foo" };
 *    bin["oo"] = { "foo" };
 *    bin["ba"] = { "bar" };
 *    bin["ar"] = { "bar", "ar", "arc" };
 *    bin["rc"] = { "arc" };
 *
 * Now assume we're looking for "arc". We're scanning the pattern to find
 * the bin which has the less amount of files listed insided.  The patterns
 * gives us the bins "ar" and "rc", and:
 *
 *    bin["ar"] has 3 items
 *    bin["rc"] has 1
 *
 * Therefore we'll look for "arc" in the bin["rc"] list.
 */

#define ST_MIN_BIN_SIZE		4

struct shared_file;

struct st_entry {
	const char *string;				/* atom */
	struct shared_file *sf;
	guint32 mask;
};

struct st_bin {
	int nslots, nvals;
	struct st_entry **vals;
};

struct _search_table {
	int nentries, nchars, nbins;
	struct st_bin **bins;
	struct st_bin all_entries;
	guchar index_map[(guchar) -1];
};

static void
destroy_entry(struct st_entry *entry)
{
	g_assert(entry != NULL);

	atom_str_free_null(&entry->string);
	shared_file_unref(&entry->sf);
	wfree(entry, sizeof *entry);
}

/**
 * Initialize a bin.
 */
static void
bin_initialize(struct st_bin *bin, int size)
{
	int i;

	bin->nvals = 0;
	bin->nslots = size;

	bin->vals = halloc(bin->nslots * sizeof bin->vals[0]);
	for (i = 0; i < bin->nslots; i++)
		bin->vals[i] = NULL;
}

/**
 * Allocate a bin.
 */
static struct st_bin *
bin_allocate(void)
{
	struct st_bin *bin = walloc(sizeof *bin);

	bin_initialize(bin, ST_MIN_BIN_SIZE);
	return bin;
}

/**
 * Destroy a bin.
 *
 * @note Do NOT destroy the st_entry's, since they may be shared.
 */
static void
bin_destroy(struct st_bin *bin)
{
	HFREE_NULL(bin->vals);
	bin->nslots = 0;
	bin->nvals = 0;
}

/**
 * Inserts an item into a bin.
 */
static void
bin_insert_item(struct st_bin *bin, struct st_entry *entry)
{
	if (bin->nvals == bin->nslots) {
		bin->nslots *= 2;
		bin->vals = hrealloc(bin->vals, bin->nslots * sizeof bin->vals[0]);
	}
	bin->vals[bin->nvals++] = entry;
}

/**
 * Makes a bin take as little memory as needed.
 */
static void
bin_compact(struct st_bin *bin)
{
	g_assert(bin->vals != NULL);	/* Or it would not have been allocated */

	bin->vals = hrealloc(bin->vals, bin->nvals * sizeof bin->vals[0]);
	bin->nslots = bin->nvals;
}

static guchar map[(guchar) -1];

static void
setup_map(void)
{
	guint i;

	for (i = 0; i < G_N_ELEMENTS(map); i++)	{
		guchar c;

		if (i > 0 && utf8_byte_is_allowed(i)) {
			if (is_ascii_upper(i)) {
				c = ascii_tolower(i);
			} else if (
				is_ascii_punct(i) || is_ascii_cntrl(i) || is_ascii_space(i)
			) {
				c = ' ';
			} else { 
				c = i;
			}
		} else {
			c = 0;
		}
		map[i] = c;
	}
}

/**
 * Initialize permanent data in search table.
 */
void
st_initialize(search_table_t *table)
{
	static guchar fold_map[(guchar) -1];
	guchar cur_char = '\0';
	guint i;

	table->nentries = table->nchars = 0;
	setup_map();

	/*
	 * The indexing map is used to avoid having 256*256 bins.
	 */

	for (i = 0; i < G_N_ELEMENTS(table->index_map); i++) {
		guchar map_char = map[i];

		if (fold_map[map_char]) {
			table->index_map[i] = fold_map[map_char];
		} else {
			fold_map[map_char] = cur_char;
			table->index_map[i] = cur_char;
			cur_char++;
		}
	}

	table->nchars = cur_char;
	table->nbins = table->nchars * table->nchars;
	table->bins = NULL;
	table->all_entries.vals = 0;

	if (GNET_PROPERTY(dbg) > 3)
		printf("search table will use max of %d bins (%d indexing chars)\n",
			table->nbins, table->nchars);
}

/**
 * Allocates a new search_table_t. Use st_free() to free it.
 */
search_table_t *
st_alloc(void)
{
	search_table_t *table = walloc(sizeof *table);
	return table;
}

void
st_free(search_table_t **ptr)
{
	g_assert(ptr);
	if (*ptr) {
		search_table_t *table = *ptr;
		wfree(table, sizeof *table);
		*ptr = NULL;
	}
}


/**
 * Recreate variable parts of the search table.
 */
void
st_create(search_table_t *table)
{
	int i;

	table->bins = halloc(table->nbins * sizeof table->bins[0]);
	for (i = 0; i < table->nbins; i++)
		table->bins[i] = NULL;

    bin_initialize(&table->all_entries, ST_MIN_BIN_SIZE);
}

/**
 * Destroy a search table.
 */
void
st_destroy(search_table_t *table)
{
	int i;

	if (table->bins) {
		for (i = 0; i < table->nbins; i++) {
			struct st_bin *bin = table->bins[i];

			if (bin) {
				bin_destroy(bin);
				wfree(bin, sizeof *bin);
			}
		}
		HFREE_NULL(table->bins);
	}

	if (table->all_entries.vals) {
		for (i = 0; i < table->all_entries.nvals; i++) {
			destroy_entry(table->all_entries.vals[i]);
			table->all_entries.vals[i] = NULL;
		}
		bin_destroy(&table->all_entries);
	}
}

/**
 * Compute character mask "hash", using one bit per letter of the alphabet,
 * plus one for any digit.
 */
static guint32
mask_hash(const char *s) {
	guchar c;
	guint32 mask = 0;

	while ((c = (guchar) *s++)) {
		if (is_ascii_space(c))
			continue;
		else if (is_ascii_digit(c))
			mask |= MASK_DIGIT;
		else {
			int idx = ascii_tolower(c) - 97;
			if (idx >= 0 && idx < 26)
				mask |= MASK_LETTER(idx);
		}
	}

	return mask;
}

/**
 * Get key of two-char pair.
 */
static inline int
st_key(search_table_t *table, const char k[2])
{
	return table->index_map[(guchar) k[0]] * table->nchars +
		table->index_map[(guchar) k[1]];
}

/**
 * Insert an item into the search_table
 * one-char strings are silently ignored.
 *
 * @return TRUE if the item was inserted; FALSE otherwise.
 */
gboolean
st_insert_item(search_table_t *table, const char *s, gpointer data)
{
	size_t i, len;
	struct st_entry *entry;
	GHashTable *seen_keys;

	len = utf8_char_count(s);
	if ((size_t) -1 == len || len < 2)
		return FALSE;

	seen_keys = g_hash_table_new(NULL, NULL);

	entry = walloc(sizeof *entry);
	entry->string = atom_str_get(s);
	entry->sf = shared_file_ref(data);
	entry->mask = mask_hash(entry->string);

	len = strlen(entry->string);
	for (i = 0; i < len - 1; i++) {
		int key = st_key(table, &entry->string[i]);

		/* don't insert item into same bin twice */
		if (g_hash_table_lookup(seen_keys, GINT_TO_POINTER(key)))
			continue;

		g_hash_table_insert(seen_keys, GINT_TO_POINTER(key),
			GINT_TO_POINTER(1));

		g_assert(key < table->nbins);
		if (table->bins[key] == NULL)
			table->bins[key] = bin_allocate();

		bin_insert_item(table->bins[key], entry);
	}
	bin_insert_item(&table->all_entries, entry);
	table->nentries++;

	g_hash_table_destroy(seen_keys);
	return TRUE;
}

/**
 * Minimize space consumption.
 */
void
st_compact(search_table_t *table)
{
	int i;

	if (!table->all_entries.nvals)
		return;			/* Nothing in table */

	bin_compact(&table->all_entries);
	for (i = 0; i < table->nbins; i++)
		if (table->bins[i])
			bin_compact(table->bins[i]);
}

/**
 * Apply pattern matching on text, matching at the *beginning* of words.
 * Patterns are lazily compiled as needed, using pattern_compile_fast().
 */
static gboolean
entry_match(const char *text, size_t tlen,
	cpattern_t **pw, word_vec_t *wovec, size_t wn)
{
	size_t i;

	for (i = 0; i < wn; i++) {
		size_t j, offset = 0, amount = wovec[i].amount;

		if (pw[i] == NULL)
			pw[i] = pattern_compile_fast(wovec[i].word, wovec[i].len);

		for (j = 0; j < amount; j++) {
			const char *pos;

			pos = pattern_qsearch(pw[i], text, tlen, offset, qs_begin);
			if (pos)
				offset = (pos - text) + pw[i]->len;
			else
				break;
		}
		if (j != amount)	/* Word does not occur as many time as we want */
			return FALSE;
	}

	return TRUE;
}

/**
 * Fill non-NULL query hash vector for query routing.
 *
 * This needs to be called when st_search() is not called when processing
 * a query, otherwise the qhery hash vector won't be properly initialized
 * and the query would be improperly dropped by qrt_build_query_target(),
 * hence never routed.
 */
void
st_fill_qhv(const char *search_term, query_hashvec_t *qhv)
{
	char *search;
	word_vec_t *wovec;
	guint wocnt;
	guint i;

	if (NULL == qhv)
		return;

	search = UNICODE_CANONIZE(search_term);
	wocnt = word_vec_make(search, &wovec);

	for (i = 0; i < wocnt; i++) {
		if (wovec[i].len >= QRP_MIN_WORD_LENGTH)
			qhvec_add(qhv, wovec[i].word, QUERY_H_WORD);
	}

	if (search != search_term)
		G_FREE_NULL(search);

	if (wocnt > 0)
		word_vec_free(wovec, wocnt);
}

/**
 * Do an actual search.
 */
void
st_search(
	search_table_t *table,
	const char *search_term,
	st_search_callback callback,
	gpointer ctx,
	int max_res,
	query_hashvec_t *qhv)
{
	char *search;
	int key, nres;
	guint i, len;
	struct st_bin *best_bin = NULL;
	int best_bin_size = INT_MAX;
	word_vec_t *wovec;
	guint wocnt;
	cpattern_t **pattern;
	struct st_entry **vals;
	guint vcnt;
	int scanned = 0;		/* measure search mask efficiency */
	guint32 search_mask;
	size_t minlen;
	guint random_offset;  /* Randomizer for search returns */

	search = UNICODE_CANONIZE(search_term);
	if (GNET_PROPERTY(search_debug) > 4 && 0 != strcmp(search, search_term)) {
		g_message(" Original search term: \"%s\"", search_term);
		g_message("Canonical search term: \"%s\"", search);
	}
	len = strlen(search);

	/*
	 * Find smallest bin
	 */

	if (len >= 2) {
		for (i = 0; i < len - 1; i++) {
			struct st_bin *bin;
			if (is_ascii_space(search[i]) || is_ascii_space(search[i+1]))
				continue;
			key = st_key(table, search + i);
			if ((bin = table->bins[key]) == NULL) {
				best_bin = NULL;
				break;
			}
			if (bin->nvals < best_bin_size) {
				best_bin = bin;
				best_bin_size = bin->nvals;
			}
		}

		if (GNET_PROPERTY(dbg) > 6)
			printf("st_search(): str=\"%s\", len=%d, best_bin_size=%d\n",
				search, len, best_bin_size);
	}

	/*
	 * If the best_bin is NULL, we did not find a matching bin, and we're
	 * sure we won't be able to find the search string.
	 *
	 * Note that on search strings like "r e m ", we always have a letter
	 * followed by spaces, so we won't search that.
	 *		--RAM, 06/10/2001
	 */

	if (best_bin == NULL) {
		/*
		 * If we have a `qhv', we need to compute the word vector anway,
		 * for query routing...
		 */

		if (qhv == NULL)
			goto finish;
	}

	/*
	 * Prepare matching patterns
	 */

	wocnt = word_vec_make(search, &wovec);

	/*
	 * Compute the query hashing information for query routing, if needed.
	 */

	if (qhv != NULL) {
		for (i = 0; i < wocnt; i++) {
			if (wovec[i].len >= QRP_MIN_WORD_LENGTH)
				qhvec_add(qhv, wovec[i].word, QUERY_H_WORD);
		}
	}

	if (wocnt == 0 || best_bin == NULL) {
		if (wocnt > 0)
			word_vec_free(wovec, wocnt);
		goto finish;
	}

	g_assert(best_bin_size > 0);	/* Allocated bin, it must hold something */


	pattern = walloc0(wocnt * sizeof *pattern);

	/*
	 * Prepare matching optimization, an idea from Mike Green.
	 *
	 * At library building time, we computed a mask hash, made from the
	 * lowercased file name, using one bit per different letter, roughly
	 * (see mask_hash() for the exact algorigthm).
	 *
	 * We're now going to compute the same mask on the query, and compare
	 * it bitwise with the mask for each file.  If the file does not hold
	 * at least all the chars present in the query, it's no use applying
	 * the pattern matching algorithm, it won't match at all.
	 *
	 *		--RAM, 01/10/2001
	 */

	search_mask = mask_hash(search);

	/*
	 * Prepare second matching optimization: since all words in the query
	 * must match the exact amount of time, we can compute the minimum length
	 * the searched file must have.  We add one character after each word
	 * but the last, to account for space between words.
	 *		--RAM, 11/07/2002
	 */

	for (minlen = 0, i = 0; i < wocnt; i++)
		minlen += wovec[i].len + 1;
	minlen--;
	g_assert(minlen <= INT_MAX);

	/*
	 * Search through the smallest bin
	 */

	vcnt = best_bin->nvals;
	vals = best_bin->vals;
	random_offset = random_u32() % vcnt;

	nres = 0;
	for (i = 0; i < vcnt; i++) {
		const struct st_entry *e;
		struct shared_file *sf;
		size_t canonic_len;

		/*
		 * As we only return a limited count of results, pick a random
		 * offset, so that repeated searches will match different items
		 * instead of always the first - with some probability.
		 */
		e = vals[(i + random_offset) % vcnt];
		
		if ((e->mask & search_mask) != search_mask)
			continue;		/* Can't match */

		sf = e->sf;

		canonic_len = shared_file_name_canonic_len(sf);
		if (canonic_len < minlen)
			continue;		/* Can't match */

		scanned++;

		if (entry_match(e->string, canonic_len, pattern, wovec, wocnt)) {
			if (GNET_PROPERTY(dbg) > 5)
				printf("MATCH: %s\n", shared_file_name_nfc(sf));

			callback(ctx, sf);
			nres++;
			if (nres >= max_res)
				break;
		}
	}

	if (GNET_PROPERTY(dbg) > 6)
		printf("st_search(): scanned %d entry from the %d in bin, %d matches\n",
			scanned, best_bin_size, nres);

	for (i = 0; i < wocnt; i++)
		if (pattern[i])					/* Lazily compiled by entry_match() */
			pattern_free(pattern[i]);

	wfree(pattern, wocnt * sizeof *pattern);
	word_vec_free(wovec, wocnt);

finish:
	if (search != search_term) {
		G_FREE_NULL(search);
	}
}

/* vi: set ts=4 sw=4 cindent: */
