#include "interface.hpp"


using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
    Interface inter = inter.getInstance();
    inter.interface();
    return 0;
}
