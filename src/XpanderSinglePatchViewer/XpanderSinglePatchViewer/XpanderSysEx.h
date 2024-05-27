//This file is part of XpanderSinglePatchViewer

//XpanderSinglePatchViewer is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//XpanderSinglePatchViewer is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with XpanderSinglePatchViewer.
//If not, see <http://www.gnu.org/licenses/>.

//============================================================================
// This C/C++ source code compiles with Microsoft Visual Studio 2022
// with WIN32 target and *no* "UNICODE" or "MBCS" character set in the project's
// settings.
// Some types used here can be Microsoft specific
// see http://msdn.microsoft.com/en-us/library/cc953fe1.aspx

//============================================================================
// Latest version of this source code can be found here:
// https://github.com/xplorer2716/OberheimXpanderMidiSpec

// CURRENT VERSION IS: 1.2
// 1.2
// - reviewed amount/sign/quantize modulation entries decoding
//
// 1.1
// - modulation destinations names were shown as sources
// - wrong modulation amount when quantized modulations were defined.

// 1.0
// - first release
//============================================================================

#ifndef _XPANDERSYSEX__
#define _XPANDERSYSEX__

// utility struct to display names of enum values
typedef struct  _NumberStringPair {
	unsigned char iNumber;
	const char* pszString;
} NumberStringPair;

// some constants

// Single patch data sysex length is 399 bytes
// 6 bytes for sysex intro: F0 10 02....
static const int PRG_DUMP_DATA_FOLLOWS_INTRO_LENGTH = 6;
// 188 double bytes data for the values and 8 double bytes for the patch name
static const int PATCHNAME_LENGTH = 8;
static const int OBWORDS_DATA_LENGTH = 196 - PATCHNAME_LENGTH;
// 6 (intro) +196*2 (data+name) +1 (EOX) = 399

// 27 modulation sources
static const int  MODULATION_SOURCE_COUNT = 27;
// 47 modulation destinations
static const int MODULATION_DEST_COUNT = 47;
// only 20 modulations entries per patch
static const int MODULATION_MAX_ENTRIES = 20;

// modulations bit masks
// 6 bits unsigned amount
static const int MODULATION_VALUE_MASK = 0x3F;
// 7th bit is sign
static const int MODULATION_SIGN_MASK = 0x40;
//8th bit is quantize
static const int MODULATION_QTZ_MASK = 0x80;

//============================================================================
// SINGLE PATCH DATA TYPES
//============================================================================

