/* C++ code produced by gperf version 3.0.2 */
/* Command-line: gperf TokenLookup.gperf  */
/* Computed positions: -k'1,3,$' */

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

#line 80 "TokenLookup.gperf"


QT_BEGIN_NAMESPACE

namespace QPatternist
{

#line 74 "TokenLookup.gperf"
struct TokenMap
{
    const char *name;
    const Tokenizer::TokenType token;
}


/* The strings below are in UTF-16 encoding. Subsequently, each ASCII
 * character is stored as the ASCII character, followed by a null byte.
 * Sorted alphabetically. */;
/* maximum key range = 228, duplicates = 0 */

class TokenLookup
{
private:
  static inline unsigned int hash (const char *str, unsigned int len);
public:
  static const struct TokenMap *value (const char *str, unsigned int len);
};

inline unsigned int
TokenLookup::hash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230,  25, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230,   0,   2,   5,
       25,   0,  20,  20,  35,  85, 230, 230,  40, 110,
       25,  65,  80,   0,  60,   5,  10,   0,  55,   5,
       20,   0, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230, 230, 230, 230, 230,
      230, 230, 230, 230, 230, 230
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

const struct TokenMap *
TokenLookup::value (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 99,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 22,
      MIN_HASH_VALUE = 2,
      MAX_HASH_VALUE = 229
    };

