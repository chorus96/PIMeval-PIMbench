// PIMeval Simulator - Application Utilities

#ifndef PIM_FUNC_SIM_APPS_UTIL_H
#define PIM_FUNC_SIM_APPS_UTIL_H

#include <iostream>
#if defined(_OPENMP)
#include <omp.h>
#endif
#include <vector>
#include <iomanip>
#include <chrono>
#include <random>

#include "libpimeval.h"
#include <map>
#include <fstream>

using namespace std;

#define MAX_NUMBER 1024

void getVector(uint64_t vectorLength, std::vector<int> &srcVector)
{
  srcVector.resize(vectorLength);
#pragma omp parallel for
  for (uint64_t i = 0; i < vectorLength; ++i)
  {
    srcVector[i] = i % MAX_NUMBER;
  }
}

void getMatrix(int row, int column, int padding, std::vector<std::vector<int>> &inputMatrix)
{
  inputMatrix.resize(row + 2 * padding, std::vector<int>(column + 2 * padding, 0));
#pragma omp parallel for
  for (int i = padding; i < row + padding; ++i)
  {
    for (int j = padding; j < column + padding; ++j)
    {
      inputMatrix[i][j] = i*j % MAX_NUMBER;
    }
  }
}

bool createDevice(const char *configFile)
{
  if (configFile == nullptr)
  {
    // Each rank has 8 chips; Total Bank = 16; Each Bank contains 32 subarrays;
    unsigned numRanks = 4;
    unsigned numBankPerRank = 128; // 8 chips * 16 banks
    unsigned numSubarrayPerBank = 32;
    unsigned numRows = 1024;
    unsigned numCols = 8192;

    PimStatus status = pimCreateDevice(PIM_FUNCTIONAL, numRanks, numBankPerRank, numSubarrayPerBank, numRows, numCols);
    if (status != PIM_OK)
    {
      std::cout << "Abort" << std::endl;
      return false;
    }
  }
  else
  {
    PimStatus status = pimCreateDeviceFromConfig(PIM_FUNCTIONAL, configFile);
    if (status != PIM_OK)
    {
      std::cout << "Abort" << std::endl;
      return false;
    }
  }
  return true;
}

#endif
