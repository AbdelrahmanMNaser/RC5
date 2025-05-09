#include <stdio.h>

/* Define types for clarity */
typedef unsigned int u4;
typedef unsigned char u1;

/* An RC5 context needs to know how many rounds it has, and its subkeys. */
typedef struct
{
    u4 *xk;
    int nr;
} rc5_ctx;

/* Where possible, these should be replaced with actual rotate instructions.
For Turbo C++, this is done with _lrotl and _lrotr. */
#define ROTL32(X, C) (((X) << (C)) | ((X) >> (32 - (C))))
#define ROTR32(X, C) (((X) >> (C)) | ((X) << (32 - (C))))

/* Function prototypes for dealing with RC5 basic operations. */
void rc5_init(rc5_ctx *, int);
void rc5_destroy(rc5_ctx *);
void rc5_key(rc5_ctx *, u1 *, int);
void rc5_encrypt(rc5_ctx *, u4 *, int);
void rc5_decrypt(rc5_ctx *, u4 *, int);

/* Scrub out all sensitive values. */
void rc5_destroy(rc5_ctx *c)
{
    int i;
    for (i = 0; i < (c->nr) * 2 + 2; i++)
        c->xk[i] = 0;
    free(c->xk);
}

/* Allocate memory for rc5 context’s xk and such. */
void rc5_init(rc5_ctx *c, int rounds)
{
    c->nr = rounds;
    c->xk = (u4 *)malloc(4 * (rounds * 2 + 2));
}

void rc5_encrypt(rc5_ctx *c, u4 *data, int blocks)
{
    u4 *d, *sk;
    int h, i, rc;
    d = data;
    sk = (c->xk) + 2;
    for (h = 0; h < blocks; h++)
    {
        d[0] += c->xk[0];
        d[1] += c->xk[1];
        for (i = 0; i < c->nr * 2; i += 2)
        {
            d[0] ^= d[1];
            rc = d[1] & 31;
            d[0] = ROTL32(d[0], rc);
            d[0] += sk[i];
            d[1] ^= d[0];
            rc = d[0] & 31;
            d[1] = ROTL32(d[1], rc);
            d[1] += sk[i + 1];
        }
        d += 2;
    }
}

void rc5_decrypt(rc5_ctx *c, u4 *data, int blocks)
{
    u4 *d, *sk;
    int h, i, rc;
    d = data;
    sk = (c->xk) + 2;
    for (h = 0; h < blocks; h++)
    {
        for (i = c->nr * 2 - 2; i >= 0; i -= 2)
        {
            d[1] -= sk[i + 1];
            rc = d[0] & 31;
            d[1] = ROTR32(d[1], rc);
            d[1] ^= d[0];
            d[0] -= sk[i];
            rc = d[1] & 31;
            d[0] = ROTR32(d[0], rc);
            d[0] ^= d[1];
        }
        d[0] -= c->xk[0];
        d[1] -= c->xk[1];
        d += 2;
    }
}

void rc5_key(rc5_ctx *c, u1 *key, int keylen)
{
    u4 *pk, A, B; /* padded key */
    int xk_len, pk_len, i, num_steps, rc;
    u1 *cp;
    xk_len = c->nr * 2 + 2;
    pk_len = keylen / 4;
    if ((keylen % 4) != 0)
        pk_len += 1;
    pk = (u4 *)malloc(pk_len * 4);
    if (pk == NULL)
    {
        printf("An error occurred!\n");
        exit(-1);
    }
    /* Initialize pk –– this should work on Intel machines, anyway.... */
    for (i = 0; i < pk_len; i++)
        pk[i] = 0;
    cp = (u1 *)pk;
    for (i = 0; i < keylen; i++)
        cp[i] = key[i];
    /* Initialize xk. */
    c->xk[0] = 0xb7e15163; /* P32 */
    for (i = 1; i < xk_len; i++)
        c->xk[i] = c->xk[i - 1] + 0x9e3779b9; /* Q32 */
    /* Expand key into xk. */
    if (pk_len > xk_len)
        num_steps = 3 * pk_len;
    else
        num_steps = 3 * xk_len;
    A = B = 0;
    for (i = 0; i < num_steps; i++)
    {
        A = c->xk[i % xk_len] = ROTL32(c->xk[i % xk_len] + A + B, 3);
        rc = (A + B) & 31;
        B = pk[i % pk_len] = ROTL32(pk[i % pk_len] + A + B, rc);
    }
    /* Clobber sensitive data before deallocating memory. */
    for (i = 0; i < pk_len; i++)
        pk[i] = 0;
    free(pk);
}

void main(void)
{
    rc5_ctx c;
    u4 data[8];
    char key[] = "ABCDE";
    int i;
    printf("-----------------------------------------------\n");
    
    for (i = 0; i < 8; i++)
        data[i] = i;
    
    rc5_init(&c, 10); /* 10 rounds */
    rc5_key(&c, (u1 *)key, 5);
    rc5_encrypt(&c, data, 4);

    printf("Encryptions:\n");
    for (i = 0; i < 8; i += 2)
        printf("Block %01d = %08lx %08lx\n", i / 2, data[i], data[i + 1]);
    
    rc5_decrypt(&c, data, 2);
    rc5_decrypt(&c, data + 4, 2);
   
    printf("Decryptions:\n");
    for (i = 0; i < 8; i += 2)
        printf("Block %01d = %08lx %08lx\n", i / 2, data[i], data[i + 1]);
    rc5_destroy(&c);
}
