/*****************************************************************************

Copyright (c) 1994, 2013, Oracle and/or its affiliates. All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA

*****************************************************************************/

/********************************************************************//**
@file include/page0cur.h
The page cursor

Created 10/4/1994 Heikki Tuuri
*************************************************************************/

#ifndef page0cur_h
#define page0cur_h

#include "univ.i"

#include "buf0types.h"
#include "page0page.h"
#include "rem0rec.h"
#include "data0data.h"
#include "mtr0mtr.h"


#define PAGE_CUR_ADAPT

/* Page cursor search modes; the values must be in this order! */

#define	PAGE_CUR_UNSUPP	0
#define	PAGE_CUR_G	1
#define	PAGE_CUR_GE	2
#define	PAGE_CUR_L	3
#define	PAGE_CUR_LE	4
/*#define PAGE_CUR_LE_OR_EXTENDS 5*/ /* This is a search mode used in
				 "column LIKE 'abc%' ORDER BY column DESC";
				 we have to find strings which are <= 'abc' or
				 which extend it */
#ifdef UNIV_SEARCH_DEBUG
# define PAGE_CUR_DBG	6	/* As PAGE_CUR_LE, but skips search shortcut */
#endif /* UNIV_SEARCH_DEBUG */

#ifdef UNIV_DEBUG
/*********************************************************//**
Gets pointer to the page frame where the cursor is positioned.
@return	page */
UNIV_INLINE
page_t*
page_cur_get_page(
/*==============*/
	page_cur_t*	cur);	/*!< in: page cursor */
/*********************************************************//**
Gets pointer to the buffer block where the cursor is positioned.
@return	page */
UNIV_INLINE
buf_block_t*
page_cur_get_block(
/*===============*/
	page_cur_t*	cur);	/*!< in: page cursor */
/*********************************************************//**
Gets pointer to the page frame where the cursor is positioned.
@return	page */
UNIV_INLINE
page_zip_des_t*
page_cur_get_page_zip(
/*==================*/
	page_cur_t*	cur);	/*!< in: page cursor */
/*********************************************************//**
Gets the record where the cursor is positioned.
@return	record */
UNIV_INLINE
rec_t*
page_cur_get_rec(
/*=============*/
	page_cur_t*	cur);	/*!< in: page cursor */
#else /* UNIV_DEBUG */
# define page_cur_get_page(cur)		page_align((cur)->rec)
# define page_cur_get_block(cur)	(cur)->block
# define page_cur_get_page_zip(cur)	buf_block_get_page_zip((cur)->block)
# define page_cur_get_rec(cur)		(cur)->rec
#endif /* UNIV_DEBUG */
/*********************************************************//**
Sets the cursor object to point before the first user record
on the page. */
UNIV_INLINE
void
page_cur_set_before_first(
/*======================*/
	const buf_block_t*	block,	/*!< in: index page */
	page_cur_t*		cur);	/*!< in: cursor */
/*********************************************************//**
Sets the cursor object to point after the last user record on
the page. */
UNIV_INLINE
void
page_cur_set_after_last(
/*====================*/
	const buf_block_t*	block,	/*!< in: index page */
	page_cur_t*		cur);	/*!< in: cursor */
/*********************************************************//**
Returns TRUE if the cursor is before first user record on page.
@return	TRUE if at start */
UNIV_INLINE
ibool
page_cur_is_before_first(
/*=====================*/
	const page_cur_t*	cur);	/*!< in: cursor */
/*********************************************************//**
Returns TRUE if the cursor is after last user record.
@return	TRUE if at end */
UNIV_INLINE
ibool
page_cur_is_after_last(
/*===================*/
	const page_cur_t*	cur);	/*!< in: cursor */
/**********************************************************//**
Positions the cursor on the given record. */
UNIV_INLINE
void
page_cur_position(
/*==============*/
	const rec_t*		rec,	/*!< in: record on a page */
	const buf_block_t*	block,	/*!< in: buffer block containing
					the record */
	page_cur_t*		cur);	/*!< out: page cursor */
