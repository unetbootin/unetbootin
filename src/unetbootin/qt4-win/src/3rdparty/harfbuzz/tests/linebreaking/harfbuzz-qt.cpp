#include <harfbuzz-external.h>
#include <Qt/private/qunicodetables_p.h>

HB_LineBreakClass HB_GetLineBreakClass(HB_UChar32 ch)
{
#if QT_VERSION >= 0x040300
    return (HB_LineBreakClass)QUnicodeTables::lineBreakClass(ch);
#else
#error "This test currently requires Qt >= 4.3"
#endif
}

void HB_GetUnicodeCharProperties(HB_UChar32 ch, HB_CharCategory *category, int *combiningClass)
{
    *category = (HB_CharCategory)QChar::category(ch);
    *combiningClass = QChar::combiningClass(ch);
}

HB_CharCategory HB_GetUnicodeCharCategory(HB_UChar32 ch)
{
    return (HB_CharCategory)QChar::category(ch);
}

int HB_GetUnicodeCharCombiningClass(HB_UChar32 ch)
{
    return QChar::combiningClass(ch);
}

HB_UChar16 HB_GetMirroredChar(HB_UChar16 ch)
{
    return QChar::mirroredChar(ch);
}

