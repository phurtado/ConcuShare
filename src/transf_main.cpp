#include <iostream>
#include "common.h"

using namespace std;

int main(int argc, char *argv[]) {
	cout << "lala main transf" << endl;
	for(int i = 0; i < argc; i++)
	cout << "Arg " << i << " es " << argv[i] << endl;
	
	return 0;
}