/**********************************************************//**
Moves the cursor to the next record on page. */
UNIV_INLINE
void
page_cur_move_to_next(
/*==================*/
	page_cur_t*	cur);	/*!< in/out: cursor; must not be after last */
/**********************************************************//**
Moves the cursor to the previous record on page. */
UNIV_INLINE
void
page_cur_move_to_prev(
/*==================*/
	page_cur_t*	cur);	/*!< in/out: cursor; not before first */
#ifndef UNIV_HOTBACKUP
/***********************************************************//**
Inserts a record next to page cursor. Returns pointer to inserted record if
succeed, i.e., enough space available, NULL otherwise. The cursor stays at
the same logical position, but the physical position may change if it is
pointing to a compressed page that was reorganized.

IMPORTANT: The caller will have to update IBUF_BITMAP_FREE
if this is a compressed leaf page in a secondary index.
This has to be done either within the same mini-transaction,
or by invoking ibuf_reset_free_bits() before mtr_commit().

@return	pointer to record if succeed, NULL otherwise */
UNIV_INLINE
rec_t*
page_cur_tuple_insert(
/*==================*/
	page_cur_t*	cursor,	/*!< in/out: a page cursor */
	const dtuple_t*	tuple,	/*!< in: pointer to a data tuple */
	dict_index_t*	index,	/*!< in: record descriptor */
	ulint**		offsets,/*!< out: offsets on *rec */
	mem_heap_t**	heap,	/*!< in/out: pointer to memory heap, or NULL */
	ulint		n_ext,	/*!< in: number of externally stored columns */
	mtr_t*		mtr)	/*!< in: mini-transaction handle, or NULL */
	__attribute__((nonnull(1,2,3,4,5), warn_unused_result));
#endif /* !UNIV_HOTBACKUP */
/***********************************************************//**
Inserts a record next to page cursor. Returns pointer to inserted record if
succeed, i.e., enough space available, NULL otherwise. The cursor stays at
the same logical position, but the physical position may change if it is
pointing to a compressed page that was reorganized.

IMPORTANT: The caller will have to update IBUF_BITMAP_FREE
if this is a compressed leaf page in a secondary index.
This has to be done either within the same mini-transaction,
or by invoking ibuf_reset_free_bits() before mtr_commit().

@return	pointer to record if succeed, NULL otherwise */
UNIV_INLINE
rec_t*
page_cur_rec_insert(
/*================*/
	page_cur_t*	cursor,	/*!< in/out: a page cursor */
	const rec_t*	rec,	/*!< in: record to insert */
	dict_index_t*	index,	/*!< in: record descriptor */
	ulint*		offsets,/*!< in/out: rec_get_offsets(rec, index) */
	mtr_t*		mtr);	/*!< in: mini-transaction handle, or NULL */
/***********************************************************//**
Inserts a record next to page cursor on an uncompressed page.
Returns pointer to inserted record if succeed, i.e., enough
space available, NULL otherwise. The cursor stays at the same position.
@return	pointer to record if succeed, NULL otherwise */
UNIV_INTERN
rec_t*
page_cur_insert_rec_low(
/*====================*/
	rec_t*		current_rec,/*!< in: pointer to current record after
				which the new record is inserted */
	dict_index_t*	index,	/*!< in: record descriptor */
	const rec_t*	rec,	/*!< in: pointer to a physical record */
	ulint*		offsets,/*!< in/out: rec_get_offsets(rec, index) */
	mtr_t*		mtr)	/*!< in: mini-transaction handle, or NULL */
	__attribute__((nonnull(1,2,3,4), warn_unused_result));
