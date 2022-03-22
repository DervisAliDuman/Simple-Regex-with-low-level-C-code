#include "Regex.h"

int main(int argc, char* argv[]){
    signal(SIGINT, signalHandler);
    inputCheck(argc);

    char* regex = argv[1];
    reader(argv[2],regex);

    return 0;
}
