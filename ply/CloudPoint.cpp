#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>
using namespace std;

int main() {
    ofstream myfile;
    string file, output;
    int max_x, min_x, max_y, min_y,max_z, min_z, testCase;
    double x, y, z;

    max_x = 10;
    min_x = -10;
    x = min_x;

    max_y = 10;
    min_y = -10;
    y = min_y;

    max_z = 10;
    min_z = -10;
    z = min_z;

    file = "test.txt";
    myfile.open (file);
     
    testCase = 0;

    while (x <= max_x){
        while (y <= max_y){ 
            switch (testCase){
                case 0:
                z = sin(sqrt(x*x + y*y));
                break;
                case 1:
                z = sin(x);
                break;
                default:
                z = 0;
                }

            output = to_string(x) + " " + to_string(y) + " " + to_string(z) + "\n";
            myfile << output;
            y++;
        }
        x++;
        y = min_y;
    }    

    myfile.close();
    return 0;
}