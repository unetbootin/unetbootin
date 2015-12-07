#include <string.h>// bogus
/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: gperf -CEot -L ANSI-C --key-positions='*' -N findDoctypeEntry -F ,PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards  */

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

struct PubIDInfo {
    enum eMode { 
        eQuirks,         
        eQuirks3,       
        eAlmostStandards
    };

    const char* name;
    eMode mode_if_no_sysid;
    eMode mode_if_sysid;
};
/* maximum key range = 712, duplicates = 0 */

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
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716,   0, 716, 716, 716, 716, 716, 716,   0,
      716, 716, 716,   0, 716,   0,  15,   0,  10,  25,
        5,   0,   5,  25,  10,   0, 716,   0,   0, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716,  10,   5,   0,
       40,   0,  20,   0,   0,   0,   0, 716,   0,   0,
       10,  45,   0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   5, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716, 716, 716, 716, 716,
      716, 716, 716, 716, 716, 716
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[79]];
      /*FALLTHROUGH*/
      case 79:
        hval += asso_values[(unsigned char)str[78]];
      /*FALLTHROUGH*/
      case 78:
        hval += asso_values[(unsigned char)str[77]];
      /*FALLTHROUGH*/
      case 77:
        hval += asso_values[(unsigned char)str[76]];
      /*FALLTHROUGH*/
      case 76:
        hval += asso_values[(unsigned char)str[75]];
      /*FALLTHROUGH*/
      case 75:
        hval += asso_values[(unsigned char)str[74]];
      /*FALLTHROUGH*/
      case 74:
        hval += asso_values[(unsigned char)str[73]];
      /*FALLTHROUGH*/
      case 73:
        hval += asso_values[(unsigned char)str[72]];
      /*FALLTHROUGH*/
      case 72:
        hval += asso_values[(unsigned char)str[71]];
      /*FALLTHROUGH*/
      case 71:
        hval += asso_values[(unsigned char)str[70]];
      /*FALLTHROUGH*/
      case 70:
        hval += asso_values[(unsigned char)str[69]];
      /*FALLTHROUGH*/
      case 69:
        hval += asso_values[(unsigned char)str[68]];
      /*FALLTHROUGH*/
      case 68:
        hval += asso_values[(unsigned char)str[67]];
      /*FALLTHROUGH*/
      case 67:
        hval += asso_values[(unsigned char)str[66]];
      /*FALLTHROUGH*/
      case 66:
        hval += asso_values[(unsigned char)str[65]];
      /*FALLTHROUGH*/
      case 65:
        hval += asso_values[(unsigned char)str[64]];
      /*FALLTHROUGH*/
      case 64:
        hval += asso_values[(unsigned char)str[63]];
      /*FALLTHROUGH*/
      case 63:
        hval += asso_values[(unsigned char)str[62]];
      /*FALLTHROUGH*/
      case 62:
        hval += asso_values[(unsigned char)str[61]];
      /*FALLTHROUGH*/
      case 61:
        hval += asso_values[(unsigned char)str[60]];
      /*FALLTHROUGH*/
      case 60:
        hval += asso_values[(unsigned char)str[59]];
      /*FALLTHROUGH*/
      case 59:
        hval += asso_values[(unsigned char)str[58]];
      /*FALLTHROUGH*/
      case 58:
        hval += asso_values[(unsigned char)str[57]];
      /*FALLTHROUGH*/
      case 57:
        hval += asso_values[(unsigned char)str[56]];
      /*FALLTHROUGH*/
      case 56:
        hval += asso_values[(unsigned char)str[55]];
      /*FALLTHROUGH*/
      case 55:
        hval += asso_values[(unsigned char)str[54]];
      /*FALLTHROUGH*/
      case 54:
        hval += asso_values[(unsigned char)str[53]];
      /*FALLTHROUGH*/
      case 53:
        hval += asso_values[(unsigned char)str[52]];
      /*FALLTHROUGH*/
      case 52:
        hval += asso_values[(unsigned char)str[51]];
      /*FALLTHROUGH*/
      case 51:
        hval += asso_values[(unsigned char)str[50]];
      /*FALLTHROUGH*/
      case 50:
        hval += asso_values[(unsigned char)str[49]];
      /*FALLTHROUGH*/
      case 49:
        hval += asso_values[(unsigned char)str[48]];
      /*FALLTHROUGH*/
      case 48:
        hval += asso_values[(unsigned char)str[47]];
      /*FALLTHROUGH*/
      case 47:
        hval += asso_values[(unsigned char)str[46]];
      /*FALLTHROUGH*/
      case 46:
        hval += asso_values[(unsigned char)str[45]];
      /*FALLTHROUGH*/
      case 45:
        hval += asso_values[(unsigned char)str[44]];
      /*FALLTHROUGH*/
      case 44:
        hval += asso_values[(unsigned char)str[43]];
      /*FALLTHROUGH*/
      case 43:
        hval += asso_values[(unsigned char)str[42]];
      /*FALLTHROUGH*/
      case 42:
        hval += asso_values[(unsigned char)str[41]];
      /*FALLTHROUGH*/
      case 41:
        hval += asso_values[(unsigned char)str[40]];
      /*FALLTHROUGH*/
      case 40:
        hval += asso_values[(unsigned char)str[39]];
      /*FALLTHROUGH*/
      case 39:
        hval += asso_values[(unsigned char)str[38]];
      /*FALLTHROUGH*/
      case 38:
        hval += asso_values[(unsigned char)str[37]];
      /*FALLTHROUGH*/
      case 37:
        hval += asso_values[(unsigned char)str[36]];
      /*FALLTHROUGH*/
      case 36:
        hval += asso_values[(unsigned char)str[35]];
      /*FALLTHROUGH*/
      case 35:
        hval += asso_values[(unsigned char)str[34]];
      /*FALLTHROUGH*/
      case 34:
        hval += asso_values[(unsigned char)str[33]];
      /*FALLTHROUGH*/
      case 33:
        hval += asso_values[(unsigned char)str[32]];
      /*FALLTHROUGH*/
      case 32:
        hval += asso_values[(unsigned char)str[31]];
      /*FALLTHROUGH*/
      case 31:
        hval += asso_values[(unsigned char)str[30]];
      /*FALLTHROUGH*/
      case 30:
        hval += asso_values[(unsigned char)str[29]];
      /*FALLTHROUGH*/
      case 29:
        hval += asso_values[(unsigned char)str[28]];
      /*FALLTHROUGH*/
      case 28:
        hval += asso_values[(unsigned char)str[27]];
      /*FALLTHROUGH*/
      case 27:
        hval += asso_values[(unsigned char)str[26]];
      /*FALLTHROUGH*/
      case 26:
        hval += asso_values[(unsigned char)str[25]];
      /*FALLTHROUGH*/
      case 25:
        hval += asso_values[(unsigned char)str[24]];
      /*FALLTHROUGH*/
      case 24:
        hval += asso_values[(unsigned char)str[23]];
      /*FALLTHROUGH*/
      case 23:
        hval += asso_values[(unsigned char)str[22]];
      /*FALLTHROUGH*/
      case 22:
        hval += asso_values[(unsigned char)str[21]];
      /*FALLTHROUGH*/
      case 21:
        hval += asso_values[(unsigned char)str[20]];
      /*FALLTHROUGH*/
      case 20:
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
        hval += asso_values[(unsigned char)str[10]];
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
const struct PubIDInfo *
findDoctypeEntry (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 77,
      MIN_WORD_LENGTH = 4,
      MAX_WORD_LENGTH = 80,
      MIN_HASH_VALUE = 4,
      MAX_HASH_VALUE = 715
    };

  static const struct PubIDInfo wordlist[] =
    {
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"html", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd w3 html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html 3//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//w3c//dtd html 3.2//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html strict//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html level 3//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html level 2//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html strict level 3//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html level 0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html strict level 2//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html 3.2//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html strict level 0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//w30//dtd w3 html 2.0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html 3.0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html//en//3.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html 3.0//en//", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html level 1//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html 2.0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html//en//2.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html strict//en//3.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html level 3//en//3.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html strict level 1//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html 2.0 strict//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html strict//en//2.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html strict level 3//en//3.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html 2.0 level 2//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html level 2//en//2.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//w3c//dtd html 3.2 final//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html 2.1e//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd html 4.0 frameset//en", PubIDInfo::eQuirks, PubIDInfo::eQuirks},
      {"-//ietf//dtd html level 0//en//2.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html 2.0 strict level 2//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html strict level 2//en//2.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//w3o//dtd w3 html 3.0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//webtechs//dtd mozilla html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//w3o//dtd w3 html 3.0//en//", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html strict level 0//en//2.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd html 3 1995-03-24//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd html experimental 970421//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//w3o//dtd w3 html strict 3.0//en//", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html 2.0 level 1//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html level 1//en//2.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html 3.2 final//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd xhtml 1.0 frameset//en", PubIDInfo::eAlmostStandards, PubIDInfo::eAlmostStandards},
      {"-//ietf//dtd html 2.0 strict level 1//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//ietf//dtd html strict level 1//en//2.0", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd html 4.01 frameset//en", PubIDInfo::eQuirks, PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd html 3.2 draft//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//w3c//dtd html 3.2s draft//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//metrius//dtd metrius presentational//en", PubIDInfo::eQuirks, PubIDInfo::eQuirks},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//webtechs//dtd mozilla html 2.0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd html experimental 19960712//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-/w3c/dtd html 4.0 transitional/en", PubIDInfo::eQuirks, PubIDInfo::eQuirks},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd html 4.0 transitional//en", PubIDInfo::eQuirks, PubIDInfo::eQuirks},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//spyglass//dtd html 2.0 extended//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//w3c//dtd xhtml 1.0 transitional//en", PubIDInfo::eAlmostStandards, PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//netscape comm. corp.//dtd html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//w3c//dtd html 4.01 transitional//en", PubIDInfo::eQuirks, PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//netscape comm. corp.//dtd strict html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//as//dtd html 3.0 aswedit + extensions//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//sq//dtd html 2.0 hotmetal + extensions//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//sun microsystems corp.//dtd hotjava html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//sun microsystems corp.//dtd hotjava strict html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//o'reilly and associates//dtd html 2.0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//microsoft//dtd internet explorer 3.0 html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//microsoft//dtd internet explorer 2.0 html//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"+//silmaril//dtd html pro v0r11 19970101//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"-//microsoft//dtd internet explorer 3.0 html strict//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//microsoft//dtd internet explorer 2.0 html strict//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//microsoft//dtd internet explorer 3.0 tables//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//microsoft//dtd internet explorer 2.0 tables//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//advasoft ltd//dtd html 3.0 aswedit + extensions//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//o'reilly and associates//dtd html extended 1.0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//o'reilly and associates//dtd html extended relaxed 1.0//en", PubIDInfo::eQuirks3, PubIDInfo::eQuirks3},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//softquad//dtd hotmetal pro 4.0::19971010::extensions to html 4.0//en", PubIDInfo::eQuirks, PubIDInfo::eQuirks},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"",PubIDInfo::eAlmostStandards,PubIDInfo::eAlmostStandards},
      {"-//softquad software//dtd hotmetal pro 6.0::19990601::extensions to html 4.0//en", PubIDInfo::eQuirks, PubIDInfo::eQuirks}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
