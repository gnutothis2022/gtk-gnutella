/*
 * $Id$
 *
 * Copyright (c) 2002, Raphael Manfredi
 *
 * Unicode Transformation Format 8 bits.
 *
 * This code has been heavily inspired by utf8.c/utf8.h from Perl 5.6.1,
 * written by Larry Wall et al.
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

#include <string.h>

#include "utf8.h"

/*
 * How wide is an UTF-8 encoded char, depending on its first byte?
 */
static guint8 utf8len[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 000-015: ASCII */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 016-031: ASCII */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 032-047: ASCII */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 048-063: ASCII */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 064-079: ASCII */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 080-095: ASCII */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 096-111: ASCII */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 112-127: ASCII */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 128-143: invalid! */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 128-159: invalid! */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 160-175: invalid! */
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		/* 176-191: invalid! */
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,		/* 192-207 */
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,		/* 208-223 */
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,		/* 224-239 */
	4,4,4,4,4,4,4,4,5,5,5,5,6,6,			/* 240-253 */
	7,7										/* 254-255: special */
};

#define UTF8_SKIP(s)	utf8len[*((guchar *) s)]

/*
 * The following table is from Unicode 3.1.
 *
 * Code Points           1st Byte    2nd Byte    3rd Byte    4th Byte
 *
 *    U+0000..U+007F      00..7F   
 *    U+0080..U+07FF      C2..DF      80..BF   
 *    U+0800..U+0FFF      E0          A0..BF      80..BF  
 *    U+1000..U+FFFF      E1..EF      80..BF      80..BF  
 *   U+10000..U+3FFFF     F0          90..BF      80..BF      80..BF
 *   U+40000..U+FFFFF     F1..F3      80..BF      80..BF      80..BF
 *  U+100000..U+10FFFF    F4          80..8F      80..BF      80..BF

 */

#define CHAR(x)					((guchar) (x))
#define UTF8_IS_ASCII(x)		(CHAR(x) < 0x80)
#define UTF8_IS_START(x)		(CHAR(x) >= 0xc0 && CHAR(x) <= 0xfd)
#define UTF8_IS_CONTINUATION(x)	(CHAR(x) >= 0x80 && CHAR(x) <= 0xbf)
#define UTF8_IS_CONTINUED(x)	(CHAR(x) & 0x80)

#define UTF8_CONT_MASK			(CHAR(0x3f))
#define UTF8_ACCU_SHIFT			6
#define UTF8_ACCUMULATE(o,n)	\
	(((o) << UTF8_ACCU_SHIFT) | (CHAR(n) & UTF8_CONT_MASK))

#define UNISKIP(v) (			\
	(v) <  0x80 		? 1 :	\
	(v) <  0x800 		? 2 :	\
	(v) <  0x10000 		? 3 :	\
	(v) <  0x200000		? 4 :	\
	(v) <  0x4000000	? 5 :	\
	(v) <  0x80000000	? 6 : 7)

#define UNI_SURROGATE_FIRST		0xd800
#define UNI_SURROGATE_LAST		0xdfff
#define UNI_REPLACEMENT			0xfffd
#define UNI_BYTE_ORDER_MARK		0xfffe
#define UNI_ILLEGAL				0xffff

#define UNICODE_IS_SURROGATE(x)	\
	((x) >= UNI_SURROGATE_FIRST && (x) <= UNI_SURROGATE_LAST)

#define UNICODE_IS_REPLACEMENT(x)		((x) == UNI_REPLACEMENT)
#define UNICODE_IS_BYTE_ORDER_MARK(x)	((x) == UNI_BYTE_ORDER_MARK)
#define UNICODE_IS_ILLEGAL(x)			((x) == UNI_ILLEGAL)

/*
 * utf8_is_valid_char
 *
 * Are the first bytes of string `s' forming a valid UTF-8 character?
 * Returns amount of bytes used to encode that character, or 0 if invalid.
 */
