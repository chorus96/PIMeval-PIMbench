// File: pimCmdFuse.cpp
// PIMeval Simulator - PIM API Fusion

#include "pimCmdFuse.h"
#include <cstdio>

//! @brief  Pim CMD: PIM API Fusion
bool
pimCmdFuse::execute()
{
  if (m_debugCmds) {
    std::printf("PIM-Cmd: API Fusion\n");
  }

  // Functional simulation
  // TODO: skip original updateStats
  bool success = true;
  for (auto& api : m_prog.m_apis) {
    PimStatus status = api();
    if (status != PIM_OK) {
      success = false;
      break;
    }
  }

  // Analyze API fusion opportunities
  success = success && updateStats();
  return success;
}

//! @brief  Pim CMD: PIM API Fusion - update stats
bool
pimCmdFuse::updateStats() const
{
  // TODO: Parse m_prog and update stats
  return true;
}