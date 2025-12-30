// File: libpimeval.h
// PIMeval Simulator - Library Interface

#ifndef LAVA_LIB_PIM_EVAL_H
#define LAVA_LIB_PIM_EVAL_H

#include <cstdint>
#include <cstdarg>
#include <vector>
#include <functional>

//! @brief  PIM API return status
enum PimStatus {
  PIM_ERROR = 0,
  PIM_OK,
};

//! @brief  PIM device types
enum PimDeviceEnum {
  PIM_DEVICE_NONE = 0,
  PIM_FUNCTIONAL,
  PIM_DEVICE_AQUABOLT,
};

/**
 * @enum PimDeviceProtocol
 * @brief Enum representing different memory protocols.
 *
 * @var PIM_DEVICE_PROTOCOL_DDR
 * Standard DDR protocol. Typically used in general-purpose memory systems.
 *
 * @var PIM_DEVICE_PROTOCOL_LPDDR
 * Low Power DDR (LPDDR) protocol.
 *
 * @var PIM_DEVICE_PROTOCOL_HBM
 * High Bandwidth Memory (HBM) protocol.
 * 
 * @var PIM_DEVICE_PROTOCOL_GDDR
 * Graphics Double Data Rate (GDDR) protocol.
*/
enum PimDeviceProtocolEnum {
  PIM_DEVICE_PROTOCOL_DDR = 0,
  PIM_DEVICE_PROTOCOL_LPDDR,
  PIM_DEVICE_PROTOCOL_HBM,
  PIM_DEVICE_PROTOCOL_GDDR,
};

//! @brief  PIM allocation types
enum PimAllocEnum {
  PIM_ALLOC_AUTO = 0, // Auto determine vertical or horizontal layout based on device type
  PIM_ALLOC_V,        // V layout, multiple regions per core
  PIM_ALLOC_H,        // H layout, multiple regions per core
  PIM_ALLOC_V1,       // V layout, at most 1 region per core
  PIM_ALLOC_H1,       // H layout, at most 1 region per core
};

//! @brief  PIM data copy types
enum PimCopyEnum {
  PIM_COPY_V,
  PIM_COPY_H,
};

//! @brief  PIM datatypes
enum PimDataType {
  PIM_BOOL = 0,
  PIM_INT8,
  PIM_INT16,
  PIM_INT32,
  PIM_INT64,
  PIM_UINT8,
  PIM_UINT16,
  PIM_UINT32,
  PIM_UINT64,
  PIM_FP32,
  PIM_FP16,
  PIM_BF16,
  PIM_FP8,
};

//! @brief  PIM device properties
struct PimDeviceProperties {
  PimDeviceEnum deviceType = PIM_DEVICE_NONE;
  PimDeviceEnum simTarget = PIM_DEVICE_NONE;
  unsigned numRanks = 0;
  unsigned numBankPerRank = 0;
  unsigned numSubarrayPerBank = 0;
  unsigned numRowPerSubarray = 0;
  unsigned numColPerSubarray = 0;
  unsigned numPIMCores = 0;
  unsigned numRowPerCore = 0;
  bool isHLayoutDevice = false;
};

typedef int PimCoreId;
typedef int PimObjId;

// PIMeval simulation
// CPU runtime between start/end timer will be measured for modeling DRAM refresh
void pimStartTimer();
void pimEndTimer();
void pimShowStats();
void pimResetStats();
bool pimIsAnalysisMode();

// Device creation and deletion
/**
 * @brief Creates and initializes a PIM (Processing-In-Memory) device with the specified configuration.
 *
 * @param deviceType      The type of PIM device to create (see PimDeviceEnum).
 * @param numRanks        Number of ranks in the device.
 * @param numBankPerRank  Number of banks per rank.
 * @param numSubarrayPerBank Number of subarrays per bank.
 * @param numRows         Number of rows in each subarray.
 * @param numCols         Number of columns in each row.
 * @param bufferSize      Optional on-chip buffer size (B) for the device (default is 0). This parameter is only applicable for AiM.
 * @return PimStatus      Status code indicating success or failure of device creation.
 */
PimStatus pimCreateDeviceFromConfig(PimDeviceEnum deviceType, const char* configFileName);
PimStatus pimGetDeviceProperties(PimDeviceProperties* deviceProperties);
PimStatus pimDeleteDevice();

