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
//
// Latest version of this source code can be found here:
// https://github.com/xplorer2716/OberheimXpanderMidiSpec
//============================================================================
// CURRENT VERSION IS: 1.2
//
// 1.2
// - fix negative quantized moduluation values
//
// 1.1
// - modulation destinations names were shown as sources
// - wrong modulation amount when quantized modulations were defined.

// 1.0
// - first release
//============================================================================

// windows stuff
#include "stdafx.h"
#include <windows.h>

//stdlib
#include <assert.h>
#include <stdlib.h>

//Xpander header
#include "XpanderSysEx.h"

// utility
typedef enum _ReturnCodes {
	RETURN_ERROR = 1,
	RETURN_OK = 0
};
// UI stuff :)
static const char* SINGLE_LINE = "---------------------------\n";
static const char* DOUBLE_LINE = "===========================\n";

//----------------------------------------------------------------------------
/*! Locate a single patch sysex data from the specified file
@param [in] pFile: the opened file to get data from
@return true is single patch data found, else false.
@remark when data are found, current file position is set to the
beginning of the data.
*/
bool LocateSinglePatchData(FILE* pFile) {
	bool bSinglePatchDataFound = false;
	bool bEndOfFile = false;

	unsigned char intro[PRG_DUMP_DATA_FOLLOWS_INTRO_LENGTH];

	while (!bSinglePatchDataFound && !bEndOfFile) {
		memset(intro, 0, PRG_DUMP_DATA_FOLLOWS_INTRO_LENGTH);
		// try to identify Single Patch data: F0 10 02 01 00...
		int nbBytesRead = fread(intro, sizeof(char), PRG_DUMP_DATA_FOLLOWS_INTRO_LENGTH, pFile);
		if (nbBytesRead != PRG_DUMP_DATA_FOLLOWS_INTRO_LENGTH) { bEndOfFile = true; break; }
		if ((intro[0] == 0xF0) && (intro[1] == 0x10) && (intro[2] == 0x02) && (intro[3] == 0x01) && (intro[4] == 0x00)) {
			// bingo...
			fprintf(stdout, DOUBLE_LINE);
			fprintf(stdout, "Program type:\t %02Xh\nProgram number:\t %02Xh (%02d)\n", intro[4], intro[5], intro[5]);
			{bSinglePatchDataFound = true; break; }
		}
		else {
			// try one byte after beginning...
			fseek(pFile, -(PRG_DUMP_DATA_FOLLOWS_INTRO_LENGTH - 1), SEEK_CUR);
		}
	}

	return (!bEndOfFile);
}