  static const struct TokenMap wordlist[] =
    {
      {"",ERROR}, {"",ERROR},
#line 125 "TokenLookup.gperf"
      {"eq",                       EQ},
      {"",ERROR},
#line 103 "TokenLookup.gperf"
      {"by",                       BY},
#line 126 "TokenLookup.gperf"
      {"every",                    EVERY},
      {"",ERROR},
#line 96 "TokenLookup.gperf"
      {"as",                       AS},
      {"",ERROR},
#line 121 "TokenLookup.gperf"
      {"else",                     ELSE},
#line 190 "TokenLookup.gperf"
      {"where",                    WHERE},
#line 177 "TokenLookup.gperf"
      {"stable",                   STABLE},
#line 99 "TokenLookup.gperf"
      {"at",                       AT},
      {"",ERROR},
#line 104 "TokenLookup.gperf"
      {"case",                     CASE},
      {"",ERROR},
#line 102 "TokenLookup.gperf"
      {"boundary-space",           BOUNDARY_SPACE},
#line 120 "TokenLookup.gperf"
      {"element",                  ELEMENT},
#line 105 "TokenLookup.gperf"
      {"castable",                 CASTABLE},
#line 100 "TokenLookup.gperf"
      {"attribute",                ATTRIBUTE},
      {"",ERROR},
#line 127 "TokenLookup.gperf"
      {"except",                   EXCEPT},
#line 134 "TokenLookup.gperf"
      {"ge",                       GE},
      {"",ERROR},
#line 106 "TokenLookup.gperf"
      {"cast",                     CAST},
#line 183 "TokenLookup.gperf"
      {"treat",                    TREAT},
#line 191 "TokenLookup.gperf"
      {"xquery",                   XQUERY},
#line 154 "TokenLookup.gperf"
      {"ne",                       NE},
      {"",ERROR},
#line 171 "TokenLookup.gperf"
      {"satisfies",                SATISFIES},
      {"",ERROR}, {"",ERROR},
#line 136 "TokenLookup.gperf"
      {"gt",                       GT},
#line 124 "TokenLookup.gperf"
      {"encoding",                 ENCODING},
#line 97 "TokenLookup.gperf"
      {"ascending",                ASCENDING},
      {"",ERROR},
#line 98 "TokenLookup.gperf"
      {"assign",                   ASSIGN},
#line 112 "TokenLookup.gperf"
      {"declare",                  DECLARE},
#line 135 "TokenLookup.gperf"
      {"greatest",                 GREATEST},
#line 181 "TokenLookup.gperf"
      {"then",                     THEN},
      {"",ERROR},
#line 94 "TokenLookup.gperf"
      {"ancestor-or-self",         ANCESTOR_OR_SELF},
#line 148 "TokenLookup.gperf"
      {"le",                       LE},
#line 119 "TokenLookup.gperf"
      {"document-node",            DOCUMENT_NODE},
#line 180 "TokenLookup.gperf"
      {"text",                     TEXT},
      {"",ERROR},
#line 174 "TokenLookup.gperf"
      {"schema",                   SCHEMA},
      {"",ERROR},
#line 118 "TokenLookup.gperf"
      {"document",                 DOCUMENT},
      {"",ERROR},
#line 114 "TokenLookup.gperf"
      {"descendant",               DESCENDANT},
      {"",ERROR},
#line 150 "TokenLookup.gperf"
      {"lt",                       LT},
#line 95 "TokenLookup.gperf"
      {"and",                      AND},
#line 155 "TokenLookup.gperf"
      {"node",                     NODE},
#line 147 "TokenLookup.gperf"
      {"least",                    LEAST},
#line 172 "TokenLookup.gperf"
      {"schema-attribute",         SCHEMA_ATTRIBUTE},
      {"",ERROR},
#line 128 "TokenLookup.gperf"
      {"external",                 EXTERNAL},
      {"",ERROR},
#line 116 "TokenLookup.gperf"
      {"descending",               DESCENDING},
#line 157 "TokenLookup.gperf"
      {"no-preserve",              NO_PRESERVE},
#line 113 "TokenLookup.gperf"
      {"default",                  DEFAULT},
#line 149 "TokenLookup.gperf"
      {"let",                      LET},
#line 173 "TokenLookup.gperf"
      {"schema-element",           SCHEMA_ELEMENT},
      {"",ERROR}, {"",ERROR},
#line 110 "TokenLookup.gperf"
      {"construction",             CONSTRUCTION},
#line 115 "TokenLookup.gperf"
      {"descendant-or-self",       DESCENDANT_OR_SELF},
#line 175 "TokenLookup.gperf"
      {"self",                     SELF},
#line 156 "TokenLookup.gperf"
      {"no-inherit",               NO_INHERIT},
      {"",ERROR},
#line 131 "TokenLookup.gperf"
      {"follows",                  FOLLOWS},
#line 93 "TokenLookup.gperf"
      {"ancestor",                 ANCESTOR},
      {"",ERROR}, {"",ERROR}, {"",ERROR},
#line 182 "TokenLookup.gperf"
      {"to",                       TO},
#line 133 "TokenLookup.gperf"
      {"function",                 FUNCTION},
#line 108 "TokenLookup.gperf"
      {"collation",                COLLATION},
      {"",ERROR},
#line 178 "TokenLookup.gperf"
      {"strict",                   STRICT},
      {"",ERROR},
#line 146 "TokenLookup.gperf"
      {"lax",                      LAX},
      {"",ERROR},
#line 122 "TokenLookup.gperf"
      {"empty",                    EMPTY},
      {"",ERROR},
#line 158 "TokenLookup.gperf"
      {"of",                       OF},
#line 168 "TokenLookup.gperf"
      {"preserve",                 PRESERVE},
#line 129 "TokenLookup.gperf"
      {"following",                FOLLOWING},
      {"",ERROR}, {"",ERROR},
#line 144 "TokenLookup.gperf"
      {"is",                       IS},
#line 165 "TokenLookup.gperf"
      {"precedes",                 PRECEDES},
#line 123 "TokenLookup.gperf"
      {"empty-sequence",           EMPTY_SEQUENCE},
      {"",ERROR}, {"",ERROR},
#line 130 "TokenLookup.gperf"
      {"following-sibling",        FOLLOWING_SIBLING},
#line 142 "TokenLookup.gperf"
      {"instance",                 INSTANCE},
#line 186 "TokenLookup.gperf"
      {"unordered",                UNORDERED},
#line 101 "TokenLookup.gperf"
      {"base-uri",                 BASEURI},
#line 170 "TokenLookup.gperf"
      {"return",                   RETURN},
      {"",ERROR},
#line 187 "TokenLookup.gperf"
      {"validate",                 VALIDATE},
      {"",ERROR},
#line 111 "TokenLookup.gperf"
      {"copy-namespaces",          COPY_NAMESPACES},
#line 159 "TokenLookup.gperf"
      {"option",                   OPTION},
#line 138 "TokenLookup.gperf"
      {"if",                       IF},
      {"",ERROR},
#line 166 "TokenLookup.gperf"
      {"preceding",                PRECEDING},
      {"",ERROR}, {"",ERROR},
#line 141 "TokenLookup.gperf"
      {"in",                       IN},
      {"",ERROR},
#line 143 "TokenLookup.gperf"
      {"intersect",                INTERSECT},
#line 185 "TokenLookup.gperf"
      {"union",                    UNION},
      {"",ERROR},
#line 167 "TokenLookup.gperf"
      {"preceding-sibling",        PRECEDING_SIBLING},
#line 161 "TokenLookup.gperf"
      {"ordering",                 ORDERING},
#line 176 "TokenLookup.gperf"
      {"some",                     SOME},
#line 107 "TokenLookup.gperf"
      {"child",                    CHILD},
      {"",ERROR},
#line 160 "TokenLookup.gperf"
      {"ordered",                  ORDERED},
#line 188 "TokenLookup.gperf"
      {"variable",                 VARIABLE},
      {"",ERROR}, {"",ERROR}, {"",ERROR},
#line 163 "TokenLookup.gperf"
      {"or",                       OR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
#line 109 "TokenLookup.gperf"
      {"comment",                  COMMENT},
      {"",ERROR}, {"",ERROR},
#line 184 "TokenLookup.gperf"
      {"typeswitch",               TYPESWITCH},
      {"",ERROR},
#line 140 "TokenLookup.gperf"
      {"inherit",                  INHERIT},
#line 117 "TokenLookup.gperf"
      {"div",                      DIV},
      {"",ERROR}, {"",ERROR},
#line 152 "TokenLookup.gperf"
      {"module",                   MODULE},
      {"",ERROR},
#line 132 "TokenLookup.gperf"
      {"for",                      FOR},
#line 153 "TokenLookup.gperf"
      {"namespace",                NAMESPACE},
      {"",ERROR}, {"",ERROR},
#line 189 "TokenLookup.gperf"
      {"version",                  VERSION},
      {"",ERROR}, {"",ERROR},
#line 179 "TokenLookup.gperf"
      {"strip",                    STRIP},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
#line 162 "TokenLookup.gperf"
      {"order",                    ORDER},
#line 164 "TokenLookup.gperf"
      {"parent",                   PARENT},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR},
#line 151 "TokenLookup.gperf"
      {"mod",                      MOD},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR},
#line 139 "TokenLookup.gperf"
      {"import",                   IMPORT},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR},
#line 169 "TokenLookup.gperf"
      {"processing-instruction",   PROCESSING_INSTRUCTION},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR},
#line 145 "TokenLookup.gperf"
      {"item",                     ITEM},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR}, {"",ERROR}, {"",ERROR}, {"",ERROR},
      {"",ERROR},
#line 137 "TokenLookup.gperf"
      {"idiv",                     IDIV}
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
#line 192 "TokenLookup.gperf"


} /* Close the QPatternist namespace. */

QT_END_NAMESPACE
