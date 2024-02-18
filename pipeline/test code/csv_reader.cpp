#include<fstream>
#include<string>
#include<vector>
#include<iostream>

using namespace std;

void loadDataFromDb();

int main()
{
    loadDataFromDb();
    return 0;
}


void loadDataFromDb()
{
    ifstream dbfile("water.csv");
    string line;
    getline(dbfile, line);
    while(getline(dbfile, line))
    {
        cout << line << endl;
    }
}