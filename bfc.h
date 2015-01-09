#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

extern "C"
{
const char* bfCompileFrontBracket(int id);
const char* bfCompileEndBracket(int id);
const char* bfCompilePutchar();
const char* bfCompileGetchar();

const char* bfCompile(const char* src);
}
