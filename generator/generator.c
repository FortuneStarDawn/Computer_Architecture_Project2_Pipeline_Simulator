#include <stdio.h>
int encode(unsigned int buff);
//78563412
int main()
{
    int buff;
    FILE *iimage, *dimage;
    iimage = fopen("iimage.bin", "wb");
    dimage = fopen("dimage.bin", "wb");
    buff = encode(0x00000008);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x00000029);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x14010002);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x3C017FFF);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x0C000000);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x0C000006);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x23FE0008);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x03DEF008);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x0C000009);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x0C00000A);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xAC1E0000);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xAC1F0004);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x8C1F0000);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x03E00000);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x001FF020);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x0000F020);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x03DEF020);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x0000E820);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x17BE0000);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x3421FFFF);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x00210020);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x00011040);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x00021902);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x00022103);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x2845F000);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xAC0103FC);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x840603FE);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x940703FE);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x800803FD);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x900903FD);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x2401ABCD);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xA00103FE);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x8C0103FC);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x00000820);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x3C018000);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x00211022);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x00210820);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0x840003FF);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xFC210800);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xFC210800);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xFC210800);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xFC210800);
    fwrite(&buff, sizeof(int), 1, iimage);
    buff = encode(0xFC210800);
    fwrite(&buff, sizeof(int), 1, iimage);

    buff = encode(0x00000000);
    fwrite(&buff, sizeof(int), 1, dimage);
    buff = encode(0x00000001);
    fwrite(&buff, sizeof(int), 1, dimage);
    buff = encode(0x00000000);
    fwrite(&buff, sizeof(int), 1, dimage);

    return 0;
}

int encode(unsigned int origin)
{
    int answer=0;
    answer |= origin>>24;
    answer |= origin>>8&0xFF00;
    answer |= origin<<8&0xFF0000;
    answer |= origin<<24;
    return answer;
}
