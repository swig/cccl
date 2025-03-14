#include <iostream>

/** Simply display the arguments passed in to the executable */
int main(int argc, char* argv[]) {
    std::cout << "exeargs";
    for (int i=1; i<argc; ++i)
        std::cout << " \"" << argv[i] << "\"";
    std::cout << std::endl;
    return 0;
}