gint utf8_is_valid_char(guchar *s)
{
	guchar u = *s;
	gint len;
	gint slen;
	guint32 v;
	guint32 ov;

	if (UTF8_IS_ASCII(u))
		return 1;

	if (!UTF8_IS_START(u))
		return 0;

	len = UTF8_SKIP(s);

	if (len < 2 || !UTF8_IS_CONTINUATION(s[1]))
		return 0;

	for (slen = len -1, s++, ov = v = u; slen; slen--, s++, ov = v) {
		if (!UTF8_IS_CONTINUATION(*s))
			return 0;
		v = UTF8_ACCUMULATE(v, *s);
		if (v < ov)
			return 0;
	}

	if (UNISKIP(v) < len)
		return 0;

	return len;
}

/*
 * utf8_is_valid_string
 *
 * Returns amount of UTF-8 chars when first `len' bytes of the given string
 * `s' form valid a UTF-8 string, 0 meaning the string is not valid UTF-8.
 *
 * If `len' is 0, the length is computed with strlen().
 */
gint utf8_is_valid_string(guchar *s, gint len)
{
	guchar *x = s;
	guchar *send;
	gint n = 0;

	if (!len)
		len = strlen(s);
	send = s + len;

	while (x < send) {
		gint clen = utf8_is_valid_char(x);
		if (clen == 0)
			return 0;
		x += clen;
		n++;
	}

	if (x != send)
		return 0;

	return n;
}

/*
 * utf8_decode_char
 *
 * Returns the character value of the first character in the string `s',
 * which is assumed to be in UTF-8 encoding and no longer than `len'.
 * `retlen' will be set to the length, in bytes, of that character.
 *
 * If `s' does not point to a well-formed UTF-8 character, the behaviour
 * is dependent on the value of `warn'.  When FALSE, it is assumed that
 * the caller will raise a warning, and this function will silently just
 * set `retlen' to -1 and return zero.
 */
