#pragma once
#include "common/common.h"
#include "fmfileio/fileio.h"
#include "ym3438.h"
#include "ymdeltat.h"
#include "emu2149/emu2149.h"
#include "common/fmtimer.h"
#include <cstdint>

namespace FM
{
struct Nuked2608State
{
	ym3438_t* chip = nullptr;
	PSG* ssg = nullptr;
	int clock;
	uint32_t dramSize;
};

class OPNA : public Timer
{
public:
    OPNA(IFILEIO* pfileio);
    virtual ~OPNA();
    void setfileio(IFILEIO* pfileio);
    bool Init(uint32_t clock, uint32_t rate, bool = false, const TCHAR* rhythmpath=0);
    bool LoadRhythmSample(const TCHAR* path);

    bool SetRate(uint32_t clock, uint32_t rate, bool = false);
    void Mix(Sample* buffer, int nsamples);
        
    void Reset();
    void SetReg(uint32_t addr, uint32_t data);
    uint32_t GetReg(uint32_t addr);

    void SetPrescaler(uint p);
    
    void SetVolumeFM(int db);
    void SetVolumePSG(int db);
    void SetVolumeADPCM(int db);
    void SetVolumeRhythmTotal(int db);
    void SetVolumeRhythm(int index, int db);

    void SetStatus(uint32_t bits);
    void ResetStatus(uint32_t bits);
    inline uint32_t ReadStatus() { return status & 0x03; }

protected:
    Nuked2608State state;

    int internalRate[2];
    float volumeRatio[2];

    uint32_t rate;
    uint32_t status;

private:
    void StopDevice();
    void UpdateStream(Sample** outputs, int nSamples);
	void UpdateSsgStream(Sample** outputs, int nSamples);
    void LinearInterpolate(char chanType, size_t nSamples, size_t);

    Sample* buffer[2][2];
    Sample* resampBuf[2][2];
    float rateRatio[2];

    uint32_t stmask;
        
};

}