/***********************************************************//**
Inserts a record next to page cursor on a compressed and uncompressed
page. Returns pointer to inserted record if succeed, i.e.,
enough space available, NULL otherwise.
The cursor stays at the same position.

IMPORTANT: The caller will have to update IBUF_BITMAP_FREE
if this is a compressed leaf page in a secondary index.
This has to be done either within the same mini-transaction,
or by invoking ibuf_reset_free_bits() before mtr_commit().

@return	pointer to record if succeed, NULL otherwise */
UNIV_INTERN
rec_t*
page_cur_insert_rec_zip(
/*====================*/
	page_cur_t*	cursor,	/*!< in/out: page cursor */
	dict_index_t*	index,	/*!< in: record descriptor */
	const rec_t*	rec,	/*!< in: pointer to a physical record */
	ulint*		offsets,/*!< in/out: rec_get_offsets(rec, index) */
	mtr_t*		mtr)	/*!< in: mini-transaction handle, or NULL */
	__attribute__((nonnull(1,2,3,4), warn_unused_result));
/*************************************************************//**
Copies records from page to a newly created page, from a given record onward,
including that record. Infimum and supremum records are not copied.

IMPORTANT: The caller will have to update IBUF_BITMAP_FREE
if this is a compressed leaf page in a secondary index.
This has to be done either within the same mini-transaction,
or by invoking ibuf_reset_free_bits() before mtr_commit(). */
UNIV_INTERN
void
page_copy_rec_list_end_to_created_page(
/*===================================*/
	page_t*		new_page,	/*!< in/out: index page to copy to */
	rec_t*		rec,		/*!< in: first record to copy */
	dict_index_t*	index,		/*!< in: record descriptor */
	mtr_t*		mtr);		/*!< in: mtr */
/***********************************************************//**
Deletes a record at the page cursor. The cursor is moved to the
next record after the deleted one. */
UNIV_INTERN
void
page_cur_delete_rec(
/*================*/
	page_cur_t*		cursor,	/*!< in/out: a page cursor */
	const dict_index_t*	index,	/*!< in: record descriptor */
	const ulint*		offsets,/*!< in: rec_get_offsets(
					cursor->rec, index) */
	mtr_t*			mtr);	/*!< in: mini-transaction handle */
#ifndef UNIV_HOTBACKUP
/****************************************************************//**
Searches the right position for a page cursor.
@return	number of matched fields on the left */
UNIV_INLINE
ulint
page_cur_search(
/*============*/
	const buf_block_t*	block,	/*!< in: buffer block */
	const dict_index_t*	index,	/*!< in: record descriptor */
	const dtuple_t*		tuple,	/*!< in: data tuple */
	ulint			mode,	/*!< in: PAGE_CUR_L,
					PAGE_CUR_LE, PAGE_CUR_G, or
					PAGE_CUR_GE */
	page_cur_t*		cursor);/*!< out: page cursor */
/****************************************************************//**
Searches the right position for a page cursor. */
UNIV_INTERN
void
page_cur_search_with_match(
/*=======================*/
	const buf_block_t*	block,	/*!< in: buffer block */
	const dict_index_t*	index,	/*!< in: record descriptor */
	const dtuple_t*		tuple,	/*!< in: data tuple */
	ulint			mode,	/*!< in: PAGE_CUR_L,
					PAGE_CUR_LE, PAGE_CUR_G, or
					PAGE_CUR_GE */
	ulint*			iup_matched_fields,
					/*!< in/out: already matched
					fields in upper limit record */
	ulint*			iup_matched_bytes,
					/*!< in/out: already matched
					bytes in a field not yet
					completely matched */
	ulint*			ilow_matched_fields,
					/*!< in/out: already matched
					fields in lower limit record */
	ulint*			ilow_matched_bytes,
					/*!< in/out: already matched
					bytes in a field not yet
					completely matched */
	page_cur_t*		cursor);/*!< out: page cursor */
/***********************************************************//**
Positions a page cursor on a randomly chosen user record on a page. If there
are no user records, sets the cursor on the infimum record. */
UNIV_INTERN
void
page_cur_open_on_rnd_user_rec(
/*==========================*/
	buf_block_t*	block,	/*!< in: page */
	page_cur_t*	cursor);/*!< out: page cursor */
