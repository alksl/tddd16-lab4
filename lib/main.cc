#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <ast.hh>
#include <parser.hh>
#include <symtab.hh>

extern int yyparse(void);
extern int yydebug;
extern int errorCount;
extern int warningCount;

static char *optionString = "dh";

void Usage(char *program)
{
    std::cerr << "Usage:\n"
         << program << " [-d] [filename]\n"
         << program << " -h\n"
         << "\n"
         << "Options:\n"
         << "  -h               Shows this message.\n"
         << "  -d               Turn on parser debugging.\n";

    exit(1);
}

int main(int argc, char **argv)
{
    int          option;
    extern FILE *yyin;

    //
    // Set up the symbol table
    //

    currentFunction = new FunctionInformation("main.");
    kIntegerType    = new TypeInformation("integer", sizeof(long));
    kRealType       = new TypeInformation("real", sizeof(double));

    kFPrintFunction = new FunctionInformation("putreal");
    kIPrintFunction = new FunctionInformation("putint");
    kFReadFunction = new FunctionInformation("getreal");
    kIReadFunction = new FunctionInformation("getint");

    kIPrintFunction->SetReturnType(kIntegerType);
    kIPrintFunction->AddParameter("x", kIntegerType);
    kFPrintFunction->SetReturnType(kIntegerType);
    kFPrintFunction->AddParameter("x", kRealType);
    kIReadFunction->SetReturnType(kIntegerType);
    kFReadFunction->SetReturnType(kRealType);

    currentFunction->AddSymbol(kIntegerType);
    currentFunction->AddSymbol(kRealType);
    currentFunction->AddSymbol(kIPrintFunction);
    currentFunction->AddSymbol(kFPrintFunction);
    currentFunction->AddSymbol(kIReadFunction);
    currentFunction->AddSymbol(kFReadFunction);


    // Add a return type for the main scope for consistancy
    currentFunction->SetReturnType(kIntegerType);

    //
    // Check command-line arguments
    //
    opterr = 0;
    optopt = '?';
    while ((option = getopt(argc, argv, optionString)) != EOF)
    {
        switch (option)
        {
        case 'd':
            yydebug = 1;
            break;
        case 'h':
            Usage(argv[0]);
            break;
        case '?':
            Usage(argv[0]);
            break;
        }
    }

    if (argv[optind])
    {
        yyin = fopen(argv[optind], "r");
        if (yyin == NULL)
        {
            perror(argv[optind]);
            exit(1);
        }
        optind += 1;
    }

    if (optind < argc)
        Usage(argv[0]);

    //
    // Compile the input
    //

    yyparse();

    return 0;
}

