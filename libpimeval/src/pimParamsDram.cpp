// File: pimParamsDram.cc
// PIMeval Simulator - DRAM parameters

#include "pimParamsDram.h"
#include "pimUtils.h"
#include "pimParamsHBMDram.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <stdexcept>

// Static factory method to create appropriate subclass based on protocol enum
std::unique_ptr<pimParamsDram> pimParamsDram::create(PimDeviceProtocolEnum deviceProtocol)
{
  if (deviceProtocol == PIM_DEVICE_PROTOCOL_HBM)
  {
    return std::make_unique<pimParamsHBMDram>();
  } 
  else
  {
    std::string errorMessage("PIM-Error: Inavalid DRAM protocol parameter.\n");
    throw std::invalid_argument(errorMessage);
  }
}

// Static factory method to create appropriate subclass based on config file
std::unique_ptr<pimParamsDram> pimParamsDram::createFromConfig(const std::string& memConfigFilePath)
{
  std::unordered_map<std::string, std::string> params = pimUtils::readParamsFromConfigFile(memConfigFilePath);

  // Check if the "protocol" key exists
  if (params.find("protocol") == params.end())
  {
    std::string errorMessage("PIM-Error: Missing DRAM protocol parameter.\n");
    throw std::invalid_argument(errorMessage);
  }

  // Extract protocol from params
  std::string deviceProtocol = params["protocol"];

  // Instantiate the appropriate subclass based on the protocol
  if (deviceProtocol == "HBM" || deviceProtocol == "HBM2") {
    return std::make_unique<pimParamsHBMDram>(params);
  }
  else
  {
    throw std::invalid_argument("Unknown protocol: " + deviceProtocol);
  }
}