// ModulationFlags
typedef enum _ModulationFlags { //bitfield
	MODFLAG_KEYBD = 0x01,
	MODFLAG_LAG = 0x02,
	MODFLAG_LEV_1 = 0x04,
	MODFLAG_VIB = 0x08
} ModulationFlags;
static const NumberStringPair ModulationFlagsNames[] = {
		{ MODFLAG_KEYBD, "KEYBD" },
		{ MODFLAG_LAG, "LAG" },
		{ MODFLAG_LEV_1, "LEV_1" },
		{ MODFLAG_VIB, "VIB" }
};
static const int MODULATIONFLAGS_COUNT = 4;
//-----------------------------------------------
// VCOWaveFlags
typedef enum _VCOWaveFlags { // bitfield
	VCOWAVEFLAG_TRI = 0x01,
	VCOWAVEFLAG_SAW = 0x02,
	VCOWAVEFLAG_PULSE = 0x04,
	VCOWAVEFLAG_SYNC = 0x08,
	VCOWAVEFLAG_NOISE = 0x10
}VCOWaveFlags;
static const NumberStringPair VCOWavesFlagsNames[] = {
		{ VCOWAVEFLAG_TRI, "TRI" },
		{ VCOWAVEFLAG_SAW, "SAW" },
		{ VCOWAVEFLAG_PULSE, "PULSE" },
		{ VCOWAVEFLAG_SYNC, "SYNC" },
		{ VCOWAVEFLAG_NOISE, "NOISE" }
};
static const int VCOWAVEFLAGS_COUNT = 5;
//-----------------------------------------------
// VCFFilterTypes
typedef enum _VCFFilterTypes {
	VCFFILTER_LOW_1, VCFFILTER_LOW_2, VCFFILTER_LOW_3, VCFFILTER_LOW_4,
	VCFFILTER_HIGH_1, VCFFILTER_HIGH_2, VCFFILTER_HIGH_3,
	VCFFILTER_BAND_2, VCFFILTER_BAND_4,
	VCFFILTER_NOTCH_2,
	VCFFILTER_PHASE_3,
	VCFFILTER_HIGH_2L, VCFFILTER_HIGH_3L,
	VCFFILTER_NOTCH_2L,
	VCFFILTER_PHASE_3L
} VCFFilterTypes;
static const char* VCFFilterTypesNames[] = {
	"LOW_1", "LOW_2", "LOW_3", "LOW_4",
	"HIGH_1", "HIGH_2", "HIGH_3",
	"BAND_2", "BAND_4",
	"NOTCH_2",
	"PHASE_3",
	"HIGH_2L", "HIGH_3L",
	"NOTCH_2L",
	"PHASE_3L"
};
static const int VCFFILTERTYPES_COUNT = 15;
//-----------------------------------------------
// FMDestinationTypes
typedef enum _FMDestinationTypes {
	FMDEST_VCO,
	FMDESTFM_VCF
} FMDestinationTypes;
static const char* FMDestinationTypesNames[] = {
	"VCO1_FREQ",
	"VCF_FREQ"
};
static const int FMDESTINATIONTYPES_COUNT = 2;
//-----------------------------------------------
// ModulationSourcesFlags
typedef enum _ModulationSourcesFlags {
	MODSRC_KBD, MODSRC_LAG, MODSRC_VEL, MODSRC_RVEL, MODSRC_PRES,
	MODSRC_TRK1, MODSRC_TRK2, MODSRC_TRK3,
	MODSRC_RMP1, MODSRC_RMP2, MODSRC_RMP3, MODSRC_RMP4,
	MODSRC_ENV1, MODSRC_ENV2, MODSRC_ENV3, MODSRC_ENV4, MODSRC_ENV5,
	MODSRC_PED1, MODSRC_PED2,
	MODSRC_LFO1, MODSRC_LFO2, MODSRC_LFO3, MODSRC_LFO4, MODSRC_LFO5,
	MODSRC_VIB, MODSRC_LEV1, MODSRC_LEV2
} ModulationSourcesFlags;
static const char* ModulationSourcesFlagsNames[] = {
	"KBD", "LAG", "VEL", "RVEL", "PRES",
	"TRK1", "TRK2", "TRK3",
	"RMP1", "RMP2", "RMP3", "RMP4",
	"ENV1", "ENV2", "ENV3", "ENV4", "ENV5",
	"PED1", "PED2",
	"LFO1", "LFO2", "LFO3", "LFO4", "LFO5",
	"VIB", "LEV1", "LEV2"
};
static const int MODULATIONSOURCESFLAGS_COUNT = 27;
//-----------------------------------------------
// LagModeFlags
typedef enum _LagModeFlags { //bitfield
	LAGMODE_LEGATO = 0x01,
	LAGMODE_EXPO = 0x02,
	LAGMODE_EQUAL_TIME = 0x04
} LagModeFlags;
static const NumberStringPair LagModeFlagsNames[] = {
		{ LAGMODE_LEGATO, "LEGATO" },
		{ LAGMODE_EXPO, "EXPO" },
		{ LAGMODE_EQUAL_TIME, "EQUAL_TIME" },
};
static const int LAGMODEFLAGS_COUNT = 3;
//-----------------------------------------------
// TriggerTypes
typedef enum _TriggerTypes {
	TRIGGER_OFF,
	TRIGGER_SINGLE,
	TRIGGER_MULTI,
	TRIGGER_EXTRIG
} TriggerTypes;
static const char* TriggerTypesNames[] = {
	"OFF", "SINGLE", "MULTI", "EXTRIG"
};
static const int TRIGGERTYPES_COUNT = 4;
//-----------------------------------------------
// WaveTypes
typedef enum _WaveTypes {
	WAVET_TRIANGLE, WAVET_UP_SAW, WAVET_DOWN_SAW, WAVET_SQUARE,
	WAVET_RANDOM, WAVET_NOISE, WAVET_SAMPLE
} WaveTypes;
static const char* WaveTypesNames[] = {
	"TRIANGLE", "UP_SAW", "DOWN_SAW", "SQUARE", "RANDOM", "NOISE", "SAMPLE"
};
static const int WAVETYPES_COUNT = 7;
//-----------------------------------------------
// LagFlags
typedef enum _LagFlags { //bitfield
	LAGF_LAG = 0x01
} LagFlags;
static const NumberStringPair LagFlagsNames[] = {
		{ LAGF_LAG, "LAG" }
};
static const int LAGFLAGS_COUNT = 1;
//-----------------------------------------------
// EnveloppeModeFlags
typedef enum _EnveloppeModeFlags { //bitfield
	ENVMODE_RESET = 0x01,
	ENVMODE_INVALID_VALUE = 0x02,	// original MIDI SPEC did not mention this unused bit
	ENVMODE_MULTI = 0x04,			// SINGLE if not MULTI
	ENVMODE_GATED = 0x08,
	ENVMODE_EXTRIG = 0x10,
	ENVMODE_LFOTRIG = 0x20,
	ENVMODE_DADR = 0x40,
	ENVMODE_FREERUN = 0x80
} EnveloppeModeFlags;
static const NumberStringPair EnveloppeModeFlagsNames[] = {
		{ ENVMODE_RESET, "RESET" },
		{ ENVMODE_INVALID_VALUE, "INVALID VALUE !" },
		{ ENVMODE_MULTI, "MULTI" },
		{ ENVMODE_GATED, "GATED" },
		{ ENVMODE_EXTRIG, "EXTRIG" },
		{ ENVMODE_LFOTRIG, "LFOTRIG" },
		{ ENVMODE_DADR, "DADR" },
		{ ENVMODE_FREERUN, "FREERUN" }
};
static const int ENVELOPPEMODEFLAGS_COUNT = 8;
//-----------------------------------------------
// LFOTriggerCodes
typedef enum _LFOTriggerCodes {
	LFOTRIG_LFO1,
	LFOTRIG_LFO2,
	LFOTRIG_LFO3,
	LFOTRIG_LFO4,
	LFOTRIG_LFO5,
	LFOTRIG_VIB
} LFOTriggerCodes;
static const char* LFOTriggerCodesNames[] = {
	"LFO1", "LFO2", "LFO3", "LFO4", "LFO5", "VIB"
};
static const int LFOTRIGGERCODES_COUNT = 6;
//-----------------------------------------------
// RampFlags
typedef enum _RampFlags { //bitfield
	RAMPF_GATED = 0x01,
	RAMPF_LFOTRIG = 0x02,
	RAMPF_EXTRIG = 0x04,
	RAMPF_MULTI = 0x08		//SINGLE if not MULTI
} RampFlags;
static const NumberStringPair RampFlagsNames[] = {
		{ RAMPF_GATED, "GATED" },
		{ RAMPF_LFOTRIG, "LFOTRIG" },
		{ RAMPF_EXTRIG, "EXTRIG" },
		{ RAMPF_MULTI, "MULTI" }
};
static const int RAMPFLAGS_COUNT = 4;
//-----------------------------------------------
// ModulationDestinationTypes
typedef enum _ModulationDestinationTypes {
	VCO1_FRQ, VCO1_PW, VCO1_VOL,
	VCO2_FRQ, VCO2_PW, VCO2_VOL,
	VCF_FRQ, VCF_RES, VCA1_VOL, VCA2_VOL,
	LFO1_SPD, LFO1_AMP,
	LFO2_SPD, LFO2_AMP,
	LFO3_SPD, LFO3_AMP,
	LFO4_SPD, LFO4_AMP,
	LFO5_SPD, LFO5_AMP,
	ENV1_DLY, ENV1_ATK, ENV1_DCY, ENV1_REL, ENV1_AMP,
	ENV2_DLY, ENV2_ATK, ENV2_DCY, ENV2_REL, ENV2_AMP,
	ENV3_DLY, ENV3_ATK, ENV3_DCY, ENV3_REL, ENV3_AMP,
	ENV4_DLY, ENV4_ATK, ENV4_DCY, ENV4_REL, ENV4_AMP,
	ENV5_DLY, ENV5_ATK, ENV5_DCY, ENV5_REL, ENV5_AMP,
	FM_AMP, LAG_SPD
} ModulationDestinationTypes;
static const char* ModulationDestinationsTypesNames[] = {
	"VCO1_FRQ", "VCO1_PW", "VCO1_VOL",
	"VCO2_FRQ", "VCO2_PW", " VCO2_VOL",
	"VCF_FRQ", "VCF_RES", "VCA1_VOL", "VCA2_VOL", //10
	"LFO1_SPD", "LFO1_AMP",
	"LFO2_SPD", "LFO2_AMP",
	"LFO3_SPD", "LFO3_AMP",
	"LFO4_SPD", "LFO4_AMP",
	"LFO5_SPD", "LFO5_AMP", //20
	"ENV1_DLY", "ENV1_ATK", "ENV1_DCY", "ENV1_REL", "ENV1_AMP",
	"ENV2_DLY", "ENV2_ATK", "ENV2_DCY", "ENV2_REL", "ENV2_AMP", //30
	"ENV3_DLY", "ENV3_ATK", "ENV3_DCY", "ENV3_REL", "ENV3_AMP",
	"ENV4_DLY", "ENV4_ATK", "ENV4_DCY", "ENV4_REL", "ENV4_AMP", //40
	"ENV5_DLY", "ENV5_ATK", "ENV5_DCY", "ENV5_REL", "ENV5_AMP", //45
	"FM_AMP", "LAG_SPD" //47
};
static const int MODULATIONDESTINATIONTYPES_COUNT = 47;

