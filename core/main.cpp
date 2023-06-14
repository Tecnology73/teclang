#include <map>
#include <string>
#include <iostream>
#include <chrono>
#include "Parser.h"
#include "Generator.h"

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::time_point<std::chrono::steady_clock> Time;

std::map<std::string, std::string> parseArgs(int argc, char *argv[]) {
    std::map<std::string, std::string> args{
        {"input",    R"(P:\Cpp\teclang\source\simple.lang)"},
        {"output",   R"(P:\Cpp\teclang\out\out.ll)"},
        {"optLevel", "0"}
    };

    for (unsigned i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0)
            args["input"] = argv[++i];
        else if (strcmp(argv[i], "-o") == 0)
            args["output"] = argv[++i];
        else if (strcmp(argv[i], "-O") == 0)
            args["optLevel"] = argv[++i];
        else
            std::cerr << "Unrecognized argument: " << argv[i] << std::endl;
    }

    return args;
}

/*llvm::OptimizationLevel toOptLevel(const std::string &optLevel) {
    if (optLevel == "1")
        return llvm::OptimizationLevel::O1;
    if (optLevel == "2")
        return llvm::OptimizationLevel::O2;
    if (optLevel == "3")
        return llvm::OptimizationLevel::O3;

    return llvm::OptimizationLevel::O0;
}*/

int main(int argc, char *argv[]) {
    auto args = parseArgs(argc, argv);

    auto generator = std::make_unique<Generator>();
    auto parser = std::make_unique<Parser>(args["input"]);

    // Start
    Time start = Clock::now();

    std::cout << "--- PARSER ---\n";
    int result = parser->Parse();
    std::cout << "---  END   ---\n\n";

    Time parse = Clock::now();

    if (result != 0) {
        std::cerr << "Parser Error: " << result << std::endl;
        return result;
    }

    // Run Generator
    std::cout << "--- GENERATOR ---\n";
    int generatorResult = generator->Generate();
    std::cout << "---    END    ---\n\n";

    Time generate = Clock::now();

    if (generatorResult != 0) {
        std::cerr << "Generator Error: " << generatorResult << std::endl;
        return generatorResult;
    }

    // Dump IR
    int dumpResult = generator->DumpModule(args["output"]/*, toOptLevel(args["optLevel"])*/);
    if (dumpResult != 0) {
        std::cerr << "IR Dump Error: " << dumpResult << std::endl;
        return dumpResult;
    }

    std::cout << "IR Dump complete\n";

    Time end = Clock::now();

    // Time log
    std::cout << "--- TIME ---\n";
    std::cout << "Parser: "
              << std::chrono::duration<double, std::micro>(parse - start).count()
              << "us\n";
    std::cout << "Generator: "
              << std::chrono::duration<double, std::micro>(generate - parse).count()
              << "us\n";
    std::cout << "IR Dump: "
              << std::chrono::duration<double, std::micro>(end - generate).count()
              << "us\n";
    std::cout << "Total: "
              << std::chrono::duration<double, std::micro>(end - start).count()
              << "us\n"
              << std::endl;

    return 0;
}