//----------------------------------------------------------------------------
/*! Read the the single patch data from file to a SinglePatch struct
@remark: assumes that the file current position is at the beginning of the data
@param [in] pFile: file to read data from
@param [out] pPatch: the single patch data struct
*/
void ReadSinglePatchData(FILE* pFile, SinglePatch* pPatch) {
	//  data in sysex are double bytes values (short)
	unsigned short shortArray[OBWORDS_DATA_LENGTH];
	memset(shortArray, 0, OBWORDS_DATA_LENGTH);

	int iReadBytes = 0;
	iReadBytes = fread(shortArray, sizeof(short), OBWORDS_DATA_LENGTH, pFile);

	assert(iReadBytes == OBWORDS_DATA_LENGTH);

	// for each double byte, repack the value, and set the Patch property accordingly
	unsigned char* pByte = (unsigned char*)pPatch;
	for (int i = 0; i < OBWORDS_DATA_LENGTH; i++) {
		// 8th bit of the 8 bits value is the first bit of the high byte
		unsigned char cValue = ((shortArray[i] & 0x100) >> 1) | (shortArray[i] & 0x0ff);
		*pByte = cValue;
		pByte++;
	}

	// name is 2 bytes/char, high byte never used, thus wchar_t compatible
	iReadBytes = 0;
	iReadBytes = fread(&pPatch->name, sizeof(wchar_t), PATCHNAME_LENGTH, pFile);
	assert(iReadBytes == PATCHNAME_LENGTH);
}
//----------------------------------------------------------------------------
/*! Dump a SinglePatch struct to the stdout with human-readable informations
@param [in] pPatch: the patch to dump
*/
void DumpPatch(const SinglePatch* pPatch) {
	// NAME ------------------------------------
	fprintf(stdout, "NAME:\t%S\n", &pPatch->name);

	// VCO1  ------------------------------------
	fprintf(stdout, SINGLE_LINE);
	fprintf(stdout, "VCO1.freq:\t %02Xh\t %4d\n", pPatch->vco[0].freq, pPatch->vco[0].freq);
	fprintf(stdout, "VCO1.detune:\t %02Xh\t %4d\n", pPatch->vco[0].detune, pPatch->vco[0].detune);
	fprintf(stdout, "VCO1.pw:\t %02Xh\t %4d\n", pPatch->vco[0].pw, pPatch->vco[0].pw);
	fprintf(stdout, "VCO1.vol:\t %02Xh\t %4d\n", pPatch->vco[0].vol, pPatch->vco[0].vol);
	fprintf(stdout, "VCO1.mod:\t %02Xh\t %4d : ", pPatch->vco[0].mod, pPatch->vco[0].mod);
	// show bitfields
	for (int i = 0; i < ::MODULATIONFLAGS_COUNT; i++) {
		if ((ModulationFlagsNames[i].iNumber & pPatch->vco[0].mod) == ModulationFlagsNames[i].iNumber) {
			fprintf(stdout, "%s ", ModulationFlagsNames[i].pszString);
		}
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "VCO1.wave:\t %02Xh\t %4d : ", pPatch->vco[0].wave, pPatch->vco[0].wave);
	for (int i = 0; i < ::VCOWAVEFLAGS_COUNT; i++) {
		if ((VCOWavesFlagsNames[i].iNumber & pPatch->vco[0].wave) == VCOWavesFlagsNames[i].iNumber) {
			fprintf(stdout, "%s ", VCOWavesFlagsNames[i].pszString);
		}
	}
	fprintf(stdout, "\n");

	//VCO2 ------------------------------------
	fprintf(stdout, SINGLE_LINE);
	fprintf(stdout, "VCO2.freq:\t %02Xh\t %4d\n", pPatch->vco[1].freq, pPatch->vco[1].freq);
	fprintf(stdout, "VCO2.detune:\t %02Xh\t %4d\n", pPatch->vco[1].detune, pPatch->vco[1].detune);
	fprintf(stdout, "VCO2.pw:\t %02Xh\t %4d\n", pPatch->vco[1].pw, pPatch->vco[1].pw);
	fprintf(stdout, "VCO2.vol:\t %02Xh\t %4d\n", pPatch->vco[1].vol, pPatch->vco[1].vol);
	fprintf(stdout, "VCO2.mod:\t %02Xh\t %4d : ", pPatch->vco[1].mod, pPatch->vco[1].mod);
	for (int i = 0; i < 4; i++) {
		if ((ModulationFlagsNames[i].iNumber & pPatch->vco[1].mod) == ModulationFlagsNames[i].iNumber) {
			fprintf(stdout, "%s ", ModulationFlagsNames[i].pszString);
		}
	}
	fprintf(stdout, "\n");
	fprintf(stdout, "VCO2.wave:\t %02Xh\t %4d : ", pPatch->vco[1].wave, pPatch->vco[1].wave);
	for (int i = 0; i < ::VCOWAVEFLAGS_COUNT; i++) {
		if ((VCOWavesFlagsNames[i].iNumber & pPatch->vco[1].wave) == VCOWavesFlagsNames[i].iNumber) {
			fprintf(stdout, "%s ", VCOWavesFlagsNames[i].pszString);
		}
	}
	fprintf(stdout, "\n");

	//VCF ------------------------------------
	fprintf(stdout, SINGLE_LINE);
	fprintf(stdout, "VCF.freq:\t %02Xh\t %4d\n", pPatch->vcf.freq, pPatch->vcf.freq);
	fprintf(stdout, "VCF.res:\t %02Xh\t %4d\n", pPatch->vcf.res, pPatch->vcf.res);
	fprintf(stdout, "VCF.mode:\t %02Xh\t %4d : %s\n", pPatch->vcf.fmode, pPatch->vcf.fmode, VCFFilterTypesNames[pPatch->vcf.fmode]);
	fprintf(stdout, "VCF.vca1:\t %02Xh\t %4d\n", pPatch->vcf.vca1, pPatch->vcf.vca1);
	fprintf(stdout, "VCF.vca2:\t %02Xh\t %4d\n", pPatch->vcf.vca2, pPatch->vcf.vca2);
	fprintf(stdout, "VCF.mod:\t %02Xh\t %4d : ", pPatch->vcf.mod, pPatch->vcf.mod);
	for (int i = 0; i < ::MODULATIONFLAGS_COUNT; i++) {
		if ((ModulationFlagsNames[i].iNumber & pPatch->vcf.mod) == ModulationFlagsNames[i].iNumber) {
			fprintf(stdout, "%s ", ModulationFlagsNames[i].pszString);
		}
	}
	fprintf(stdout, "\n");

	//FM LAG ------------------------------------
	fprintf(stdout, SINGLE_LINE);

	fprintf(stdout, "FMLAG.amp\t %02Xh\t %4d\n", pPatch->fm_lag.fm_amp, pPatch->fm_lag.fm_amp);
	fprintf(stdout, "FMLAG.dest:\t %02Xh\t %4d : %s\n", pPatch->fm_lag.fm_dest, pPatch->fm_lag.fm_dest, FMDestinationTypesNames[pPatch->fm_lag.fm_dest]);
	fprintf(stdout, "FMLAG.lag_in:\t %02Xh\t %4d : %s\n", pPatch->fm_lag.lag_in, pPatch->fm_lag.lag_in, ModulationSourcesFlagsNames[pPatch->fm_lag.lag_in]);
	fprintf(stdout, "FMLAG.lag_rate:\t %02Xh\t %4d\n", pPatch->fm_lag.lag_rate, pPatch->fm_lag.lag_rate);
	fprintf(stdout, "FMLAG.lag_mode:\t %02Xh\t %4d : ", pPatch->fm_lag.lag_mode, pPatch->fm_lag.lag_mode);
	for (int i = 0; i < ::LAGMODEFLAGS_COUNT; i++) {
		if ((LagModeFlagsNames[i].iNumber & pPatch->fm_lag.lag_mode) == LagModeFlagsNames[i].iNumber) {
			fprintf(stdout, "%s ", LagModeFlagsNames[i].pszString);
		}
	}
	fprintf(stdout, "\n");

	//LFO (x5) ------------------------------------
	for (int i = 0; i < 5; i++) {
		fprintf(stdout, SINGLE_LINE);
		fprintf(stdout, "LFO[%01d].speed:\t %02Xh\t %4d\n", i + 1, pPatch->lfo[i].speed, pPatch->lfo[i].speed);
		fprintf(stdout, "LFO[%01d].trg_mod:\t %02Xh\t %4d : %s\n", i + 1, pPatch->lfo[i].retrig_mode, pPatch->lfo[i].retrig_mode, TriggerTypesNames[pPatch->lfo[i].retrig_mode]);
		fprintf(stdout, "LFO[%01d].lag:\t %02Xh\t %4d : ", i + 1, pPatch->lfo[i].lag, pPatch->lfo[i].lag);
		for (int j = 0; j < ::LAGFLAGS_COUNT; j++) {
			if ((LagFlagsNames[j].iNumber & pPatch->lfo[i].lag) == LagFlagsNames[j].iNumber) {
				fprintf(stdout, "%s ", LagFlagsNames[j].pszString);
			}
		}
		fprintf(stdout, "\n");
		fprintf(stdout, "LFO[%01d].wave:\t %02Xh\t %4d : %s\n", i + 1, pPatch->lfo[i].wave, pPatch->lfo[i].wave, WaveTypesNames[pPatch->lfo[i].wave]);
		fprintf(stdout, "LFO[%01d].retrig:\t %02Xh\t %4d\n", i + 1, pPatch->lfo[i].retrig, pPatch->lfo[i].retrig);
		fprintf(stdout, "LFO[%01d].sample\t %02Xh\t %4d : %s\n", i + 1, pPatch->lfo[i].sample, pPatch->lfo[i].sample, ModulationSourcesFlagsNames[pPatch->lfo[i].sample]);
		fprintf(stdout, "LFO[%01d].amp:\t %02Xh\t %4d\n", i + 1, pPatch->lfo[i].amp, pPatch->lfo[i].amp);
	}

	//ENV (x5) ------------------------------------
	for (int i = 0; i < 5; i++) {
		fprintf(stdout, SINGLE_LINE);
		fprintf(stdout, "ENV[%01d].flags:\t %02Xh\t %4d : ", i + 1, pPatch->env[i].flags, pPatch->env[i].flags);

		for (int j = 0; j < ::ENVELOPPEMODEFLAGS_COUNT; j++) {
			if ((EnveloppeModeFlagsNames[j].iNumber & pPatch->env[i].flags) == EnveloppeModeFlagsNames[j].iNumber) {
				fprintf(stdout, "%s ", EnveloppeModeFlagsNames[j].pszString);
			}
		}
		fprintf(stdout, "\n");

		fprintf(stdout, "ENV[%01d].lfo_trg:\t %02Xh\t %4d : %s\n", i + 1, pPatch->env[i].lfotrig, pPatch->env[i].lfotrig, LFOTriggerCodesNames[pPatch->env[i].lfotrig]);
		fprintf(stdout, "ENV[%01d].delay:\t %02Xh\t %4d\n", i + 1, pPatch->env[i].delay, pPatch->env[i].delay);
		fprintf(stdout, "ENV[%01d].attck:\t %02Xh\t %4d\n", i + 1, pPatch->env[i].attack, pPatch->env[i].attack);
		fprintf(stdout, "ENV[%01d].decay:\t %02Xh\t %4d\n", i + 1, pPatch->env[i].decay, pPatch->env[i].decay);
		fprintf(stdout, "ENV[%01d].sustain:\t %02Xh\t %4d\n", i + 1, pPatch->env[i].sustain, pPatch->env[i].sustain);
		fprintf(stdout, "ENV[%01d].rel:\t %02Xh\t %4d\n", i + 1, pPatch->env[i].release, pPatch->env[i].release);
		fprintf(stdout, "ENV[%01d].amp:\t %02Xh\t %4d\n", i + 1, pPatch->env[i].amp, pPatch->env[i].amp);
	}

	//TRACK (x3) ------------------------------------
	for (int i = 0; i < 3; i++) {
		fprintf(stdout, SINGLE_LINE);
		fprintf(stdout, "TRACK[%01d].input:\t %02Xh\t %4d : %s\n", i + 1, pPatch->track[i].input, pPatch->track[i].input, ModulationSourcesFlagsNames[pPatch->track[i].input]);
		// track points (x5)
		fprintf(stdout, "TRACK[%01d].points:\t    ", i + 1);
		for (int j = 0; j < 5; j++) {
			if (j < 4) {
				fprintf(stdout, "%d,", pPatch->track[i].point[j]);
			}
			else {
				fprintf(stdout, "%d", pPatch->track[i].point[j]);
			}
		}
		fprintf(stdout, "\n");
	}

	// RAMP (x4) ------------------------------------
	for (int i = 0; i < 4; i++) {
		fprintf(stdout, SINGLE_LINE);
		fprintf(stdout, "RAMP[%01d].rate:\t %02Xh\t %4d\n", i + 1, pPatch->ramp[i].rate, pPatch->ramp[i].rate);
		fprintf(stdout, "RAMP[%01d].flags:\t %02Xh\t %4d : ", i + 1, pPatch->ramp[i].flags, pPatch->ramp[i].flags);

		for (int j = 0; j < ::RAMPFLAGS_COUNT; j++) {
			if ((RampFlagsNames[j].iNumber & pPatch->ramp[i].flags) == RampFlagsNames[j].iNumber) {
				fprintf(stdout, "%s ", RampFlagsNames[j].pszString);
			}
		}
		fprintf(stdout, "\n");

		fprintf(stdout, "RAMP[%01d].lfotrg:\t %02Xh\t %4d : %s\n", i + 1, pPatch->ramp[i].lfotrig, pPatch->ramp[i].lfotrig, LFOTriggerCodesNames[pPatch->ramp[i].lfotrig]);
	}

	// MOD MATRIX (x20) ------------------------------------
	for (int i = 0; i < ::MODULATION_MAX_ENTRIES; i++) {
		fprintf(stdout, SINGLE_LINE);

		// seems that unused modulations entries are garbage
		char source = pPatch->mod[i].source;
		char dest = pPatch->mod[i].dest;
		if (source >= MODULATION_SOURCE_COUNT || dest >= MODULATION_DEST_COUNT) {
			fprintf(stdout, "MOD[%02d]: UNUSED ENTRY\n", i + 1);
		}

		else {
			// get the 6 bits unsigned value
			char amount = (char)pPatch->mod[i].amountSignAndQuantize & MODULATION_VALUE_MASK;
			// sign bit
			if ((pPatch->mod[i].amountSignAndQuantize & MODULATION_SIGN_MASK) == MODULATION_SIGN_MASK) {
				amount *= -1;
			}
			// quantize bit
			const char* pszQuantized;
			if ((pPatch->mod[i].amountSignAndQuantize & MODULATION_QTZ_MASK) == MODULATION_QTZ_MASK) {
				pszQuantized = "[Q]";
			}
			else {
				pszQuantized = "";
			}
			fprintf(stdout, "MOD[%02d]: %s modulates %s, amount:%d %s\n", i + 1,
				ModulationSourcesFlagsNames[pPatch->mod[i].source],
				ModulationDestinationsTypesNames[pPatch->mod[i].dest],
				amount, pszQuantized);
		}
	}
}
//----------------------------------------------------------------------------
/*! Main
@remarks
- first argument is the raw sysex filename to dump
- if several single patch data are found, they are all dumped one after each
other
- to save the output to a text file, enter the following command line:
XpanderSinglePatchViewer [your_raw_sysex_file] > [OutputfileName]
where:
- [your_raw_sysex_file] is the name of the sysex file
- [OutputfileName] is the name of the file to write into.
*/
int _tmain(int argc, _TCHAR* argv[])
{
	SinglePatch patch;
	memset(&patch, 0, sizeof(SinglePatch));

	fprintf(stdout, "Oberheim Xpander/Matrix 12 single patch viewer");
	fprintf(stdout, "The latest version of this utility can be found here: https://github.com/xplorer2716/OberheimXpanderMidiSpec");

	// get sysex filename as argument
	if (argc < 2) {
		fprintf(stderr, "Please specify a file name!\n");
		exit(RETURN_ERROR);
	}
	FILE* pFile = NULL;
	// open binary to avoid ascii code interpretation
	errno_t err = fopen_s(&pFile, argv[1], "rb");
	if (pFile == NULL) {
		fprintf(stderr, "Incorrect file name!\n");
		exit(RETURN_ERROR);
	}

	bool bAtLeastOneSinglePatchDataFound = false;

	// identify single patch data from sysex file
	while (LocateSinglePatchData(pFile) == true) {
		if (!bAtLeastOneSinglePatchDataFound) { bAtLeastOneSinglePatchDataFound = true; }

		// read data into the path
		ReadSinglePatchData(pFile, &patch);
		// dump the patch data
		DumpPatch(&patch);
	}

	//close the file
	fclose(pFile);

	if (!bAtLeastOneSinglePatchDataFound) {
		fprintf(stderr, "NO single patch data found!\n");
		exit(RETURN_ERROR);
	}
	else {
		exit(RETURN_OK);
	}
}
