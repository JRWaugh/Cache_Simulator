#pragma once

#include <memory>
#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include "MainMemory.h"

using namespace std;

enum class Policy { FIFO = 1, LRU, Random };

class Cache : public MainMemory {
    static const unsigned char mDirtyBit = 1;
    static const unsigned char mValidBit = 2;
    friend ostream& operator<<(ostream& os, const Cache& c);

private:
    /*****	 Cache Set Config	 *****/
    /* LRU | 0 | 1 | 2 |...| N | MRU */
    vector<vector<pair<unsigned char, unsigned int>>> mSets;

    /* Cache parameters. */
    uint32c_t mBlockSize, mSetSize, mTotalSize, mSetCount;
    Policy mPolicy;

    /* Cache statistics. */
    unsigned int mStoreHit, mStoreMiss, mLoadHit, mLoadMiss, mDirtyEvict;

    /* Pointer to the next cache level or main memory. */
    shared_ptr<MainMemory> mLowerMem;

    /* Random generator for Random Replacement strategy. */
    mt19937 gen;

public:
    Cache(uint32c_t blockSize, uint32c_t setSize, uint32c_t totalSize, Policy policy, uint32c_t accessTime, uint32c_t accessTimeLower);
    std::optional<uint32c_t> accessAddress(uint32c_t address, const unsigned char instruction);
    void setLowerMem(const shared_ptr<MainMemory> lowerMem) {
        mLowerMem = lowerMem;
    }
    void invalidateCache();
    void resetCacheStats() {
        /* Used for clearing out cache statistics without changing any of the parameters. */
        mDirtyEvict = mStoreHit = mStoreMiss = mLoadHit = mLoadMiss = 0;
    }
    const double getAMAT() const;
};
