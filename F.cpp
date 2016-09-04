/*--------
(1) Read in a Rainbow table (built using B.cpp)
(2) Read 1000 digests from standard input and  output the preimage. 
------------*/

#include <iostream>
#include <unordered_map>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "sha1.h"

using namespace std;
#define MAX_LEN 100
#define L_CHAIN 5
unsigned long TOTAL_SHA = 0; // Count the number of hashes performed.

unsigned char M[1024 * 512][3]; // array to store the word read from the table (head of chain)
unsigned char D[1024 * 512][3]; // array to store the digest read from the table  (end of chain)

//-------   Data Structure for searching    -----------//
unordered_map<unsigned int, unsigned int> HashTable;
unordered_map<unsigned int, unsigned int>::const_iterator G;

//-----------    Hash     ----------------------------//
int Hash(unsigned char m[3], unsigned int d[5])
{
  SHA1 sha;
  sha.Reset();
  sha.Input(m[0]);
  sha.Input(m[1]);
  sha.Input(m[2]);
  sha.Result(d);

  TOTAL_SHA = TOTAL_SHA + 1;
  return (0);
}

//-----------    Reduce  -----------------------------//
//   d:   input digest
//   m:   output word
//   i:   the index of the reduce function
//---------------------------------------------------//
int Reduce(unsigned int d[5], unsigned char m[3], int i)
{
  m[0] = (unsigned char)((d[0] + i) % 256); //8 bits
  m[1] = (unsigned char)((d[1]) % 256);     //8 bits
  m[2] = (unsigned char)((d[2]) % 256);     //8 bits

  return (0);
}

struct HexCharStruct
{
  unsigned char c;
  HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
  return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
  return HexCharStruct(_c);
}

//------------  Read in the Table ------------------//
//   Store the result in M and D                    //
int ReadT()
{
  FILE *pFile;
  if ((pFile = fopen("table.data", "rb")) == NULL)
  {
    cout << "cannot open file." << endl;
    exit(0);
  }
  fseek(pFile, 0, SEEK_END);
  long lSize = ftell(pFile);
  rewind(pFile);
int N_CHAIN = lSize/6;
  for (int i = 0; i < N_CHAIN; i++)
  {
    fread(&M[i], 3, 1, pFile);
    fread(&D[i], 3, 1, pFile);
    //cout << hex(M[i][0]) << (int)M[i][1] << (int)M[i][2] << "  " << (int)D[i][0] << (int)D[i][1] << (int)D[i][2] << endl;
  }

  fclose(pFile);
  return N_CHAIN;
}
int destWordExists(unsigned char d[3], int n_chain)
{
  unsigned char *pD;
  for (int i = 0; i < n_chain; i++)
  {
    pD = D[i];
    if (*pD == d[0] && *(pD + 1) == d[1] && *(pD + 2) == d[2])
    {
      return i;
    }
  }
  return -1;
}

//------------------------------------------------------------------------------------
//      Given a digest,  search for the pre-image   answer_m[3].
//------------------------------------------------------------------------------------
int search(unsigned int target_d[5], unsigned char answer_m[3])
{
  int j, i;
  //unsigned char Colour_m[MAX_LEN][3];
  //unsigned int Colour_d[MAX_LEN][5];
  unsigned char Colour_m[3];
  unsigned int Colour_d[5];
  unsigned int flag[MAX_LEN];
  Colour_d[0] = target_d[0];
  Colour_d[1] = target_d[1];
  Colour_d[2] = target_d[2];
  Colour_d[3] = target_d[3];
  Colour_d[4] = target_d[4];
  //cout << Colour_d[0] << Colour_d[1] << Colour_d[2] << endl;
  // for (j = 0; j < L_CHAIN; j++)
  // {
  //   Colour_d[j][0] = target_d[0];
  //   Colour_d[j][1] = target_d[1];
  //   Colour_d[j][2] = target_d[2];
  //   Colour_d[j][3] = target_d[3];
  //   Colour_d[j][4] = target_d[4];
  // }

  for (j = L_CHAIN - 1; j >= 0; j--)
  {
    Reduce(Colour_d, Colour_m, j);
    // check if Colour_m is in the data structure;
    if (int index = destWordExists(Colour_m, MAX_LEN) >= 0)
    {
      cout << "word found" << endl;
      Colour_m[0] = M[index][0];
      Colour_m[1] = M[index][1];
      Colour_m[2] = M[index][2];
      for (i = 0; i < j; i++)
      {
        Hash(Colour_m, Colour_d);
        Reduce(Colour_d, Colour_m, i);
      }
      // Colour_m is now the pre-image
      answer_m[0] = Colour_m[0];
      answer_m[1] = Colour_m[1];
      answer_m[2] = Colour_m[2];
      return 1;
    }
    Hash(Colour_m, Colour_d);

    //-------- search for the digest Colour_d[k] in the data structure.

    //-------- if found, call transverse the chain starting from the head to find the pre-image.
  }
  return (0);
}

//-----------   reading the next digest from the standard input  ----------------//
void readnextd(unsigned int d[5])
{
  cin.setf(ios::hex, ios::basefield);
  cin.setf(ios::uppercase);
  cin >> d[0];
  cin >> d[1];
  cin >> d[2];
  cin >> d[3];
  cin >> d[4];
}

int main(int argc, char *argv[])
{
  int found;
  int total_found;
  int total_not_found;
int chain_length;
  SHA1 sha;
  unsigned int d[5];  // 32 x 5 = 160 bits
  unsigned char m[3]; // 24 x 3

  //------------ R E A D     R A I N B O W    T A B L E  --------//
  chain_length = ReadT();
  cout << "READ RAINBOW DONE" << endl;

  //--------  PROJECT  INPUT/OUTPUT FORMAT ----------------//

  total_found = 0;
  total_not_found = 0;

  cout.setf(ios::hex, ios::basefield); //   setting display to Hexdecimal format.  (this is the irritating part of using C++).
  cout.setf(ios::uppercase);

  string line;
  ifstream myFile;
  myFile.open("SAMPLE_INPUT.data.txt");
  if (!myFile.is_open())
  {
    cout << "unable to open file" << endl;
    exit(0);
  }

  for (int i = 0; i < 5000; i++)
  {
    getline(myFile, line);
    istringstream iss(line);
    int j = 0;
    do
    {
      string sub;
      iss >> sub;
      std::stringstream ss;
      ss << std::hex << sub;
      ss >> d[j];
      j++;
    } while (iss);
    //cout << d[0] << d[1] << d[2] << d[3] << d[4] << endl;
    //readnextd(d);
    if (search(d, m) > 0)
    {
      total_found++;
      //------   print the word in hexdecimal format   -----------
      cout << setw(1) << (unsigned int)m[0] / 16;
      cout << setw(1) << (unsigned int)m[0] % 8;
      cout << setw(1) << (unsigned int)m[1] / 16;
      cout << setw(1) << (unsigned int)m[1] % 8;
      cout << setw(1) << (unsigned int)m[2] / 16;
      cout << setw(1) << (unsigned int)m[2] % 8 << endl;
    }
    else
    {
      total_not_found++;
      //cout << setw(6) << 0 << endl;
    }
  }
  myFile.close();
  cout.setf(ios::dec);
  cout << "Accuracy       C is: " << total_found / 5000.0 << endl;
  cout << "Speedup factor F is: " << (5000.0 / TOTAL_SHA) * 8388608 << endl;
}