//----------------------------------------------------------------------------
// Single patch STRUCT
struct SinglePatch
{
	//VCO
	struct vco
	{
		unsigned char freq;/* Frequency */
		char detune;		/* Detune*/
		unsigned char pw;	/* Pulse Width*/
		unsigned char vol;	/* Volume*/
		unsigned char mod;	/* Standard Modulation flags */
		unsigned char wave;/* VCO Wave Shape flags */
	} vco[2];

	// VCF
	struct vcf
	{
		unsigned char freq;	/* Frequency */
		unsigned char res;		/* Resonance */
		unsigned char fmode;	/* Filter Mode Code */
		unsigned char vca1;	 /* VCA 1 level*/
		unsigned char vca2;	 /* VCA 2 level*/
		unsigned char mod;		/* Standard Modulation flags */
	} vcf;

	//FM LAG
	struct fm_lag
	{
		unsigned char fm_amp;	 /* FM Amplitude */
		unsigned char fm_dest;	 /* FM destination code*/
		unsigned char lag_in;	 /* Lag Input code */
		unsigned char lag_rate;	 /* Lag Rate */
		unsigned char lag_mode; /* Lag Mode */
	} fm_lag;

	//LFO (x5)
	struct lfo
	{
		unsigned char speed;		/* LFO Speed*/
		unsigned char retrig_mode;	/* Retrigger mode */
		unsigned char lag;			/* Lag flag*/
		unsigned char wave;		/* Wave Shape code */
		unsigned char retrig;		/* Retrigger Point */
		unsigned char sample;		/* LFO Sample Source */
		unsigned char amp;			/* LFO Amplitude */
	} lfo[5];

