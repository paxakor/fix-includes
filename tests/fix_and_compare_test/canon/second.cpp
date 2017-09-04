// small copyright

#include "second.h"

#include <library/log.h>
#include <library/smart_ptr.h>
#include <second.h>
#include <util/test.h>

#include <fstream>
#include <iostream>
#include <map>
#include <memory>

/*
** BBBBBBB   IIIIIIII    GGGGGGGG
** BB    BB     II      GG      GG
** BB    BB     II      GG        
** BB    BB     II      GG        
** BBBBBBB      II      GG        
** BB    BB     II      GG    GGGG
** BB    BB     II      GG      GG
** BB    BB     II      GG      GG
** BBBBBBB   IIIIIIII    GGGGGGGG
*/

#ifndef NDEBUG
#include <some/thing.h>

#include <memory>
#elif
#   if __has_include(<filesystem>)
#       warning "Congratulations! You have nice STL version!"
#   endif
#include <oh/author/of/ this/  include / is /soooooo/strange.hhhhm>

#include <map>
#endif

using namespace std;  // lol


void magic_swap(void* a, void * b) {
    a = b;
    b = a;
}

