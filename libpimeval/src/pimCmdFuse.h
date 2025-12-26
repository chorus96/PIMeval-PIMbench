// File: pimCmdFuse.h
// PIMeval Simulator - PIM API Fusion

#ifndef LAVA_PIM_CMD_FUSE_H
#define LAVA_PIM_CMD_FUSE_H

#include "libpimeval.h"
#include "pimCmd.h"


//! @class  pimCmdFuse
//! @brief  Pim CMD: PIM API Fusion
class pimCmdFuse : public pimCmd
{
public:
  pimCmdFuse(PimProg prog) : pimCmd(PimCmdEnum::NOOP), m_prog(prog) {}
  virtual ~pimCmdFuse() {}
  virtual bool execute() override;
  virtual bool updateStats() const override;
private:
  PimProg m_prog;
};

#endif
