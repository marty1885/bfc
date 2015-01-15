#include <bfc.h>
using namespace std;

static char BFAsmHead[] =
"section .text\n\
global main\n\
\n\
extern malloc\n\
extern free\n\
extern memset\n\
extern putchar\n\
extern exit\n\
extern getchar\n\
\n\
main:\n\
;prepare space for program\n\
mov rdi, 0x1000 ;4KB should be enough \n\
call malloc\n\
mov rcx,rax ;store memory pointer in rcx\n\
push rcx ;protect rcx \n\
;clear memory\n\
mov edx,0x1000\n\
mov esi,0\n\
mov rdi,rcx\n\
call memset\n\
pop rcx\n\
mov rbx,rcx ;store current data pointer in rbx\n\
;Brainfuck program start\n\
\n";

static char BFAsmEnd [] =
"\n\
        ;program end\n\
        ;release memory\n\
        mov rdi,rcx\n\
        call free\n\
        mov edi,0\n\
        call exit\n\
        \n\
        ";

extern "C"
{

    const char* bfCompileFrontBracket(int id)
    {
        static char* str = NULL;
        if(str != NULL)
            delete [] str;
        str = new char [100];
        sprintf(str,"\
                cmp BYTE[rbx],0\n\
                jz JUPM_END_FLAG_%d\n\
                JUPM_FRONT_FLAG_%d:\n"
                ,id,id);
        return (const char*)str;
    }

    const char* bfCompileEndBracket(int id)
    {
        static char* str = NULL;
        if(str != NULL)
            delete [] str;
        str = new char [100];
        sprintf(str,"\
                JUPM_END_FLAG_%d:\n\
                cmp BYTE[rbx],0\n\
                jnz JUPM_FRONT_FLAG_%d\n\
                "
                ,id,id);
        return (const char*)str;
    }

    const char* bfCompilePutchar()
    {
        static char* str = NULL;
        if(str != NULL)
            delete [] str;
        str = new char [100];
        sprintf(str,"\
                mov di,[rbx]\n\
                push rcx\n\
                push rbx\n\
                call putchar\n\
                pop rbx\n\
                pop rcx\n\
                ");
        return (const char*)str;
    }

    const char* bfCompileGetchar()
    {
        static char* str = NULL;
        if(str != NULL)
            delete [] str;
        str = new char [100];
        sprintf(str,"\
                xor rax,rax\n\
                call getchar\n\
                mov BYTE[rbx],al\n\
                ");
        return (const char*)str;
    }

    int bfCompileCode(string& asmCode,char ch,int length)
    {
        char str[30] = {0};
        static int jumpFlagCount = 0;
        static vector<int> jumpFlagId;
        int id = 0;
        switch(ch)
        {
            case '+':
                if(length == 1)
                    sprintf(str,"inc BYTE[rbx]\n");
                else
                    sprintf(str,"add BYTE[rbx],%d\n",length);
                asmCode.append(str);
                break;

            case '-':
                if(length == 1)
                    sprintf(str,"dec BYTE[rbx]\n");
                else
                    sprintf(str,"sub BYTE[rbx],%d\n",length);
                asmCode.append(str);
                break;

            case '[':
                for(int w=0;w<length;w++)
                {
                    jumpFlagId.push_back(jumpFlagCount);
                    asmCode.append(bfCompileFrontBracket(jumpFlagCount));
                    jumpFlagCount++;
                }
                break;

            case ']':
                for(int w=0;w<length;w++)
                {
                    id = jumpFlagId[jumpFlagId.size()-1];
                    jumpFlagId.pop_back();
                    asmCode.append(bfCompileEndBracket(id));
                }
                break;

            case '>':
                if(length == 1)
                    sprintf(str,"inc rbx\n");
                else
                    sprintf(str,"add rbx,%d\n",length);
                asmCode.append(str);
                break;

            case '<':
                if(length == 1)
                    sprintf(str,"dec rbx\n");
                else
                    sprintf(str,"sub rbx,%d\n",length);
                asmCode.append(str);
                break;

            case '.':
                for(int w=0;w<length;w++)
                    asmCode.append(bfCompilePutchar());
                break;

            case ',':
                for(int w=0;w<length;w++)
                    asmCode.append(bfCompileGetchar());
                break;


        }
        return 1;
    }

    const char* bfCompile(const char* src)
    {
        string sourceCode = src;
        sourceCode.erase(std::remove(sourceCode.begin(), sourceCode.end(), '\n'),
                sourceCode.end());//remove \n from raw input
        sourceCode.erase(std::remove(sourceCode.begin(), sourceCode.end(), ' '),
                sourceCode.end());//remove ' ' from raw input
        sourceCode.append(" ");//so the complier will compiler the last chunk of code

        string asmCode;
        int size = sourceCode.size();

        const char* codeStr = sourceCode.c_str();
        char ch = codeStr[0];
        for(int i=1;i<size;i++)
        {
            //seprate code into chunks
            int length = size-i;
            for(int w=0;i<size;i++,w++)
            {
                if(ch != codeStr[i])
                {
                    length = w+1;
                    break;
                }

            }
            //convert to asm
            bfCompileCode(asmCode,ch,length);

            ch = codeStr[i];
            //cout << length << " ";
        }

        static string *code = NULL;
        if(code != NULL)
            delete code;
        code = new string;
        *code = string(BFAsmHead) + asmCode + string(BFAsmEnd);
        return code->c_str();
    }

}
