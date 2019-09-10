// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2018 The PIVX developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPORK_H
#define SPORK_H

#include "base58.h"
#include "hash.h"
#include "key.h"
#include "main.h"
#include "net.h"
#include "sporkid.h"
#include "sync.h"
#include "util.h"

#include "obfuscation.h"
#include "protocol.h"

#define SPORK_2_SWIFTTX_DEFAULT 978307200                         //2001-1-1
#define SPORK_3_SWIFTTX_BLOCK_FILTERING_DEFAULT 1424217600        //2015-2-18
#define SPORK_5_MAX_VALUE_DEFAULT 1000                            //1000 PIV
#define SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT_DEFAULT 4070908800 //OFF
#define SPORK_9_MASTERNODE_BUDGET_ENFORCEMENT_DEFAULT 4070908800  //OFF
#define SPORK_10_MASTERNODE_PAY_UPDATED_NODES_DEFAULT 4070908800  //OFF
#define SPORK_13_ENABLE_SUPERBLOCKS_DEFAULT 4070908800            //OFF
#define SPORK_14_NEW_PROTOCOL_ENFORCEMENT_DEFAULT 4070908800      //OFF
#define SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2_DEFAULT 4070908800    //OFF
#define SPORK_16_ZEROCOIN_MAINTENANCE_MODE_DEFAULT 4070908800     //OFF

class CSporkMessage;
class CSporkManager;

extern std::map<uint256, CSporkMessage> mapSporks;
extern CSporkManager sporkManager;

//
// Spork Classes
// Keep track of all of the network spork settings
//

class CSporkMessage
{
private:


public:
    std::vector<unsigned char> vchSig;
    SporkId nSporkID;
    int64_t nValue;
    int64_t nTimeSigned;

    CSporkMessage(SporkId nSporkID, int64_t nValue, int64_t nTimeSigned) : nSporkID(nSporkID), nValue(nValue), nTimeSigned(nTimeSigned) {}
    CSporkMessage() : nSporkID((SporkId)0), nValue(0), nTimeSigned(0) {}

    uint256 GetHash() { return HashQuark(BEGIN(nSporkID), END(nTimeSigned)); }
    bool Sign(std::string strSignKey);
    bool CheckSignature(bool fRequireNew = false);
    void Relay();

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(nSporkID);
        READWRITE(nValue);
        READWRITE(nTimeSigned);
        READWRITE(vchSig);
    }
};


class CSporkManager
{
private:
    mutable CCriticalSection cs;
    std::string strMasterPrivKey;
    std::map<int, CSporkMessage> mapSporksActive;

public:
    CSporkManager() {}

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(mapSporksActive);
        // we don't serialize private key to prevent its leakage
    }

    void Clear();
    void LoadSporksFromDB();

    void ProcessSpork(CNode* pfrom, std::string& strCommand, CDataStream& vRecv);
    int64_t GetSporkValue(SporkId nSporkID);
    void ExecuteSpork(SporkId nSporkID, int nValue);
    bool UpdateSpork(SporkId nSporkID, int64_t nValue);

    bool IsSporkActive(SporkId nSporkID);
    std::string GetSporkNameByID(SporkId id);
    SporkId GetSporkIDByName(std::string strName);

    bool SetPrivKey(std::string strPrivKey);
    std::string ToString() const;
};

#endif
