#include <iostream>
#include <version>

int main()
{
    std::cout << "Milestone 0: Build successful!" << std::endl;
#if defined(__clang__)
    std::cout << "Compiled with Clang version: " << __clang_version__ << std::endl;
#endif
    return 0;
}
