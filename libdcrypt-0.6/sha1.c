/*
 *
 * Copyright (C) 1998 David Mazieres (dm@uun.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 */

/* This file comes from a public domain SHA-1 implementation by Steve
 * Reid, with numerous performance enhancements added by Emmett
 * Witchel. */

/*
   Test Vectors (from FIPS PUB 180-1)
   "abc"
   A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
   "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
   84983E44 1C3BD26E BAAE4AA1 F95129E5 E54670F1
   A million repetitions of "a"
   34AA973C D4C4DAA4 F61EEB2B DBAD2731 6534016F
 */


#include "dcinternal.h"

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
/* number of bytes in sha1 block */
#define SHA_BLKSIZE 64

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#define blk0(i) (tmp[i] = getint (&block[4*i]))
#define blk(i) (tmp[i&15] = rol(tmp[(i+13)&15]^tmp[(i+8)&15] \
    ^tmp[(i+2)&15]^tmp[i&15],1))


/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5); \
     w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5); \
     w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5); \
     w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5); \
     w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

enum { blocksize = 64 };

/* Initialize new context */
void
sha1_newstate (u_int32_t state[5])
{
  /* SHA1 initialization constants */
  state[0] = 0x67452301;
  state[1] = 0xefcdab89;
  state[2] = 0x98badcfe;
  state[3] = 0x10325476;
  state[4] = 0xc3d2e1f0;
}

/* Hash a single 512-bit block. This is the core of the algorithm. */
void
sha1_transform (u_int32_t state[5],
		const u_char block[64])
{
  register u_int32_t a, b, c, d, e;
  u_int32_t tmp[16];

  /* Copy context->state[] to working vars */
  a = state[0];
  b = state[1];
  c = state[2];
  d = state[3];
  e = state[4];
  /* 4 rounds of 20 operations each. Loop unrolled. */
  /* Copy from block to tmp folded into computation */
  R0 (a, b, c, d, e, 0); R0 (e, a, b, c, d, 1); R0 (d, e, a, b, c, 2);
  R0 (c, d, e, a, b, 3); R0 (b, c, d, e, a, 4); R0 (a, b, c, d, e, 5);
  R0 (e, a, b, c, d, 6); R0 (d, e, a, b, c, 7); R0 (c, d, e, a, b, 8);
  R0 (b, c, d, e, a, 9); R0 (a, b, c, d, e, 10); R0 (e, a, b, c, d, 11);
  R0 (d, e, a, b, c, 12); R0 (c, d, e, a, b, 13); R0 (b, c, d, e, a, 14);
  R0 (a, b, c, d, e, 15); R1 (e, a, b, c, d, 16); R1 (d, e, a, b, c, 17);
  R1 (c, d, e, a, b, 18); R1 (b, c, d, e, a, 19); R2 (a, b, c, d, e, 20);
  R2 (e, a, b, c, d, 21); R2 (d, e, a, b, c, 22); R2 (c, d, e, a, b, 23);
  R2 (b, c, d, e, a, 24); R2 (a, b, c, d, e, 25); R2 (e, a, b, c, d, 26);
  R2 (d, e, a, b, c, 27); R2 (c, d, e, a, b, 28); R2 (b, c, d, e, a, 29);
  R2 (a, b, c, d, e, 30); R2 (e, a, b, c, d, 31); R2 (d, e, a, b, c, 32);
  R2 (c, d, e, a, b, 33); R2 (b, c, d, e, a, 34); R2 (a, b, c, d, e, 35);
  R2 (e, a, b, c, d, 36); R2 (d, e, a, b, c, 37); R2 (c, d, e, a, b, 38);
  R2 (b, c, d, e, a, 39); R3 (a, b, c, d, e, 40); R3 (e, a, b, c, d, 41);
  R3 (d, e, a, b, c, 42); R3 (c, d, e, a, b, 43); R3 (b, c, d, e, a, 44);
  R3 (a, b, c, d, e, 45); R3 (e, a, b, c, d, 46); R3 (d, e, a, b, c, 47);
  R3 (c, d, e, a, b, 48); R3 (b, c, d, e, a, 49); R3 (a, b, c, d, e, 50);
  R3 (e, a, b, c, d, 51); R3 (d, e, a, b, c, 52); R3 (c, d, e, a, b, 53);
  R3 (b, c, d, e, a, 54); R3 (a, b, c, d, e, 55); R3 (e, a, b, c, d, 56);
  R3 (d, e, a, b, c, 57); R3 (c, d, e, a, b, 58); R3 (b, c, d, e, a, 59);
  R4 (a, b, c, d, e, 60); R4 (e, a, b, c, d, 61); R4 (d, e, a, b, c, 62);
  R4 (c, d, e, a, b, 63); R4 (b, c, d, e, a, 64); R4 (a, b, c, d, e, 65);
  R4 (e, a, b, c, d, 66); R4 (d, e, a, b, c, 67); R4 (c, d, e, a, b, 68);
  R4 (b, c, d, e, a, 69); R4 (a, b, c, d, e, 70); R4 (e, a, b, c, d, 71);
  R4 (d, e, a, b, c, 72); R4 (c, d, e, a, b, 73); R4 (b, c, d, e, a, 74);
  R4 (a, b, c, d, e, 75); R4 (e, a, b, c, d, 76); R4 (d, e, a, b, c, 77);
  R4 (c, d, e, a, b, 78); R4 (b, c, d, e, a, 79);
  /* Add the working vars back into context.state[] */
  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  state[4] += e;
}

