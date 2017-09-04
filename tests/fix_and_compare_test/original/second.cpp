// small copyright

#include "second.h"

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

#include <memory>
#include <map>
#include <second.h>
#include <util/test.h>
#include <library/smart_ptr.h>
#include <library/log.h>

#ifndef NDEBUG
#   include <some/thing.h>
#   include <memory>
#elif
#   if __has_include(<filesystem>)
#       warning "Congratulations! You have nice STL version!"
#   endif
#   include    <oh/author/of/ this/  include / is /soooooo/strange.hhhhm>
#   include    <map>
#endif

#   include  <iostream>  
 #include<fstream>
using namespace std;  // lol


void magic_swap(void* a, void * b) {
    a = b;
    b = a;
}

