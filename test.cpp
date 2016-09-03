#include <iostream>
#include <fstream>

using namespace std;

int main(int argc)
{
    string line;
    ifstream myFile;
    myFile.open("SAMPLE_INPUT.data.txt");
    if (myFile.is_open())
    {
        while (getline(myFile, line))
        {
            cout << line << endl;
        }
        myFile.close();
    }
    else
    {
        cout << "unable to open file" << endl;
    }
    return (0);
}
