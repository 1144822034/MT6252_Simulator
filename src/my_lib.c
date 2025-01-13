void my_memcpy(void *dest, void *src, int size);

void my_memset(void *dest, char value, int len);
u8 my_mem_compare(u8 *src, u8 *dest, u32 len);

/**
 * @brief 内存拷贝
 * @param dest 目标地址
 * @param src 源地址
 */
void my_memcpy(void *dest, void *src, int len)
{
    memcpy(dest, src, len);
}

void my_memset(void *dest, char value, int len)
{
    memset(dest, value, len);
}

/**
 * 内存比较 1 相等 0 不相等
 */
u8 my_mem_compare(u8 *src, u8 *dest, u32 len)
{
    while (*src++ == *dest++)
    {
        len--;
        if (len == 0)
            return 1;
    }
    return 0;
}