// "MostSignificantByteFirst"
short msb_bigendian[] = { 0x0000, 0x4d6f, 0x7374, 0x5369, 0x676e, 0x6966, 0x6963, 0x616e, 0x7442, 0x7974, 0x6546, 0x6972, 0x7374, 0x0000 };

// "LeastSignificantByteFirst"
short lsb_littleendian[] = { 0x0000, 0x654c, 0x7361, 0x5374, 0x6769, 0x696e, 0x6966, 0x6163, 0x746e, 0x7942, 0x6574, 0x6946, 0x7372, 0x0074, 0x0000 };

int main(int, char **)
{
    // make sure the linker doesn't throw away the arrays
    char *msb_bigendian_string = (char *) msb_bigendian;
    char *lsb_littleendian_string = (char *) lsb_littleendian;
    (void) msb_bigendian_string;
    (void) lsb_littleendian_string;
    return msb_bigendian[1] == lsb_littleendian[1];
}