guint32 utf8_decode_char(guchar *s, gint len, gint *retlen, gboolean warn)
{
	guint32 v = *s;
	guint32 ov = 0;
	gint clen = 1;
	gint expectlen = 0;
	gint warning = -1;
	char msg[128];

	g_assert(s);

#define UTF8_WARN_EMPTY				0
#define UTF8_WARN_CONTINUATION		1
#define UTF8_WARN_NON_CONTINUATION	2
#define UTF8_WARN_FE_FF				3
#define UTF8_WARN_SHORT				4
#define UTF8_WARN_OVERFLOW			5
#define UTF8_WARN_SURROGATE			6
#define UTF8_WARN_BOM				7
#define UTF8_WARN_LONG				8
#define UTF8_WARN_FFFF				9

	if (len == 0) {
		warning = UTF8_WARN_EMPTY;
		goto malformed;
	}

	if (UTF8_IS_ASCII(v)) {
		if (retlen)
			*retlen = 1;
		return *s;
	}

	if (UTF8_IS_CONTINUATION(v)) {
		warning = UTF8_WARN_CONTINUATION;
		goto malformed;
	}

	if (UTF8_IS_START(v) && len > 1 && !UTF8_IS_CONTINUATION(s[1])) {
		warning = UTF8_WARN_NON_CONTINUATION;
		goto malformed;
	}

	if (v == 0xfe || v == 0xff) {
		warning = UTF8_WARN_FE_FF;
		goto malformed;
	}

	if      (!(v & 0x20)) { clen = 2; v &= 0x1f; }
	else if (!(v & 0x10)) { clen = 3; v &= 0x0f; }
	else if (!(v & 0x08)) { clen = 4; v &= 0x07; }
	else if (!(v & 0x04)) { clen = 5; v &= 0x03; }
	else if (!(v & 0x02)) { clen = 6; v &= 0x01; }
	else if (!(v & 0x01)) { clen = 7; v = 0; }

	if (retlen)
		*retlen = clen;

	expectlen = clen;

	if (len < expectlen) {
		warning = UTF8_WARN_SHORT;
		goto malformed;
	}

	for (clen--, s++, ov = v; clen; clen--, s++, ov = v) {
		if (!UTF8_IS_CONTINUATION(*s)) {
			s--;
			warning = UTF8_WARN_NON_CONTINUATION;
			goto malformed;
		} else
			v = UTF8_ACCUMULATE(v, *s);

		if (v < ov) {
			warning = UTF8_WARN_OVERFLOW;
			goto malformed;
		} else if (v == ov) {
			warning = UTF8_WARN_LONG;
			goto malformed;
		}
	}

	if (UNICODE_IS_SURROGATE(v)) {
		warning = UTF8_WARN_SURROGATE;
		goto malformed;
	} else if (UNICODE_IS_BYTE_ORDER_MARK(v)) {
		warning = UTF8_WARN_BOM;
		goto malformed;
	} else if (expectlen > UNISKIP(v)) {
		warning = UTF8_WARN_LONG;
		goto malformed;
	} else if (UNICODE_IS_ILLEGAL(v)) {
		warning = UTF8_WARN_FFFF;
		goto malformed;
	}

	return v;

malformed:

	if (!warn) {
		if (retlen)
			*retlen = -1;
		return 0;
	}

	switch (warning) {
	case UTF8_WARN_EMPTY:
		g_snprintf(msg, sizeof(msg), "empty string");
		break;
	case UTF8_WARN_CONTINUATION:
		g_snprintf(msg, sizeof(msg),
			"unexpected continuation byte 0x%02lu", (gulong) v);
		break;
	case UTF8_WARN_NON_CONTINUATION:
		g_snprintf(msg, sizeof(msg),
			"unexpected non-continuation byte 0x%02lu "
			"after start byte 0x%02ld", (gulong) s[1], (gulong) v);
		break;
	case UTF8_WARN_FE_FF:
		g_snprintf(msg, sizeof(msg), "byte 0x%02lu", (gulong) v);
		break;
	case UTF8_WARN_SHORT:
		g_snprintf(msg, sizeof(msg), "%d byte%s, need %d",
			len, len == 1 ? "" : "s", expectlen);
		break;
	case UTF8_WARN_OVERFLOW:
		g_snprintf(msg, sizeof(msg), "overflow at 0x%02lu, byte 0x%02lu",
			(gulong) ov, (gulong) *s);
		break;
	case UTF8_WARN_SURROGATE:
		g_snprintf(msg, sizeof(msg), "UTF-16 surrogate 0x04%lu", (gulong) v);
		break;
	case UTF8_WARN_BOM:
		g_snprintf(msg, sizeof(msg), "byte order mark 0x%04lu", (gulong) v);
		break;
	case UTF8_WARN_LONG:
		g_snprintf(msg, sizeof(msg), "%d byte%s, need %d",
			expectlen, expectlen == 1 ? "" : "s", UNISKIP(v));
		break;
	case UTF8_WARN_FFFF:
		g_snprintf(msg, sizeof(msg), "character 0x%04lu", (gulong) v);
		break;
	default:
		g_snprintf(msg, sizeof(msg), "unknown reason");
		break;
	}

	g_warning("malformed UTF-8 character: %s", msg);

	if (retlen)
		*retlen = expectlen ? expectlen : len;

	return 0;
}

/*
 * utf8_to_iso8859
 *
 * Convert UTF-8 string to ISO-8859-1 inplace.  If `space' is TRUE, all
 * characters outside the U+0000 .. U+00FF range are turned to space U+0020.
 * Otherwise, we stop at the first out-of-range character.
 *
 * If `len' is 0, the length of the string is computed with strlen().
 *
 * Returns length of decoded string.
 */
gint utf8_to_iso8859(guchar *s, gint len, gboolean space)
{
	guchar *x = s;
	guchar *xw = s;			/* Where we write back ISO-8859 chars */
	guchar *send;

	if (!len)
		len = strlen(s);
	send = s + len;

	while (x < send) {
		gint clen;
		guint32 v = utf8_decode_char(x, len, &clen, FALSE);

		if (clen == -1)
			break;

		g_assert(clen >= 1);

		if (v & 0xffffff00) {	/* Not an ISO-8859-1 character */
			if (!space)
				break;
			v = 0x20;
		}

		*xw++ = (guchar) v;
		x += clen;
		len -= clen;
	}

	*xw = '\0';

	return xw - s;
}