void
sha1_state2bytes (void *_cp, const u_int32_t state[5])
{
  u_char *cp = (u_char *) (_cp);
  size_t i;
  for (i = 0; i < 5; i++) {
    u_int32_t v = *state++;
    cp[0] = v >> 24;
    cp[1] = v >> 16;
    cp[2] = v >> 8;
    cp[3] = v;
    cp += 4;
  }
}

static void
sha1_consume (mdblock *_mdb, const u_char block[64])
{
  sha1_ctx *sc = (sha1_ctx *) _mdb;
  sha1_transform (sc->state, block);
}

void
sha1_init (sha1_ctx *sc)
{
  mdblock_init (&sc->mdb, sha1_consume);
  sha1_newstate (sc->state);
}

void
sha1_update (sha1_ctx *sc, const void *bytes, size_t len)
{
  mdblock_update (&sc->mdb, bytes, len);
}

void
sha1_final (sha1_ctx *sc, u_char out[20])
{
  mdblock_finish (&sc->mdb, 1);
  sha1_state2bytes (out, sc->state);
  bzero (sc->state, sizeof (sc->state));
}

void
sha1_hash (void *digest, const void *buf, size_t len)
{
  sha1_ctx sc;

  sha1_init (&sc);
  sha1_update (&sc, buf, len);
  sha1_final (&sc, digest);
}

void
hmac_sha1 (const char *key, size_t keylen, 
	   void *out, const void *data, size_t dlen)
{
  char block[64];
  sha1_ctx sc;
  unsigned i;

  sha1_init (&sc);
  memset (block, 0x36, sizeof (block));
  for (i = 0; i < sizeof (block) && (i < keylen); i++)
    block[i] ^= key[i];
  sha1_update (&sc, block, sizeof (block));
  sha1_update (&sc, data, dlen);
  sha1_final (&sc, out);

  sha1_init (&sc);
  for (i = 0; i < sizeof (block); i++)
    block[i] ^= 0x36 ^ 0x5c;
  sha1_update (&sc, block, sizeof (block));
  sha1_update (&sc, out, 20);
  sha1_final (&sc, out);
}

void
hmac_sha1_init (const char *key, size_t keylen, sha1_ctx *sc)
{
  char block[64];
  unsigned i;

  sha1_init (sc);
  memset (block, 0x36, sizeof (block));
  for (i = 0; (i < sizeof (block)) && (i < keylen); i++)
    block[i] ^= key[i];
  sha1_update (sc, block, sizeof (block));
}

void
hmac_sha1_final (const char *key, size_t keylen, sha1_ctx *scp, u_char out[20])
{ 
  char block[64];
  unsigned i;

  assert (scp);
  sha1_final (scp, out);

  sha1_init (scp);
  memset (block, 0x45, sizeof (block));
  for (i = 0; (i < sizeof (block)) && (i < keylen); i++)
    block[i] ^= key[i];
  sha1_update (scp, block, sizeof (block));
  sha1_update (scp, out, 20);
  sha1_final (scp, out);
}
