// Copyright 2017, Pavel Korozevtsev.

#include "category.hpp"
#include "source_file.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <vector>

class IncludeCategorySearcher {
private:
    // clang-format off
    /* Headers for C++ mode
    ** see http://en.cppreference.com/w/cpp/header for more info
    */
    inline const static std::unordered_set<std::string_view> StdCppHeaders = {
        "algorithm", "any", "array", "atomic", "bitset", "cassert", "cctype", "cerrno", "cfenv", "cfloat", "charconv",
        "chrono", "cinttypes", "climits", "clocale", "cmath", "codecvt", "complex", "condition_variable", "csetjmp",
        "csignal", "cstdarg", "cstddef", "cstdint", "cstdio", "cstdlib", "cstring", "ctime", "cuchar", "cwchar",
        "cwctype", "deque", "exception", "execution", "filesystem", "forward_list", "fstream", "functional", "future",
        "initializer_list", "iomanip", "ios", "iosfwd", "iostream", "istream", "iterator", "limits", "list", "locale",
        "map", "memory", "memory_resource", "mutex", "new", "numeric", "optional", "ostream", "queue", "random",
        "ratio", "regex", "scoped_allocator", "set", "shared_mutex", "sstream", "stack", "stdexcept", "streambuf",
        "string", "string_view", "strstream", "system_error", "thread", "tuple", "type_traits", "typeindex",
        "typeinfo", "unordered_map", "unordered_set", "utility", "valarray", "variant", "vector"
    };
    inline const static std::unordered_set<std::string_view> ExperimentalCppHeaders = {
        "experimental/algorithm",     "experimental/any",             "experimental/chrono",
        "experimental/deque",         "experimental/exception_list",  "experimental/execution_policy",
        "experimental/filesystem",    "experimental/forward_list",    "experimental/functional",
        "experimental/future",        "experimental/list",            "experimental/map",
        "experimental/memory",        "experimental/memory_resource", "experimental/numeric",
        "experimental/optional",      "experimental/ratio",           "experimental/regex",
        "experimental/set",           "experimental/string",          "experimental/string_view",
        "experimental/system_error",  "experimental/tuple",           "experimental/type_traits",
        "experimental/unordered_map", "experimental/unordered_set",   "experimental/utility",
        "experimental/vector"
    };
    inline const static std::unordered_set<std::string_view> LegacyCHeaders = {
        "assert.h", "ctype.h",  "errno.h",  "fenv.h",   "float.h",  "inttypes.h", "limits.h",
        "locale.h", "math.h",   "setjmp.h", "signal.h", "stdarg.h", "stddef.h",   "stdint.h",
        "stdio.h",  "stdlib.h", "string.h", "time.h",   "uchar.h",  "wchar.h",    "wctype.h"
    };
    inline const static std::unordered_set<std::string_view> UnsupportedCHeaders = {
        "stdatomic.h", "stdnoreturn.h", "threads.h"
    };
    inline const static std::unordered_set<std::string_view> DeprecatedCHeaders = {
        "ccomplex", "ciso646",  "complex.h",  "cstdalign", "cstdbool",
        "ctgmath",  "iso646.h", "stdalign.h", "stdbool.h", "tgmath.h"
    };

    /* Headers for C mode
    ** see http://en.cppreference.com/w/c/header for more info
    */
    inline const static std::unordered_set<std::string_view> StdCHeaders = {
        "assert.h",  "complex.h", "ctype.h",  "errno.h",  "fenv.h",   "float.h",       "inttypes.h", "iso646.h",
        "limits.h",  "locale.h",  "math.h",   "setjmp.h", "signal.h", "stdalign.h",    "stdarg.h",   "stdatomic.h",
        "stdbool.h", "stddef.h",  "stdint.h", "stdio.h",  "stdlib.h", "stdnoreturn.h", "string.h",   "tgmath.h",
        "threads.h", "time.h",    "uchar.h",  "wchar.h",  "wctype.h"
    };

    /* Posix headers
    ** see http://pubs.opengroup.org/onlinepubs/9699919799/idx/head.html for more info
    */
    inline const static std::unordered_set<std::string_view> PosixHeaders = {
        "aio.h",        "arpa/inet.h",    "assert.h",     "complex.h",   "cpio.h",      "ctype.h",
        "dirent.h",     "dlfcn.h",        "errno.h",      "fcntl.h",     "fenv.h",      "float.h",
        "fmtmsg.h",     "fnmatch.h",      "ftw.h",        "glob.h",      "grp.h",       "iconv.h",
        "inttypes.h",   "iso646.h",       "langinfo.h",   "libgen.h",    "limits.h",    "locale.h",
        "math.h",       "monetary.h",     "mqueue.h",     "ndbm.h",      "net/if.h",    "netdb.h",
        "netinet/in.h", "netinet/tcp.h",  "nl_types.h",   "poll.h",      "pthread.h",   "pwd.h",
        "regex.h",      "sched.h",        "search.h",     "semaphore.h", "setjmp.h",    "signal.h",
        "spawn.h",      "stdarg.h",       "stdbool.h",    "stddef.h",    "stdint.h",    "stdio.h",
        "stdlib.h",     "string.h",       "strings.h",    "stropts.h",   "sys/ipc.h",   "sys/mman.h",
        "sys/msg.h",    "sys/resource.h", "sys/select.h", "sys/sem.h",   "sys/shm.h",   "sys/socket.h",
        "sys/stat.h",   "sys/statvfs.h",  "sys/time.h",   "sys/times.h", "sys/types.h", "sys/uio.h",
        "sys/un.h",     "sys/utsname.h",  "sys/wait.h",   "syslog.h",    "tar.h",       "termios.h",
        "tgmath.h",     "time.h",         "trace.h",      "ulimit.h",    "unistd.h",    "utime.h",
        "utmpx.h",      "wchar.h",        "wctype.h",     "wordexp.h"
    };
    // clang-format on

private:
    static auto GetIncludeCategoryForC(std::string_view path, bool global) {
        if (StdCHeaders.count(path))
            return STDC;
        if (PosixHeaders.count(path))
            return POSIX;
        return global ? GLOBAL : LOCAL;
    }

    static auto GetIncludeCategoryForCpp(std::string_view path, bool global) {
        if (StdCppHeaders.count(path))
            return STDCPP;
        if (ExperimentalCppHeaders.count(path))
            return EXPCPP;
        if (LegacyCHeaders.count(path))
            return STDC;
        if (PosixHeaders.count(path))
            return POSIX;
        if (UnsupportedCHeaders.count(path)) {
            std::cout << "Achtung! Header <" << path << "> is unsupported by C++\n";
            return STDC;
        }
        if (DeprecatedCHeaders.count(path)) {
            std::cout << "Achtung! Header <" << path << "> is deprecated in C++\n";
            return STDC;
        }
        return global ? GLOBAL : LOCAL;
    }

public:
    static auto GetIncludeCategory(const Line& line, Lang mode) {
        const auto[path, global] = line.IncludePath();
        switch (mode) {
        case C:
            return GetIncludeCategoryForC(path, global);
        case CPP:
            return GetIncludeCategoryForCpp(path, global);
        default:
            throw std::runtime_error("Not implemented");
        }
    }
};

IncludeCategory GetIncludeCategory(const Line& line, Lang mode) {
    return IncludeCategorySearcher::GetIncludeCategory(line, mode);
}
