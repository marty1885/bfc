#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>

#include <bfc.h>

using namespace std;

static const char helpString[]=
"bfc - BrainFuckCompiler\n\
A Brainfuck compiler for 64bit Linux and mac\n\
Marten Chang 2015.\n\
\n\
Options:\n\
\t-o output file path\n\
\t-S output ASM source file insted of executable\n\
\t-h show this help\n\
\n\
";

int main(int argc,char** argv)
{
    bool outputAsm = false;
    bool outSet = false;
    if(argc == 1)//no args
    {
        cout << helpString;
        exit(0);
    }

    string sourcePath;
    string outputPath;

    for(int i=1;i<argc;i++)
    {
        if(argv[i][0] == '-')
        {
            if(argv[i][1] == 'o')
            {
                outputPath = argv[i+1];
                outSet = true;
                i++;
            }
            if(argv[i][1] == 'h')
            {
                cout << helpString;
                exit(0);
            }
            if(argv[i][1] == 'S')
                outputAsm = true;
        }
        else
            sourcePath = argv[i];
    }
    if(sourcePath.compare("") == 0)//if no source
    {
        cout << "no source file setted. exit\n";
        exit(0);
    }
    if(outSet == false)
    {
        outputPath = sourcePath + ".o";
    }


    ifstream in(sourcePath.c_str());
    string sourceCode((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

    string asmCode = bfCompile(sourceCode.c_str());

    if(outputAsm == false)
    {
        ofstream asmOut("cache.asm");
        asmOut << asmCode;
        asmOut.close();
        char* command = new char[4000];
        sprintf(command,"nasm -f elf64 cache.asm -o cache.o && gcc ./cache.o -o %s",
                outputPath.c_str());

        int success = system(command);
        remove("./cache.asm");
        return success;
    }
    else
    {
        ofstream asmOut(outputPath.c_str());
        asmOut << asmCode;
        asmOut.close();
    }
    return 1;
}
