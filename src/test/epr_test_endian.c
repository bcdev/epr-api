#include <stdio.h>
#include "../epr_api.h"

#define BE_MAGIC_NUMBER  1162761801UL
#define LE_MAGIC_NUMBER  1230392901UL
#define LE_MAGIC_BYTE_0  'E'
#define LE_MAGIC_BYTE_1  'N'
#define LE_MAGIC_BYTE_2  'V'
#define LE_MAGIC_BYTE_3  'I'

int epr_is_little_endian_order(void)
{
    uint le_value = LE_MAGIC_NUMBER;
    return (((uchar*)(&le_value))[0] == LE_MAGIC_BYTE_0)
        && (((uchar*)(&le_value))[1] == LE_MAGIC_BYTE_1)
        && (((uchar*)(&le_value))[2] == LE_MAGIC_BYTE_2)
        && (((uchar*)(&le_value))[3] == LE_MAGIC_BYTE_3);
}

int epr_is_big_endian_order(void)
{
    uint be_value = BE_MAGIC_NUMBER;
    return (((uchar*)(&be_value))[0] == LE_MAGIC_BYTE_0)
        && (((uchar*)(&be_value))[1] == LE_MAGIC_BYTE_1)
        && (((uchar*)(&be_value))[2] == LE_MAGIC_BYTE_2)
        && (((uchar*)(&be_value))[3] == LE_MAGIC_BYTE_3);
}


int main(int argc, char** argv)
{
    if (epr_is_little_endian_order())
    {
        printf("LE\n");
    }
    else if (epr_is_big_endian_order())
    {
        printf("BE\n");
    }
    else
    {
        printf("?\n");
    }
    return 0;
}
