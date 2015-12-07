int mutablePrimitive1;
int mutablePrimitive2;
const int constPrimitive1 = 4;
const int constPrimitive2 = 3;

class ComplexClass
{
public:
    ComplexClass();
    ComplexClass(const ComplexClass &);
    ~ComplexClass();
};

ComplexClass mutableComplex1;
ComplexClass mutableComplex2;
const ComplexClass constComplex1;
const ComplexClass constComplex2;

int main()
{
    int localVariable;
    localVariable = 0;
    return localVariable;
}