	//ENV (x5)
	struct env
	{
		unsigned char flags;	/* Miscellaneous flags */
		unsigned char lfotrig;	/* Triggering LFO code */
		unsigned char delay;	/* Delay Time*/
		unsigned char attack;	/* Attack Time*/
		unsigned char decay;	/* Decay Time*/
		unsigned char sustain;	/* Sustain Level*/
		unsigned char release;	/* Release Time */
		unsigned char amp;		/* Envelope Amplitude */
	} env[5];

	// TRACK (x3)
	struct track
	{
		unsigned char input;	/* Tracking Generator Source */
		unsigned char point[5];	/* Mapped vals for Input Pts*/
	} track[3];

	// RAMP (x4)
	struct ramp
	{
		unsigned char rate; /* Ramp Time*/
		unsigned char flags;/* Miscellaneous flags */
		unsigned char lfotrig;/* LFO Trigger Code*/
	} ramp[4];

	//MOD MATRIX (x20)
	struct mod
	{
		unsigned char source;/* Modulation Source code*/
		unsigned char amountSignAndQuantize;
		unsigned char dest;	/* Modulation Destination*/
	} mod[20];

	//NAME
	struct name
	{
		wchar_t character[8 + 1]; /*Name*/
	} name;
};

//============================================================================
// The source code hereafter is "as is" from the original MIDI spec
// It is NOT TESTED (maybe it will even not compile if uncommented)
//============================================================================

