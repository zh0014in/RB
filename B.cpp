/*----------------------------------
This program build the rainbow table  for the other program F.cpp.
------------------------------------*/

#include <iostream>
#include <sstream>
#include <unordered_map>
#include "sha1.h"

using namespace std;

//  A table to store all the words and digests.
//    infeasible to have such large table in practice.ı
//    for programming convenient, we store the whole table in memory.
#define HT 1048576
#define QLength 16000
unsigned char Word[HT][3];
unsigned char M[HT][3];
unsigned char D[HT][3];
unsigned char *pM;
unsigned char *pD;
unsigned int Q[QLength];

unordered_map<unsigned long, unsigned int> HashTable;
unordered_map<unsigned long, unsigned int>::const_iterator G;

void printDigest(unsigned int d[5])
{
  for (int i = 0; i < 5; i++)
  {
    cout << hex << d[i] << " ";
  }
  cout << endl;
}

void printWord(unsigned char m[3])
{
  for (int i = 0; i < 3; i++)
  {
    cout << hex << m[i] << " ";
  }
  cout << endl;
}

unsigned int char2int(unsigned char m[3]){
  unsigned int result = 0;
  result += m[0] << 16;
  result += m[1] << 8;
  result += m[2];
  return result;
}

int insert2Q(unsigned char m[3]){
  if(Q[QLength-1] > 0){
    return -2;
  }
  unsigned int result = char2int(m);
  for(int i = 0; i < QLength; i++){
    if(Q[i] > result){
      for(int j = QLength - 1; j > i; j--){
        Q[j] = Q[j-1];
      }
      Q[i] = result;
      return 1;
    }
    else if(Q[i] == result){
      return -1;
    }
    else{
      continue;
    }
  }
  return 0;
}

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
  m[0] = (unsigned char)((d[(0+i)%5] + i) % 256); //8 bits
  m[1] = (unsigned char)((d[(1+i)%5]) % 256);     //8 bits
  m[2] = (unsigned char)((d[(2+i)%5]) % 256);     //8 bits

  return (0);
}

int meetInitialValue(unsigned char iv[3], unsigned char m[3])
{
  if (iv[0] == m[0] && iv[1] == m[1] && iv[2] == m[2])
  {
    return 1;
  }
  return -1;
}

int destWordExists(unsigned char m[3], int n_chain)
{
  unsigned char *pD;
  for (int i = 0; i < n_chain; i++)
  {
    pD = D[i];
    if (*pD == m[0] && *(pD + 1) == m[1] && *(pD + 2) == m[2])
    {
      return i;
    }
  }
  return -1;
}

void writeToFile(int rounds, int N_CHAIN)
{

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
}

int buildT(int rounds)
{
  unsigned int d[5];
  unsigned char m[3];
  int N_CHAIN = 0;
  m[0] = Word[0][0];
  m[1] = Word[0][1];
  m[2] = Word[0][2];
  for (int i = 0; i < HT; i++)
  {
    //next_word(m);
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

    // copy destination word to D
    D[N_CHAIN][0] = m[0];
    D[N_CHAIN][1] = m[1];
    D[N_CHAIN][2] = m[2];
    N_CHAIN++;
    cout << "built " << dec << N_CHAIN << " records." << endl;

    if (meetInitialValue(M[N_CHAIN], m) > 0)
    {
      do
      {
        next_word(m);
        cout << "word exists "; // << m[0] << ", " << m[1] << ", " << m[2] << endl;
        printWord(m);
      } while (destWordExists(m, N_CHAIN) >= 0);
      continue;
    }
    //printWord(m);
  }
  //writeToFile(rounds, N_CHAIN);
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
  for(int i = 0; i < QLength; i++){
    Q[i] = 0;
  }
  //----   Build the table.
  cout << "building the table..." << endl;
  buildT(rounds);
}