#endif /* !UNIV_HOTBACKUP */
/***********************************************************//**
Parses a log record of a record insert on a page.
@return	end of log record or NULL */
UNIV_INTERN
byte*
page_cur_parse_insert_rec(
/*======================*/
	ibool		is_short,/*!< in: TRUE if short inserts */
	byte*		ptr,	/*!< in: buffer */
	byte*		end_ptr,/*!< in: buffer end */
	buf_block_t*	block,	/*!< in: page or NULL */
	dict_index_t*	index,	/*!< in: record descriptor */
	mtr_t*		mtr);	/*!< in: mtr or NULL */
/**********************************************************//**
Parses a log record of copying a record list end to a new created page.
@return	end of log record or NULL */
UNIV_INTERN
byte*
page_parse_copy_rec_list_to_created_page(
/*=====================================*/
	byte*		ptr,	/*!< in: buffer */
	byte*		end_ptr,/*!< in: buffer end */
	buf_block_t*	block,	/*!< in: page or NULL */
	dict_index_t*	index,	/*!< in: record descriptor */
	mtr_t*		mtr);	/*!< in: mtr or NULL */
/***********************************************************//**
Parses log record of a record delete on a page.
@return	pointer to record end or NULL */
UNIV_INTERN
byte*
page_cur_parse_delete_rec(
/*======================*/
	byte*		ptr,	/*!< in: buffer */
	byte*		end_ptr,/*!< in: buffer end */
	buf_block_t*	block,	/*!< in: page or NULL */
	dict_index_t*	index,	/*!< in: record descriptor */
	mtr_t*		mtr);	/*!< in: mtr or NULL */
/*******************************************************//**
Removes the record from a leaf page. This function does not log
any changes. It is used by the IMPORT tablespace functions.
@return	true if success, i.e., the page did not become too empty */
UNIV_INTERN
bool
page_delete_rec(
/*============*/
	const dict_index_t*	index,	/*!< in: The index that the record
					belongs to */
	page_cur_t*		pcur,	/*!< in/out: page cursor on record
					to delete */
	page_zip_des_t*		page_zip,/*!< in: compressed page descriptor */
	const ulint*		offsets);/*!< in: offsets for record */

/** Index page cursor */

struct page_cur_t{
	const dict_index_t*	index;
	rec_t*		rec;	/*!< pointer to a record on page */
	ulint*		offsets;
	buf_block_t*	block;	/*!< pointer to the block containing rec */
};

/** Index page cursor */
class PageCur
{
public:
	/** Constructor
	@param[in/out]	mtr	mini-transaction
	@param[in]	index	B-tree index
	@param[in/out]	block	B-tree page
	@param[in]	rec	B-tree record in block */
	PageCur(mtr_t& mtr, dict_index_t& index,
		const buf_block_t& block, const rec_t* rec)
		: mtr (&mtr), index (&index), block (&block), rec (rec),
		  offsets (0) {
		ut_ad(rec);
		if (dict_table_is_comp(this->index->table)) {
			init();
		}
	}

	/** Constructor.
	Initializes the cursor to point to the page infimum.
	@param[in/out]	mtr	mini-transaction
	@param[in]	index	B-tree index
	@param[in/out]	block	B-tree page */
	PageCur(mtr_t& mtr, dict_index_t& index, buf_block_t& block)
		: mtr (&mtr), index (&index), block (&block),
		  rec (0), offsets (0) {
		if (dict_table_is_comp(this->index->table)) {
			init();
		}
	}

	/** Destructor */
	~PageCur() { delete[] offsets; }

	/** Get the mini-transaction */
	mtr_t* getMtr() const { return(mtr); }

	/** Get the record */
	const rec_t* getRec() const {
		ut_ad(rec);
		ut_ad(!offsets == !dict_table_is_comp(index->table));
		ut_ad(!offsets || rec_offs_validate(rec, index, offsets));
		return(rec);
	}

	/** Get the offsets */
	const ulint* getOffsets() const {
		ut_ad(rec_offs_validate(rec, index, offsets));
		return(offsets);
	}

