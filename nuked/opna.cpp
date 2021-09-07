#include "opna.h"
#include "common/chip_defs.h"
#include <cmath>
#include <algorithm>
#include <iostream>

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
    return std::min(hi, std::max(v, lo));
}

namespace FM
{
namespace // SSG callbacks
{
void setSsgClock(void *param, int clock)
{
	auto state = reinterpret_cast<Nuked2608State*>(param);
	if (state->ssg) PSG_set_clock(state->ssg, clock);
}

void writeSsg(void *param, int address, int data)
{
	auto state = reinterpret_cast<Nuked2608State*>(param);
	if (state->ssg) PSG_writeIO(state->ssg, address, data);
}

int readSsg(void *param)
{
	auto state = reinterpret_cast<Nuked2608State*>(param);
	return (state->ssg ? PSG_readIO(state->ssg) : 0);
}

void resetSsg(void *param)
{
	auto state = reinterpret_cast<Nuked2608State*>(param);
	if (state->ssg) PSG_reset(state->ssg);
}

const struct OPN2mod_psg_callbacks SSG_INTF =
{
	setSsgClock,
	writeSsg,
	readSsg,
	resetSsg
};
}

enum SoundSourceIndex : char { FM = 0, SSG = 1 };
#define CHIP_SMPL_BUF_SIZE_ 0x10000

OPNA::OPNA(IFILEIO* pfileio)
{
    for (int pan = STEREO_LEFT; pan <= STEREO_RIGHT; ++pan) {
		for (int src = 0; src < 2; ++src)
		{
			buffer[src][pan]    = new Sample[CHIP_SMPL_BUF_SIZE_];
			resampBuf[src][pan] = new Sample[CHIP_SMPL_BUF_SIZE_];
		}
	}
}

OPNA::~OPNA()
{
	StopDevice();
    for (int pan = STEREO_LEFT; pan <= STEREO_RIGHT; ++pan) {
		for (int src = 0; src < 2; ++src)
		{
			delete[] buffer[src][pan];
			delete[] resampBuf[src][pan];
		}
	}
}

void OPNA::setfileio(IFILEIO* pfileio)
{
	// unused
}

#define DRAM_SIZE 262144 // 256KiB

bool OPNA::Init(uint32_t clock, uint32_t rate, bool ipflag, const TCHAR* path)
{
	if (!state.ssg)
	{
		state.ssg = PSG_new(0, 0); // will be set later
		PSG_setVolumeMode(state.ssg, 1);	// YM2149 volume mode
		PSG_set_quality(state.ssg, 0);
	}

	if (!state.chip)
	{
		state.chip = reinterpret_cast<ym3438_t*>(calloc(1, sizeof(ym3438_t)));
		if (!state.chip) {
			StopDevice();
			return 0;
		}
	}

	state.clock = clock;
	state.dramSize = DRAM_SIZE;
    SetRate(clock, rate);
	SetVolumeFM(0);
	SetVolumePSG(0);
    SetVolumeADPCM(0);
    SetVolumeRhythmTotal(0);
    for (int i=0; i<6; i++)
        SetVolumeRhythm(0, 0);
	SetPrescaler(0);
    return true;
}

void OPNA::Reset()
{
    OPN2_FlushBuffer(state.chip);
	OPN2_Reset(state.chip, state.clock, &SSG_INTF, &state, state.dramSize);
	Timer::Reset();
	SetPrescaler(0);

	stmask = ~0x1c;
}

void OPNA::StopDevice()
{
	if (state.chip) {
		OPN2_Destroy(state.chip);
		free(state.chip);
		state.chip = nullptr;
	}
	if (state.ssg)
	{
		PSG_delete(state.ssg);
		state.ssg = nullptr;
	}
}

// This is not needed; the samples are embedded in 2608rom.h
bool OPNA::LoadRhythmSample(const TCHAR* path)
{
	return true;
}

bool OPNA::SetRate(uint32_t clock, uint32_t rate, bool ipflag)
{
	internalRate[FM] = clock / 144;
	int clockSsg = clock / 4;
	internalRate[SSG] = clockSsg / 8;
	PSG_set_clock(state.ssg, clockSsg);
	PSG_set_rate(state.ssg, internalRate[SSG]);

    this->rate = rate;
    rateRatio[FM] = static_cast<float>(internalRate[FM]) / rate;
    rateRatio[SSG] = static_cast<float>(internalRate[SSG]) / rate;

	Reset();

    return true;
}

void OPNA::LinearInterpolate(char chanType, size_t nSamples, size_t)
{
    Sample** src = buffer[chanType];
	for (int pan = STEREO_LEFT; pan <= STEREO_RIGHT; ++pan) {
		for (size_t n = 0; n < nSamples; ++n) {
			float curnf = n * rateRatio[chanType];
			int curni = static_cast<int>(curnf);
			float sub = curnf - curni;
			if (sub) {
				resampBuf[chanType][pan][n] = static_cast<Sample>(src[pan][curni] + (src[pan][curni + 1] - src[pan][curni]) * sub);
			}
			else /* if (sub == 0) */ {
				resampBuf[chanType][pan][n] = src[pan][curni];
			}
		}
	}
}

void OPNA::Mix(Sample* stream, int nSamples)
{
	Sample **bufFM, **bufSSG;

	// Set FM buffer
	if (internalRate[FM] == rate)
    {
		UpdateStream(buffer[FM], nSamples);
		bufFM = buffer[FM];
	}
	else
    {
		size_t intrSize = static_cast<size_t>(ceil(nSamples * rateRatio[FM]));
		UpdateStream(buffer[FM], intrSize);
		LinearInterpolate(FM, nSamples, intrSize);
        bufFM = resampBuf[FM];
	}

	// Set SSG buffer
	if (internalRate[SSG] == rate)
    {
		UpdateSsgStream(buffer[SSG], nSamples);
		bufSSG = buffer[SSG];
	}
	else
    {
		size_t intrSize = static_cast<size_t>(std::ceil(nSamples * rateRatio[SSG]));
		UpdateSsgStream(buffer[SSG], intrSize);
		LinearInterpolate(SSG, nSamples, intrSize);
        bufSSG = resampBuf[SSG];
	}

	for (size_t i = 0; i < nSamples; ++i) {
		for (int pan = STEREO_LEFT; pan <= STEREO_RIGHT; ++pan) {
			double s = volumeRatio[FM] * bufFM[pan][i] + volumeRatio[SSG] * bufSSG[pan][i];
			*stream++ += clamp<Sample>(s, INT32_MIN, INT32_MAX);
		}
	}
}

void OPNA::SetPrescaler(uint p)
{
	static const char table[3][2] = { { 6, 4 }, { 3, 2 }, { 2, 1 } };

	uint fmclock = state.clock / table[p][0] / 24; // clock / t / 12 / 2
	SetTimerBase(fmclock);

	PSG_set_clock(state.ssg, state.clock / table[p][1]);
}

void OPNA::SetReg(uint32_t addr, uint32_t data)
{
	// Handle timer-related stuff for compatibility with fmgen
	switch (addr)
	{
	case 0x24: case 0x25: // Timer A
		SetTimerA(addr, data);
		break;
		
	case 0x26: // Timer B
		SetTimerB(data);
		break;

	case 0x27: // Timer control
		SetTimerControl(data);
		break;

	case 0x2d: case 0x2e: case 0x2f: // Prescaler
		SetPrescaler(addr-0x2d);
		break;

	case 0x110: // IRQ flag control
		if (data & 0x80)
			status = 0;
		else
			stmask = ~(data & 0x1f);
		break;
	
	}

    if (addr & 0x100)
    {
		// Port B
		OPN2_WriteBuffered(state.chip, 2, addr & 0xff);
		OPN2_WriteBuffered(state.chip, 3, data & 0xff);
    }
    else
    {
		// Port A
        OPN2_WriteBuffered(state.chip, 0, addr & 0xff);
		OPN2_WriteBuffered(state.chip, 1, data & 0xff);
    }
}

uint32_t OPNA::GetReg(uint32_t addr)
{
    if (addr & 0x100) OPN2_WriteBuffered(state.chip, 2, addr & 0xff); // Port B
    else OPN2_WriteBuffered(state.chip, 0, addr & 0xff); // Port A
	return OPN2_Read(state.chip, 1);
}

void OPNA::SetVolumeFM(int db)
{
	db = std::min(db, 20);
	if (db > -192)
		volumeRatio[FM] = pow(10.0, db / 40.0);
	else
		volumeRatio[FM] = 0;
}

void OPNA::SetVolumePSG(int db)
{
	db = std::min(db, 20);
	if (db > -192)
		volumeRatio[SSG] = pow(10.0, db / 40.0);
	else
		volumeRatio[SSG] = 0;
}

void OPNA::SetVolumeADPCM(int db)
{
    db = std::min(db, 20);
	float ratio;
	if (db > -192)
		ratio = pow(10.0, db / 40.0);
	else
		ratio = 0;

    SetReg(0x10b, static_cast<uint8_t>(255 * ratio));
}

void OPNA::SetVolumeRhythmTotal(int db)
{
	db = std::min(db, 20);
	SetReg(0x11, -(db * 2 / 3));
}

void OPNA::SetVolumeRhythm(int index, int db)
{
    db = std::min(db, 20);
    SetReg(0x18 + static_cast<uint32_t>(index), -(db * 2 / 3));
}

void OPNA::SetStatus(uint bits)
{
	if (!(status & bits))
		status |= bits & stmask;
}

void OPNA::ResetStatus(uint bits)
{
	status &= ~bits;
}

void OPNA::UpdateStream(Sample** outputs, int nSamples)
{
	Sample* bufl = outputs[STEREO_LEFT];
	Sample* bufr = outputs[STEREO_RIGHT];

	for (int i = 0; i < nSamples; ++i) {
		Sample lr[2];
		OPN2_Generate(state.chip, lr);
		*bufl++ = lr[0];
		*bufr++ = lr[1];
	}
}

void OPNA::UpdateSsgStream(Sample** outputs, int nSamples)
{
	if (state.ssg) {
		Sample* bufl = outputs[STEREO_LEFT];
		Sample* bufr = outputs[STEREO_RIGHT];
		for (int i = 0; i < nSamples; ++i) {
			int16_t s = PSG_calc(state.ssg) << 1;
			*bufl++ = s;
			*bufr++ = s;
		}
	}
	else {
		memset(outputs[STEREO_LEFT], 0, nSamples);
		memset(outputs[STEREO_RIGHT], 0, nSamples);
	}
}

}