// Resource allocation and deletion
PimObjId pimAlloc(PimAllocEnum allocType, uint64_t numElements, PimDataType dataType);
PimObjId pimAllocAssociated(PimObjId assocId, PimDataType dataType);
// Buffer will always be allocated in H layout; Current assumption is buffer is global and shared across all PIM cores in a chip/device. This assumption is based on AiM.
// The buffer is used for broadcasting data to all PIM cores in a chip/device.
// Please note that each chip/device will hold the same data in their respective buffers.
// TODO: Support per-core buffers (like UPMEM)
PimObjId pimAllocBuffer(uint32_t numElements, PimDataType dataType);
PimStatus pimFree(PimObjId obj);

// Data transfer
// Note: idxBegin and idxEnd specify the range of indexes to be processed by the PIM.
// The size of the host-side vector should match the size of this range on the PIM side.
// If the default values for idxBegin and idxEnd are used, the entire range of the PIM object will be considered.
// For PIM_BOOL type, please use std::vector<uint8_t> instead of std::vector<bool> as host data.
PimStatus pimCopyHostToDevice(void* src, PimObjId dest, uint64_t idxBegin = 0, uint64_t idxEnd = 0);
PimStatus pimCopyDeviceToHost(PimObjId src, void* dest, uint64_t idxBegin = 0, uint64_t idxEnd = 0);
PimStatus pimCopyDeviceToDevice(PimObjId src, PimObjId dest, uint64_t idxBegin = 0, uint64_t idxEnd = 0);
PimStatus pimCopyObjectToObject(PimObjId src, PimObjId dest);
PimStatus pimConvertType(PimObjId src, PimObjId dest);