///*--------------------------------------------------------------------------*/
///*MULTI PATCH - XPANDER*/
///*--------------------------------------------------------------------------*/
//struct multiXpPatch {
//	struct bankXp {
//		transT trans[6] ; /* Transpose */
//		u63 volume[6]; /* Voice Volume */
//		panT pan[6] ; /* Pan Position */
//		s31 detune[6] ; /* Detune Amount*/
//	} bankXp ;
//	struct vib {
//		u63 speed ; /* Vibrato Speed*/
//		lagT lag ; /* Vibrato Lag Enable */
//		waveT wave ; /* Vibrato Wave Shape */
//		u63 amp ; /* Vibrato Amplitude */
//		vmodT speedModSource;/* Speed Modulation Source*/
//		vmodT ampModSource ; /* Amp Modulation Source */
//		s63 speedModAmt ; /* Speed Mod Amount */
//		s63 ampModAmt ; /* Amp Mod Amount*/
//	} vib ;
//
//	cmzT cvmidi[6] ; /* Voice Assign */
//	chanT zoneInput[3] ; /* Zone Inputs */
//	struct ZoneLimits {
//		u127 lowerLimit ; /* Lower Note Limit*/
//		u127 upperLimit ; /* Upper Note Limit*/
//	} ZoneLimits[3] ;
//	nassT mode[3] ; /* Note Assign Modes */
//} ;
//
///*--------------------------------------------------------------------------*/
///*MULTI PATCH - M12*/
///*--------------------------------------------------------------------------*/
//
//struct multiM12Patch {
//	struct bankM12 {
//		transT trans[6] ; /* Transpose */
//		u63 volume[6]; /* Voice Volume */
//		panT pan[6] ; /* Pan Position */
//		s31 detune[6] ; /* Detune Amount*/
//		vassT vassign[6] ; /* Voice Assignment */
//	} bankM12[2] ;
//	struct vib vib ;
//	struct zone
//	{
//		chanT channel ; /* MIDI I/O channel*/
//		u127 lowerLimit ; /* Lower Note Limit */
//		u127 upperLimit ; /* Upper Note Limit */
//		nassT mode ; /* Note Assignment Mode */
//		zonefT flags ; /* Zone Enables */
//	} zone[6] ;
//
//	struct name
//	{
//		char character[8] ; /* ASCII Name */
//	} name ;
//
//} ;
//
//typedef enum {
//	LEFT, LF2, LF1, MID, RT1, RT2, RIGHT, OFF
//} panT ;
//
//typedef enum {
//	ZONE1, ZONE2, ZONE3, ZONE4, ZONE5, ZONE6,
//	CHAN1, CHAN2, CHAN3, CHAN4, CHAN6, CHAN6, CHAN7, CHAN8,
//	CHAN9, CHAN10, CHAN11, CHAN12, CHAN13, CHAN14, CHAN15,
//	CHAN16
//} vassT ;
//
//typedef enum {OFF, LEV2, PED2} vmodT ;
//
//typedef enum {
//	CHAN1, CHAN2, CHAN3, CHAN4, CHAN5, CHAN6, CHAN7, CHAN8,
//	CHAN9, CHAN10, CHAN11, CHAN12, CHAN13, CHAN14, CHAN15,
//	CHAN16, OMNI
//} chanT ;
//
//typedef enum {
//	ROTATE, REASSIGN, RESET, UNI_LOW, UNI_HIGH, UNI_LAST
//} nassT ;
//
//
//typedef struct {
//	:3 ;
//	bool CONTROLLERS :1 ;
//	bool KEYBOARD :1 ;
//	bool VOICE_ROB :1 ;
//	bool MIDI_OUT :1 ;
//	bool MIDI_IN :1 ;
//} zonefT ;

#endif // _XPANDERSYSEX__