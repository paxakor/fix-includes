### Config format.
Program will search for config .fix-includes in parent directories.
You are able to add your own categories in .fix-includes. Use following format of record (one record per line):
"number regexp", where number is category weight (int), and regexp will be used as filter for include path.
You can also add record like "Lang: C", "Lang: C++" or "Lang: Auto" to specify language.

### Default weights.
By default includes will be sorted according to the order specified in source_file.hpp (enum IncludeCategory).
Corresponding weights will be calculated according to the formula: (Category + 1) * 1000.