// Logic and Arithmetic Operation
// Mixed data type extensions:
// - pimAdd/pimSub: If src1 is an integer vector, src2 can be a Boolean vector for accumulation purposes.
PimStatus pimAdd(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimSub(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimMul(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimDiv(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimAbs(PimObjId src, PimObjId dest);
PimStatus pimNot(PimObjId src, PimObjId dest);
PimStatus pimAnd(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimOr(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimXor(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimXnor(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimMin(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimMax(PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimAddScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimSubScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimMulScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimDivScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimAndScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimOrScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimXorScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimXnorScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimMinScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);
PimStatus pimMaxScalar(PimObjId src, PimObjId dest, uint64_t scalarValue);

// Relational operations - Dest object is BOOL type
PimStatus pimGT(PimObjId src1, PimObjId src2, PimObjId destBool);
PimStatus pimLT(PimObjId src1, PimObjId src2, PimObjId destBool);
PimStatus pimEQ(PimObjId src1, PimObjId src2, PimObjId destBool);
PimStatus pimNE(PimObjId src1, PimObjId src2, PimObjId destBool);
PimStatus pimGTScalar(PimObjId src, PimObjId destBool, uint64_t scalarValue);
PimStatus pimLTScalar(PimObjId src, PimObjId destBool, uint64_t scalarValue);
PimStatus pimEQScalar(PimObjId src, PimObjId destBool, uint64_t scalarValue);
PimStatus pimNEScalar(PimObjId src, PimObjId destBool, uint64_t scalarValue);

// multiply src1 with scalarValue and add the multiplication result with src2. Save the result to dest. 
PimStatus pimScaledAdd(PimObjId src1, PimObjId src2, PimObjId dest, uint64_t scalarValue);
PimStatus pimPopCount(PimObjId src, PimObjId dest);

// Only supported by bit-parallel PIM
PimStatus pimPrefixSum(PimObjId src, PimObjId dest);

// MAC operation: dest += src1 * src2
// Note: src2 is a global buffer that holds a vector of values to be multiplied with src1.
// Note: dest must be of the same data type as src1 and src2; Size of dest must be equal to the total number of PIM cores in the device.
// Note: The MAC operation is performed in parallel across all PIM cores, and each PIM core writes its local MAC value to the specific id of the dest.
// Note: User needs to ensure that dest vector is of size equal to the total number of PIM cores in the device, and contains `0` or any value that the user wants it to have as initial values.
PimStatus pimMAC(PimObjId src1, PimObjId src2, void* dest);

// Note: Reduction sum range is [idxBegin, idxEnd)
PimStatus pimRedSum(PimObjId src, void* sum, uint64_t idxBegin = 0, uint64_t idxEnd = 0);
// Min/Max Reduction APIs
PimStatus pimRedMin(PimObjId src, void* min, uint64_t idxBegin = 0, uint64_t idxEnd = 0);
PimStatus pimRedMax(PimObjId src, void* max, uint64_t idxBegin = 0, uint64_t idxEnd = 0);

// Bit slice operations
PimStatus pimBitSliceExtract(PimObjId src, PimObjId destBool, unsigned bitIdx);
PimStatus pimBitSliceInsert(PimObjId srcBool, PimObjId dest, unsigned bitIdx);
// Conditional operations
// pimCondCopy:         dest[i] = cond ? src[i] : dest[i]
// pimCondBroadcast:    dest[i] = cond ? scalar : dest[i]
// pimCondSelect:       dest[i] = cond ? src1[i] : src2[i]
// pimCondSelectScalar: dest[i] = cond ? src[i] : scalar
PimStatus pimCondCopy(PimObjId condBool, PimObjId src, PimObjId dest);
PimStatus pimCondBroadcast(PimObjId condBool, uint64_t scalarBits, PimObjId dest);
PimStatus pimCondSelect(PimObjId condBool, PimObjId src1, PimObjId src2, PimObjId dest);
PimStatus pimCondSelectScalar(PimObjId condBool, PimObjId src1, uint64_t scalarBits, PimObjId dest);

PimStatus pimBroadcastInt(PimObjId dest, int64_t value);
PimStatus pimBroadcastUInt(PimObjId dest, uint64_t value);
PimStatus pimBroadcastFP(PimObjId dest, float value);
PimStatus pimRotateElementsRight(PimObjId src);
PimStatus pimRotateElementsLeft(PimObjId src);
PimStatus pimShiftElementsRight(PimObjId src);
PimStatus pimShiftElementsLeft(PimObjId src);
PimStatus pimShiftBitsRight(PimObjId src, PimObjId dest, unsigned shiftAmount);
PimStatus pimShiftBitsLeft(PimObjId src, PimObjId dest, unsigned shiftAmount);

// AES sbox and inverse-box APIs
// Note: AES S-box and inverse S-box are treated separately because their bit-serial performance models differ.
// However, it is the user's responsibility to provide the appropriate LUT to ensure correct functionality.
// The function pimAesInverseSbox expects an inverse S-box LUT as its input.
PimStatus pimAesSbox(PimObjId src, PimObjId dest, const std::vector<uint8_t>& lut); 
PimStatus pimAesInverseSbox(PimObjId src, PimObjId dest, const std::vector<uint8_t>& lut); 

////////////////////////////////////////////////////////////////////////////////
// Experimental Feature: PIM API Fusion                                       //
////////////////////////////////////////////////////////////////////////////////
struct PimProg {
  template <typename... Args>
  void add(PimStatus(*api)(Args...), Args... args) {
    m_apis.push_back([=]() { return api(args...); });
  }
  std::vector<std::function<PimStatus()>> m_apis;
};
PimStatus pimFuse(PimProg prog);

////////////////////////////////////////////////////////////////////////////////
// Warning: Avoid using below customized APIs for functional simulation       //
//          Some are PIM architecture dependent, some are in progress         //
////////////////////////////////////////////////////////////////////////////////

// Data copy APIs that supports data transposition between V/H layout
PimStatus pimCopyHostToDeviceWithType(PimCopyEnum copyType, void* src, PimObjId dest, uint64_t idxBegin = 0, uint64_t idxEnd = 0);
PimStatus pimCopyDeviceToHostWithType(PimCopyEnum copyType, PimObjId src, void* dest, uint64_t idxBegin = 0, uint64_t idxEnd = 0);

// Dual contact reference: Create a new PimObjId that references to the negation of the original PimObjId
// Do not use a dual contact reference PimObjId as refId
PimObjId pimCreateDualContactRef(PimObjId refId);

// Ranged reference: Create a new PimObjId that references to a range of the original PimObjId
// This is not available for now
PimObjId pimCreateRangedRef(PimObjId refId, uint64_t idxBegin, uint64_t idxEnd);


////////////////////////////////////////////////////////////////////////////////
// Warning: Do not use below micro-ops level APIs for functional simulation   //
////////////////////////////////////////////////////////////////////////////////

// BitSIMD micro ops
// Note: Below APIs are for low-level micro-ops programming but not for functional simulation
// BitSIMD-V: Row-wide bit registers per subarray
enum PimRowReg {
  PIM_RREG_NONE = 0,
  PIM_RREG_SA,
  PIM_RREG_R1,
  PIM_RREG_R2,
  PIM_RREG_R3,
  PIM_RREG_R4,
  PIM_RREG_R5,
  PIM_RREG_R6,
  PIM_RREG_R7,
  PIM_RREG_R8,
  PIM_RREG_R9,
  PIM_RREG_R10,
  PIM_RREG_R11,
  PIM_RREG_R12,
  PIM_RREG_R13,
  PIM_RREG_R14,
  PIM_RREG_R15,
  PIM_RREG_R16,
  PIM_RREG_MAX
};

#endif