	/** Determine if the cursor is positioned on a user record. */
	bool isUser() const { return(page_rec_is_user_rec(rec)); }
	/** Determine if the cursor is before the first user record. */
	bool isBeforeFirst() const { return(page_rec_is_infimum(rec)); }
	/** Determine if the cursor is after the last user record. */
	bool isAfterLast() const { return(page_rec_is_supremum(rec)); }

	/** Move to the next record.
	@return true if the next record is a user record */
	bool next() {
		bool wasSentinel = isBeforeFirst();
		ut_ad(!isAfterLast());
		ut_ad(!offsets == !dict_table_is_comp(index->table));
		rec = page_rec_get_next_const(rec);

		if (!offsets) {
			return(!isAfterLast());
		} else if (isAfterLast()) {
			adjustSentinelOffsets();
			return(false);
		} else {
			adjustOffsets(wasSentinel);
			return(true);
		}
	}

	/** Move to the previous record.
	@return true if the previous record is a user record */
	bool prev() {
		bool wasSentinel = isAfterLast();
		ut_ad(!isBeforeFirst());
		rec = page_rec_get_prev_const(rec);

		if (!offsets) {
			return(!isBeforeFirst());
		} else if (isBeforeFirst()) {
			adjustSentinelOffsets();
			return(false);
		} else {
			adjustOffsets(wasSentinel);
			return(true);
		}
	}

	/** Insert the record that the page cursor is pointing to,
	to another page.
	@param[in/out] rec	record after which to insert
	@return	pointer to record if enough space available, NULL otherwise */
	rec_t* insert (rec_t* current) const;

private:
	/** Copy constructor */
	PageCur(const PageCur&);
	/** Assignment operator */
	PageCur& operator== (const PageCur&);
	/** Initialize a page cursor, either to rec or the page infimum.
	Allocates and initializes offsets[]. */
	void init(void);

	/** Adjust rec_get_offsets() after moving the cursor. */
	void adjustOffsets(bool wasSentinel) {
		ut_ad(isUser());
		ut_ad(offsets);
		ut_ad(dict_table_is_comp(index->table));

		if (wasSentinel) {
			rec_offs_set_n_fields(
				offsets,
				page_is_leaf(page_align(rec))
				? dict_index_get_n_fields(index)
				: dict_index_get_n_unique_in_tree(index) + 1);
recalc:
			ut_ad(rec_offs_n_fields(offsets)
			      + (1 + REC_OFFS_HEADER_SIZE)
			      == rec_offs_get_n_alloc(offsets));
			rec_init_offsets(rec, index, offsets);
		} else {
			ut_ad(rec_offs_n_fields(offsets)
			      == page_is_leaf(page_align(rec))
			      ? dict_index_get_n_fields(index)
			      : dict_index_get_n_unique_in_tree(index) + 1);
			/* TODO: optimize.
			(remove index->trx_id_offset,
			add ifield->fixed_offset) */
			goto recalc;
		}
	}

	/** Adjust rec_get_offsets() after moving the cursor to the
	page infimum or page supremum. */
	void adjustSentinelOffsets() {
		ut_ad(!isUser());
		ut_ad(offsets);
		ut_ad(dict_table_is_comp(index->table));

		rec_offs_set_n_fields(offsets, 1);

		if (page_rec_is_comp(rec)) {
			rec_offs_base(offsets)[0]
				= REC_N_NEW_EXTRA_BYTES | REC_OFFS_COMPACT;
			rec_offs_base(offsets)[1] = 8;
			rec_offs_make_valid(rec, index, offsets);
		} else {
			rec_init_offsets(rec, index, offsets);
		}
	}

	/** The mini-transaction */
	mtr_t*			mtr;
	/** The index B-tree */
	dict_index_t*const	index;
	/** The page the cursor is positioned on */
	const buf_block_t*	block;
	/** Cursor position (current record) */
	const rec_t*		rec;
	/** Offsets to the record fields. NULL for ROW_FORMAT=REDUNDANT. */
	ulint*			offsets;
};

#ifndef UNIV_NONINL
#include "page0cur.ic"
#endif

#endif
