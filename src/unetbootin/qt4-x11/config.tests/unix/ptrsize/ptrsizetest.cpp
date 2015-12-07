/* Sample program for configure to test pointer size on target
platforms.
*/

template<int>
struct QPointerSizeTest
{
};

template<>
struct QPointerSizeTest<8>
{
    enum { PointerSize = 8 };
};

int main( int, char ** )
{
    return QPointerSizeTest<sizeof(void*)>::PointerSize;
}

