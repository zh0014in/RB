/*----------------------------------
This program build the rainbow table  for the other program F.cpp.
------------------------------------*/

#include <iostream>
#include <sstream>
#include <unordered_map>
#include "sha1.h"

using namespace std;

//  A table to store all the words and digests.
//    infeasible to have such large table in practice.
//    for programming convenient, we store the whole table in memory.
#define HT 10000 // 1048576
unsigned char Word[HT][3];
unsigned char M[HT][3];
unsigned char D[HT][3];
unsigned char *pM;
unsigned char *pD;

unordered_map<unsigned long, unsigned int> HashTable;
unordered_map<unsigned long, unsigned int>::const_iterator G;

//----  return the next word to be considered
void next_word(unsigned char m[3])
{
  if (pM == NULL)
  {
    pM = Word[0];
  }
  else
  {
    pM += 3;
  }
  m[0] = *pM;
  m[1] = *(pM + 1);
  m[2] = *(pM + 2);
}

//-------   Hash
int Hash(unsigned char m[3], unsigned int d[5])
{
  SHA1 sha;
  sha.Reset();
  sha.Input(m[0]);
  sha.Input(m[1]);
  sha.Input(m[2]);
  sha.Result(d);
  return (0);
}

//-------  Reduce
int Reduce(unsigned int d[5], unsigned char m[3], int i)
{
  m[0] = (unsigned char)((d[0] + i) % 256); //8 bits
  m[1] = (unsigned char)((d[1]) % 256);     //8 bits
  m[2] = (unsigned char)((d[2]) % 256);     //8 bits

  return (0);
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

int buildT(int rounds)
{
  unsigned int d[5];
  unsigned char m[3];
  int N_CHAIN = 0;
  for (int i = 0; i < HT; i++)
  {
    next_word(m);
    // copy value of source word to M first;
    M[N_CHAIN][0] = m[0];
    M[N_CHAIN][1] = m[1];
    M[N_CHAIN][2] = m[2];
    // build the chain.
    // check whether to keep the chain.
    // You may want to drop the chain, for e.g. if the digest is already in the table.
    // This form the main component of your program.
    for (int j = 0; j < rounds; j++)
    {
      Hash(m, d);
      Reduce(d, m, j);
    }
    if (destWordExists(m,N_CHAIN) >= 0)
    {
      continue;
    }
    // copy destination word to D
    D[N_CHAIN][0] = m[0];
    D[N_CHAIN][1] = m[1];
    D[N_CHAIN][2] = m[2];
    N_CHAIN++;
    cout << "built " << N_CHAIN << " records." << endl;
  }

  //---    Write to the output file
  //note that to reduce the size of the table, it is not neccessary to write the full digest.
  cout << "writing to file." << endl;
  std::stringstream sstm;
  sstm << "table_" << rounds << ".data";
  const char *fileName = sstm.str().c_str();
  FILE *pFile = fopen("table.data", "wb");
  for (long i = 0; i < N_CHAIN; i++)
  {
    fwrite(&(M[i][2]), sizeof(unsigned char), 1, pFile);
    fwrite(&(M[i][1]), sizeof(unsigned char), 1, pFile);
    fwrite(&(M[i][0]), sizeof(unsigned char), 1, pFile);

    fwrite(&(D[i][2]), sizeof(unsigned char), 1, pFile);
    fwrite(&(D[i][1]), sizeof(unsigned char), 1, pFile);
    fwrite(&(D[i][0]), sizeof(unsigned char), 1, pFile);
  }
  return (0);
}

int main(int argc, char *argv[])
{
  cout << "Building table with " << argv[1] << " turns." << endl;
  int rounds = atoi(argv[1]);
  SHA1 sha;

  //----  Setting the parameters
  cout << "setting the parameters..." << endl;
  for (int i = 0; i < HT; i++)
  {
    Word[i][0] = i & 0xff;
    Word[i][1] = (i >> 4) & 0xff;
    Word[i][2] = (i >> 8) & 0xff;
  }
  //----   Build the table.
  cout << "building the table..." << endl;
  buildT(rounds);
}
