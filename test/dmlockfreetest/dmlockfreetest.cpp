
#include "dmlockfree.h"

int main( int argc, char* argv[] ) {

    Idmlockfree* module = dmlockfreeGetModule();
    if (module)
    {
        module->Test();
        module->Release();
    }
    return 0;
}
