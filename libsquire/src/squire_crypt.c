#include <squire_crypt.h>

#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
#define QR(a, b, c, d) (			\
	a += b,  d ^= a,  d = ROTL(d,16),	\
	c += d,  b ^= c,  b = ROTL(b,12),	\
	a += b,  d ^= a,  d = ROTL(d, 8),	\
	c += d,  b ^= c,  b = ROTL(b, 7))
#define ROUNDS 20
 
void squire_chacha_block(uint32_t out[16], uint32_t const in[16]){
	int i;
	uint32_t x[16];

	for (i = 0; i < 16; ++i)	
		x[i] = in[i];
	// 10 loops × 2 rounds/loop = 20 rounds
	for (i = 0; i < ROUNDS; i += 2) {
		// Odd round
		QR(x[0], x[4], x[ 8], x[12]); // column 0
		QR(x[1], x[5], x[ 9], x[13]); // column 1
		QR(x[2], x[6], x[10], x[14]); // column 2
		QR(x[3], x[7], x[11], x[15]); // column 3
		// Even round
		QR(x[0], x[5], x[10], x[15]); // diagonal 1 (main diagonal)
		QR(x[1], x[6], x[11], x[12]); // diagonal 2
		QR(x[2], x[7], x[ 8], x[13]); // diagonal 3
		QR(x[3], x[4], x[ 9], x[14]); // diagonal 4
	}
	for (i = 0; i < 16; ++i)
		out[i] = x[i] + in[i];
}

extern void squire_chacha(const char key[32], uint64_t pos, uint64_t nonce, uint8_t * buf, size_t length){
    uint32_t in[16];
    uint32_t out[16];
    in[0] = 0x65787061; // expa
    in[1] = 0x6e642033; // nd 3
    in[2] = 0x322d6279; // 2-by
    in[3] = 0x7465206b; // te k
    for(int i=0; i<8*4; i++){
        ((char*)(in+4))[i] = key[i];
    }

    size_t p = 0;
    while(1){
        in[12] = pos>>32;
        in[13] = pos&0xffffffff;
        in[14] = nonce>>32;
        in[15] = nonce&0xffffffff;
        squire_chacha_block(out, in);
        for(int i=0; i<64; i++){
            buf[p+i] ^= ((uint8_t*)out)[i];
            if(p+i>=length) return;
        }
        p += 64;
        pos += 64;
    }
}