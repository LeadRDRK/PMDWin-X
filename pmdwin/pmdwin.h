//=============================================================================
//	Professional Music Driver [P.M.D.] version 4.8
//			Programmed By M.Kajihara
//			Windows Converted by C60
//=============================================================================

#ifndef PMDWINCORE_H
#define PMDWINCORE_H

#include "common/common.h"
#include "opnaw.h"
#include "ppz8l.h"
#include "ppsdrv.h"
#include "p86drv.h"
#include "ipmdwin.h"
#include "fmfileio/ifileio.h"

//#include "types.h"


//=============================================================================
//	バージョン情報
//=============================================================================
#define	DLLVersion			 40		// 上１桁：major, 下２桁：minor version


typedef int Sample;

#pragma pack( push, enter_include1 )
#pragma pack(2)

typedef struct pcmendstag
{
	uint16_t pcmends;
	uint16_t pcmadrs[256][2];
} PCMEnds;

#pragma pack( pop, enter_include1 )


#define	PVIHeader	"PVI2"
#define	PPCHeader	"ADPCM DATA for  PMD ver.4.4-  "
//#define ver 		"4.8o"
//#define vers		0x48
//#define verc		"o"
//#define date		"Jun.19th 1997"

//#define max_part1	22		// ０クリアすべきパート数(for PMDPPZ)
#define max_part2	11		// 初期化すべきパート数  (for PMDPPZ)

#define mdata_def	64
#define voice_def	 8
#define effect_def	64

#define fmvd_init	0		// ９８は８８よりもＦＭ音源を小さく


//=============================================================================
//	PMDWin class
//=============================================================================

#if _WIN32
	#define DLLEXPORT __declspec(dllexport)
#else
	#define DLLEXPORT
#endif

class PMDWIN : public IPMDWIN, ISETFILEIO
{
public:
	DLLEXPORT PMDWIN();
	DLLEXPORT virtual ~PMDWIN();
	
