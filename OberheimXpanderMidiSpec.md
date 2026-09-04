Matrix-12/Xpander MIDI Specification

![](data:image/jpeg;base64...)

image courtesy of http://www.soundofmusic.se

> Note: This document is a rework of three old plain text files dealing with the MIDI spec of the Xpander/Matrix-12.
>
> - <http://machines.hyperreal.org/manufacturers/Oberheim/Xpander.Matrix-12/info/Xpander.MIDI-spec.txt>
> - <http://www.soundofmusic.se/files/xpander_usergroup.htm>
> - <http://www.soundofmusic.se/files/xpander_midi_controller_notes.txt>
>
> This rework has been done to get a more human readable and unique set of information. Changes or additional information about the MIDI implementation were added when required. Redundant information between these files has been removed.
>
> The last version of this document is available here:
>
> - <https://github.com/xplorer2716/OberheimXpanderMidiSpec>
>
> This document and example source code are released under the GPL V3.0 license.
>
> [*https://github.com/xplorer2716*](https://github.com/xplorer2716) *– April 2024*

This document assumes familiarity with the MIDI 1.0 specification and the operation of the Xpander (or Matrix-12). Unless otherwise stated, all specifications and references to Xpander apply to both Xpander and Matrix-12. For further information on the Xpander refer to the 0berheim Xpander Owner's Manual (Part number 950036).

# Change log

| Date | Change |
| --- | --- |
| 05/27/2024 | Minor updates |
| 08/15/2008 | Program data dump follows and [bulk data format](#bulk-data-format-cc-programming-language) updates.<br>Sample C/C++ source code of a single patch viewer application provided in the archive containing this document.<br>“Xpander MIDI Controller Notes” chapter moved before “MIDI Implementation”. |
| 08/04/2008 | Initial revision. |

**Contents**

- [Change log](#change-log)
- [XPANDER OVERVIEW](#xpander-overview)
  - [MIDI Control](#midi-control)
  - [Page Editing](#page-editing)
  - [Edit Modes](#edit-modes)
  - [Voice Assignment](#voice-assignment)
    - [Zones](#zones)
  - [Master MIDI Page](#master-midi-page)
    - [Controller assignment (Lever 1/2, Pedal 1/2)](#controller-assignment-lever-12-pedal-12)
    - [MIDI Enables](#midi-enables)
    - [Send](#send)
    - [Midi Reset](#midi-reset)
  - [Triggers and Gates](#triggers-and-gates)
- [XPANDER MIDI CONTROLLER NOTES](#xpander-midi-controller-notes)
  - [Pitch Bend](#pitch-bend)
  - [Individualized Vibrato Stored by Patch / Mod Lever](#individualized-vibrato-stored-by-patch--mod-lever)
  - [Single Patch Global Vib (programable enables per patch)](#single-patch-global-vib-programable-enables-per-patch)
  - [Programming Sustain Pedal (Non-MIDI)](#programming-sustain-pedal-non-midi)
  - [Midi Sustain Controller](#midi-sustain-controller)
  - [Velocity](#velocity)
- [IMPLEMENTATION NOTES](#implementation-notes)
  - [Status Bytes](#status-bytes)
  - [Data bytes](#data-bytes)
  - [All Notes Off](#all-notes-off)
  - [Modes](#modes)
  - [Power On Defaults](#power-on-defaults)
  - [Note Off (Status = 1000nnnn / 8xH)](#note-off-status--1000nnnn--8xh)
  - [Note On (Status = 1001nnnn / 9xH)](#note-on-status--1001nnnn--9xh)
  - [Control Change (Status = 1011nnnn / BxH)](#control-change-status--1011nnnn--bxh)
  - [Program Change (Status = 1100nnnn / CxH)](#program-change-status--1100nnnn--cxh)
  - [Channel Pressure (Status = 1101nnnn / DxH)](#channel-pressure-status--1101nnnn--dxh)
  - [Pitch Wheel Change (Status = 1110nnnn / ExH)](#pitch-wheel-change-status--1110nnnn--exh)
  - [System Exclusive (Status = F0H)](#system-exclusive-status--f0h)
  - [Tune Request (Status = F6H)](#tune-request-status--f6h)
  - [EOX (Status = F7H)](#eox-status--f7h)
- [APPENDIX A: MIDI IMPLEMENTATION SUMMARY](#appendix-a-midi-implementation-summary)
  - [Transmitted Data - Channel Voice Messages](#transmitted-data---channel-voice-messages)
    - [Program select](#program-select)
  - [Recognized Receive Data - Channel Voice Messages](#recognized-receive-data---channel-voice-messages)
    - [Note off](#note-off)
    - [Note on](#note-on)
    - [Control Change](#control-change)
    - [Program select](#program-select-1)
    - [Pitch Bend Change](#pitch-bend-change)
  - [Recognized Receive Data - Channel Mode Messages](#recognized-receive-data---channel-mode-messages)
    - [All Note off](#all-note-off)
    - [OMNI off](#omni-off)
    - [OMNI on](#omni-on)
    - [MONO on](#mono-on)
    - [POLY on](#poly-on)
  - [Recognized Receive Data - System messages](#recognized-receive-data---system-messages)
- [APPENDIX B: SYSTEM EXCLUSIVE INFORMATION](#appendix-b-system-exclusive-information)
  - [Transmitted Data - Channel Voice Messages](#transmitted-data---channel-voice-messages-1)
    - [Program data dump follows](#program-data-dump-follows)
    - [Copy](#copy)
    - [Store](#store)
    - [Page edit follows](#page-edit-follows)
    - [Page and subpage select](#page-and-subpage-select)
    - [Programmer mode switches](#programmer-mode-switches)
    - [Up/Down](#updown)
    - [Modulation Edit follows](#modulation-edit-follows)
  - [Recognized Receive Data](#recognized-receive-data)
    - [Program data dump request](#program-data-dump-request)
    - [Program data dump follows](#program-data-dump-follows-1)
    - [All data dump request](#all-data-dump-request)
    - [All data dump request (Matrix-12)](#all-data-dump-request-matrix-12)
    - [Copy](#copy-1)
    - [Display control command](#display-control-command)
    - [Store](#store-1)
    - [Page edit follows](#page-edit-follows-1)
    - [Page and Subpage select](#page-and-subpage-select-1)
    - [Transpose](#transpose)
    - [Programmer mode switches](#programmer-mode-switches-1)
    - [Up/Down](#updown-1)
    - [Modulation edit follows](#modulation-edit-follows-1)
    - [Card select (Matrix-12 only)](#card-select-matrix-12-only)
- [PAGE NUMBER DEFINITION](#page-number-definition)
- [BULK DATA FORMAT (C/C++ programming language)](#bulk-data-format-cc-programming-language)
  - [Source code](#source-code)
    - [XpanderSysEx.h](#xpandersysexh)
    - [XpanderSysExViewer.cpp](#xpandersysexviewercpp)
    - [XpanderSysExViewer.README.txt](#xpandersysexviewerreadmetxt)
  - [Demo application: XpanderSysExViewer](#demo-application-xpandersysexviewer)
- [DECIMAL/HEXADECIMAL/BINARY CONVERSION TABLE (unsigned values)](#decimalhexadecimalbinary-conversion-table-unsigned-values)

# XPANDER OVERVIEW

The Xpander is an unusual musical instrument both in operation and in its MIDI implementation. To understand some of the ramifications of MIDI applied to the Xpander it is necessary to understand certain terms and concepts. This section discusses some of the important concepts and features of the Xpander.

## MIDI Control

It should be mentioned at the start that at the present time almost all functions of the Xpander which are accessible from the front panel may be controlled via MIDI system exclusive data. In addition, every button-press and knob-turn may be transmitted to MIDI. This means that almost every feature described in this document is controllable from MIDI.

The Xpander/Matrix-12 remote editing system is designed so that virtually every action on the front panel is echoed to MIDI, and conversely any action which can be performed at the front panel can also be performed remotely from MIDI.

## Page Editing

The Xpander differs from most synthesizers in the fact that it's voices are separately programmable. This means that there are six independent monophonic synthesizers in one package (twelve in the Matrix-12) that share a common front panel and MIDI interface. More than 1200 controls share the same set of six front panel knobs and switches. This is accomplished by grouping the controls into units called pages. For example, all of the controls for Oscillator 1 are displayed at once when the 'VCO 1 page select" button is pressed. The controls in the PAGE MODIFIER section allow editing of the frequency, detune, pulse width and volume of Oscillator 1. Other pages allow control of the VCF, FM, envelopes, LFOS, transpositions, the MIDI and cassette interfaces, and many other aspects of the Xpander's sound and operation.

## Edit Modes

The Xpander's voices can be operated in several different modes. In Single Patch mode all voices play the same sound. The page select buttons select pages which pertain to the sound of a voice and any modification of the sound is played by all voices. There are 100 single patch programs for storing sounds.

Each stores all information needed to play a sound on one voice. When a single patch is recalled, that sound is copied to all voices. Single patch mode emulates a normal homogenous synthesizer and may be used to create sounds to be used in a multi patch. When using the Xpander merely to double another synthesizer, single patch mode is normally used.

In Multi Patch mode, a different single patch sound can be assigned to each voice. The page select buttons select pages which affect this collection of sounds. Each of the 1 00 multi patch programs will remember the assignment of patches to voices as well as the relative transpositions, panning, volume, and control sources of each voice. So in effect, a single patch remembers a sound, while a multi patch remembers information about a collection of sounds but not the sounds themselves. Multi patch mode must be used when voices are to play different sounds. A multi patch, unlike a single patch, remembers keyboard assignment modes, and must be used when creating patches that play all voices in unison.

A submode of multi patch mode is Patch Edit mode where the Xpander still plays separate sounds on each voice, but the controls affect the sound of one or more voices as though in single patch mode. This submode is useful for editing one or more voices in the context of an entire multi patch.

## Voice Assignment

It is desirable to have completely independent control of each voice by assigning each voice to its own MIDI channel. But it is also necessary to be able to do polyphonic voice assignment so that a note stream may be sent on one channel and the machine will assign voices to those notes as available. The Xpander provides the flexibility to operate in both modes simultaneously and even makes the configuration programmable with a multi patch. This is accomplished by allowing independent assignment of the control source for each voice.

### Zones

Options for control sources are any of the six Control voltage/Gate inputs on the rear panel (Xpander only), any one of the sixteen MIDI channels, or one of three Zones (six Zones on the Matrix-12).

These Zones, an Oberheim innovation, are polyphonic note assignment modules. For each Zone, its control source (one of the sixteen MIDI channels or OMNI) , the range of notes over which it is active (MIDI notes 1 through 127), and the method by which it should assign notes to voices (including Rotate, Reassign, Reset, and three unison modes) may be specified.

The following additional parameters apply only to Matrix-12 Zones: The built in keyboard may be used as a control source, MIDI IN and MIDI OUT may be enabled or disabled separately, and the VOICE ROB feature may been enabled or disabled. When MIDI OUT is enabled, notes played on the keyboard will also go to MIDI OUT on the selected Basic Channel. If the CONTROLLERS option is enabled, the levers, pressure and pedal inputs will also be transmitted to MIDI OUT.

## Master MIDI Page

Accessing the master MIDI page allows further control of the MIDI configuration. These parameters are not programmable. The Xpander's Basic Channel is the MIDI channel which is used when addressing the machine as a whole, i.e. program changes, global controllers and "All Notes Off" commands. The Basic Channel may be set to any one of the sixteen MIDI channels.

### Controller assignment (Lever 1/2, Pedal 1/2)

The controller assignment page is used to define what MIDI data is used when a patch refers to "Lever 1", "Lever 2", "Pedal 1', or "Pedal 2".

The defaults are:

* Lever 1 is assigned to MIDI "Channel Pitch Bend".
* Lever 2 is assigned to MIDI Continuous Controller #1 which is commonly the modulation lever or wheel.
* Pedal 1 is assigned to the Xpander's Pedal 1 jack.
* Pedal 2 is assigned to the Xpander’s Pedal 2 jack.
* Pressure is assigned to MIDI "Channel Pressure".

Any of these may be assigned to any of the MIDI controllers (O - 121). In addition, Lever 2 may be assigned to "Channel Pitch Bend".

There is a fundamental difference between Lever 1/Pedal 1 and Lever 2/ Pedal 2. The difference is that while Lever 1 and Pedal 1 are separate for each channel, Lever 2 and Pedal 2 are only received on the Basic Channel and sent to all voices. An example of this practice is using the default assignments given above, modulate the pitch of each voice with Lever 1, and modulate the amount of vibrato on each voice with Lever 2.

If each voice is receiving notes from a different channel, this will allow independent pitch bends on each voice while Controller #1 data received on the Basic Channel will affect all voices in parallel.

### MIDI Enables

The MIDI Enables page allows selection of which information is transmitted and received via MIDI. The SYSTEMX option controls flow of system exclusive information which includes all page editing and patch transfer.

The XMITCV option (Xpander only), when selected, converts all Control voltage/Gate information received at the back of the Xpander into MIDI Note On/Note Off information. When CV POLY is selected, the CV information is transmitted on the Basic Channel. When CV MONO is selected, the CV information is transmitted on consecutive MIDI channels starting with the Basic Channel.

The ECHO option allows the Xpander to function as a mixer, accumulating commands appearing at its MIDI input (excepting system exclusive commands), combining them with locally generated data (including system exclusive and converted CVs), and re-transmitting them to its MIDI output.

The CONTROL option determines whether "Channel Pitch Bend", "Channel Pressure", and "Channel Controller" information is accepted from MIDI.

The PATCH option enables recognition and transmission of "Channel Patch Change" commands. The MIDI Enables page also allows selection of the scaling of the velocity in Note On and Note Off commands.

The choices are:

* LINEAR - the velocity is not modified before application as a modulation amount.
* EXPO 1 - the velocity is passed through an exponential table where an input of 1 corresponds to an output of 1 and an input of 127 corresponds to an output of 255.
* EXPO 2 - the same as EXPO 1 except the input range for maximum output range is 16 to 120. This is primarily for machines which are capable of sending only a limited range of velocity values.
* EXPO 3 (Matrix-1 2 only) - Similar to the other EXPO modes with linear response in the normal playing range.

The final control on this page is the DEFAULTS option (Xpander Main Processor Software Version 1.1 and earlier only), which when in the NO DEFAULTS mode leaves the MIDI configuration intact when power is cycled. The DEFAULTS ON selection will reset the MIDI configuration at power up (see below).

### Send

The SEND subpage (one of the few not accessible from MIDI) allows the user to send the contents of the current edit buffer to MIDI. There is a separate protocol for requesting patches from and sending patches to an Xpander which is under external control.

### Midi Reset

The MIDI RESET subpage allows the user to reset all MIDI parameters to the default status. That is:

* Basic Channel = 1
* All options on the MIDI Enables page OFF.
* All notes off

The MIDI MUTE key allows the user to clear the gates of all voices as though an All Notes Off command had been received. Alternatively, selecting the TUNE page will both remove the gates of all voices and reset all envelopes.

## Triggers and Gates

Traditionally, triggers have been used to start the attack phase of an envelope. On monophonic synthesizers, a single trigger is generated by playing a key while no other keys are down, and a multi trigger is generated by playing a new key, regardless of whether the voice is already playing. In other words, one single trigger is generated when the gate becomes active, and multi triggers are generated by changing the note while the gate is not changing. The gate is active as long as the key is down.

In the Xpander, the envelopes may be triggered independently from either the single or multi trigger sources as well as from an external signal or any of the LFOS. In addition, the LFOs themselves may be reset to a programmable point in their cycle by the single or multi triggers, an external source, or any of the LFOS.

In general, when a voice is not gated and it receives a Note On command, both a multi and a single trigger are generated and its gate is set active. Successive Note On commands to that voice while it is still gated only generate new multi triggers. When a voice is not gated, successive Note Off commands will also generate multi triggers. However, Zones will not pass Note On commands to already gated voices, but instead turn off an already playing voice and re-gate it.

When the XMITCV option (Xpander only) is selected, a back panel gate which becomes active will generate a MIDI Note On command with a velocity of 40H and the associated CV as the note. If the CV changes while the gate is active, a Note Off for the old CV followed by a Note On with the updated CV will be sent. Internally, however, the CV which changes while the gate is active will generate a multi trigger, but not a single trigger.

# XPANDER MIDI CONTROLLER NOTES

MIDI controllers must be enabled on the Xpander.

1. Select Master Page.

2. Select MIDI.

3. Select Enables.

4. Select (underline) Control.

5. Press page 2 and return to MIDI page.

6. Select CTRLS and check MIDI controller assignments for correct controller routing.

7. Switch to NO DEFAULTS on MIDI ENABLES page if settings are to remain the same on power up.

## Pitch Bend

1. Select VCO 1.

2. Select page 2.

3. Select (underline) Lev 1.

4. Repeat for VCO 2.

## Individualized Vibrato Stored by Patch / Mod Lever

1. Select VCO 1.

2. Select FREQ to modulate.

3. Select LFO X (modulation source).

3. Choose which LFO to use (1 to 5). Enter number in X SELECT.

4. Increase mod amount to hear vibrato.

5. Select LFO page and enter number of the LFO mod source.

6. Reduce LFO AMP amount to 0.

7. Select AMP to modulate.

8. Select LEVER 2 as mod source and adjust amount to full.

9. Adjust LFO mod amount on VCO 1 FREQ for desired depth.

10. Repeat procedure for VCO 2 if desired.

## Single Patch Global Vib (programable enables per patch)

Single patch VIB is a special preassigned multi-patch vib that can add vibrato to all enabled single patches without having to program frequency modulation (vibrato) into each patch separately. For each single patch, VIB is enabled by selecting (underlining) VIB while in page 2 of both VCO 1 and VCO 2 pages. "VIB enable" can be stored in each single patch.

After routing depth amount desired MIDI controller (LEV 2), VIB can be used as a live-performance mod wheel (lever) controller on all enabled single patches.

1. Select VCO 1.

2. Select page 2.

3. Select (underline) VIB.

4. Repeat procedure for VCO 2.

5. Select Master page.

6. Select VIB.

7. Increase AMP amount to audible vibrato depth.

8. Adjust LFO SPEED, and WAVE SHAPE to preference.

9. Reduce AMP amount to 0.

10. Select VIB page 2.

11. Select MIDI controller (LEV 2).

12. Adjust VIB depth amount on Xpander while mod wheel (lever) controller on full.

Note: "VIB enable" should be stored into each patch program that user may use VIB on.

## Programming Sustain Pedal (Non-MIDI)

This procedure is for programming sustain pedal control into a patch when using an Oberheim footswitch directly into an Xpander external Pedal input controller.

1. Plug pedal into "PEDAL 2" for global control.

2. Observe which envelope is modulating VCA 2.(press mod dot). Enter ENV X and number of VCA envelope.

3. Without pressing down on the pedal, increase "RELEASE" amount of the envelope to desired amount of sustain, as if the pedal was depressed.

4. Press "RELEASE" button to modulate and select "PED 2" as source.

5. Reduce the modulation of "PED 2" to a negative amount. Continue reducing, until the sustain level of the sound returns to the desired amount when the foot pedal is not depressed.

## Midi Sustain Controller

MIDI controls must be enabled on the Xpander first, in order for any MIDI controller information to be received. The MIDI Controller number being used (Yamaha sust ped = 64) should be assigned to the appropriate Modulation Source (PED 2) on the MIDI CTRLS page. When using a MIDI controller to operate sustain, the pedal modulation amount (affecting env RELEASE) may operate best with a positive amount.

## Velocity

1. Single patch; select Filter/VCA.

2. Reduce VCA 1 amount to 7.

3. Select VCA 1 to modulate.

4. Select VELOCITY as modulation source.

5. Increase Velocity mod amount to full for reference.

6. Check keyboard response and adjust VEL mod amount.

7. Repeat procedure for filter envelope if desired.

# IMPLEMENTATION NOTES

This section discusses the specifics of the data actually transmitted and received via MIDI. There is also discussion of how ambiguous aspects of the MIDI specification 1.0 were interpreted.

## Status Bytes

Running status is implemented.

## Data bytes

No action is taken on a command until all status and data bytes for that command have been received correctly. This includes patch transfers.

When the ECHO option is on, a received message is not transmitted until all of the corresponding data bytes have been received and the message has been determined valid. When retransmitted, a complete message, including status byte, is always sent.

## All Notes Off

On machines with Main Processor software versions 1.4 and later (both Xpander and Matrix-12), an All Notes Off command received on a given MIDI channel will only turn off notes that were turned on from the same channel. On units with earlier software versions, an All Notes Off command will turn off all notes that were turned on by MIDI commands. Turning a note off involves removing the gate from a voice and starting the release phase of the envelopes.

## Modes

The Xpander does respond to Mode Change messages because the POLY/MONO and OMN I ON/OFF status is programmable with each mufti patch and the Xpander is capable of operating in more than one mode simultaneously. The only response to Mode Change messages is to perform an All Notes Off operation for the specified channel.

There are, however, some similarities between voice configurations and normal MIDI modes. Assigning a voice directly to a MIDI channel (or CV) effectively puts that voice in MONO/OMNI OFF mode. Using a Zone is the same as putting the voices assigned to that Zone into POLY mode. If the Zone input is specified as OMNI, then those voices are in POLY/OMNI ON mode, otherwise they are in POLY/OMNI OFF mode.

Thus a single Xpander multi patch can, for example, be simultaneously run from a CV/Gate input, a single MIDI channel in MONO/OMNI OFF, one Zone in POLY/OMNI ON, and another Zone in POLY/OMNI OFF. This configuration, complete with channel assignments, is programmable with the patch.

## Power On Defaults

(Xpander Main Processor software version 1.1 and earlier only)

If the 'DEFAULTS ON' option is selected, when power is applied the Xpander will reset to Basic channel one- (1), with all voice messages disabled except Note On/Note Off. If the "NO DEFAULTS" option is selected, the state of the MIDI parameters at power-up will be the same as it was before power down.

Note:the DEFAULTS ON/NO DEFAULTS option has been eliminated starting with Xpander Main Processor software version 1.2 ; the Matrix-1 2 will always be in NO DEFAULTS mode.

## Note Off (Status = 1000nnnn / 8xH)

Any voice which is both playing the specified note and listening to the specified channel (directly or through a Zone) will have its gate removed. If any voice is assigned directly to the specified channel, but is not currently gated, reception of a note off event will generate a multi trigger ( but not a single trigger) on the voice, and its pitch will be updated to the value specified in the Note Off event. In addition, that voice's release velocity will be updated.

## Note On (Status = 1001nnnn / 9xH)

Any voice assigned directly to the specified channel will have its pitch and velocity assigned to those specified in the Note On event. A multi trigger will be generated for that voice and, if the voice was not already gated, a single trigger will be generated as well.

If a Zone is listening to the specified channel and the note value is within the limits specified for the Zone, the note and corresponding velocity will be assigned according to the assignment mode to voices in that Zone. A new single and multi trigger will be generated for the affected voices.

If there are no un-gated voices in the Zone, the next voice in the rotation will be "robbed" of its gate and the new note will be played on that voice as though the voice had been free.

## Control Change (Status = 1011nnnn / BxH)

All controllers are received as seven bit values, that is, no distinction is made between MSB and LSB controllers (controllers #0 - 31 and #32 - 63) and values from 1 through 126 are not ignored for switches (controllers 64 - 95). Channel mode messages are only received on the Basic channel and are ignored except that they perform an All Notes Off command.

Control changes which are received for controllers assigned to Pressure, Lever 1, or Pedal 1 are maintained separately for each MIDI channel. Control changes which are received on any channel to which any voice or Zone is assigned will, if the specified controller is assigned to Lever 2 or Pedal 2, update the global Lever 2 or Pedal 2 values respectively.

## Program Change (Status = 1100nnnn / CxH)

Program change messages received on the Basic channel will select that patch number. If the machine is in Multi patch mode, a multi patch will be selected. If in Single mode, a single patch will be selected. If in Patch edit mode, all selected voices will receive that single patch.

## Channel Pressure (Status = 1101nnnn / DxH)

The pressure values used in voice modulation are separate for each channel and are only updated by the channel pressure message. This means that when a key is released, a pressure value of zero should be sent, since a Note Off does not automatically clear-the pressure.

## Pitch Wheel Change (Status = 1110nnnn / ExH)

Only eight bits of pitch bend are used. A Pitch Wheel Change message can be assigned to either the Lever 1 or Lever 2 modulation sources (or both). Data sent to lever 1 is kept separate for each channel. If a pitch wheel change message comes in on any channel to which a voice or Zone is assigned and Lever 2 is assigned to BENDER, the Lever 2 value used by all voices will be updated.

The intended use for this difference is to make ft possible to run the voices "monophonically" and have separate pitch bends or to have all voices respond to a pitch bend on any of the channels.

## System Exclusive (Status = F0H)

The Xpander/Matrix-12 uses the system exclusive format extensively to allow exhaustive control from external sources. Patch request and send, page select, edit mode select, and edit value are some of the commands. For more detail on system exclusive formats see Appendix B.

## Tune Request (Status = F6H)

The Xpander does not transmit this message, but will respond to a received Tune Request by selecting the TUNE page and auto-calibrating the VCO frequency and pulse width and the VCF frequency and resonance.

## EOX (Status = F7H)

The Xpander expects incoming system exclusive transmissions to contain a known amount of information. Thus, the command byte will define the amount of information to be received. When that number of data bytes has been received, the transmission is considered to be complete. The machine then waits for the next status byte.

The EOX (End of transmission) message is therefore considered a No-op by the Xpander, although is transmitted as the last byte of every system exclusive message generated. Edit page commands, however, may contain up to six consecutive updates in one transmission. These are processed three bytes at a time.

# APPENDIX A: MIDI IMPLEMENTATION SUMMARY

## Transmitted Data - Channel Voice Messages

### Program select

| Status | Data bytes | Description |
| --- | --- | --- |
| C0H | 0nnn nnnn = 0 through 99**bytes** | Program select (If enabled) |

## Recognized Receive Data - Channel Voice Messages

Note:

1. xxxx: Basic Channel number minus one (i.e., 0000 is Ch. 1, 0001 is Ch.2, etc.).
2. kkkkkkk = Note number.

### Note off

| Status | Data bytes | Description |
| --- | --- | --- |
| 1000 xxxx | 0kkk kkkk<br>0vvv vvvv | Note Off (See notes 1,2)<br>0vvv vvvv = note off velocity |

### Note on

| Status | Data bytes | Description |
| --- | --- | --- |
| 1001 xxxx | 0kkk kkkk<br>0vvv vvvv | Note On (See notes 1,2)<br>0vvv vvvv - 0 Note Off<br>0vv vvvv &gt; 0 note on velocity |

### Control Change

| Status | Data bytes | Description |
| --- | --- | --- |
| 1011 xxxx | 0ccc cccc<br>0nnn nnnn | Control Change (if enabled)<br>0ccc cccc = control number<br>0nnn nnnn = control value |

### Program select

| Status | Data bytes | Description |
| --- | --- | --- |
| 1100 xxxx | 0nnn nnnn | Program Select (If enabled)<br>0nnn nnnn = 0 through 99 |

### Pitch Bend Change

| Status | Data bytes | Description |
| --- | --- | --- |
| 1110 xxxx | 0nnn nnnn<br>0nnn nnnn | Pitch Bend Change LSB<br>Pitch Bend Change MSB |

## Recognized Receive Data - Channel Mode Messages

### All Note off

| Status | Data bytes | Description |
| --- | --- | --- |
| 1011 xxxx | 7BH<br>00H | All Notes Off command.<br>The Xpander turns off all notes that were turned on by MIDI. |

### OMNI off

| Status | Data bytes | Description |
| --- | --- | --- |
| 1011 xxxx | 7CH<br>00H | OMNI mode off<br>The Xpander turns off all notes that were turned on by MIDI. |

### OMNI on

| Status | Data bytes | Description |
| --- | --- | --- |
| 1011 xxxx | 7DH<br>00H | OMNI mode on.<br>The Xpander turns off all notes that were turned on by MIDI. |

### MONO on

| Status | Data bytes | Description |
| --- | --- | --- |
| 1011 xxxx | 7EH<br>00H | MONO mode on<br>The Xpander turns off all notes that were turned on by MIDI. |

### POLY on

| Status | Data bytes | Description |
| --- | --- | --- |
| 1011 xxxx | 7FH<br>00H | POLY mode on<br>The Xpander turns off all notes that were turned on by MIDI. |

## Recognized Receive Data - System messages

| Status | Data bytes | Description |
| --- | --- | --- |
| F6H |  | Tune Request (tune all) |

# APPENDIX B: SYSTEM EXCLUSIVE INFORMATION

Note: The Xpander and Matrix-12 both use System Exclusive Device Number 02H except during Multi Patch data dumps when the Matrix-12 uses 04H.

## Transmitted Data - Channel Voice Messages

### Program data dump follows

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**01H**<br>0ccc cccc<br>0ppp pppp<br>&lt;data&gt; | System exclusive. Oberheim I.D. number.<br>Xpander = 02H (See note 3)<br>Matrix-12 = 04H (Multi patch dump only)<br>Command byte 1 Program data dump follows.<br>Command byte 2: Program type<br>0ccc cccc = 0 :voice data<br>0ccc cccc = 1: multi patch data<br>Command byte 3: Program number<br>Program data. See [bulk data format](#bulk-data-format-cc-programming-language) for details.<br>End of System Exclusive status byte (EOX) |

### Copy

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>02H<br>**04H**<br>0xxx xxxx<br>0000 000x | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 2<br>Command byte 1 : Copy<br>Data : Lower 7 bits first followed by<br>most significant bit.<br>End of System Exclusive status byte (EOX) |

### Store

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>02H<br>**07H**<br>0xxx xxxx | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 2<br>Command byte 1 : Store<br>Data: program number.<br>End of System Exclusive status byte (EOX) |

### Page edit follows

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>02H<br>**0AH**<br>00H<br>&lt;data&gt; | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 2<br>Command byte 1 : Page edit follows<br>See “recognized receive data”, same command.<br>End of System Exclusive status byte (EOX) |

### Page and subpage select

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>02H<br>**0BH**<br>0ppp pppp<br>0ppp pppp | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 2<br>Command byte 1 : Page and subpage select<br>Page number<br>Subpage number<br>End of System Exclusive status byte (EOX) |

### Programmer mode switches

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>02H<br>**0DH**<br>0xxx xxxx<br>0000 000x | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 2<br>Command byte 1 : Programmer Mode Switches<br>Data : Lower 7 bits first followed by<br>most significant bit.<br>End of System Exclusive status byte (EOX) |

### Up/Down

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>02H<br>**0EH**<br>0000 0xx0 | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 2<br>Command byte 1 : Up/Down<br>Data: Format = Set bit 3 = “+” key Set bit 2 = “-“ key<br>End of System Exclusive status byte (EOX) |

### Modulation Edit follows

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>02H<br>**0FH**<br>&lt;data&gt; | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 2<br>Command byte 1 : Modulation Edit follows<br>Data. See “recognized receive data”, same command.<br>End of System Exclusive status byte (EOX) |

## Recognized Receive Data

### Program data dump request

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**00H**<br>0ccc cccc<br>0ppp pppp | System exclusive. Oberheim I.D. number<br>Device number - Xpander = 02H (See note 3)<br>Command byte 1 : Program data dump request<br>Command byte 2: Program type<br>0ccc cccc = 0 voice data<br>0ccc cccc &gt; 0 multi patch data<br>Command byte 3: Program number<br>End of System Exclusive status byte (EOX ) |

On receipt of this message, an Xpander or Matrix-12 will respond by transmitting a Program Data Dump Follows message for the requested patch. This does not affect the contents of the edit buffer.

### Program data dump follows

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**01H**<br>0ccc cccc<br>0ppp pppp<br>&lt;data&gt; | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H (See note 3)<br>Matrix-12 = 04H (Multi patch dump only)<br>Command byte 1 Program data dump follows.<br>Command byte 2: Program type<br>0ccc cccc = 0 voice data ,<br>0ccc cccc = 1 multi patch data<br>Command byte 3: Program number (0 to 99)<br>Program data. See [bulk data format](#bulk-data-format-cc-programming-language) for details.<br>End of System Exclusive status byte (EOX) |

On receipt of this message the data is stored into patch memory at the specified location. This does not affect the contents of the edit buffer.

### All data dump request

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**02H**<br>00H | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : All data dump request<br>Command byte 2: voice data<br>End of System Exclusive status byte (EOX) |

### All data dump request (Matrix-12)

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**02H**<br>01H | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1: All data dump request<br>Command byte 2 Multi patch data (Matrix-12)<br>End of System Exclusive status byte (EOX |

Receipt of this message causes the entire contents of the Xpander or Matrix-12 to be dumped in a series of Program Data Dump Follows messages. The patches are dumped in sequential ascending order, first single patches, then multi patches. Each patch is sent as a separate complete SysEx message bracketed by F0H-F7H. The contents of the edit buffer are not affected.

### Copy

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0dd dddd<br>**04H**<br>0xxx xxxx<br>0000 000x | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Copy<br>Data : Lower 7 bits first followed by most significant bit. :&lt;id_lo&gt;: See Definitions for Programmer Mode (0DH),<br>:&lt;id_hi&gt;: but low order 2 bits of id_lo must be 0.<br>End of System Exclusive status byte (EOX) |

This message may only be used in multi patch mode when a single voice is currently selected. The effect is to copy the contents of that voice's edit buffer to the edit buffers of the specified voices. Note that this can not be used to copy an edit buffer from one bank to the other on a Matrix-12.

### Display control command

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>0ccc cccc<br>&lt;data&gt; | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Display control command<br>**05H** = Xpander<br>**06H** = Matrix-12<br>00H = display control off<br>01H : &lt;disposition&gt;: 80 bytes of ASCII data follows<br>02H = display on<br>ASCII data<br>End of System Exclusive status byte (EOX) |

This message will cas the receiving unit to display up to 80 characters of ASCII in the center two displays. All characters must be in the range 20H-5FH (space to underscore). The data is written into a separate buffer which is then displayed. Each message rewrites the buffer starting at the first character so that if fewer than 80 characters are written, data from a previous message may appear in the display. If no text is sent before the EOX in a <disposition>=1 message, the entire previous contents of the message buffer will be displayed.

### Store

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**07H**<br>0xxx xxxx | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Store<br>Data: Program number (0 to 99)<br>End of System Exclusive status byte (EOX) |

Store the current edit buffer (single or multi) into the specified patch location.

In single patch mode this will always store the current edit buffer into a single patch location.

In multi mode, the effect depends on what is actively selected: If a multi patch is selected, then the multi patch edit buffer will be stored. But if a single voice within a multi-patch is selected, that voice's edit buffer will be stored as a single patch.

### Page edit follows

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**0AH**<br>00H<br>&lt;data&gt; | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte .1 : Page edit follows<br>[&lt;id&gt; 00 &lt;rot_lo&gt; &lt;rot_hi&gt; &lt;val_lo&gt; &lt;val_hi&gt;]<br>&lt;id&gt;:<br>= 00H - 05H for actions on with top buttons.<br>= 08H - 0DH for actions on with bottom buttons.<br>= 18H - 1DH for actions on rotary encoders.<br>&lt;rot&gt; (8 bit value in two bytes, MS bit in d0 of second byte) : amount that a rotary switch has been turned as an 8 bit, 2's complement number.<br>&lt;val&gt; (same as &lt;rot&gt;): value for controller as 8 bit 2Us complement number. Note that flags (e.g. VCO2 Sync) have a value of 0 or 1.<br>End of System Exclusive status byte (EOX) |

In the special case when a modulation is cleared from the modulation routings page on the Matrix-12, <id> is 0AH, and <val> is 3FH.

### Page and Subpage select

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**0BH**<br>0ppp pppp<br>0ppp pppp | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Page and Subpage select<br>Page number<br>Subpage number<br>End of System Exclusive status byte (EOX) |

This message selects the specified page and sub page. If this page is not defined in the current mode, the command will be ignored.

### Transpose

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**0CH**<br>&lt;xpose_lo&gt;<br>&lt;xpose_hi&gt; | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Programmer mode switches<br>These bytes represent a 8 bit, 2Us complement<br>number in the range of -24 to +24 (?)<br>End of System Exclusive status byte (EOX) |

This sets the Master Transpose of the unit.

### Programmer mode switches

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**0DH**<br>&lt;id_lo&gt;<br>&lt;id_hi&gt; | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Programmer mode switches<br>&lt;id&gt; (8 bit quantity in two bytes) as follows:<br>SINGLE 01H (01H 00H)<br>MULTI 02H (02H 00H)<br>1/7 04H (04H 00H)<br>2/8 08H (08H 00H)<br>3/9 10H (10H 00H)<br>4/10 20H (20H 00H)<br>5/11 40H (40H 00H)<br>6/12 80H (00H 01H)<br>End of System Exclusive status byte (EOX) |

Note that pressing several (Patch Edit) buttons simultaneously produces an <id> that is the inclusive OR of the appropriate bits.

(Note: if you wish, that changing from SINGLE to MULTI mode or vice-versa generates a report of the pedal and lever controller values. Power-On generates reports of the pedal positions only. Invoking the "OBERHEIM" patch via STORE-CLEAR generates controller values and a jump to the VCO1 page.)

### Up/Down

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**0EH**<br>0000 0xx0 | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Up/Down<br>Data Format: Set bit 3 = “+” key, Set bit 2 = “-“ key<br>End of System Exclusive status byte (EOX) |

Note that these values have been corrected since the first printing of this specification.

Changing program numbers with the “+” and “-“ keys generates the system exclusive message above. It does not generate a normal MIDI patch change message. Changing program numbers by typing the two digits generates a MIDI program change message, but not the system exclusive message.

The messages generated are identical in SINGLE, MULTI, and Reselecting single voices within a MULTI patch mode.

### Modulation edit follows

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**0FH**<br>00H<br>&lt;data&gt; | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Modulation edit follows<br>[&lt;id&gt; 00 &lt;action&gt; &lt;val_lo&gt; &lt;val_hi&gt;]<br>&lt;id&gt;:0-5, where 0 is the first modulation for the page, 1 is the second, etc.<br>&lt;action&gt;: specifies val (8 bits in 2 bytes):<br>00 Add Modulation Source: ID of source range 0-26<br>01 Delete Modulation N/A<br>02 Change Source New source ID range 0-26<br>03 Set value Value (unsigned)<br>04 Dial Value Amount of change<br>05 Set Quantize 0 = off, 1 = quantize<br>06 Toggle Quantize N/A<br>07 “+” or “-“ : 0 = “+”, 1 = “-“<br>End of System Exclusive status byte (EOX) |

### Card select (Matrix-12 only)

| Status | Data bytes | Description |
| --- | --- | --- |
| F0H<br>F7H | 10H<br>0ddd dddd<br>**10H**<br>0nnn nnnn | System exclusive. Oberheim I.D. number<br>Device number: Xpander = 02H<br>Command byte 1 : Card select (Matrix-12 only)<br>Voice Card number:<br>0 to select voices 1-6<br>1 to select voices 7 - 12<br>End of System Exclusive status byte (EOX) |

# PAGE NUMBER DEFINITION

| Page # (dec) | Page # (hex) | Page description |
| --- | --- | --- |
| 0 | 0H | Master x |
| 1 | 1H | Tune x |
| 2 | 2H | Chain |
| 3 | 3H | Master Misc (M12) |
| 3 | 3H | Gate (Xp) x |
| 4 | 4H | Voice On/Off |
| 5 | 5H | MIDI |
| 6 | 6H | Cassette |
| 7 | 7H | Send Cassette |
| 8 | 8H | Check Cassette |
| 9 | 9H | From Cassette |
| 15 | FH | Load All |
| 16 | 10H | Load One |
| 17 | 11H | Load Single |
| 18 | 2H | Load Multi |
| 19 | 13H | Load Chain |
| 20 | 14H | Start Load |
| 21 | 15H | Load Global |
| 22 | 16H | MIDI Basic Channel |
| 23 | 17H | MIDI Controllers |
| 24 | 18H | MIDI Enables |
| 25 | 19H | MIDI Send Patch |
| 26 | 1AH | MIDI Reset |
| 27 | 1BH | MIDI Mute |
| 29 | 1DH | Version x |
| 32 | 20H | VCO 1 x |
| 33 | 21H | VCO 2 x |
| 34 | 22H | VCF / VCA x |
| 35 | 23H | FM / Lag x |
| 40 | 28H | Env 1 x |
| 41 | 29H | Env 2 x |
| 42 | 2AH | Env 3 x |
| 43 | 2BH | Env 4 x |
| 44 | 2CH | Env 5 x |
| 48 | 30H | LFO 1 x |
| 49 | 31H | LFO 2 x |
| 50 | 32H | LFO 3 x |
| 51 | 33H | LFO 4 x |
| 52 | 34H | LFO 5 x |
| 56 | 38H | Track 1 |
| 57 | 39H | Track 2 |
| 58 | 3AH | Track 3 |
| 64 | 40H | Ramp 1 x |
| 65 | 41H | Ramp 2 x |
| 66 | 42H | Ramp 3 x |
| 67 | 43H | Ramp 4 x |
| 80 | 50H | Zone 1(M12) x |
| 81 | 51H | Zone 2(M12) x |
| 82 | 52H | Zone 3(M12) x |
| 83 | 53H | Zone 4(M12) x |
| 84 | 54H | Zone 5(M12) x |
| 88 | 58H | Misc Name(M12) |
| 89 | 59H | Name(Xp) |
| 96 | 60H | Transpose |
| 97 | 61H | Pan |
| 98 | 62H | VASSIGN / CVMIDI |
| 99 | 63H | Zone (Xp) |
| 100 | 64H | Zone Input(Xp) |
| 101 | 65H | Zone Limits(Xp) |
| 102 | 66H | Zone Mode(Xp) |
| 103 | 67H | Volume |
| 104 | 68H | VIB |
| 105 | 69H | Name(M12) |
| 106 | 6AH | Detune |
| 107 | 6BH | Mod Routings |

# BULK DATA FORMAT (C/C++ programming language)

Because MIDI requires that the most significant bit of bytes transmitted within a system exclusive message be 0, 8 bit quantities must be transmitted in two bytes. In fact, with the Xpander, all data are transmitted as a pair of bytes, even though most only require the first byte, and the second byte is thus generally zero. There are only a few cases where both bytes are used.

Unused bits are sometimes set in uploads, so it is wise to mask out the unused bits before interpreting the bits from an upload.

Different ways are used to represent negative numbers depending on the parameters. See the implementation hereafter for details.

## Source code

Complete commented source code for Microsoft Visual Studio 2022 is provided in the archive containing this document (see the SysExImplementation folder).

This source code was kept as simple as possible so it could be easily translated into another programming language. It is published under the GNU General Public License V3.

### XpanderSysEx.h

This file contains the bit masks, values, and structures for the single patch data.

### XpanderSysExViewer.cpp

This is a demo application using the XpanderSysEx.h file. Look in this file to see how values are extracted from SysEx file.

### XpanderSysExViewer.README.txt

You should read this file first.

## Demo application: XpanderSysExViewer

This command line utility dumps in a human readable format the content of a single patch raw SysEx file. To run this software, open a command prompt with the following command line:

C:\>XpanderSinglePatchViewer [your\_raw\_sysex\_file]

Where [your\_raw\_sysex\_file] is the file you want to dump.

If you specify a Multi patch sysex file, all single patches of the bank will be dumped, but not the multi patch data itself. This will be implemented in the next revision of this document.

For a single patch sysex file, the output will be similar to the following :

Program type: 00h

Program number: 3Bh (59)

NAME: JUMP

---------------------------

VCO1.freq: 00h 0

VCO1.detune: 00h 0

VCO1.pw: 1Fh 31

VCO1.vol: 3Fh 63

VCO1.mod: 0Eh 14 : LAG LEV\_1 VIB

VCO1.wave: 03h 3 : TRI SAW

---------------------------

VCO2.freq: 00h 0

VCO2.detune: 11h 17

VCO2.pw: 1Fh 31

VCO2.vol: 3Fh 63

VCO2.mod: 0Eh 14 : LAG LEV\_1 VIB

VCO2.wave: 06h 6 : SAW PULSE

---------------------------

VCF.freq: 49h 73

VCF.res: 00h 0

VCF.mode: 01h 1 : LOW\_2

VCF.vca1: 3Fh 63

VCF.vca2: 00h 0

VCF.mod: 04h 4 : LEV\_1

---------------------------

FMLAG.amp 00h 0

FMLAG.dest: 01h 1 : VCF\_FREQ

FMLAG.lag\_in: 00h 0 : KBD

FMLAG.lag\_rate: 02h 2

FMLAG.lag\_mode: 01h 1 : LEGATO

---------------------------

LFO[1].speed: 1Ch 28

LFO[1].trg\_mod: 00h 0 : OFF

LFO[1].lag: 00h 0 :

LFO[1].wave: 00h 0 : TRIANGLE

LFO[1].retrig: 00h 0

LFO[1].sample 00h 0 : KBD

LFO[1].amp: 3Fh 63

---------------------------

LFO[2].speed: 39h 57

LFO[2].trg\_mod: 00h 0 : OFF

LFO[2].lag: 00h 0 :

LFO[2].wave: 00h 0 : TRIANGLE

LFO[2].retrig: 00h 0

LFO[2].sample 00h 0 : KBD

LFO[2].amp: 00h 0

---------------------------

LFO[3].speed: 30h 48

LFO[3].trg\_mod: 00h 0 : OFF

LFO[3].lag: 00h 0 :

LFO[3].wave: 00h 0 : TRIANGLE

LFO[3].retrig: 00h 0

LFO[3].sample 00h 0 : KBD

LFO[3].amp: 3Fh 63

---------------------------

LFO[4].speed: 30h 48

LFO[4].trg\_mod: 00h 0 : OFF

LFO[4].lag: 00h 0 :

LFO[4].wave: 00h 0 : TRIANGLE

LFO[4].retrig: 00h 0

LFO[4].sample 00h 0 : KBD

LFO[4].amp: 3Fh 63

---------------------------

LFO[5].speed: 0Dh 13

LFO[5].trg\_mod: 00h 0 : OFF

LFO[5].lag: 00h 0 :

LFO[5].wave: 00h 0 : TRIANGLE

LFO[5].retrig: 00h 0

LFO[5].sample 00h 0 : KBD

LFO[5].amp: 3Fh 63

---------------------------

ENV[1].flags: 84h 132 : MULTI FREERUN

ENV[1].lfo\_trg: 00h 0 : LFO1

ENV[1].delay: 11h 17

ENV[1].attck: 08h 8

ENV[1].decay: 07h 7

ENV[1].sustain: 17h 23

ENV[1].rel: 17h 23

ENV[1].amp: 12h 18

---------------------------

ENV[2].flags: 04h 4 : MULTI

ENV[2].lfo\_trg: 00h 0 : LFO1

ENV[2].delay: 00h 0

ENV[2].attck: 00h 0

ENV[2].decay: 00h 0

ENV[2].sustain: 3Fh 63

ENV[2].rel: 09h 9

ENV[2].amp: 3Fh 63

---------------------------

ENV[3].flags: 04h 4 : MULTI

ENV[3].lfo\_trg: 00h 0 : LFO1

ENV[3].delay: 00h 0

ENV[3].attck: 0Fh 15

ENV[3].decay: 00h 0

ENV[3].sustain: 3Fh 63

ENV[3].rel: 19h 25

ENV[3].amp: 3Fh 63

---------------------------

ENV[4].flags: 84h 132 : MULTI FREERUN

ENV[4].lfo\_trg: 00h 0 : LFO1

ENV[4].delay: 00h 0

ENV[4].attck: 00h 0

ENV[4].decay: 00h 0

ENV[4].sustain: 3Fh 63

ENV[4].rel: 00h 0

ENV[4].amp: 3Fh 63

---------------------------

ENV[5].flags: 84h 132 : MULTI FREERUN

ENV[5].lfo\_trg: 00h 0 : LFO1

ENV[5].delay: 00h 0

ENV[5].attck: 07h 7

ENV[5].decay: 00h 0

ENV[5].sustain: 3Fh 63

ENV[5].rel: 09h 9

ENV[5].amp: 3Fh 63

---------------------------

TRACK[1].input: 00h 0 : KBD

TRACK[1].points: 60,63,25,56,54

---------------------------

TRACK[2].input: 00h 0 : KBD

TRACK[2].points: 0,15,31,47,63

---------------------------

TRACK[3].input: 00h 0 : KBD

TRACK[3].points: 0,15,31,47,63

---------------------------

RAMP[1].rate: 1Fh 31

RAMP[1].flags: 00h 0 :

RAMP[1].lfotrg: 00h 0 : LFO1

---------------------------

RAMP[2].rate: 1Fh 31

RAMP[2].flags: 00h 0 :

RAMP[2].lfotrg: 00h 0 : LFO1

---------------------------

RAMP[3].rate: 1Fh 31

RAMP[3].flags: 00h 0 :

RAMP[3].lfotrg: 00h 0 : LFO1

---------------------------

RAMP[4].rate: 1Fh 31

RAMP[4].flags: 00h 0 :

RAMP[4].lfotrg: 00h 0 : LFO1

---------------------------

MOD[01]: ENV2 modulates RMP2, amount 63

---------------------------

MOD[02]: ENV5 modulates TRK2, amount 60

---------------------------

MOD[03]: LFO2 modulates KBD, amount 58

---------------------------

MOD[04]: LFO2 modulates RVEL, amount 58

---------------------------

MOD[05]: PRES modulates ENV2, amount 41

---------------------------

MOD[06]: PED1 modulates SINGLE, amount 55

---------------------------

MOD[07]: ENV1 modulates RMP2, amount 0

---------------------------

MOD[08]: LFO1 modulates PRES, amount 52

---------------------------

MOD[09]: LFO1 modulates LAG, amount -52

---------------------------

MOD[10]: ENV3 modulates KBD, amount 13

---------------------------

MOD[11]: UNUSED ENTRY

---------------------------

MOD[12]: UNUSED ENTRY

---------------------------

MOD[13]: UNUSED ENTRY

---------------------------

MOD[14]: UNUSED ENTRY

---------------------------

MOD[15]: UNUSED ENTRY

---------------------------

MOD[16]: UNUSED ENTRY

---------------------------

MOD[17]: UNUSED ENTRY

---------------------------

MOD[18]: UNUSED ENTRY

---------------------------

MOD[19]: UNUSED ENTRY

---------------------------

MOD[20]: UNUSED ENTRY

# DECIMAL/HEXADECIMAL/BINARY CONVERSION TABLE (unsigned values)

| decimal | hexa | binary | decimal | hexa | binary | decimal | hexa | binary | decimal | hexa | binary |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 0 | 0 | 0 | 64 | 40 | 1000000 | 128 | 80 | 10000000 | 192 | c0 | 11000000 |
| 1 | 1 | 1 | 65 | 41 | 1000001 | 129 | 81 | 10000001 | 193 | c1 | 11000001 |
| 2 | 2 | 10 | 66 | 42 | 1000010 | 130 | 82 | 10000010 | 194 | c2 | 11000010 |
| 3 | 3 | 11 | 67 | 43 | 1000011 | 131 | 83 | 10000011 | 195 | c3 | 11000011 |
| 4 | 4 | 100 | 68 | 44 | 1000100 | 132 | 84 | 10000100 | 196 | c4 | 11000100 |
| 5 | 5 | 101 | 69 | 45 | 1000101 | 133 | 85 | 10000101 | 197 | c5 | 11000101 |
| 6 | 6 | 110 | 70 | 46 | 1000110 | 134 | 86 | 10000110 | 198 | c6 | 11000110 |
| 7 | 7 | 111 | 71 | 47 | 1000111 | 135 | 87 | 10000111 | 199 | c7 | 11000111 |
| 8 | 8 | 1000 | 72 | 48 | 1001000 | 136 | 88 | 10001000 | 200 | c8 | 11001000 |
| 9 | 9 | 1001 | 73 | 49 | 1001001 | 137 | 89 | 10001001 | 201 | c9 | 11001001 |
| 10 | a | 1010 | 74 | 4a | 1001010 | 138 | 8a | 10001010 | 202 | ca | 11001010 |
| 11 | b | 1011 | 75 | 4b | 1001011 | 139 | 8b | 10001011 | 203 | cb | 11001011 |
| 12 | c | 1100 | 76 | 4c | 1001100 | 140 | 8c | 10001100 | 204 | cc | 11001100 |
| 13 | d | 1101 | 77 | 4d | 1001101 | 141 | 8d | 10001101 | 205 | cd | 11001101 |
| 14 | e | 1110 | 78 | 4e | 1001110 | 142 | 8e | 10001110 | 206 | ce | 11001110 |
| 15 | f | 1111 | 79 | 4f | 1001111 | 143 | 8f | 10001111 | 207 | cf | 11001111 |
| 16 | 10 | 10000 | 80 | 50 | 1010000 | 144 | 90 | 10010000 | 208 | d0 | 11010000 |
| 17 | 11 | 10001 | 81 | 51 | 1010001 | 145 | 91 | 10010001 | 209 | d1 | 11010001 |
| 18 | 12 | 10010 | 82 | 52 | 1010010 | 146 | 92 | 10010010 | 210 | d2 | 11010010 |
| 19 | 13 | 10011 | 83 | 53 | 1010011 | 147 | 93 | 10010011 | 211 | d3 | 11010011 |
| 20 | 14 | 10100 | 84 | 54 | 1010100 | 148 | 94 | 10010100 | 212 | d4 | 11010100 |
| 21 | 15 | 10101 | 85 | 55 | 1010101 | 149 | 95 | 10010101 | 213 | d5 | 11010101 |
| 22 | 16 | 10110 | 86 | 56 | 1010110 | 150 | 96 | 10010110 | 214 | d6 | 11010110 |
| 23 | 17 | 10111 | 87 | 57 | 1010111 | 151 | 97 | 10010111 | 215 | d7 | 11010111 |
| 24 | 18 | 11000 | 88 | 58 | 1011000 | 152 | 98 | 10011000 | 216 | d8 | 11011000 |
| 25 | 19 | 11001 | 89 | 59 | 1011001 | 153 | 99 | 10011001 | 217 | d9 | 11011001 |
| 26 | 1a | 11010 | 90 | 5a | 1011010 | 154 | 9a | 10011010 | 218 | da | 11011010 |
| 27 | 1b | 11011 | 91 | 5b | 1011011 | 155 | 9b | 10011011 | 219 | db | 11011011 |
| 28 | 1c | 11100 | 92 | 5c | 1011100 | 156 | 9c | 10011100 | 220 | dc | 11011100 |
| 29 | 1d | 11101 | 93 | 5d | 1011101 | 157 | 9d | 10011101 | 221 | dd | 11011101 |
| 30 | 1e | 11110 | 94 | 5e | 1011110 | 158 | 9e | 10011110 | 222 | de | 11011110 |
| 31 | 1f | 11111 | 95 | 5f | 1011111 | 159 | 9f | 10011111 | 223 | df | 11011111 |
| 32 | 20 | 100000 | 96 | 60 | 1100000 | 160 | a0 | 10100000 | 224 | e0 | 11100000 |
| 33 | 21 | 100001 | 97 | 61 | 1100001 | 161 | a1 | 10100001 | 225 | e1 | 11100001 |
| 34 | 22 | 100010 | 98 | 62 | 1100010 | 162 | a2 | 10100010 | 226 | e2 | 11100010 |
| 35 | 23 | 100011 | 99 | 63 | 1100011 | 163 | a3 | 10100011 | 227 | e3 | 11100011 |
| 36 | 24 | 100100 | 100 | 64 | 1100100 | 164 | a4 | 10100100 | 228 | e4 | 11100100 |
| 37 | 25 | 100101 | 101 | 65 | 1100101 | 165 | a5 | 10100101 | 229 | e5 | 11100101 |
| 38 | 26 | 100110 | 102 | 66 | 1100110 | 166 | a6 | 10100110 | 230 | e6 | 11100110 |
| 39 | 27 | 100111 | 103 | 67 | 1100111 | 167 | a7 | 10100111 | 231 | e7 | 11100111 |
| 40 | 28 | 101000 | 104 | 68 | 1101000 | 168 | a8 | 10101000 | 232 | e8 | 11101000 |
| 41 | 29 | 101001 | 105 | 69 | 1101001 | 169 | a9 | 10101001 | 233 | e9 | 11101001 |
| 42 | 2a | 101010 | 106 | 6a | 1101010 | 170 | aa | 10101010 | 234 | ea | 11101010 |
| 43 | 2b | 101011 | 107 | 6b | 1101011 | 171 | ab | 10101011 | 235 | eb | 11101011 |
| 44 | 2c | 101100 | 108 | 6c | 1101100 | 172 | ac | 10101100 | 236 | ec | 11101100 |
| 45 | 2d | 101101 | 109 | 6d | 1101101 | 173 | ad | 10101101 | 237 | ed | 11101101 |
| 46 | 2e | 101110 | 110 | 6e | 1101110 | 174 | ae | 10101110 | 238 | ee | 11101110 |
| 47 | 2f | 101111 | 111 | 6f | 1101111 | 175 | af | 10101111 | 239 | ef | 11101111 |
| 48 | 30 | 110000 | 112 | 70 | 1110000 | 176 | b0 | 10110000 | 240 | f0 | 11110000 |
| 49 | 31 | 110001 | 113 | 71 | 1110001 | 177 | b1 | 10110001 | 241 | f1 | 11110001 |
| 50 | 32 | 110010 | 114 | 72 | 1110010 | 178 | b2 | 10110010 | 242 | f2 | 11110010 |
| 51 | 33 | 110011 | 115 | 73 | 1110011 | 179 | b3 | 10110011 | 243 | f3 | 11110011 |
| 52 | 34 | 110100 | 116 | 74 | 1110100 | 180 | b4 | 10110100 | 244 | f4 | 11110100 |
| 53 | 35 | 110101 | 117 | 75 | 1110101 | 181 | b5 | 10110101 | 245 | f5 | 11110101 |
| 54 | 36 | 110110 | 118 | 76 | 1110110 | 182 | b6 | 10110110 | 246 | f6 | 11110110 |
| 55 | 37 | 110111 | 119 | 77 | 1110111 | 183 | b7 | 10110111 | 247 | f7 | 11110111 |
| 56 | 38 | 111000 | 120 | 78 | 1111000 | 184 | b8 | 10111000 | 248 | f8 | 11111000 |
| 57 | 39 | 111001 | 121 | 79 | 1111001 | 185 | b9 | 10111001 | 249 | f9 | 11111001 |
| 58 | 3a | 111010 | 122 | 7a | 1111010 | 186 | ba | 10111010 | 250 | fa | 11111010 |
| 59 | 3b | 111011 | 123 | 7b | 1111011 | 187 | bb | 10111011 | 251 | fb | 11111011 |
| 60 | 3c | 111100 | 124 | 7c | 1111100 | 188 | bc | 10111100 | 252 | fc | 11111100 |
| 61 | 3d | 111101 | 125 | 7d | 1111101 | 189 | bd | 10111101 | 253 | fd | 11111101 |
| 62 | 3e | 111110 | 126 | 7e | 1111110 | 190 | be | 10111110 | 254 | fe | 11111110 |
| 63 | 3f | 111111 | 127 | 7f | 1111111 | 191 | bf | 10111111 | 255 | ff | 11111111 |