#include <iostream>
#include <cstdlib>
#include <cstring>


class CCRC32
{
public:
    CCRC32() { initialize(); }

    unsigned long FullCRC(const unsigned char *sData, unsigned long ulDataLength)
    {
        unsigned long ulCRC = 0xffffffff;
        PartialCRC(&ulCRC, sData, ulDataLength);
        return(ulCRC ^ 0xffffffff);
    }

    void PartialCRC(unsigned long *ulCRC, const unsigned char *sData, unsigned long ulDataLength)
    {
        while(ulDataLength--) {
            *ulCRC = (*ulCRC >> 8) ^ ulTable[(*ulCRC & 0xFF) ^ *sData++];
        }
    }

private:
    void initialize(void)
    {
        unsigned long ulPolynomial = 0x04C11DB7;
        memset(&ulTable, 0, sizeof(ulTable));
        for(int iCodes = 0; iCodes <= 0xFF; iCodes++) {
            ulTable[iCodes] = Reflect(iCodes, 8) << 24;
            for(int iPos = 0; iPos < 8; iPos++) {
                ulTable[iCodes] = (ulTable[iCodes] << 1)
                    ^ ((ulTable[iCodes] & (1 << 31)) ? ulPolynomial : 0);
            }

            ulTable[iCodes] = Reflect(ulTable[iCodes], 32);
        }
    }
    unsigned long Reflect(unsigned long ulReflect, const char cChar)
    {
        unsigned long ulValue = 0;
        // Swap bit 0 for bit 7, bit 1 For bit 6, etc....
        for(int iPos = 1; iPos < (cChar + 1); iPos++) {
            if(ulReflect & 1) {
                ulValue |= (1 << (cChar - iPos));
            }
            ulReflect >>= 1;
        }
        return ulValue;
    }
    unsigned long ulTable[256]; // CRC lookup table array.
};


int main(int argc, char **argv)
{
    CCRC32 crc;
    char *name;
    if (argc < 2) {
        std::cerr << "usage: crc <string>\n";
        return 0;
    } else {
        name = argv[1];
    }
    std::cout << crc.FullCRC((unsigned char *)name, strlen(name)) << std::endl;
}