	// IUnknown
	HRESULT WINAPI QueryInterface(
			/* [in] */ REFIID riid,
			/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
	ULONG WINAPI AddRef(void);
	ULONG WINAPI Release(void);
	
	// IPCMMUSICDRIVER
	DLLEXPORT bool WINAPI init(const TCHAR *path);
	DLLEXPORT int WINAPI music_load(const TCHAR *filename);
	DLLEXPORT int WINAPI music_load2(const uint8_t *musdata, int size);
	DLLEXPORT TCHAR* WINAPI getmusicfilename(TCHAR *dest);
	DLLEXPORT void WINAPI music_start(void);
	DLLEXPORT void WINAPI music_stop(void);
	DLLEXPORT int WINAPI getloopcount(void);
	DLLEXPORT bool WINAPI getlength(const TCHAR *filename, int *length, int *loop);
	DLLEXPORT int WINAPI getpos(void);
	DLLEXPORT void WINAPI setpos(int pos);
	DLLEXPORT void WINAPI getpcmdata(int16_t *buf, int nsamples);
	
	// IFMPMD
	DLLEXPORT bool WINAPI loadrhythmsample(const TCHAR *path);
	DLLEXPORT bool WINAPI setpcmdir(const TCHAR **path);
	DLLEXPORT void WINAPI setpcmrate(int rate);
	DLLEXPORT void WINAPI setppzrate(int rate);
	DLLEXPORT void WINAPI setfmcalc55k(bool flag);
	DLLEXPORT void WINAPI setppzinterpolation(bool ip);
	DLLEXPORT void WINAPI setfmwait(int nsec);
	DLLEXPORT void WINAPI setssgwait(int nsec);
	DLLEXPORT void WINAPI setrhythmwait(int nsec);
	DLLEXPORT void WINAPI setadpcmwait(int nsec);
	DLLEXPORT void WINAPI fadeout(int speed);
	DLLEXPORT void WINAPI fadeout2(int speed);
	DLLEXPORT void WINAPI setpos2(int pos);
	DLLEXPORT int WINAPI getpos2(void);
	DLLEXPORT TCHAR* WINAPI getpcmfilename(TCHAR *dest);
	DLLEXPORT TCHAR* WINAPI getppzfilename(TCHAR *dest, int bufnum);
	DLLEXPORT bool WINAPI getlength2(const TCHAR *filename, int *length, int *loop);
	
	// IPMDWIN
	DLLEXPORT void WINAPI setppsuse(bool value);
	DLLEXPORT void WINAPI setrhythmwithssgeffect(bool value);
	DLLEXPORT void WINAPI setpmd86pcmmode(bool value);
	DLLEXPORT bool WINAPI getpmd86pcmmode(void);
	DLLEXPORT void WINAPI setppsinterpolation(bool ip);
	DLLEXPORT void WINAPI setp86interpolation(bool ip);
	DLLEXPORT int WINAPI maskon(int ch);
	DLLEXPORT int WINAPI maskoff(int ch);
	DLLEXPORT void WINAPI setfmvoldown(int voldown);
	DLLEXPORT void WINAPI setssgvoldown(int voldown);
	DLLEXPORT void WINAPI setrhythmvoldown(int voldown);
	DLLEXPORT void WINAPI setadpcmvoldown(int voldown);
	DLLEXPORT void WINAPI setppzvoldown(int voldown);
	DLLEXPORT int WINAPI getfmvoldown(void);
	DLLEXPORT int WINAPI getfmvoldown2(void);
	DLLEXPORT int WINAPI getssgvoldown(void);
	DLLEXPORT int WINAPI getssgvoldown2(void);
	DLLEXPORT int WINAPI getrhythmvoldown(void);
	DLLEXPORT int WINAPI getrhythmvoldown2(void);
	DLLEXPORT int WINAPI getadpcmvoldown(void);
	DLLEXPORT int WINAPI getadpcmvoldown2(void);
	DLLEXPORT int WINAPI getppzvoldown(void);
	DLLEXPORT int WINAPI getppzvoldown2(void);
	DLLEXPORT char* WINAPI getmemo(char *dest, const uint8_t *musdata, int size, int al);
	DLLEXPORT char* WINAPI getmemo2(char *dest, const uint8_t *musdata, int size, int al);
	DLLEXPORT char* WINAPI getmemo3(char *dest, const uint8_t *musdata, int size, int al);
	DLLEXPORT int	WINAPI fgetmemo(char *dest, const TCHAR *filename, int al);
	DLLEXPORT int	WINAPI fgetmemo2(char *dest, const TCHAR *filename, int al);
	DLLEXPORT int	WINAPI fgetmemo3(char *dest, const TCHAR *filename, int al);
	DLLEXPORT TCHAR* WINAPI getppcfilename(TCHAR *dest);
	DLLEXPORT TCHAR* WINAPI getppsfilename(TCHAR *dest);
	DLLEXPORT TCHAR* WINAPI getp86filename(TCHAR *dest);
	DLLEXPORT int WINAPI ppc_load(const TCHAR *filename);
	DLLEXPORT int WINAPI pps_load(const TCHAR *filename);
	DLLEXPORT int WINAPI p86_load(const TCHAR *filename);
	DLLEXPORT int WINAPI ppz_load(const TCHAR *filename, int bufnum);
	DLLEXPORT OPEN_WORK* WINAPI getopenwork(void);
	DLLEXPORT QQ* WINAPI getpartwork(int ch);
	
	// IFILESTREAM
	DLLEXPORT void WINAPI setfileio(IFILEIO* pfileio);
	
private:
	FM::OPNAW*	opna;
	PPZ8*		ppz8;
	PPSDRV*		ppsdrv;
	P86DRV*		p86drv;
	FilePath	filepath;
	FileIO*		fileio;
	IFILEIO*	pfileio;
	
	OPEN_WORK open_work;
	QQ FMPart[NumOfFMPart], SSGPart[NumOfSSGPart], ADPCMPart, RhythmPart;
	QQ ExtPart[NumOfExtPart], DummyPart, EffPart, PPZ8Part[NumOfPPZ8Part];
	
	PMDWORK pmdwork;
	EFFWORK effwork;
	
	Stereo16bit		wavbuf2[nbufsample];
	StereoSample	wavbuf[nbufsample];
	StereoSample	wavbuf_conv[nbufsample];
	
	char	*pos2;						// buf に余っているサンプルの先頭位置
	int		us2;						// buf に余っているサンプル数
	int64_t	upos;						// 演奏開始からの時間(μs)
	int64_t	fpos;						// fadeout2 開始時間
	int		seed;						// 乱数の種
	
	uint8_t mdataarea[mdata_def*1024];
	uint8_t vdataarea[voice_def*1024];		//@不要？
	uint8_t edataarea[effect_def*1024];		//@不要？
	PCMEnds pcmends;
	
protected:
	int uRefCount;		// 参照カウンタ
	void _init(void);
	void opnint_start(void);
	void data_init(void);
	void opn_init(void);
	void mstop(void);
	void mstop_f(void);
	void silence(void);
	void mstart(void);
	void mstart_f(void);
	void play_init(void);
	void setint(void);
	void calc_tb_tempo(void);
	void calc_tempo_tb(void);
	void settempo_b(void);
	void FM_Timer_main(void);
	void TimerA_main(void);
	void TimerB_main(void);
	void mmain(void);
	void syousetu_count(void);
	void fmmain(QQ *qq);
	void psgmain(QQ *qq);
	void rhythmmain(QQ *qq);
	void adpcmmain(QQ *qq);
	void pcm86main(QQ *qq);
	void ppz8main(QQ *qq);
	uint8_t *rhythmon(QQ *qq, uint8_t *bx, int al, int *result);
	void effgo(QQ *qq, int al);
	void eff_on2(QQ *qq, int al);
	void eff_main(QQ *qq, int al);
	void effplay(void);
	void efffor(const int *si);
	void effend(void);
	void effsweep(void);
	uint8_t *pdrswitch(QQ *qq, uint8_t *si);
	char* WINAPI _getmemo(char *dest, const uint8_t *musdata, int size, int al);
	char* WINAPI _getmemo2(char *dest, const uint8_t *musdata, int size, int al);
	char* WINAPI _getmemo3(char *dest, const uint8_t *musdata, int size, int al);
	int	WINAPI _fgetmemo(char *dest, const TCHAR *filename, int al);
	int	WINAPI _fgetmemo2(char *dest, const TCHAR *filename, int al);
	int	WINAPI _fgetmemo3(char *dest, const TCHAR *filename, int al);
	
	int silence_fmpart(QQ *qq);
	void keyoff(QQ *qq);
	void kof1(QQ *qq);
	void keyoffp(QQ *qq);
	void keyoffm(QQ *qq);
	void keyoff8(QQ *qq);
	void keyoffz(QQ *qq);
	int ssgdrum_check(QQ *qq, int al);
	uint8_t *commands(QQ *qq, uint8_t *si);
	uint8_t *commandsp(QQ *qq, uint8_t *si);
	uint8_t *commandsr(QQ *qq, uint8_t *si);
	uint8_t *commandsm(QQ *qq, uint8_t *si);
	uint8_t *commands8(QQ *qq, uint8_t *si);
	uint8_t *commandsz(QQ *qq, uint8_t *si);
	uint8_t *special_0c0h(QQ *qq, uint8_t *si, uint8_t al);
	uint8_t *_vd_fm(QQ *qq, uint8_t *si);
	uint8_t *_vd_ssg(QQ *qq, uint8_t *si);
	uint8_t *_vd_pcm(QQ *qq, uint8_t *si);
	uint8_t *_vd_rhythm(QQ *qq, uint8_t *si);
	uint8_t *_vd_ppz(QQ *qq, uint8_t *si);
	uint8_t *comt(uint8_t *si);
	uint8_t *comat(QQ *qq, uint8_t *si);
	uint8_t *comatm(QQ *qq, uint8_t *si);
	uint8_t *comat8(QQ *qq, uint8_t *si);
	uint8_t *comatz(QQ *qq, uint8_t *si);
	uint8_t *comstloop(QQ *qq, uint8_t *si);
	uint8_t *comedloop(QQ *qq, uint8_t *si);
	uint8_t *comexloop(QQ *qq, uint8_t *si);
	uint8_t *extend_psgenvset(QQ *qq, uint8_t *si);
	int lfoinit(QQ *qq, int al);
	int lfoinitp(QQ *qq, int al);
	uint8_t *lfoset(QQ *qq, uint8_t *si);
	uint8_t *psgenvset(QQ *qq, uint8_t *si);
	uint8_t *rhykey(uint8_t *si);
	uint8_t *rhyvs(uint8_t *si);
	uint8_t *rpnset(uint8_t *si);
	uint8_t *rmsvs(uint8_t *si);
	uint8_t *rmsvs_sft(uint8_t *si);
	uint8_t *rhyvs_sft(uint8_t *si);
	
	uint8_t *vol_one_up_psg(QQ *qq, uint8_t *si);
	uint8_t *vol_one_up_pcm(QQ *qq, uint8_t *si);
	uint8_t *vol_one_down(QQ *qq, uint8_t *si);
	uint8_t *portap(QQ *qq, uint8_t *si);
	uint8_t *portam(QQ *qq, uint8_t *si);
	uint8_t *portaz(QQ *qq, uint8_t *si);
	uint8_t *psgnoise_move(uint8_t *si);
	uint8_t *mdepth_count(QQ *qq, uint8_t *si);
	uint8_t *toneadr_calc(QQ *qq, int dl);
	void neiroset(QQ *qq, int dl);
	
	int oshift(QQ *qq, int al);
	int oshiftp(QQ *qq, int al);
	void fnumset(QQ *qq, int al);
	void fnumsetp(QQ *qq, int al);
	void fnumsetm(QQ *qq, int al);
	void fnumset8(QQ *qq, int al);
	void fnumsetz(QQ *qq, int al);
	uint8_t *panset(QQ *qq, uint8_t *si);
	uint8_t *panset_ex(QQ *qq, uint8_t *si);
	uint8_t *pansetm(QQ *qq, uint8_t *si);
	uint8_t *panset8(QQ *qq, uint8_t *si);
	uint8_t *pansetz(QQ *qq, uint8_t *si);
	uint8_t *pansetz_ex(QQ *qq, uint8_t *si);
	void panset_main(QQ *qq, int al);
	uint8_t calc_panout(QQ *qq);
	uint8_t *calc_q(QQ *qq, uint8_t *si);
	void fm_block_calc(int *cx, int *ax);
	int ch3_setting(QQ *qq);
	void cm_clear(int *ah, int *al);
	void ch3mode_set(QQ *qq);
	void ch3_special(QQ *qq, int ax, int cx);
	void volset(QQ *qq);
	void volsetp(QQ *qq);
	void volsetm(QQ *qq);
	void volset8(QQ *qq);
	void volsetz(QQ *qq);
	void otodasi(QQ *qq);
	void otodasip(QQ *qq);
	void otodasim(QQ *qq);
	void otodasi8(QQ *qq);
	void otodasiz(QQ *qq);
	void keyon(QQ *qq);
	void keyonp(QQ *qq);
	void keyonm(QQ *qq);
	void keyon8(QQ *qq);
	void keyonz(QQ *qq);
	int lfo(QQ *qq);
	int lfop(QQ *qq);
	uint8_t *lfoswitch(QQ *qq, uint8_t *si);
	void lfoinit_main(QQ *qq);
	void lfo_change(QQ *qq);
	void lfo_exit(QQ *qq);
	void lfin1(QQ *qq);
	void lfo_main(QQ *qq);
	int rnd(int ax);
	void fmlfo_sub(QQ *qq, int al, int bl, uint8_t *vol_tbl);
	void volset_slot(int dh, int dl, int al);
	void porta_calc(QQ *qq);
	int soft_env(QQ *qq);
	int soft_env_main(QQ *qq);
	int soft_env_sub(QQ *qq);
	int ext_ssgenv_main(QQ *qq);
	void esm_sub(QQ *qq, int ah);
	void psgmsk(void);
	int get07(void);
	void md_inc(QQ *qq);
	uint8_t *pcmrepeat_set(QQ *qq, uint8_t * si);
	uint8_t *pcmrepeat_set8(QQ *qq, uint8_t * si);
	uint8_t *ppzrepeat_set(QQ *qq, uint8_t * si);
	uint8_t *pansetm_ex(QQ *qq, uint8_t * si);
	uint8_t *panset8_ex(QQ *qq, uint8_t * si);
	uint8_t *pcm_mml_part_mask(QQ *qq, uint8_t * si);
	uint8_t *pcm_mml_part_mask8(QQ *qq, uint8_t * si);
	uint8_t *ppz_mml_part_mask(QQ *qq, uint8_t * si);
	void pcmstore(uint16_t pcmstart, uint16_t pcmstop, uint8_t *buf);
	void pcmread(uint16_t pcmstart, uint16_t pcmstop, uint8_t *buf);
	
	uint8_t *hlfo_set(QQ *qq, uint8_t *si);
	uint8_t *vol_one_up_fm(QQ *qq, uint8_t *si);
	uint8_t *porta(QQ *qq, uint8_t *si);
	uint8_t *slotmask_set(QQ *qq, uint8_t *si);
	uint8_t *slotdetune_set(QQ *qq, uint8_t *si);
	uint8_t *slotdetune_set2(QQ *qq, uint8_t *si);
	void fm3_partinit(QQ *qq, uint8_t *ax);
	uint8_t *fm3_extpartset(QQ *qq, uint8_t *si);
	uint8_t *ppz_extpartset(QQ *qq, uint8_t *si);
	uint8_t *volmask_set(QQ *qq, uint8_t *si);
	uint8_t *fm_mml_part_mask(QQ *qq, uint8_t *si);
	uint8_t *ssg_mml_part_mask(QQ *qq, uint8_t *si);
	uint8_t *rhythm_mml_part_mask(QQ *qq, uint8_t *si);
	uint8_t *_lfoswitch(QQ *qq, uint8_t *si);
	uint8_t *_volmask_set(QQ *qq, uint8_t *si);
	uint8_t *tl_set(QQ *qq, uint8_t *si);
	uint8_t *fb_set(QQ *qq, uint8_t *si);
	uint8_t *fm_efct_set(QQ *qq, uint8_t *si);
	uint8_t *ssg_efct_set(QQ *qq, uint8_t *si);
	void fout(void);
	void neiro_reset(QQ *qq);
	int music_load3(const uint8_t *musdata, int size, const TCHAR *current_dir);
	int ppc_load2(const TCHAR *filename);
	int ppc_load3(const uint8_t *pcmdata, int size);
	TCHAR *search_pcm(TCHAR *dest, const TCHAR *filename, const TCHAR *current_dir);
	void swap(int *a, int *b);
};
#undef DLLEXPORT


#endif // PMDWINCORE_H
