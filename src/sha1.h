/* ================ sha1.h ================ */
/*
SHA-1 in C
By Steve Reid <steve@edmweb.com>
100% Public Domain
*/

typedef struct {
    u_int32_t state[5];
    u_int32_t count[2];
    unsigned char buffer[64];
} SHA1_CTX;

void SHA1Transform(u_int32_t state[5], const unsigned char buffer[64]);//该函数的功能就是进行运算，得出160位的消息摘要（message digest）并储存在context-state[ ]中，它是整个SHA-1算法的核心.
void SHA1Init(SHA1_CTX* context);//初始化这5个字的缓冲区
void SHA1Update(SHA1_CTX* context, const unsigned char* data, u_int32_t len);//完成的数据填充（附加）操作
void SHA1Final(unsigned char digest[20], SHA1_CTX* context);//整个算法的入口与出口。该函数通过调用该文件内其他函数完成了SHA-1算法的整个流程。
