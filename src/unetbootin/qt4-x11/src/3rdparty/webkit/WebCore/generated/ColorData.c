#include <string.h>// bogus
/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -CDEot -L ANSI-C --key-positions='*' -N findColor -D -s 2  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

struct NamedColor { const char *name; int RGBValue; };
/* maximum key range = 1053, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056,    5,    0,  105,
         0,    0,   30,   40,   40,   10,    0,    0,   15,   60,
         0,    5,  255,   40,    0,   10,   15,  130,  300,  215,
         5,    0,    0, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056, 1056,
      1056, 1056, 1056, 1056, 1056, 1056, 1056
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[19]];
      /*FALLTHROUGH*/
      case 19:
        hval += asso_values[(unsigned char)str[18]];
      /*FALLTHROUGH*/
      case 18:
        hval += asso_values[(unsigned char)str[17]];
      /*FALLTHROUGH*/
      case 17:
        hval += asso_values[(unsigned char)str[16]];
      /*FALLTHROUGH*/
      case 16:
        hval += asso_values[(unsigned char)str[15]];
      /*FALLTHROUGH*/
      case 15:
        hval += asso_values[(unsigned char)str[14]];
      /*FALLTHROUGH*/
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      /*FALLTHROUGH*/
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      /*FALLTHROUGH*/
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      /*FALLTHROUGH*/
      case 11:
        hval += asso_values[(unsigned char)str[10]+1];
      /*FALLTHROUGH*/
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      /*FALLTHROUGH*/
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      /*FALLTHROUGH*/
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      /*FALLTHROUGH*/
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      /*FALLTHROUGH*/
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      /*FALLTHROUGH*/
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      /*FALLTHROUGH*/
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#ifdef __GNUC_STDC_INLINE__
__attribute__ ((__gnu_inline__))
#endif
#endif
const struct NamedColor *
findColor (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 149,
      MIN_WORD_LENGTH = 3,
      MAX_WORD_LENGTH = 20,
      MIN_HASH_VALUE = 3,
      MAX_HASH_VALUE = 1055
    };

  static const struct NamedColor wordlist[] =
    {
      {"red", 0xff0000},
      {"darkred", 0x8b0000},
      {"tan", 0xd2b48c},
      {"linen", 0xfaf0e6},
      {"sienna", 0xa0522d},
      {"indianred", 0xcd5c5c},
      {"teal", 0x008080},
      {"grey", 0x808080},
      {"green", 0x008000},
      {"gray", 0x808080},
      {"darkgrey", 0xa9a9a9},
      {"darkgreen", 0x006400},
      {"beige", 0xf5f5dc},
      {"orange", 0xffa500},
      {"darkgray", 0xa9a9a9},
      {"orangered", 0xff4500},
      {"khaki", 0xf0e68c},
      {"seagreen", 0x2e8b57},
      {"gold", 0xffd700},
      {"darkorange", 0xff8c00},
      {"darkkhaki", 0xbdb76b},
      {"indigo", 0x4b0082},
      {"goldenrod", 0xdaa520},
      {"maroon", 0x800000},
      {"gainsboro", 0xdcdcdc},
      {"lime", 0x00ff00},
      {"greenyellow", 0xadff2f},
      {"darkgoldenrod", 0xb8860b},
      {"slategrey", 0x708090},
      {"slategray", 0x708090},
      {"salmon", 0xfa8072},
      {"darkseagreen", 0x8fbc8f},
      {"seashell", 0xfff5ee},
      {"darksalmon", 0xe9967a},
      {"tomato", 0xff6347},
      {"thistle", 0xd8bfd8},
      {"darkslategrey", 0x2f4f4f},
      {"cyan", 0x00ffff},
      {"forestgreen", 0x228b22},
      {"dimgrey", 0x696969},
      {"darkslategray", 0x2f4f4f},
      {"mistyrose", 0xffe4e1},
      {"dimgray", 0x696969},
      {"darkcyan", 0x008b8b},
      {"black", 0x000000},
      {"magenta", 0xff00ff},
      {"limegreen", 0x32cd32},
      {"coral", 0xff7f50},
      {"darkmagenta", 0x8b008b},
      {"azure", 0xf0ffff},
      {"blue", 0x0000ff},
      {"oldlace", 0xfdf5e6},
      {"cornsilk", 0xfff8dc},
      {"darkblue", 0x00008b},
      {"skyblue", 0x87ceeb},
      {"firebrick", 0xb22222},
      {"orchid", 0xda70d6},
      {"lightgrey", 0xd3d3d3},
      {"lightgreen", 0x90ee90},
      {"lightyellow", 0xffffe0},
      {"lightgray", 0xd3d3d3},
      {"darkorchid", 0x9932cc},
      {"royalblue", 0x4169e1},
      {"aqua", 0x00ffff},
      {"steelblue", 0x4682b4},
      {"bisque", 0xffe4c4},
      {"crimson", 0xdc143c},
      {"slateblue", 0x6a5acd},
      {"dodgerblue", 0x1e90ff},
      {"blanchedalmond", 0xffebcd},
      {"lightseagreen", 0x20b2aa},
      {"lightslategrey", 0x778899},
      {"lightslategray", 0x778899},
      {"brown", 0xa52a2a},
      {"lightsalmon", 0xffa07a},
      {"snow", 0xfffafa},
      {"lightcyan", 0xe0ffff},
      {"rosybrown", 0xbc8f8f},
      {"sandybrown", 0xf4a460},
      {"darkslateblue", 0x483d8b},
      {"yellow", 0xffff00},
      {"lightcoral", 0xf08080},
      {"mintcream", 0xf5fffa},
      {"aquamarine", 0x7fffd4},
      {"saddlebrown", 0x8b4513},
      {"honeydew", 0xf0fff0},
      {"pink", 0xffc0cb},
      {"lightblue", 0xadd8e6},
      {"cadetblue", 0x5f9ea0},
      {"wheat", 0xf5deb3},
      {"lawngreen", 0x7cfc00},
      {"white", 0xffffff},
      {"aliceblue", 0xf0f8ff},
      {"chocolate", 0xd2691e},
      {"yellowgreen", 0x9acd32},
      {"moccasin", 0xffe4b5},
      {"navy", 0x000080},
      {"chartreuse", 0x7fff00},
      {"ivory", 0xfffff0},
      {"palegreen", 0x98fb98},
      {"lavender", 0xe6e6fa},
      {"hotpink", 0xff69b4},
      {"olive", 0x808000},
      {"fuchsia", 0xff00ff},
      {"mediumseagreen", 0x3cb371},
      {"silver", 0xc0c0c0},
      {"olivedrab", 0x6b8e23},
      {"darkturquoise", 0x00ced1},
      {"turquoise", 0x40e0d0},
      {"violet", 0xee82ee},
      {"violetred", 0xd02090},
      {"darkviolet", 0x9400d3},
      {"palegoldenrod", 0xeee8aa},
      {"whitesmoke", 0xf5f5f5},
      {"springgreen", 0x00ff7f},
      {"burlywood", 0xdeb887},
      {"peru", 0xcd853f},
      {"floralwhite", 0xfffaf0},
      {"lightpink", 0xffb6c1},
      {"darkolivegreen", 0x556b2f},
      {"ghostwhite", 0xf8f8ff},
      {"mediumblue", 0x0000cd},
      {"mediumorchid", 0xba55d3},
      {"lightsteelblue", 0xb0c4de},
      {"lightslateblue", 0x8470ff},
      {"deepskyblue", 0x00bfff},
      {"lightskyblue", 0x87cefa},
      {"lightgoldenrodyellow", 0xfafad2},
      {"plum", 0xdda0dd},
      {"mediumaquamarine", 0x66cdaa},
      {"mediumslateblue", 0x7b68ee},
      {"blueviolet", 0x8a2be2},
      {"midnightblue", 0x191970},
      {"deeppink", 0xff1493},
      {"lemonchiffon", 0xfffacd},
      {"antiquewhite", 0xfaebd7},
      {"paleturquoise", 0xafeeee},
      {"powderblue", 0xb0e0e6},
      {"navajowhite", 0xffdead},
      {"mediumspringgreen", 0x00fa9a},
      {"cornflowerblue", 0x6495ed},
      {"palevioletred", 0xd87093},
      {"mediumvioletred", 0xc71585},
      {"purple", 0x800080},
      {"lavenderblush", 0xfff0f5},
      {"mediumturquoise", 0x48d1cc},
      {"peachpuff", 0xffdab9},
      {"mediumpurple", 0x9370d8},
      {"papayawhip", 0xffefd5}
    };

  static const short lookup[] =
    {
       -1,  -1,  -1,   0,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,   2,  -1,  -1,  -1,  -1,  -1,  -1,
        3,   4,  -1,  -1,   5,  -1,  -1,  -1,  -1,   6,
       -1,  -1,  -1,  -1,   7,   8,  -1,  -1,  -1,   9,
       -1,  -1,  -1,  10,  11,  12,  13,  -1,  14,  15,
       16,  -1,  -1,  17,  18,  19,  -1,  -1,  -1,  20,
       -1,  21,  -1,  -1,  22,  -1,  -1,  -1,  -1,  -1,
       -1,  23,  -1,  -1,  24,  -1,  -1,  -1,  -1,  25,
       -1,  26,  -1,  27,  28,  -1,  -1,  -1,  -1,  29,
       -1,  30,  31,  32,  -1,  -1,  -1,  -1,  -1,  -1,
       33,  34,  35,  36,  37,  -1,  38,  39,  40,  41,
       -1,  -1,  42,  43,  -1,  -1,  -1,  -1,  -1,  -1,
       44,  -1,  45,  -1,  46,  47,  48,  -1,  -1,  -1,
       49,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  50,
       -1,  -1,  51,  52,  -1,  -1,  -1,  -1,  53,  -1,
       -1,  -1,  54,  -1,  55,  -1,  56,  -1,  -1,  57,
       58,  59,  -1,  -1,  60,  61,  -1,  -1,  -1,  62,
       -1,  -1,  -1,  -1,  63,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  64,  -1,  65,  66,  -1,  67,
       68,  -1,  -1,  -1,  69,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  70,  71,
       -1,  -1,  -1,  -1,  72,  73,  -1,  -1,  -1,  -1,
       -1,  74,  -1,  -1,  75,  -1,  -1,  -1,  -1,  76,
       -1,  -1,  -1,  -1,  77,  78,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  79,  -1,  -1,  80,  -1,  -1,  -1,
       81,  -1,  -1,  -1,  82,  83,  84,  -1,  85,  86,
       -1,  -1,  -1,  -1,  87,  -1,  -1,  -1,  -1,  88,
       89,  -1,  -1,  -1,  90,  91,  -1,  -1,  -1,  92,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  93,  -1,  94,  -1,  95,  96,
       -1,  -1,  -1,  -1,  -1,  97,  -1,  -1,  -1,  -1,
       98,  -1,  -1,  -1,  99,  -1,  -1,  -1, 100,  -1,
       -1,  -1, 101,  -1,  -1, 102,  -1, 103,  -1, 104,
       -1, 105,  -1,  -1, 106,  -1,  -1,  -1, 107, 108,
       -1, 109,  -1,  -1, 110,  -1,  -1,  -1,  -1,  -1,
      111,  -1,  -1, 112,  -1, 113,  -1,  -1,  -1,  -1,
       -1, 114,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 115,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 116,
       -1, 117,  -1,  -1, 118,  -1,  -1,  -1,  -1, 119,
      120,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 121,  -1,  -1,  -1,  -1,
       -1,  -1, 122,  -1, 123,  -1,  -1,  -1,  -1, 124,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 125,  -1,  -1,  -1,  -1,  -1, 126,  -1,  -1,
      127,  -1,  -1,  -1, 128,  -1,  -1,  -1,  -1,  -1,
       -1, 129,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 130,  -1,  -1,  -1,  -1,
      131,  -1, 132,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 133,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 134,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 135,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 136,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      137,  -1,  -1,  -1,  -1,  -1, 138, 139,  -1, 140,
       -1,  -1,  -1, 141,  -1,  -1,  -1,  -1,  -1,  -1,
      142,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 143,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 144,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 145,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, 146,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1, 147,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 148
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
        }
    }
  return 0;
}
