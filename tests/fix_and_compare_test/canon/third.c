#include <library/dog.h>
#include <library/stupid_ptr.h>
#include <map>
#include <set>
#include <string>
#include <third.h>
#include <util/third_test.h>

#ifndef NDEBUG
    #include <some/thing.h>
    #include <memory>
#elif
    #if GNOME
        #warning "DEBUG & GNOME"
        #  include <filesystem>
    #endif
    #include    <map>
#endif

#include <iostream>  // hm, if my source file fill 32 extra bits on hdd, do i make more money?

using namespace std;  // lol
#include <fstream>  // now one space saved (no)

signed minor () {
    return 0;
}
