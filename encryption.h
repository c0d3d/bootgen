/******************************************************************************
* Copyright 2015-2020 Xilinx, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

#ifndef _ENCRYPTION_CONTEXT_
#define _ENCRYPTION_CONTEXT_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <string>
#include <stdint.h>
#include <sstream>
#include <vector>
#include <string>

#include "bootgenenum.h"
#include "options.h"

/* Forward Class References */
class PartitionHeader;
class Options;
class DataMover;
class BootImage;

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define WORDS_PER_AES_BLOCK                 4
#define WORDS_PER_AES_KEY                   8
#define BYTES_PER_AES_KEY                   32

#define ENCRYPTION_ENABLE                   0x00000040
#define EFUSE_KEY_SOURCE                    0x80000000
#define BBRAM_KEY_SOURCE                    0x00000000

#define ENCRYPTION_KEY_SRC_OFFSET           36
#define ENCRYPTION_ENABLE_WORD_OFFSET       44
#define CBC_WORD_OFFSET                     116
#define ENCRYPTION_DATA_LENGTH_OFFSET       136

#define FPGA_ENCRYPTION_KEY_SRC_OFFSET      4
#define FPGA_ENCRYPTION_ENABLE_WORD_OFFSET  12
#define FPGA_CBC_WORD_OFFSET                84
#define FPGA_ENCRYPTION_DATA_LENGTH_OFFSET  104
#define WORDS_PER_IV                        3
#define BYTES_PER_IV                        12


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class EncryptionContext
{
public:
    EncryptionContext()
        : deviceName("")
        , metalFile("")
        , bhKekIVFile("")
        , aesFilename("")
        , aesKey(NULL)
        , aesOptKey(NULL)
        , aesIv(NULL)
        , aesSeed(NULL)
        , aesContext(NULL)
        , aesLabel(NULL)
        , aesContextBytes(6)
        , aesLabelBytes(32)
        , outBufKDF(NULL)
        , koLength(0)
        , kI(NULL)
        , fixedInputDataByteLength(0)
        , fixedInputData(NULL)
        , fixedInputDataExits(false)
        , verifyKo(NULL)
        , Ko(NULL)
    { };

    virtual ~EncryptionContext() { };

    virtual Encryption::Type Type() 
    {
        return Encryption::None;
    }
    virtual void Process(BootImage& bi, PartitionHeader* partition) {};
    virtual void Process(BootImage& bi) {};
    virtual void WriteEncryptionKeyFile(const std::string& baseFileName, bool useOptionalKey, uint32_t blocks) {};
    virtual void ReadEncryptionKeyFile(const std::string& keyFileName) {};
    virtual void GenerateEncryptionKeyFile(const std::string& baseFileName, Options& options);
    virtual void GenerateAesKey(void) {};
    virtual void SetAesKey(const uint8_t* binarykey) {};
    virtual void ReadBhIv(uint8_t* bhIv) {};
    virtual void GenerateGreyKey() {};
    virtual void GenerateMetalKey() {};
    
    void SetAesFileName(std::string);
    void SetMetalKeyFile(std::string file);
    void SetBHKekIVFile(std::string file);
    void SetDeviceName(const std::string& deviceName0);
    void PackHex(const std::string & hexString, uint8_t * hexData);
    void SetRandomSeed(void);
    void SetAesKeyString(const std::string& ASCIIkey);
    std::string ConvertKeyIvToString(uint8_t *keyIv, uint8_t size);

    void GetRandomData(uint8_t* randomData, uint32_t randomDatabyteLength);
    uint32_t static GetTotalEncryptionBlocks(Binary::Length_t partitionSize, std::vector<uint32_t> encrBlocks, uint32_t defEncrBlockSize, Binary::Length_t * lastBlock);

    void CheckForSameAesKeyFiles(std::vector<std::string>);
    void CheckForExtraKeyIVPairs(uint32_t, std::string name);
    void CheckForRepeatedKeyIVPairs(std::vector<std::string> aesKeyFileVec, bool maskKey0IV0);

    std::string GetAesFileName(void);
    std::string GetMetalKeyFile(void);
    std::string GetBHKekIVFile(void);

    void CounterModeKDF(uint32_t blocks, std::string filename, bool dump);
    void ParseKDFTestVectorFile(std::string filename);
    void CAVPonCounterModeKDF(std::string filename);
    void KDF(uint32_t blocks, std::string keyFilename, bool encrDump);

    virtual void SetAesSeed(const uint8_t * key) {};
    void GenerateAesSeed(void);
    virtual const uint32_t* GetAesSeed(void) { return NULL; };

    virtual void SetAesLabel(const uint8_t * key, int bytes) {};
    virtual const uint8_t* GetAesLabel(void) { return NULL; };

    virtual void SetAesContext(const uint8_t * key, int bytes) {};
    virtual const uint8_t* GetAesContext(void) { return NULL; };

    std::string aesFilename;
protected:
    uint32_t* aesKey;
    uint32_t* aesOptKey;
    uint32_t* aesIv;
    std::vector<std::string> aesKeyVec;
    std::vector<std::string> aesIvVec;
    uint32_t* aesSeed;
    uint8_t* aesContext;
    uint8_t* aesLabel;
    uint32_t* outBufKDF;
    int aesContextBytes;
    int aesLabelBytes;
    std::string deviceName;
    std::string metalFile;
    std::string bhKekIVFile;

    uint32_t koLength;
    uint8_t* Ko;
    uint8_t* kI;
    uint32_t fixedInputDataByteLength;
    uint8_t* fixedInputData;
    bool fixedInputDataExits;
    uint8_t* verifyKo;
};

/******************************************************************************/
class NoneEncryptionContext : public EncryptionContext 
{
public:
    Encryption::Type Type()
    {
        return Encryption::None;
    }    
};

/******************************************************************************/
class AesGcmEncryptionContext
{
public:
    AesGcmEncryptionContext() { };
    ~AesGcmEncryptionContext() { };

    void AesGcm256Encrypt(unsigned char* gcm_pt, int pt_len, unsigned char* gcm_key, unsigned char* gcm_iv, unsigned char* gcm_aad, int aad_len,
        unsigned char* gcm_ct, int& ct_len, unsigned char* gcm_tag);
    void AesGcm256Decrypt(unsigned char* gcm_pt, int& pt_len, unsigned char* gcm_key, unsigned char* gcm_iv, unsigned char* gcm_aad, int aad_len,
        unsigned char* gcm_ct, int ct_len, unsigned char* gcm_tag);
};
#endif
