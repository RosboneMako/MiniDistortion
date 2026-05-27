/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "cmath" 
#include "Windows.h"

//R1.00 DO NOT DECLARE ANY GLOBAL VARIABLES
//R1.00 This may cause major issues when multiple instances are used in DAWs.

//==============================================================================
MakoDist01AudioProcessor::MakoDist01AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    
    //R4.00 Added for VALUE TREE.
    //R4.00 Define the VST parameters. These are the values that get changed by the DAW (Parameter ID).
    //R4.00 These are connected to the GUI controls in the editor with the Parameter Attachment vars.
    parameters (*this, nullptr, "PARAMETERS",
    {       
        std::make_unique<juce::AudioParameterInt>("amp_on","Amp On", 0, 1, 0),
        std::make_unique<juce::AudioParameterInt>("amp_qual","Amp Qual", 0, 3, 3),
        std::make_unique<juce::AudioParameterFloat>("amp_gain","Amp Gain", 0, 1.0f, .3f),
        std::make_unique<juce::AudioParameterInt>("amp_chan","Amp Channel", 0, 199, 1),
        std::make_unique<juce::AudioParameterInt>("amp_lp","Amp Low Pass", 2000, 12000, 12000),
        std::make_unique<juce::AudioParameterFloat>("amp_comp","Amp Comp", 0, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("amp_cratio","Amp Comp Ratio", 0, 1.0f, .5f),
        std::make_unique<juce::AudioParameterInt>("amp_compf","Amp Comp Freq", 50, 1000, 50),
        std::make_unique<juce::AudioParameterFloat>("amp_slope","Amp Slope", .0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("amp_thump","Amp Thump", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("amp_air","Amp Air", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("amp_boom","Amp Boom", .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("amp_crisp","Amp Crisp", .0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("amp_power","Amp Power", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterInt>("amp_hp","Amp High Pass", 20, 200, 20),
        std::make_unique<juce::AudioParameterFloat>("amp_vol","Amp Volume", .0f, 1.0f, .3f),
        std::make_unique<juce::AudioParameterInt>("amp_eqmode","Amp EQ Mode", 0, 19, 0),
        std::make_unique<juce::AudioParameterFloat>("amp_eq1","Amp EQ 1", -1.0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("amp_eq2","Amp EQ 2", -1.0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("amp_eq3","Amp EQ 3", -1.0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("amp_eq4","Amp EQ 4", -1.0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("amp_eq5","Amp EQ 5", -1.0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("amp_eq6","Amp EQ 6", -1.0f, 1.0f, .0f),

        std::make_unique<juce::AudioParameterInt>("ir_on","IR On", 0, 1, 0),
        std::make_unique<juce::AudioParameterInt>("ir","IR", 0, 199, 1),
        std::make_unique<juce::AudioParameterFloat>("ir_size","IR Size", -1.0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("ir_vol","IR Volume", 0.0f, 1.0f, 1.0f),
        
        std::make_unique<juce::AudioParameterInt>("mod1_on","Mod 1 On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("mod1_rate","Mod 1 Rate", .0f, 1.0f, .1f),
        std::make_unique<juce::AudioParameterFloat>("mod1_depth","Mod 1 Depth", .0f, 1.0f, .3f),
        std::make_unique<juce::AudioParameterFloat>("mod1_mix","Mod 1 Mix", .0f, 1.0f, .6f),      //R5.00 Changed.
        std::make_unique<juce::AudioParameterFloat>("mod1_synth","Mod 1 Synth", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("mod1_bronz","Mod 1 Bronz", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("mod1_vol","Mod 1 Vol", .0f, 1.0f, .5f),        

        std::make_unique<juce::AudioParameterInt>("mod2_on","Mod 2 On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("mod2_rate","Mod 2 Rate", .0f, 1.0f, .3f),        
        std::make_unique<juce::AudioParameterInt>("mod2_range","Mod 2 Range", 0, 2200, 1100),
        std::make_unique<juce::AudioParameterInt>("mod2_fstart","Mod 2 F Start", 100, 500, 300),
        std::make_unique<juce::AudioParameterFloat>("mod2_mix","Mod 2 Mix", .0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("mod2_vol","Mod 2 Vol", .0f, 1.0f, .5f),

        std::make_unique<juce::AudioParameterInt>("mod3_on","Mod 3 On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("mod3_rate","Mod 3 Rate", .0f, 1.0f, .3f),
        std::make_unique<juce::AudioParameterFloat>("mod3_depth","Mod 3 Depth", .0f, 1.0f, .2f),
        std::make_unique<juce::AudioParameterFloat>("mod3_mix","Mod 3 Mix", 0.0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("mod3_tone","Mod 3 Tone", .0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("mod3_bal","Mod 3 Bal", .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("mod3_vol","Mod 3 Vol", .0f, 1.0f, .5f),

        std::make_unique<juce::AudioParameterInt>("mod4_on","Mod 4 On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("mod4_foff","Mod 4 Foff", .0f, 1.0f, .1f),
        std::make_unique<juce::AudioParameterFloat>("mod4_doff","Mod 4 Doff", -1.0f, 1.0f, .7f), //R5.00 Changed.
        std::make_unique<juce::AudioParameterFloat>("mod4_mix","Mod 4 Mix", .0f, 1.0f, .35f),    //R5.00 Changed.
        std::make_unique<juce::AudioParameterFloat>("mod4_tone","Mod 4 Tone", .0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("mod4_bal","Mod 4 Bal", .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("mod4_vol","Mod 4 Vol", .0f, 1.0f, .25f),

        std::make_unique<juce::AudioParameterInt>("delay_on","Delay On", 0, 1, 1),
        std::make_unique<juce::AudioParameterFloat>("delay_time","Delay Time", .0f, 1.0f, .3f),
        std::make_unique<juce::AudioParameterFloat>("delay_mix","Delay Mix", .0f, 1.0f, .1f),
        std::make_unique<juce::AudioParameterFloat>("delay_offset","Delay Offset", .0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("delay_repeat","Delay Repeat", .0f, 1.0f, .3f),
        std::make_unique<juce::AudioParameterFloat>("delay_bal","Delay Balance", .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("delay_duck","Delay Duck", .0f, 1.0f, .0f),

        std::make_unique<juce::AudioParameterInt>("reverb_on","Reverb On", 0, 1, 1),
        std::make_unique<juce::AudioParameterInt>("reverb_mode","Rev Mode", 0, 19, 0),
        std::make_unique<juce::AudioParameterFloat>("reverb_mix","Rev Mix", .0f, 1.0f, .1f),
        std::make_unique<juce::AudioParameterFloat>("reverb_time","Rev Time", .0f, 1.0f, .75f),
        std::make_unique<juce::AudioParameterFloat>("reverb_bal","Rev Bal", .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("reverb_chor","Rev Chor", .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("reverb_size","Rev Size", .0f, 1.0f, .4f),
        std::make_unique<juce::AudioParameterInt>("reverb_lp","Rev LP", 500, 5500, 2500),
        std::make_unique<juce::AudioParameterInt>("reverb_hp","Rev HP", 50, 550, 50),
        std::make_unique<juce::AudioParameterInt>("reverb_pred","Rev PreD", 0, 200, 0),
        std::make_unique<juce::AudioParameterFloat>("reverb_duck","Rev Duck", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterInt>("reverb_parallel","Rev Parallel", 0, 1, 0),

        std::make_unique<juce::AudioParameterInt>("gate_on","Gate On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("gate_vol","Gate Vol", .0f, 1.0f, .2f),
        std::make_unique<juce::AudioParameterFloat>("gate_exp","Gate Exp", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("gate_avg","Gate Avg", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("gate_freq","Gate Freq", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("gate_wah","Gate Wah", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("gate_wahq","Gate WahQ", .7f, 5.0f, 1.4f),
        std::make_unique<juce::AudioParameterFloat>("gate_wahrange","Gate Range", .0f, 1.0f, .5f),

        std::make_unique<juce::AudioParameterInt>("comp_on","Comp On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("comp_vol","Comp Vol", .0f, 1.0f, .2f),
        std::make_unique<juce::AudioParameterFloat>("comp_att","Comp Att", .0f, 1.0f, .4f),  //R5.00 Changed.
        std::make_unique<juce::AudioParameterFloat>("comp_rel","Comp Rel", .0f, 1.0f, .2f),
        std::make_unique<juce::AudioParameterFloat>("comp_thr","Comp Thr", .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("comp_rat","Comp Rat", .0f, 1.0f, .5f),
        std::make_unique<juce::AudioParameterFloat>("comp_drv","Comp Drv", .0f, 1.0f, .0f),

        std::make_unique<juce::AudioParameterInt>("dist1_on","Dist 1 On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("dist1_vol","Dist 1 Vol", .0f, 1.0f, .75f),
        std::make_unique<juce::AudioParameterInt>("dist1_midf","Dist 1 MidF", 200, 2200, 1500),
        std::make_unique<juce::AudioParameterFloat>("dist1_mid","Dist 1 Mid", .0f, 1.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("dist1_midq","Dist 1 MidQ", .1f, 2.0f, .35f),
        std::make_unique<juce::AudioParameterInt>("dist1_cut","Dist 1 Cut", 2000, 7000, 7000),
        std::make_unique<juce::AudioParameterFloat>("dist1_drv","Dist1 Drv", .0f, 1.0f, .2f),

        std::make_unique<juce::AudioParameterInt>("dist2_on", "Dist 2 On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("dist2_vol","Dist 2 Vol", .0f, 1.0f, .1f),    //R5.01 Changed.
        std::make_unique<juce::AudioParameterInt>("dist2_midf","Dist 2 MidF", 400, 1400, 800),
        std::make_unique<juce::AudioParameterFloat>("dist2_mid","Dist 2 Mid", .0f, 1.0f, .75f),   //R5.00 Changed.
        std::make_unique<juce::AudioParameterFloat>("dist2_midq","Dist 2 MidQ", .1f, 2.0f, .35f),
        std::make_unique<juce::AudioParameterFloat>("dist2_high", "Dist 2 High", .0f, 1.0f, .0f),
        std::make_unique<juce::AudioParameterInt>("dist2_cut","Dist 2 Cut", 2000, 7000, 3500),
        std::make_unique<juce::AudioParameterFloat>("dist2_drv","Dist 2 Drv", .0f, 1.0f, .2f),

        std::make_unique<juce::AudioParameterInt>("eq_on", "EQ On", 0, 1, 0),
        std::make_unique<juce::AudioParameterFloat>("eq_vol", "EQ Vol", .0f, 2.0f, 1.0f),
        std::make_unique<juce::AudioParameterInt>("eq_f1f", "EQ Freq 1", 20, 500, 150),
        std::make_unique<juce::AudioParameterFloat>("eq_f1a", "EQ Ampl 1", -12.0f, 12.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("eq_f1q", "EQ Q 2", .1f, 1.4f, .35f),
        std::make_unique<juce::AudioParameterInt>("eq_f2f", "EQ Freq 2", 100, 2100, 700),
        std::make_unique<juce::AudioParameterFloat>("eq_f2a", "EQ Ampl 2", -12.0f, 12.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("eq_f2q", "EQ Q 2", .1f, 1.4f, .35f),
        std::make_unique<juce::AudioParameterInt>("eq_f3f", "EQ Freq 3", 500, 5000, 1800),
        std::make_unique<juce::AudioParameterFloat>("eq_f3a", "EQ Ampl 3", -12.0f, 12.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("eq_f3q", "EQ Q 3", .1f, 1.4f, .35f),
        std::make_unique<juce::AudioParameterInt>("eq_f4f", "EQ Freq 4", 1000, 8000, 2400),
        std::make_unique<juce::AudioParameterFloat>("eq_f4a", "EQ Ampl 4", -12.0f, 12.0f, .0f),
        std::make_unique<juce::AudioParameterFloat>("eq_f4q", "EQ Q 4", .1f, 1.4f, .35f),

    }
        
    )

#endif
{     
    //R5.00 Set our Setting pointers to the correct parameter memory addresses (values).
    Setting[e_AmpOn] = parameters.getRawParameterValue("amp_on");
    Setting[e_AmpQual] = parameters.getRawParameterValue("amp_qual");
    Setting[e_AmpGain] = parameters.getRawParameterValue("amp_gain");
    Setting[e_AmpChan] = parameters.getRawParameterValue("amp_chan");
    Setting[e_AmpLP] = parameters.getRawParameterValue("amp_lp");
    Setting[e_AmpComp] = parameters.getRawParameterValue("amp_comp");
    Setting[e_AmpCRatio] = parameters.getRawParameterValue("amp_cratio");
    Setting[e_AmpCompF] = parameters.getRawParameterValue("amp_compf");
    Setting[e_AmpSlope] = parameters.getRawParameterValue("amp_slope");
    Setting[e_AmpThump] = parameters.getRawParameterValue("amp_thump");
    Setting[e_AmpAir] = parameters.getRawParameterValue("amp_air");
    Setting[e_AmpBoom] = parameters.getRawParameterValue("amp_boom");
    Setting[e_AmpCrisp] = parameters.getRawParameterValue("amp_crisp");
    Setting[e_AmpPower] = parameters.getRawParameterValue("amp_power");
    Setting[e_AmpHP] = parameters.getRawParameterValue("amp_hp");
    Setting[e_AmpVol] = parameters.getRawParameterValue("amp_vol");
    Setting[e_AmpEQMode] = parameters.getRawParameterValue("amp_eqmode");
    Setting[e_AmpEQ1] = parameters.getRawParameterValue("amp_eq1");
    Setting[e_AmpEQ2] = parameters.getRawParameterValue("amp_eq2");
    Setting[e_AmpEQ3] = parameters.getRawParameterValue("amp_eq3");
    Setting[e_AmpEQ4] = parameters.getRawParameterValue("amp_eq4");
    Setting[e_AmpEQ5] = parameters.getRawParameterValue("amp_eq5");
    Setting[e_AmpEQ6] = parameters.getRawParameterValue("amp_eq6");

    Setting[e_IROn] = parameters.getRawParameterValue("ir_on");
    Setting[e_IR] = parameters.getRawParameterValue("ir");
    Setting[e_IRSize] = parameters.getRawParameterValue("ir_size");
    Setting[e_IRVol] = parameters.getRawParameterValue("ir_vol");

    Setting[e_Mod1On] = parameters.getRawParameterValue("mod1_on");
    Setting[e_Mod1Rate] = parameters.getRawParameterValue("mod1_rate");
    Setting[e_Mod1Depth] = parameters.getRawParameterValue("mod1_depth");
    Setting[e_Mod1Mix] = parameters.getRawParameterValue("mod1_mix");
    Setting[e_Mod1Synth] = parameters.getRawParameterValue("mod1_synth");
    Setting[e_Mod1Bronz] = parameters.getRawParameterValue("mod1_bronz");
    Setting[e_Mod1Vol] = parameters.getRawParameterValue("mod1_vol");

    Setting[e_Mod2On] = parameters.getRawParameterValue("mod2_on");
    Setting[e_Mod2Rate] = parameters.getRawParameterValue("mod2_rate");
    Setting[e_Mod2Range] = parameters.getRawParameterValue("mod2_range");
    Setting[e_Mod2FStart] = parameters.getRawParameterValue("mod2_fstart");
    Setting[e_Mod2Mix] = parameters.getRawParameterValue("mod2_mix");    
    Setting[e_Mod2Vol] = parameters.getRawParameterValue("mod2_vol");

    
    Setting[e_Mod3On] = parameters.getRawParameterValue("mod3_on");
    Setting[e_Mod3Rate] = parameters.getRawParameterValue("mod3_rate");
    Setting[e_Mod3Depth] = parameters.getRawParameterValue("mod3_depth");
    Setting[e_Mod3Mix] = parameters.getRawParameterValue("mod3_mix");
    Setting[e_Mod3Tone] = parameters.getRawParameterValue("mod3_tone");
    Setting[e_Mod3Bal] = parameters.getRawParameterValue("mod3_bal");
    Setting[e_Mod3Vol] = parameters.getRawParameterValue("mod3_vol");

    Setting[e_Mod4On] = parameters.getRawParameterValue("mod4_on");
    Setting[e_Mod4Foff] = parameters.getRawParameterValue("mod4_foff");
    Setting[e_Mod4Doff] = parameters.getRawParameterValue("mod4_doff");
    Setting[e_Mod4Mix] = parameters.getRawParameterValue("mod4_mix");
    Setting[e_Mod4Tone] = parameters.getRawParameterValue("mod4_tone");
    Setting[e_Mod4Bal] = parameters.getRawParameterValue("mod4_bal");
    Setting[e_Mod4Vol] = parameters.getRawParameterValue("mod4_vol");

    Setting[e_DelOn] = parameters.getRawParameterValue("delay_on");
    Setting[e_DelTime] = parameters.getRawParameterValue("delay_time");
    Setting[e_DelMix] = parameters.getRawParameterValue("delay_mix");
    Setting[e_DelOffset] = parameters.getRawParameterValue("delay_offset");
    Setting[e_DelRepeat] = parameters.getRawParameterValue("delay_repeat");
    Setting[e_DelBal] = parameters.getRawParameterValue("delay_bal");
    Setting[e_DelDuck] = parameters.getRawParameterValue("delay_duck");

    Setting[e_RevOn] = parameters.getRawParameterValue("reverb_on");
    Setting[e_RevMode] = parameters.getRawParameterValue("reverb_mode");
    Setting[e_RevMix] = parameters.getRawParameterValue("reverb_mix");
    Setting[e_RevTime] = parameters.getRawParameterValue("reverb_time");
    Setting[e_RevBal] = parameters.getRawParameterValue("reverb_bal");
    Setting[e_RevChor] = parameters.getRawParameterValue("reverb_chor");
    Setting[e_RevSize] = parameters.getRawParameterValue("reverb_size");
    Setting[e_RevLP] = parameters.getRawParameterValue("reverb_lp");
    Setting[e_RevHP] = parameters.getRawParameterValue("reverb_hp");
    Setting[e_RevPreD] = parameters.getRawParameterValue("reverb_pred");
    Setting[e_RevDuck] = parameters.getRawParameterValue("reverb_duck");
    Setting[e_RevPar] = parameters.getRawParameterValue("reverb_parallel");

    Setting[e_GateOn] = parameters.getRawParameterValue("gate_on");
    Setting[e_GateVol] = parameters.getRawParameterValue("gate_vol");
    Setting[e_GateExp] = parameters.getRawParameterValue("gate_exp");
    Setting[e_GateAvg] = parameters.getRawParameterValue("gate_avg");
    Setting[e_GateFreq] = parameters.getRawParameterValue("gate_freq");
    Setting[e_GateWah] = parameters.getRawParameterValue("gate_wah");
    Setting[e_GateWahQ] = parameters.getRawParameterValue("gate_wahq");
    Setting[e_GateWahRange] = parameters.getRawParameterValue("gate_wahrange");

    Setting[e_CompOn] = parameters.getRawParameterValue("comp_on");
    Setting[e_CompVol] = parameters.getRawParameterValue("comp_vol");
    Setting[e_CompAtt] = parameters.getRawParameterValue("comp_att");
    Setting[e_CompRel] = parameters.getRawParameterValue("comp_rel");
    Setting[e_CompThr] = parameters.getRawParameterValue("comp_thr");
    Setting[e_CompRat] = parameters.getRawParameterValue("comp_rat");
    Setting[e_CompDrv] = parameters.getRawParameterValue("comp_drv");

    Setting[e_Dist1On] = parameters.getRawParameterValue("dist1_on");
    Setting[e_Dist1Vol] = parameters.getRawParameterValue("dist1_vol");
    Setting[e_Dist1MidF] = parameters.getRawParameterValue("dist1_midf");
    Setting[e_Dist1Mid] = parameters.getRawParameterValue("dist1_mid");
    Setting[e_Dist1MidQ] = parameters.getRawParameterValue("dist1_midq");
    Setting[e_Dist1Cut] = parameters.getRawParameterValue("dist1_cut");
    Setting[e_Dist1Drv] = parameters.getRawParameterValue("dist1_drv");

    Setting[e_Dist2On] = parameters.getRawParameterValue("dist2_on");
    Setting[e_Dist2Vol] = parameters.getRawParameterValue("dist2_vol");
    Setting[e_Dist2MidF] = parameters.getRawParameterValue("dist2_midf");
    Setting[e_Dist2Mid] = parameters.getRawParameterValue("dist2_mid");
    Setting[e_Dist2MidQ] = parameters.getRawParameterValue("dist2_midq");
    Setting[e_Dist2High] = parameters.getRawParameterValue("dist2_high");
    Setting[e_Dist2Cut] = parameters.getRawParameterValue("dist2_cut");
    Setting[e_Dist2Drv] = parameters.getRawParameterValue("dist2_drv");

    Setting[e_EQOn] = parameters.getRawParameterValue("eq_on");
    Setting[e_EQVol] = parameters.getRawParameterValue("eq_vol");
    Setting[e_EQF1F] = parameters.getRawParameterValue("eq_f1f");
    Setting[e_EQF1A] = parameters.getRawParameterValue("eq_f1a");
    Setting[e_EQF1Q] = parameters.getRawParameterValue("eq_f1q");
    Setting[e_EQF2F] = parameters.getRawParameterValue("eq_f2f");
    Setting[e_EQF2A] = parameters.getRawParameterValue("eq_f2a");
    Setting[e_EQF2Q] = parameters.getRawParameterValue("eq_f2q");
    Setting[e_EQF3F] = parameters.getRawParameterValue("eq_f3f");
    Setting[e_EQF3A] = parameters.getRawParameterValue("eq_f3a");
    Setting[e_EQF3Q] = parameters.getRawParameterValue("eq_f3q");
    Setting[e_EQF4F] = parameters.getRawParameterValue("eq_f4f");
    Setting[e_EQF4A] = parameters.getRawParameterValue("eq_f4a");
    Setting[e_EQF4Q] = parameters.getRawParameterValue("eq_f4q");

    //R5.00 Do some initial setup stuff.
    Mako_Program_Construct();
}

MakoDist01AudioProcessor::~MakoDist01AudioProcessor()
{
    //R3.00 This should never be called unless closing as the save button was pressed.
    if (DefAmp_Save) Mako_ExternalData_Save();   
}

//==============================================================================
const juce::String MakoDist01AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MakoDist01AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MakoDist01AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MakoDist01AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MakoDist01AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MakoDist01AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MakoDist01AudioProcessor::getCurrentProgram()
{
    return 0;
}

void MakoDist01AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MakoDist01AudioProcessor::getProgramName (int index)
{
    return {};
}

void MakoDist01AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MakoDist01AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    Mako_Program_Update(false);
}

void MakoDist01AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MakoDist01AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MakoDist01AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    float tS = 0; float tS2 = 0; float tSD = 0; float tSR = 0;
    float tMixSample = 0.0f;
        
    //R1.00 Update our output and input PEAK VOLUME variables.
    //R5.00 Give the vars some time to gradually fall off.
    makoVU_Out = int(makoVU_PeakOut * 100.0f);  makoVU_PeakOut *= .98f;
    makoVU_In = int(makoVU_PeakIn * 100.0f);    makoVU_PeakIn *= .98f;

    //R1.00 Make a copy of sample rate. JUCE says this is the best place to read it.
    makoSampleRate != MakoDist01AudioProcessor::getSampleRate();
    

    //R1.00 Handle any settings changes made in the Editor. 
    if (0 < makoSettingsChanged)
    {  
        while (0 < makoSettingsChanged) Mako_SettingsUpdate(true);
    }

    // JUCE NOTES
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // JUCE NOTES
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    //R2.22 Create an array to hold the MONO audio data. Size will be unknown.
    int BScnt = buffer.getNumSamples();
    std::vector <float> channelMonoData(BScnt, 0.0f);  
    
    //R2.22 Loop thru all of the channels of audio and adjust the audio buffers for each channel.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        //R2.11 Process the AUDIO buffer data.
        for (int samp = 0; samp < BScnt; samp++)
        {
            //R4.00 Process the left channel(0) to a point (Pedal 5 EQ), then process in stereo.
            if (channel == 0)
            {
                //R1.00 Get audio SAMPLE
                tS = buffer.getSample(channel, samp);    //R1.00 Get the RAW untouched sample. 

                if (tS != 0.0000f)
                    tS = tS;

                //R2.00 Apply Effects PEDALS. 
                if (DoPed1Enabled) tS = Mako_FX_Audio_NoiseGate(tS, channel, 1);   
                if (DoPed2Enabled) tS = Mako_FX_Audio_Compressor(tS, channel, 2);  
                if (DoPed3Enabled) tS = Mako_FX_Audio_Distortion1(tS, channel, 3); 
                if (DoPed4Enabled) tS = Mako_FX_Audio_Distortion2(tS, channel, 4);

                //R1.00 Input VU Meter - Store the loudest Sample. Verify we are not clipping in the mod section!
                if (makoVU_PeakIn < abs(tS)) makoVU_PeakIn = abs(tS);
                tS = Mako_Clip(tS, vu_PreAmp);

                //R2.23 Apply MOD effects.
                if (DoMod1) tS = Mako_FX_Audio_Flange(tS, channel);
                if (DoMod2) tS = Mako_FX_Audio_ModWah(tS, channel);

                //R4.00 Do FINAL EQ in PreAmp position.
                if (InputEQ_PreEQ) tS = Mako_Amp_EQ(tS * .01f, channel);
                
                //R5.01 Frequency adjusting Compressor.
                if (DoAmpComp) tS = Mako_Amp_Comp(tS, channel);

                //R2.22 Do clean amp or IR based input EQ Amps.
                if (DoAmp)
                {
                    //R4.10 Apply high/low pass filters to shape signal before gain stage.
                    if (DoAmpBoom) tS = Mako_Amp_Boom(tS, channel);
                    if (DoAmpCrisp) tS = Mako_Amp_Crisp(tS, channel);

                    if (InputEQ_4xOS)
                        tS = Mako_Amp_InputIR_4x(tS, channel);
                    else
                        tS = Mako_Amp_InputIR(tS, channel);
                }
                else
                    tS *= (*Setting[e_AmpGain] * 20.0f);
                                
                //R1.00 Apply Speaker Impulse Response.
                if (DoIR)  tS = Mako_CabSim(tS, channel);

                //R2.21 Calc Speaker Thump and AIR.
                if (DoAmpThump) tS = Mako_Amp_Thump(tS, channel);
                if (DoAmpAir)  tS = Mako_Amp_Air(tS, channel);

                //R4.00 Do FINAL EQ if not in preAmp position.
                if (!InputEQ_PreEQ) tS = Mako_Amp_EQ(tS * .1f, channel);

                //******************************************************************
                // R1.00 POWER AMP STAGE for POST EQ Power Amp (default). 
                //******************************************************************
                if (DoAmpPower) tS = tanhf(tS * (1.0f + (*Setting[e_AmpPower] * 10.0f)));

                //R4.00 Apply the amp Volume setting.
                tS = Mako_Volume_Final(tS, channel);
               
                //R4.00 LOW PASS - Removes harshness. Moved to position after POWER to remove crispyness.
                if (DoAmpLP) tS = Filter_Calc_BiQuad(tS, channel, &fil_Amp_LP);
                if (DoAmpHP) tS = Filter_Calc_BiQuad(tS, channel, &fil_Amp_HP);

                if (DoPed5Enabled) tS = Mako_FX_Audio_EQ(tS, channel, 5);

                tS = Mako_Clip(tS, vu_Amp);

                //R5.00 Store the mono signal(Chan 0) calculated up to this point to be used as Chan 1 data.
                channelMonoData[samp] = tS;
            }

            //R4.00 If working with channel 1, pull the stored data from Chan 0 to create a two channel mono up to this point.
            if (channel) tS = channelMonoData[samp];

            //R4.00 STEREO from here on out. ***************************************

            //R1.00 Apply FX post amp code, Delay, and Reverb.
            if (DoMod3) tS = Mako_FX_Audio_Chorus(tS, channel);
            if (DoMod4) tS = Mako_FX_Audio_WideLoad(tS, channel);

            //R4.00 Do Delay and Reverb in Parallel or Series.
            switch (DoRevPar)
            {
            case rdm_Series:  //Series.
                if (DoDelMix) tS = Mako_FX_Delay(tS, channel);
                if (DoRevMix) tS = Mako_FX_Reverb(tS, channel);
                break;
            case rdm_Parallel: //Parallel.
                tSD = Mako_FX_Delay(tS, channel);
                tSR = Mako_FX_Reverb(tS, channel);
                tS = (tSD + tSR) * .5f;
                break;
            case rdm_ParDel: //Delay Only.
                tS = Mako_FX_Delay(tS, channel);
                break;
            case rdm_ParRev: //Verb only.
                tS = Mako_FX_Reverb(tS, channel);
                break;
            case rdm_None:   //none.
                break;
            }            
            
            //R1.00 Track our highest peak volume. 
            if (makoVU_PeakOut < abs(tS)) makoVU_PeakOut = abs(tS);
            tS = Mako_Clip(tS, vu_Final);

            //R4.00 Write the modified audio into the sample buffer for playback.
            channelData[samp] = tS;
        }       
    }
    
}

//R3.00 Reduce lows entering the AMP IR gain calcs.
float MakoDist01AudioProcessor::Mako_Amp_Boom(float tSample, int channel)
{  
   return Filter_Calc_BiQuad(tSample, channel, &fil_Amp_Boom);
}

//R3.00 Reduce highs entering the AMP IR gain calcs.
float MakoDist01AudioProcessor::Mako_Amp_Crisp(float tSample, int channel)
{
    return Filter_Calc_BiQuad(tSample, channel, &fil_Amp_Crisp);
}

//R4.00 Apply gain to a Low Pass filter then mix in with the original audio signal.
float MakoDist01AudioProcessor::Mako_Amp_Thump(float tSample, int channel)
{
    float tS = Filter_Calc_BiQuad(tSample, channel, &fil_Amp_Thump);
    return tSample + tanhf(tS * *Setting[e_AmpThump] * 4.0f);

}

//R4.00 Apply gain to a High Pass filter then mix in with the original audio signal.
float MakoDist01AudioProcessor::Mako_Amp_Air(float tSample, int channel)
{
    float tS = Filter_Calc_BiQuad(tSample, channel, &fil_Amp_Air);
    return tSample + tanhf(tS * *Setting[e_AmpAir] * 4.0f);
}

//R1.00 FINAL Volume.
float MakoDist01AudioProcessor::Mako_Volume_Final(float tSample, int channel)
{
    float tS = tSample;
    
    //R1.00 Slowly fade volume in when making PRESET changes.
    if (*Setting[e_AmpVol] < Amp_Vol)
        Amp_Vol = *Setting[e_AmpVol];
    else
        Amp_Vol = (Amp_Vol * RATI1000ms) + (*Setting[e_AmpVol] * RAT1000ms);
       
    //R4.00 Apply VOLUME. Square to get more range on knob.
    tS = tS * (Amp_Vol * Amp_Vol * 15.0f);
       
    //R1.00 Final CLIP check. Flag that we are clipping.
    if (tS < -.9999f)
    {
        tS = -.9999f;
        makoClipStage = 2;
    }
    else if (.9999f < tS)
    {
        tS = .9999f;
        makoClipStage = 2;
    }

    return tS;
}


//R2.00 EQ 0 
void MakoDist01AudioProcessor::Mako_EQ_Set_0()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = false;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;

    Amp_EQ1.Freq = 150.0f;
    Amp_EQ2.Freq = 500.0f;
    Amp_EQ3.Freq = 1200.0f;
    Amp_EQ4.Freq = 2500.0f;
    Amp_EQ5.Freq = 5500.0f;

    Amp_EQ1.Q = 1.414f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .35f;
    Amp_EQ4.Q = .25f;
    Amp_EQ5.Q = .35f;
}

//R2.00 EQ 1
void MakoDist01AudioProcessor::Mako_EQ_Set_1()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = false;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;

    Amp_EQ1.Freq = 100.0f;
    Amp_EQ2.Freq = 330.0f;
    Amp_EQ3.Freq = 700.0f;
    Amp_EQ4.Freq = 1800.0f;
    Amp_EQ5.Freq = 5000.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .350f;
    Amp_EQ4.Q = .150f;
    Amp_EQ5.Q = .350f;
}

//R2.00 EQ 2
void MakoDist01AudioProcessor::Mako_EQ_Set_2()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = false;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;

    Amp_EQ1.RangedB = 12.0f;
    Amp_EQ2.RangedB = 12.0f;
    Amp_EQ3.RangedB = 12.0f;
    Amp_EQ4.RangedB = 12.0f;
    Amp_EQ5.RangedB = 12.0f;

    Amp_EQ1.Freq = 80.0f;
    Amp_EQ2.Freq = 220.0f;
    Amp_EQ3.Freq = 750.0f;
    Amp_EQ4.Freq = 2200.0f;
    Amp_EQ5.Freq = 6600.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .35f;
    Amp_EQ3.Q = .35f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .35f;
}

//R2.00 EQ 3
void MakoDist01AudioProcessor::Mako_EQ_Set_3()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 12.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 12.0f;
    Amp_EQ6.RangedB = 12.0f;

    Amp_EQ1.Freq = 120.0f;
    Amp_EQ2.Freq = 600.0f;
    Amp_EQ3.Freq = 2000.0f;
    Amp_EQ4.Freq = 10000.0f;
    Amp_EQ5.Freq = 80.0f;
    Amp_EQ6.Freq = 1500.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .35f;
    Amp_EQ3.Q = .25f;
    Amp_EQ4.Q = .25f;
    Amp_EQ5.Q = 1.414f;
    Amp_EQ6.Q = 1.414f;

}

//R2.00 EQ 4
void MakoDist01AudioProcessor::Mako_EQ_Set_4()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = false;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;

    Amp_EQ1.Freq = 100.0f;
    Amp_EQ2.Freq = 400.0f;
    Amp_EQ3.Freq = 1000.0f;
    Amp_EQ4.Freq = 2500.0f;
    Amp_EQ5.Freq = 6600.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .35f;
    Amp_EQ3.Q = .35f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .707f;

}

//R2.00 EQ 5
void MakoDist01AudioProcessor::Mako_EQ_Set_5()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 50.0f;
    Amp_EQ2.Freq = 120.0f;
    Amp_EQ3.Freq = 240.0f;
    Amp_EQ4.Freq = 500.0f;
    Amp_EQ5.Freq = 1000.0f;
    Amp_EQ6.Freq = 2200.0f;

    Amp_EQ1.Q = .35f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .707f;
    Amp_EQ4.Q = .707f;
    Amp_EQ5.Q = .707f;
    Amp_EQ6.Q = .35f;
}

//R2.00 EQ 6
void MakoDist01AudioProcessor::Mako_EQ_Set_6()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 80.0f;
    Amp_EQ2.Freq = 300.0f;
    Amp_EQ3.Freq = 900.0f;
    Amp_EQ4.Freq = 1500.0f;
    Amp_EQ5.Freq = 3000.0f;
    Amp_EQ6.Freq = 6000.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .35f;
    Amp_EQ3.Q = .35f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .35f;
    Amp_EQ6.Q = .707f;
}

//R2.00 EQ 7
void MakoDist01AudioProcessor::Mako_EQ_Set_7()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 100.0f;
    Amp_EQ2.Freq = 400.0f;
    Amp_EQ3.Freq = 1000.0f;
    Amp_EQ4.Freq = 2000.0f;
    Amp_EQ5.Freq = 4000.0f;
    Amp_EQ6.Freq = 8000.0f;

    Amp_EQ1.Q = .1f;
    Amp_EQ2.Q = .5f;
    Amp_EQ3.Q = .35f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .35f;
    Amp_EQ6.Q = .1f;
}

//R2.00 EQ 8
void MakoDist01AudioProcessor::Mako_EQ_Set_8()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 80.0f;
    Amp_EQ2.Freq = 400.0f;
    Amp_EQ3.Freq = 750.0f;
    Amp_EQ4.Freq = 1500.0f;
    Amp_EQ5.Freq = 3000.0f;
    Amp_EQ6.Freq = 6000.0f;

    Amp_EQ1.Q = .35f;
    Amp_EQ2.Q = .5f;
    Amp_EQ3.Q = .5f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .25f;
    Amp_EQ6.Q = .35f;
}

//R2.00 EQ 9
void MakoDist01AudioProcessor::Mako_EQ_Set_9()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;
    
    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;
    
    Amp_EQ1.Freq = 80.0f;
    Amp_EQ2.Freq = 200.0f;
    Amp_EQ3.Freq = 400.0f;
    Amp_EQ4.Freq = 800.0f;
    Amp_EQ5.Freq = 1600.0f;
    Amp_EQ6.Freq = 3200.0f;
    
    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .707f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .707f;
    Amp_EQ6.Q = .707f;    
}

void MakoDist01AudioProcessor::Mako_EQ_Set_10()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;
    
    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;
    
    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;
   
    Amp_EQ1.Freq = 100.0f;
    Amp_EQ2.Freq = 450.0f;
    Amp_EQ3.Freq = 700.0f;
    Amp_EQ4.Freq = 900.0f;
    Amp_EQ5.Freq = 1500.0f;
    Amp_EQ6.Freq = 3000.0f;
    
    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .35f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .707f;
    Amp_EQ6.Q = .35f;    
}

void MakoDist01AudioProcessor::Mako_EQ_Set_11()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 80.0f;
    Amp_EQ2.Freq = 180.0f;
    Amp_EQ3.Freq = 600.0f;
    Amp_EQ4.Freq = 1200.0f;
    Amp_EQ5.Freq = 2000.0f;
    Amp_EQ6.Freq = 4000.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .35f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .35f;
    Amp_EQ6.Q = .35f;    
}

void MakoDist01AudioProcessor::Mako_EQ_Set_12()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 120.0f;
    Amp_EQ2.Freq = 450.0f;
    Amp_EQ3.Freq = 800.0f;
    Amp_EQ4.Freq = 1000.0f;
    Amp_EQ5.Freq = 1500.0f;
    Amp_EQ6.Freq = 2200.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .707f;
    Amp_EQ4.Q = .707f;
    Amp_EQ5.Q = 1.414f;
    Amp_EQ6.Q = .35f;
}

void MakoDist01AudioProcessor::Mako_EQ_Set_13()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 100.0f;
    Amp_EQ2.Freq = 200.0f;
    Amp_EQ3.Freq = 400.0f;
    Amp_EQ4.Freq = 800.0f;
    Amp_EQ5.Freq = 1600.0f;
    Amp_EQ6.Freq = 3200.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .707f;
    Amp_EQ4.Q = .707f;
    Amp_EQ5.Q = .707f;
    Amp_EQ6.Q = .707f;
}

void MakoDist01AudioProcessor::Mako_EQ_Set_14()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 80.0f;
    Amp_EQ2.Freq = 500.0f;
    Amp_EQ3.Freq = 900.0f;
    Amp_EQ4.Freq = 1400.0f;
    Amp_EQ5.Freq = 2500.0f;
    Amp_EQ6.Freq = 5000.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .707f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .707;
    Amp_EQ6.Q = .35f;
}

void MakoDist01AudioProcessor::Mako_EQ_Set_15()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 200.0f;
    Amp_EQ2.Freq = 700.0f;
    Amp_EQ3.Freq = 1200.0f;
    Amp_EQ4.Freq = 1900.0f;
    Amp_EQ5.Freq = 3000.0f;
    Amp_EQ6.Freq = 8000.0f;

    Amp_EQ1.Q = .35f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .707f;
    Amp_EQ4.Q = .35f;
    Amp_EQ5.Q = .707;
    Amp_EQ6.Q = .35f;
}

void MakoDist01AudioProcessor::Mako_EQ_Set_16()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 50.0f;
    Amp_EQ2.Freq = 120.0f;
    Amp_EQ3.Freq = 300.0f;
    Amp_EQ4.Freq = 600.0f;
    Amp_EQ5.Freq = 900.0f;
    Amp_EQ6.Freq = 4000.0f;

    Amp_EQ1.Q = .707f;
    Amp_EQ2.Q = .707f;
    Amp_EQ3.Q = .707f;
    Amp_EQ4.Q = .707f;
    Amp_EQ5.Q = .707;
    Amp_EQ6.Q = .35f;
}

void MakoDist01AudioProcessor::Mako_EQ_Set_17()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = false;
    Amp_EQ5.Enabled = false;
    Amp_EQ6.Enabled = false;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 30.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 30.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 20.0f;
    Amp_EQ2.Freq = 700.0f;
    Amp_EQ3.Freq = 10000.0f;
    //Amp_EQ4.Freq = 600.0f;
    //Amp_EQ5.Freq = 900.0f;
    //Amp_EQ6.Freq = 4000.0f;

    Amp_EQ1.Q = .1f;
    Amp_EQ2.Q = .35f;
    Amp_EQ3.Q = .1f;
    //Amp_EQ4.Q = .707f;
    //Amp_EQ5.Q = .707;
    //Amp_EQ6.Q = .35f;
}

void MakoDist01AudioProcessor::Mako_EQ_Set_18()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 60.0f;
    Amp_EQ2.Freq = 240.0f;
    Amp_EQ3.Freq = 660.0f;
    Amp_EQ4.Freq = 1200.0f;
    Amp_EQ5.Freq = 2400.0f;
    Amp_EQ6.Freq = 10000.0f;

    Amp_EQ1.Q = .1f;
    Amp_EQ2.Q = .35f;
    Amp_EQ3.Q = .5f;
    Amp_EQ4.Q = .5f;
    Amp_EQ5.Q = .5f;
    Amp_EQ6.Q = .1f;
}

void MakoDist01AudioProcessor::Mako_EQ_Set_19()
{
    Amp_EQ1.Enabled = true;
    Amp_EQ2.Enabled = true;
    Amp_EQ3.Enabled = true;
    Amp_EQ4.Enabled = true;
    Amp_EQ5.Enabled = true;
    Amp_EQ6.Enabled = true;

    Amp_EQ1.Gain = .50f;
    Amp_EQ2.Gain = .50f;
    Amp_EQ3.Gain = .50f;
    Amp_EQ4.Gain = .50f;
    Amp_EQ5.Gain = .50f;
    Amp_EQ6.Gain = .50f;

    Amp_EQ1.RangedB = 24.0f;
    Amp_EQ2.RangedB = 24.0f;
    Amp_EQ3.RangedB = 24.0f;
    Amp_EQ4.RangedB = 24.0f;
    Amp_EQ5.RangedB = 24.0f;
    Amp_EQ6.RangedB = 24.0f;

    Amp_EQ1.Freq = 100.0f;
    Amp_EQ2.Freq = 350.0f;
    Amp_EQ3.Freq = 700.0f;
    Amp_EQ4.Freq = 1400.0f;
    Amp_EQ5.Freq = 2800.0f;
    Amp_EQ6.Freq = 10000.0f;

    Amp_EQ1.Q = .1f;
    Amp_EQ2.Q = .35f;
    Amp_EQ3.Q = .5f;
    Amp_EQ4.Q = .5f;
    Amp_EQ5.Q = .5f;
    Amp_EQ6.Q = .1f;
}



void MakoDist01AudioProcessor::Mako_EQ_Set_Filters()
{
    //R2.00 AMPLIFIER EQ
    if ((Amp_EQ1.Enabled) && (SettingRqd[e_AmpEQ1]))
    {
        Mako_UpdateLastVar(e_AmpEQ1);
        Filter_BP_Coeffs(*Setting[e_AmpEQ1] * Amp_EQ1.RangedB, Amp_EQ1.Freq, Amp_EQ1.Q, &fil_Amp_EQ1);
    }
    if ((Amp_EQ2.Enabled) && (SettingRqd[e_AmpEQ2]))
    {
        Mako_UpdateLastVar(e_AmpEQ2);
        Filter_BP_Coeffs(*Setting[e_AmpEQ2] * Amp_EQ2.RangedB, Amp_EQ2.Freq, Amp_EQ2.Q, &fil_Amp_EQ2);
    }
    if ((Amp_EQ3.Enabled) && (SettingRqd[e_AmpEQ3]))
    {
        Mako_UpdateLastVar(e_AmpEQ3);
        Filter_BP_Coeffs(*Setting[e_AmpEQ3] * Amp_EQ3.RangedB, Amp_EQ3.Freq, Amp_EQ3.Q, &fil_Amp_EQ3);
    }
    if ((Amp_EQ4.Enabled) && (SettingRqd[e_AmpEQ4]))
    {
        Mako_UpdateLastVar(e_AmpEQ4);
        Filter_BP_Coeffs(*Setting[e_AmpEQ4] * Amp_EQ4.RangedB, Amp_EQ4.Freq, Amp_EQ4.Q, &fil_Amp_EQ4);
    }
    if ((Amp_EQ5.Enabled) && (SettingRqd[e_AmpEQ5]))
    {
        Mako_UpdateLastVar(e_AmpEQ5);
        Filter_BP_Coeffs(*Setting[e_AmpEQ5] * Amp_EQ5.RangedB, Amp_EQ5.Freq, Amp_EQ5.Q, &fil_Amp_EQ5);
    }
    if ((Amp_EQ6.Enabled) && (SettingRqd[e_AmpEQ6]))
    {
        Mako_UpdateLastVar(e_AmpEQ6);
        Filter_BP_Coeffs(*Setting[e_AmpEQ6] * Amp_EQ6.RangedB, Amp_EQ6.Freq, Amp_EQ6.Q, &fil_Amp_EQ6);
    }
    
}

//R1.00 Some settings need to be calculated here instead of editor. 
void MakoDist01AudioProcessor::Mako_SettingsUpdate(bool UpdateParms)
{   
    int StartCount = makoSettingsChanged;

    //R3.00 User has requested a database reload.
    if (DefAmp_Reload == 1)
    {
        Mako_ExternalData_Load(true);
        DefAmp_Reload = 2;           //R3.00 Reload request complete.
        makoEditorNeedsUpdated = 1;  //R3.00 Flag editor to rebuild its amp lists.
    }

    //R3.00 We have unsaved database edits, save.
    if (DefAmp_Save)
    {
        DefAmp_Save = false;
        Mako_ExternalData_Save();
    }

    //**********************************************************
    //R4.00 Amp Settings
    //**********************************************************
    if (SettingRqd[e_AmpLP])
    {
        Mako_UpdateLastVar(e_AmpLP);
        Filter_LP_Coeffs(*Setting[e_AmpLP], &fil_Amp_LP);
    }  
    if (SettingRqd[e_AmpHP])
    {
        Mako_UpdateLastVar(e_AmpHP);
        Filter_HP_Coeffs(*Setting[e_AmpHP], &fil_Amp_HP);
    }
        
    if (SettingRqd[e_AmpChan])
    {
        Mako_UpdateLastVar(e_AmpChan);
        Mako_Amp_Set();
    }   

    if (SettingRqd[e_AmpQual])
    {
        Mako_UpdateLastVar(e_AmpQual);

        switch (int(*Setting[e_AmpQual]))
        {
        case 0: Amp_Qual_Max = 128; break;
        case 1: Amp_Qual_Max = 256; break;
        case 2: Amp_Qual_Max = 512; break;
        case 3: Amp_Qual_Max = 1024; break;
        }
    }

    if (SettingRqd[e_AmpComp])
    {
        Mako_UpdateLastVar(e_AmpComp);
        Amp_Comp_Threshold = *Setting[e_AmpComp] * *Setting[e_AmpComp] * *Setting[e_AmpComp]; //R4.30 Added.
    }

    Mako_EQ_Set_Filters();

    //R3.00 Added Boom and Crisp.
    if (SettingRqd[e_AmpBoom])
    {
        Mako_UpdateLastVar(e_AmpBoom);
        Filter_FO_HP_Coeffs(20.0f + ((1.0f - *Setting[e_AmpBoom]) * 1000.0f), &fil_Amp_Boom);
    }
    if (SettingRqd[e_AmpCrisp])
    {
        Mako_UpdateLastVar(e_AmpCrisp);
        Filter_FO_LP_Coeffs(500.0f + *Setting[e_AmpCrisp] * 5000.0f, &fil_Amp_Crisp);
    }
    if (SettingRqd[e_AmpCompF])
    {
        Mako_UpdateLastVar(e_AmpCompF);
        Filter_BP_Coeffs(0.0f, *Setting[e_AmpCompF], Amp_Comp_Q, &fil_Amp_Comp); //R4.30 Added AMP Comp filtering.
    }

    //R5.00 Added. 
    if (SettingRqd[e_AmpEQMode])
    {
        Mako_UpdateLastVar(e_AmpEQMode);
        switch (int(*Setting[e_AmpEQMode]))
        {
        case 0: Mako_EQ_Set_0(); break;
        case 1: Mako_EQ_Set_1(); break;
        case 2: Mako_EQ_Set_2(); break;
        case 3: Mako_EQ_Set_3(); break;
        case 4: Mako_EQ_Set_4(); break;
        case 5: Mako_EQ_Set_5(); break;
        case 6: Mako_EQ_Set_6(); break;
        case 7: Mako_EQ_Set_7(); break;
        case 8: Mako_EQ_Set_8(); break;
        case 9: Mako_EQ_Set_9(); break;
        case 10: Mako_EQ_Set_10(); break;
        case 11: Mako_EQ_Set_11(); break;
        case 12: Mako_EQ_Set_12(); break;
        case 13: Mako_EQ_Set_13(); break;
        case 14: Mako_EQ_Set_14(); break;
        case 15: Mako_EQ_Set_15(); break;
        case 16: Mako_EQ_Set_16(); break;
        case 17: Mako_EQ_Set_17(); break;
        case 18: Mako_EQ_Set_18(); break;
        case 19: Mako_EQ_Set_19(); break;
        }
        Mako_EQ_Set_Filters();
    }
    //**********************************************************
    //R4.00 PEDAL SETTINGS
    //**********************************************************
    if (SettingRqd[e_GateAvg])  { Mako_UpdateLastVar(e_GateAvg);  Mako_FX_Setup_NoiseGate(1); }
    if (SettingRqd[e_GateFreq]) { Mako_UpdateLastVar(e_GateFreq); Mako_FX_Setup_NoiseGate(1); }
    if (SettingRqd[e_GateWahRange]) { Mako_UpdateLastVar(e_GateWahRange); Mako_FX_Setup_NoiseGate(1); }

    if (SettingRqd[e_CompThr]) { Mako_UpdateLastVar(e_CompThr);   Mako_FX_Setup_Compressor(2); }

    if (SettingRqd[e_Dist1Mid]) { Mako_UpdateLastVar(e_Dist1Mid); Mako_FX_Setup_Distortion1(3); }
    if (SettingRqd[e_Dist1MidF]) { Mako_UpdateLastVar(e_Dist1MidF); Mako_FX_Setup_Distortion1(3); }
    if (SettingRqd[e_Dist1MidQ]) { Mako_UpdateLastVar(e_Dist1MidQ); Mako_FX_Setup_Distortion1(3); }
    if (SettingRqd[e_Dist1Cut]) { Mako_UpdateLastVar(e_Dist1Cut); Mako_FX_Setup_Distortion1(3); }
    
    if (SettingRqd[e_Dist2Mid])  { Mako_UpdateLastVar(e_Dist2Mid);  Mako_FX_Setup_Distortion2(4); }
    if (SettingRqd[e_Dist2MidF]) { Mako_UpdateLastVar(e_Dist2MidF); Mako_FX_Setup_Distortion2(4); }
    if (SettingRqd[e_Dist2MidQ]) { Mako_UpdateLastVar(e_Dist2MidQ); Mako_FX_Setup_Distortion2(4); }
    if (SettingRqd[e_Dist2High]) { Mako_UpdateLastVar(e_Dist2High); Mako_FX_Setup_Distortion2(4); }
    if (SettingRqd[e_Dist2Cut]) { Mako_UpdateLastVar(e_Dist2Cut);   Mako_FX_Setup_Distortion2(4); }
    
    //R4.00 EQ Settings.
    if (SettingRqd[e_EQF1F]) { Mako_UpdateLastVar(e_EQF1F); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF1A]) { Mako_UpdateLastVar(e_EQF1A); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF1Q]) { Mako_UpdateLastVar(e_EQF1Q); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF2F]) { Mako_UpdateLastVar(e_EQF2F); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF2A]) { Mako_UpdateLastVar(e_EQF2A); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF2Q]) { Mako_UpdateLastVar(e_EQF2Q); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF3F]) { Mako_UpdateLastVar(e_EQF3F); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF3A]) { Mako_UpdateLastVar(e_EQF3A); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF3Q]) { Mako_UpdateLastVar(e_EQF3Q); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF4F]) { Mako_UpdateLastVar(e_EQF4F); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF4A]) { Mako_UpdateLastVar(e_EQF4A); Mako_FX_Setup_EQ(5); }
    if (SettingRqd[e_EQF4Q]) { Mako_UpdateLastVar(e_EQF4Q); Mako_FX_Setup_EQ(5); }
    
    //**********************************************************
    //R4.00 MODULATION SETTINGS
    //**********************************************************
    //R4.00 DEPTH settings.
    if (SettingRqd[e_Mod1Depth]) { Mako_UpdateLastVar(e_Mod1Depth); Mako_FX_Setup_Mod1(); }
    if (SettingRqd[e_Mod2Range]) { Mako_UpdateLastVar(e_Mod2Range); Mako_FX_Setup_Mod2(); }
    if (SettingRqd[e_Mod2Rate])  { Mako_UpdateLastVar(e_Mod2Rate);  Mako_FX_Setup_Mod2(); }
    if (SettingRqd[e_Mod3Depth]) { Mako_UpdateLastVar(e_Mod3Depth); Mako_FX_Setup_Mod3(); }
    if (SettingRqd[e_Mod4Foff])  { Mako_UpdateLastVar(e_Mod4Foff);  Mako_FX_Setup_Mod4(); }
    if (SettingRqd[e_Mod4Doff]) { Mako_UpdateLastVar(e_Mod4Doff); Mako_FX_Setup_Mod4(); }
 
    //R4.00 BALANCE Settings.
    if (SettingRqd[e_Mod3Bal]) { Mako_UpdateLastVar(e_Mod3Bal); Mako_FX_Setup_Mod3(); }
    if (SettingRqd[e_Mod4Bal]) { Mako_UpdateLastVar(e_Mod4Bal); Mako_FX_Setup_Mod4(); }

    //**********************************************************
    //R1.00 DELAY Settings.
    //**********************************************************
    if (SettingRqd[e_DelTime] || SettingRqd[e_DelOffset])
    {
        Mako_UpdateLastVar(e_DelTime);
        Mako_UpdateLastVar(e_DelOffset);
        Delay_B_Ring1[0] = int(2.0f * *Setting[e_DelTime] * (.5f + ((1.0f - *Setting[e_DelOffset]) * .5f)) * makoSampleRate);
        Delay_B_Ring1_Max[0] = int(2.0f * *Setting[e_DelTime] * (.5f + ((1.0f - *Setting[e_DelOffset]) * .5f)) * makoSampleRate) + 1;
        Delay_B_Ring1[1] = int(2.0f * *Setting[e_DelTime] * makoSampleRate);
        Delay_B_Ring1_Max[1] = int(2.0f * *Setting[e_DelTime] * makoSampleRate) + 1;
    }
    if (SettingRqd[e_DelMix])
    {
        Mako_UpdateLastVar(e_DelMix);
        if (*Setting[e_DelMix] < .5f)
        {
            Delay_Dry = 1.0f;
            Delay_Wet = *Setting[e_DelMix] * 2;
        }
        else
        {
            Delay_Dry = 1.0f - ((*Setting[e_DelMix] - .5f) * 2.0f);
            Delay_Wet = 1.0f;
        }
    }
    if (SettingRqd[e_DelBal])
    {
        Mako_UpdateLastVar(e_DelBal);
        if (*Setting[e_DelBal] < .5f)
        {
            Delay_BalLR[0] = 1.0f;
            Delay_BalLR[1] = *Setting[e_DelBal] * 2;
        }
        else
        {
            Delay_BalLR[0] = 1.0f - ((*Setting[e_DelBal] - .5f) * 2.0f);
            Delay_BalLR[1] = 1.0f;
        }
    }

    //**********************************************************
    //R2.00 REVERB Settings
    //**********************************************************
    if (SettingRqd[e_RevTime])
    {
        Mako_UpdateLastVar(e_RevTime);
        Reverb_TimeInv = 1.0f - *Setting[e_RevTime];
    }    
    if (SettingRqd[e_RevSize])
    {   
        Mako_UpdateLastVar(e_RevSize);
        Mako_FX_Reverb_CalcSize();        
    }
    if (SettingRqd[e_RevMode])
    {
        Mako_UpdateLastVar(e_RevMode);
        Mako_FX_Reverb_CalcSize();
    }
    //R2.22 Calc our PreDelay. Max 200mS or 38400 samples. 
    if (SettingRqd[e_RevPreD])
    {   
        Mako_UpdateLastVar(e_RevPreD);
        PreDEnd = *Setting[e_RevPreD] * .001f * makoSampleRate;
        if (PreDMax < PreDEnd) PreDEnd = PreDMax;
    }
    if (SettingRqd[e_RevMix])
    {
        Mako_UpdateLastVar(e_RevMix);
        if (*Setting[e_RevMix] < .5f)
        {
            Reverb_Dry = 1.0f;
            Reverb_Wet = *Setting[e_RevMix] * 2;
        }
        else
        {
            Reverb_Dry = 1.0f - ((*Setting[e_RevMix] - .5f) * 2.0f);
            Reverb_Wet = 1.0f;
        }
    }
    if (SettingRqd[e_RevBal])
    {
        Mako_UpdateLastVar(e_RevBal);
        if (*Setting[e_RevBal] < .5f)
        {
            Reverb_BalLR[0] = 1.0f;
            Reverb_BalLR[1] = *Setting[e_RevBal] * 2;
        }
        else
        {
            Reverb_BalLR[0] = 1.0f - ((*Setting[e_RevBal] - .5f) * 2.0f);
            Reverb_BalLR[1] = 1.0f;
        }
    }
    if (SettingRqd[e_RevLP])
    {
        Mako_UpdateLastVar(e_RevLP);
        Filter_LP_Coeffs(*Setting[e_RevLP], &fil_Rev_LP);   
    }
    if (SettingRqd[e_RevHP])
    {
        Mako_UpdateLastVar(e_RevHP);
        Filter_HP_Coeffs(*Setting[e_RevHP], &fil_Rev_HP);   
    }

    //**********************************************************
    //R4.00 SPEAKER Impulse Response Settings
    //**********************************************************
    if (SettingRqd[e_IR])
    {
        Mako_UpdateLastVar(e_IR);
        Mako_IR_Set();        
    }    
    if (SettingRqd[e_IRSize])
    {
        Mako_UpdateLastVar(e_IRSize);        
        Mako_SetIR_Size(&IR_Speaker[0]); //R5.00 Added this function.
    }    


    //R4.00 Hack for smoother transitions.
    if (makoSettingsClearbuffers == 1)
    {
        //R1.00 Clear Delay/Reverb buffers so we dont get loud bangs when updating. 
        //R1.00 Still happens anyway.
        for (int t = 0; t <= 192000; t++)
        {
            Delay_B[0][t] = 0.0f;
            Delay_B[1][t] = 0.0f;            
        }

        makoSettingsClearbuffers = 0;
    }

    //R4.00 Set DO TEST vars.
    //R4.00 These vars are used to speed up the Audio processing decision making.
    (0.001f < *Setting[e_GateOn]) ? DoPed1Enabled = true : DoPed1Enabled = false;
    (0.001f < *Setting[e_CompOn]) ? DoPed2Enabled = true : DoPed2Enabled = false;
    (0.001f < *Setting[e_Dist1On]) ? DoPed3Enabled = true : DoPed3Enabled = false;
    (0.001f < *Setting[e_Dist2On]) ? DoPed4Enabled = true : DoPed4Enabled = false;
    (0.001f < *Setting[e_EQOn]) ? DoPed5Enabled = true : DoPed5Enabled = false;
    
    (0.001f < *Setting[e_Mod1On]) ? DoMod1 = true : DoMod1 = false;
    (0.001f < *Setting[e_Mod2On]) ? DoMod2 = true : DoMod2 = false;
    (0.001f < *Setting[e_Mod3On]) ? DoMod3 = true : DoMod3 = false;
    (0.001f < *Setting[e_Mod4On]) ? DoMod4 = true : DoMod4 = false;

    (0.001f < *Setting[e_AmpOn]) ? DoAmp = true : DoAmp = false;
    (*Setting[e_AmpBoom] < .999f) ? DoAmpBoom = true : DoAmpBoom = false;
    (*Setting[e_AmpCrisp] < .999f) ? DoAmpCrisp = true : DoAmpCrisp = false;
    (0.001f < *Setting[e_AmpThump]) ? DoAmpThump = true : DoAmpThump = false;
    (0.001f < *Setting[e_AmpAir]) ? DoAmpAir = true : DoAmpAir = false;
    (0.001f < *Setting[e_AmpPower]) ? DoAmpPower = true : DoAmpPower = false;
    (*Setting[e_AmpLP] < 11999.0f) ? DoAmpLP = true : DoAmpLP = false;
    (20.1f < *Setting[e_AmpHP]) ? DoAmpHP = true : DoAmpHP = false;
    (*Setting[e_AmpComp] < .999f) ? DoAmpComp = true : DoAmpComp = false;
    (0.001f < *Setting[e_IROn]) ? DoIR = true : DoIR = false;
    (0.001f < *Setting[e_DelMix]) ? DoDelMix = true : DoDelMix = false;
    (0.001f < *Setting[e_DelOn]) ? DoDelMix = true : DoDelMix = false;
    (0.001f < *Setting[e_RevMix]) ? DoRevMix = true : DoRevMix = false;
    (0.001f < *Setting[e_RevOn]) ? DoRevMix = true : DoRevMix = false;
    
    //R4.00 Configure the Reverb-Delay parallel mode if using.
    DoRevPar = rdm_Series;
    if ((0.001f < *Setting[e_RevPar]) && DoRevMix && DoDelMix) DoRevPar = rdm_Parallel;
    if ((0.001f < *Setting[e_RevPar]) && (!DoRevMix) && (DoDelMix)) DoRevPar = rdm_ParDel;
    if ((0.001f < *Setting[e_RevPar]) && (DoRevMix) && (!DoDelMix)) DoRevPar = rdm_ParRev;
    if ((*Setting[e_RevPar] < .001f) && (!DoRevMix) && (!DoDelMix)) DoRevPar = rdm_None;

    //R4.00 Setup checks for Amplifier EQ.
    ((Amp_EQ1.Enabled) && (*Setting[e_AmpEQ1] != .0f)) ? DoAmpEQ1 = true: DoAmpEQ1 = false;
    ((Amp_EQ2.Enabled) && (*Setting[e_AmpEQ2] != .0f)) ? DoAmpEQ2 = true : DoAmpEQ2 = false;
    ((Amp_EQ3.Enabled) && (*Setting[e_AmpEQ3] != .0f)) ? DoAmpEQ3 = true : DoAmpEQ3 = false;
    ((Amp_EQ4.Enabled) && (*Setting[e_AmpEQ4] != .0f)) ? DoAmpEQ4 = true : DoAmpEQ4 = false;
    ((Amp_EQ5.Enabled) && (*Setting[e_AmpEQ5] != .0f)) ? DoAmpEQ5 = true : DoAmpEQ5 = false;
    ((Amp_EQ6.Enabled) && (*Setting[e_AmpEQ6] != .0f)) ? DoAmpEQ6 = true : DoAmpEQ6 = false;

    //R4.00 Setup checks for Six Band EQ.
    (0.0f != *Setting[e_EQF1A]) ? DoEQ1 = true : DoEQ1 = false;
    (0.0f != *Setting[e_EQF2A]) ? DoEQ2 = true : DoEQ2 = false;
    (0.0f != *Setting[e_EQF3A]) ? DoEQ3 = true : DoEQ3 = false;
    (0.0f != *Setting[e_EQF4F]) ? DoEQ4 = true : DoEQ4 = false;    
    
    //R1.00 RESET our settings changed flag. 
    if (makoSettingsChanged == StartCount) makoSettingsChanged -= 1; //R4.00 A change is not being found, decrement so we dont get stuck in a loop.
    if (makoSettingsChanged < 0) makoSettingsChanged = 0;
}

//R5.01 Update the change REQUIRED flag and decrement our setting counter.
void MakoDist01AudioProcessor::Mako_UpdateLastVar(int idx)
{    
    //R5.01 Reset flag and update counter.
    if (SettingRqd[idx])
    {
        SettingRqd[idx] = false;
        makoSettingsChanged -= 1;
    }
}

//==============================================================================
bool MakoDist01AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MakoDist01AudioProcessor::createEditor()
{
    return new MakoDist01AudioProcessorEditor (*this);
}

//==============================================================================

//R4.00 DAW parameter writer.
void MakoDist01AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    //R1.00 Save our parameters to file/DAW.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);   

    return;
}

//R4.00 DAW parameter reader.
void MakoDist01AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    //R1.00 Read our parameters from file/DAW.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
      

    //R5.00 Get all of the current settings.
    for (int t = 0; t < e_PLast; t++) SettingRqd[t] = true;

    Mako_SettingsUpdate(true);

    return;
}


//R1.00 Parameter reading helper function.
int MakoDist01AudioProcessor::makoGetParmValue_int(juce::String Pstring)
{
    auto parm = parameters.getRawParameterValue(Pstring);
    if (parm != NULL)
        return int(parm->load());
    else
        return 0;
}

//R1.00 Parameter reading helper function.
float MakoDist01AudioProcessor::makoGetParmValue_float(juce::String Pstring)
{
    auto parm = parameters.getRawParameterValue(Pstring);
    if (parm != NULL)
        return float(parm->load());
    else
        return 0.0f;
}


//R4.00 Initialize some program variables.
void MakoDist01AudioProcessor::Mako_Program_Init()
{
    //*******************************************************************
    // MAKO VARIABLES
    //*******************************************************************
    //R2.00 Only call this once. It gets called everytime you "render" a track in a DAW. 
    if ((PrePlay) && (makoSampleRate == MakoDist01AudioProcessor::getSampleRate())) return;
    PrePlay = true;

    Mako_FX_Reverb_CalcSize();

    //R1.00 Get the SAMPLE RATE.
    //char SR[100]; 
    //sprintf(SR, "RATE: %f \n", MakoDist01AudioProcessor::getSampleRate());
    //MessageBox(0, SR, "Error", MB_ICONSTOP);
    makoSampleRate = MakoDist01AudioProcessor::getSampleRate();
    if (makoSampleRate < 21000) makoSampleRate = 48000;
    if (192000 < makoSampleRate) makoSampleRate = 48000;
    
    //R1.00 Calculate some rough decay subtraction values for peak tracking (compress,autowah,etc). 
    Release_100mS = (1.0f / .100f) * (1.0f / makoSampleRate);
    Release_200mS = (1.0f / .200f) * (1.0f / makoSampleRate);
    Release_300mS = (1.0f / .300f) * (1.0f / makoSampleRate);
    Release_400mS = (1.0f / .400f) * (1.0f / makoSampleRate);
    Release_500mS = (1.0f / .500f) * (1.0f / makoSampleRate);

    //R1.00 REVERB MODULATE Vars.
    RevMod_B_Rate_Up = (1.0f / makoSampleRate) * 250.0f; //R5.00 RATE How fast we traverse thru the modulation indexes.
    RevMod_B_Rate_Down = -RevMod_B_Rate_Up;
    RevMod_B_Rate[0] = RevMod_B_Rate_Up;                 //R4.30 Starting rate.
    RevMod_B_Rate[1] = RevMod_B_Rate_Up;                 //R4.30 Starting rate.
    RevMod_B_Rate[1] = RevMod_B_Rate_Up;                 //R4.30 Starting rate.
    RevMod_B_Ring1[0] = 0;         //R1.00 index into buffer.
    RevMod_B_Offset[0] = 1190;     //R1.00 Where our mod index starts
    RevMod_B_Offset_Min[0] = 880;  //R1.00 580 - Min distance our index can get from the buffer index.
    RevMod_B_Offset_Max[0] = 1460; //R1.00 1860 - Max distance our index can get from the buffer index.
    RevMod_B_Ring1[1] = 0;
    RevMod_B_Offset[1] = 900;       //R1.00 We will modulate from 1mS (48) onward. 
    RevMod_B_Offset_Min[1] = 880;
    RevMod_B_Offset_Max[1] = 1460;

    //R1.00 DELAY Vars.
    Delay_B_Ring1[0] = 15000;
    Delay_B_Ring1_Max[0] = 15001;
    Delay_B_Ring1[1] = 30000;
    Delay_B_Ring1_Max[1] = 30001;
    
    //R1.00 MODULATE 1 Vars - FLANGER.
    Mod1_B_Rate_Up = (1.0f / makoSampleRate) * 500.0f; //R5.00 RATE How fast we traverse thru the modulation indexes.
    Mod1_B_Rate_Down = -Mod1_B_Rate_Up;
    Mod1_B_Rate[0] = Mod1_B_Rate_Up;             //R4.00 Starting rate.
    Mod1_B_Offset[0] = 50;                       //R5.00 Start our buffer somewhere close to where it will be operating.
    Mod1_B_Ring1[0] = 0;                         //R4.00 Index into buffer.
    Mod1_B_Ring1_Max[0] = 4096;                  //R4.00 Size of buffer.
    Mako_FX_Setup_Mod1();
    Filter_HP_Coeffs(20.0f, &fil_Mod1_DCRemoval); //R4.10 Pedal 1 DC Removal. Bronzer adds a DC offset to the signal that needs to be removed.
    //Filter_LP_Coeffs(1000.0f, &fil_Mod1_Crackle); //R5.00 HACK: Remove High Freq crackle / discontinuity in flanged signal.
    
    //R1.00 MODULATE 2 Vars - MOD FILTER.
    Mod2_B_Rate_Up = 1.0f / makoSampleRate;    //R4.00 RATE How fast we traverse thru the modulation indexes.
    Mod2_B_Rate_Down = -1.0f / makoSampleRate;
    Mod2_B_Rate[0] = Mod2_B_Rate_Up;           //R4.00 Starting rate left.
    Mod2_B_Rate[1] = Mod2_B_Rate_Up;           //R4.00 Starting rate right.
    
    //R4.00 MODULATE 3 Vars - CHORUS.
    Mod3_B_Rate_Up = 1.0f / makoSampleRate;    //R4.00 RATE How fast we traverse thru the modulation indexes.
    Mod3_B_Rate_Down = -1.0f / makoSampleRate;
    Mod3_B_Rate[0] = Mod3_B_Rate_Up;           //R4.00 Starting rate left.
    Mod3_B_Rate[1] = Mod3_B_Rate_Up;           //R4.00 Starting rate right.
    Mod3_B_Offset[0] = 20;                     //R5.00 Start our buffer somewhere close to where it will be operating.
    Mod3_B_Offset[0] = 20;                     //R5.00 Start our buffer somewhere close to where it will be operating.
    Mod3_B_Ring1[0] = 0;                       //R4.10 index into buffer.
    Mod3_B_Ring1[1] = 0;                       //R4.10 index into buffer.
    Mod3_B_Ring1_Max[0] = 2048;                //R5.00 Size of buffer.    
    Mod3_B_Ring1_Max[1] = 2048;                //R5.00 Size of buffer.
    Mako_FX_Setup_Mod3();

    //R4.00 MODULATE 4 Vars - WIDE LOAD.
    Mod4_B_Ring1[0] = 0;                       //R4.00 index into buffer.
    Mod4_B_Ring1[1] = 0;
    
    //R4.00 Initial AMP and IR Cab.
    Mako_Amp_Set();
    Mako_IR_Set();    

    //R2.00 Calculate the default filter coefficients.
    Filter_HP_Coeffs(20.0f, &fil_Out_HP);          //R4.00 Hi Pass filter to block any DC we add in code.
    Filter_LP_Coeffs(1500.0f, &fil_Rev_LP);        //R4.00 Reverb Filters.
    Filter_HP_Coeffs(50.0f, &fil_Rev_HP);                  
        
    //R2.00 Pre Calc our AMPLIFIER EQs.
    Filter_LP_Coeffs(2000.0f, &fil_Amp_LP);              //R4.30 Setting[e_AmpLP]. Initialize to lowest value always!
    Filter_HP_Coeffs(20.0f, &fil_Amp_HP);              //R5.00 Setting[e_AmpHP]. Initialize to lowest value always!
    Filter_FO_LP_Coeffs(150.0f, &fil_Amp_Thump);         //R4.00 THUMP
    Filter_HP_Coeffs(1500.0f, &fil_Amp_Air);             //R4.00 AIR
    Filter_BP_Coeffs(0.0f, 50.0f, Amp_Comp_Q, &fil_Amp_Comp);  //R4.30 COMP Setting[e_AmpCompF]. Initialize to lowest value always!
    Mako_EQ_Set_0();

}

//R4.00 Initialize variables, etc.
void MakoDist01AudioProcessor::Mako_Program_Construct()
{   
    Mako_Program_Init();

    //R4.30 Added.
    Amp_Comp_Threshold = *Setting[e_AmpComp] * *Setting[e_AmpComp] * *Setting[e_AmpComp];

    //R3.00 Force strings to be "" since we trigger off of this in the LOAD routine.
    for (int t = 0; t < 200; t++) DefAmp_File[t] = "";
    Mako_ExternalData_Load(true);

    //R5.00 Calc Over Sampling filters.
    Filter_LP_Coeffs_4xOS(10000.0f, &fil_AmpOverSamp);
    Filter_LP_Coeffs_4xOS(10000.0f, &fil_DistOverSamp);

    switch (int(*Setting[e_AmpEQMode]))
    {
    case 0: Mako_EQ_Set_0(); break;
    case 1: Mako_EQ_Set_1(); break;
    case 2: Mako_EQ_Set_2(); break;
    case 3: Mako_EQ_Set_3(); break;
    case 4: Mako_EQ_Set_4(); break;
    case 5: Mako_EQ_Set_5(); break;
    case 6: Mako_EQ_Set_6(); break;
    case 7: Mako_EQ_Set_7(); break;
    case 8: Mako_EQ_Set_8(); break;
    case 9: Mako_EQ_Set_9(); break;
    case 10: Mako_EQ_Set_10(); break;
    case 11: Mako_EQ_Set_11(); break;
    case 12: Mako_EQ_Set_12(); break;
    case 13: Mako_EQ_Set_13(); break;
    case 14: Mako_EQ_Set_14(); break;
    case 15: Mako_EQ_Set_15(); break;
    case 16: Mako_EQ_Set_16(); break;
    case 17: Mako_EQ_Set_17(); break;
    case 18: Mako_EQ_Set_18(); break;
    case 19: Mako_EQ_Set_19(); break;
    }
    Mako_EQ_Set_Filters();

    Mako_FX_Setup_EQ(5);
        
    //R4.30 Get exposed IR data for drawing in the EDITOR.
    for (int t = 0; t < 350; t++)
    {
        DISP_IR[0][t] = 0.0f;
        DISP_IR[1][t] = TIR1[t];
        DISP_IR[2][t] = TIR2[t];
        DISP_IR[3][t] = TIR3[t];
        DISP_IR[4][t] = TIR4[t];
        DISP_IR[5][t] = TIR5[t];
        DISP_IR[6][t] = TIR6[t];
        DISP_IR[7][t] = TIR7[t];
        DISP_IR[8][t] = TIR8[t];
        DISP_IR[9][t] = TIR9[t];
        DISP_IR[10][t] = TIR10[t];
        DISP_IR[11][t] = TIR11[t];
        DISP_IR[12][t] = TIR12[t];
        DISP_IR[13][t] = TIR13[t];
        DISP_IR[14][t] = TIR14[t];
        DISP_IR[15][t] = TIR15[t];
        DISP_IR[16][t] = TIR16[t];
        DISP_IR[17][t] = TIR17[t];
        DISP_IR[18][t] = TIR18[t];
        DISP_IR[19][t] = TIR19[t];
    }
    

    makoEditorNeedsUpdated = 1;

    Mako_Program_Update(true);

    Mako_SettingsUpdate(false);    
}

//R4.00 Initialize some variables. 
void MakoDist01AudioProcessor::Mako_Program_Update(bool ForceUpdate)
{
    //R1.00 Get the SAMPLE RATE. Exit if unchanged since last run.
    if ((makoSampleRate == MakoDist01AudioProcessor::getSampleRate()) && (!ForceUpdate)) return;

    //R1.00 Get Sample Rate.
    makoSampleRate = MakoDist01AudioProcessor::getSampleRate();
    if (makoSampleRate < 21000) makoSampleRate = 48000;
    if (192000 < makoSampleRate) makoSampleRate = 48000;
    
    //R4.00 PreCalc some averaging ratios.
    //S = (5 * (1 / Ratio)) / SampRate
    RAT20ms = 1.0f / ((.02f * makoSampleRate) / 5.0f);  
    RATI20ms = 1.0f - RAT20ms;                          //.995 @ 48kHz 
    RAT50ms = 1.0f / ((.05f * makoSampleRate) / 5.0f);  
    RATI50ms = 1.0f - RAT50ms;                          //.997 @ 48kHz
    RAT100ms = 1.0f / ((.1f * makoSampleRate) / 5.0f);  
    RATI100ms = 1.0f - RAT100ms;                        //.9989 @ 48kHz
    RAT1000ms = 1.0f / ((1.0f * makoSampleRate) / 5.0f);
    RATI1000ms = 1.0f - RAT1000ms;                      //.99989 @ 48kHz

    //R1.00 REVERB MODULATE Vars.
    RevMod_B_Rate_Up = (1.0f / makoSampleRate) * 250.0f; //R5.00 RATE How fast we traverse thru the modulation indexes.
    RevMod_B_Rate_Down = -RevMod_B_Rate_Up;
    RevMod_B_Rate[0] = RevMod_B_Rate_Up;                 //R4.30 Starting rate.
    RevMod_B_Rate[1] = RevMod_B_Rate_Up;                 //R4.30 Starting rate.

    //R1.00 MODULATE Vars.
    Mod1_B_Rate_Up = (1.0f / makoSampleRate) * 500.0f; //R5.00 RATE How fast we traverse thru the modulation indexes.
    Mod1_B_Rate_Down = -Mod1_B_Rate_Up;
    Mod1_B_Rate[0] = Mod1_B_Rate_Up;           //R1.00 Starting rate.
    Mod1_B_Rate[1] = Mod1_B_Rate_Up;           //R1.00 Starting rate.
    
    //R1.00 MODULATE 2 Vars.
    Mod2_B_Rate_Up = 1.0f / makoSampleRate;    //R1.00 RATE How fast we traverse thru the modulation indexes.
    Mod2_B_Rate_Down = -1.0f / makoSampleRate;
    Mod2_B_Rate[0] = Mod2_B_Rate_Up;           //R1.00 Starting rate.
    Mod2_B_Rate[1] = Mod2_B_Rate_Up;           //R1.00 Starting rate.

    //R4.00 MODULATE 3 Vars.
    Mod3_B_Rate_Up = 1.0f / makoSampleRate;    //R1.00 RATE How fast we traverse thru the modulation indexes.
    Mod3_B_Rate_Down = -1.0f / makoSampleRate;
    Mod3_B_Rate[0] = Mod3_B_Rate_Up;           //R1.00 Starting rate.
    Mod3_B_Rate[1] = Mod3_B_Rate_Up;           //R1.00 Starting rate.
    
    //R4.00 MODULATE 4 Vars are not needed.
    
    //R2.00 Calculate the default filter coefficients.
    Filter_HP_Coeffs(20.0f, &fil_Out_HP);
    Filter_LP_Coeffs(1500.0f, &fil_Rev_LP);      
    Filter_HP_Coeffs(50.0f, &fil_Rev_HP);        
        
    //R2.00 Pre Calc our AMPLIFIER EQs.
    Filter_LP_Coeffs(*Setting[e_AmpLP], &fil_Amp_LP);
    Filter_HP_Coeffs(*Setting[e_AmpHP], &fil_Amp_LP);
    Filter_FO_LP_Coeffs(150.0f, &fil_Amp_Thump);   
    Filter_HP_Coeffs(1500.0f, &fil_Amp_Air);  
    if (*Setting[e_AmpCompF] != 0.0f) Filter_BP_Coeffs(0.0f, *Setting[e_AmpCompF], Amp_Comp_Q, &fil_Amp_Comp); //R4.30 AMP COMP/SAG
    //Filter_FO_HP_Coeffs(20.0f + ((1.0f - Setting_Last[e_AmpBoom]) * 1000.0f), &fil_Amp_Boom);    
    //Filter_FO_LP_Coeffs(500.0f + Setting_Last[e_AmpCrisp] * 5000.0f, &fil_Amp_Crisp);
    Filter_FO_HP_Coeffs(20.0f + ((1.0f - *Setting[e_AmpBoom]) * 1000.0f), &fil_Amp_Boom);
    Filter_FO_LP_Coeffs(500.0f + *Setting[e_AmpCrisp] * 5000.0f, &fil_Amp_Crisp);
        
    if (Amp_EQ1.Enabled) Filter_BP_Coeffs(Amp_EQ1.Gain * Amp_EQ1.RangedB, Amp_EQ1.Freq, Amp_EQ1.Q, &fil_Amp_EQ1);
    if (Amp_EQ2.Enabled) Filter_BP_Coeffs(Amp_EQ2.Gain * Amp_EQ2.RangedB, Amp_EQ2.Freq, Amp_EQ2.Q, &fil_Amp_EQ2);
    if (Amp_EQ3.Enabled) Filter_BP_Coeffs(Amp_EQ3.Gain * Amp_EQ3.RangedB, Amp_EQ3.Freq, Amp_EQ3.Q, &fil_Amp_EQ3);
    if (Amp_EQ4.Enabled) Filter_BP_Coeffs(Amp_EQ4.Gain * Amp_EQ4.RangedB, Amp_EQ4.Freq, Amp_EQ4.Q, &fil_Amp_EQ4);
    if (Amp_EQ5.Enabled) Filter_BP_Coeffs(Amp_EQ5.Gain * Amp_EQ5.RangedB, Amp_EQ5.Freq, Amp_EQ5.Q, &fil_Amp_EQ5);
    if (Amp_EQ6.Enabled) Filter_BP_Coeffs(Amp_EQ6.Gain * Amp_EQ6.RangedB, Amp_EQ6.Freq, Amp_EQ6.Q, &fil_Amp_EQ6);
   
    //R4.00 Setup EQ Pedal 
    Mako_FX_Setup_EQ(5);
}

//R5.00 Load an IR file. 
void MakoDist01AudioProcessor::FILE_Mako_IR_Load_Wave(juce::String tFile)
{
    float Vabs;
    float max = 0.0f;
    int maxIdx = 0;
    int maxSign = 1;
    int LIS = 0;

    //R5.00 Setup JUCE variables to read the WAVE file.
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::File jFile = juce::File::File(tFile);
    formatManager.registerBasicFormats();
    auto fileBuffer = juce::AudioBuffer < float >::AudioBuffer();
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(jFile));

    //R5.00 Read the WAVE file.
    if (reader.get() != nullptr)
    {
        //R5.00 Left here in case we want to check duration.
        //auto duration = (float)reader->lengthInSamples / reader->sampleRate;              
        //if (duration < 1)

        auto SampCount = (float)reader->lengthInSamples;
        if ((SampCount < 3000) && (reader->sampleRate < 50000))
        {
            fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
            reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, false);

            LIS = (int)reader->lengthInSamples;
            if (1024 < LIS) LIS = 1024;           //R2.18 Limit to 1024 samples.

            //R5.00 Get the max val in case we need to normalize the IR to 1.0 max.
            float tV;
            float VScale = 1.0f;
            for (int t = 0; t < LIS; t++)
            {
                tV = fileBuffer.getSample(0, t);
                Vabs = abs(tV);
                if (max < Vabs)
                {
                    max = Vabs;
                    maxIdx = t;

                    if (0.0f < tV)
                        maxSign = 1;
                    else
                        maxSign = -1;
                }
            }

            //R5.00 Check for IR scale requirements.
            if (1.0f < max)  VScale = 1.0f / max;

            //R5.00 Store the WAVE file data into our Temporary IR buffer.
            for (int t = 0; t < LIS; t++) IR_FromFile[t] = VScale * fileBuffer.getSample(0, t);
            for (int t = LIS; t < 1024; t++) IR_FromFile[t] = 0.0f;            
        }
        else
        {
            //R5.00 Handle File errors.
            ERR_Message1 = "ERR: Speaker IR file is invalid (1024 samples).";            
        }

    }
    else
    {
        //R5.00 Handle File errors.
        ERR_Message1 = "ERR: Speaker IR file is missing or invalid.";
    }
}

//R4.00 Load our last used In/Out IRs from the Windows USER data dir for this app.
void MakoDist01AudioProcessor::Mako_ExternalData_Load(bool ForceUpdate)
{
    //R4.00 Have issues in DAW where this file is not loaded when instantiated.
    if (DefAmp_File[0] == "") MakoExternalData_Loaded = false;

    //R4.00 Only load at the start of the application.
    if ((!ForceUpdate) && (MakoExternalData_Loaded)) return;
    MakoExternalData_Loaded = true;

    //R4.00 AMPLIFIER database.
    DefAmp_File[0] = "*No amplifier";
    DefAmp_File[1] = "*Assman"; DefAmp_GainSetting[1] = 0;      DefAmp_PreGain[1] = true;
    DefAmp_File[2] = "*Train"; DefAmp_GainSetting[2] = 0;       DefAmp_PreGain[2] = true;
    DefAmp_File[3] = "*Tweedle"; DefAmp_GainSetting[3] = 0;     DefAmp_PreGain[3] = true;
    DefAmp_File[4] = "*Divine 13"; DefAmp_GainSetting[4] = 1;   DefAmp_PreGain[4] = true;
    DefAmp_File[5] = "*CL30"; DefAmp_GainSetting[5] = 1;        DefAmp_PreGain[5] = true;
    DefAmp_File[6] = "*Matches"; DefAmp_GainSetting[6] = 1;     DefAmp_PreGain[6] = true;
    DefAmp_File[7] = "*Dr Zzzs"; DefAmp_GainSetting[7] = 1;     DefAmp_PreGain[7] = true;
    DefAmp_File[8] = "*Dumbo"; DefAmp_GainSetting[8] = 2;       DefAmp_PreGain[8] = true;
    DefAmp_File[9] = "*AC30c"; DefAmp_GainSetting[9] = 2;       DefAmp_PreGain[9] = true;
    DefAmp_File[10] = "*Brit J45"; DefAmp_GainSetting[10] = 2;  DefAmp_PreGain[10] = true;
    DefAmp_File[11] = "*England 15"; DefAmp_GainSetting[11] = 2;
    DefAmp_File[12] = "*JM War 10"; DefAmp_GainSetting[12] = 2;
    DefAmp_File[13] = "*Bogo Sheba"; DefAmp_GainSetting[13] = 2;
    DefAmp_File[14] = "*Orangutan"; DefAmp_GainSetting[14] = 2;
    DefAmp_File[15] = "*JoSox"; DefAmp_GainSetting[15] = 2;
    DefAmp_File[16] = "*Reverend Red "; DefAmp_GainSetting[16] = 2;
    DefAmp_File[17] = "*Crank It"; DefAmp_GainSetting[17] = 2;
    DefAmp_File[18] = "*515 III"; DefAmp_GainSetting[18] = 2;
    DefAmp_File[19] = "*Gold 800"; DefAmp_GainSetting[19] = 2;
    for (int t = 20; t < 200; t++)
    {
        DefAmp_GainSetting[t] = 2;
    }

    DefIR_File[0] = "*None";
    DefIR_File[1] = "*AssMan";
    DefIR_File[2] = "*Train";
    DefIR_File[3] = "*Tweedle";
    DefIR_File[4] = "*Divine 13";
    DefIR_File[5] = "*CL30";
    DefIR_File[6] = "*Matches";
    DefIR_File[7] = "*Dr Zzzs";
    DefIR_File[8] = "*DumbO";
    DefIR_File[9] = "*AC30c";
    DefIR_File[10] = "*Brit J45";
    DefIR_File[11] = "*England 15";
    DefIR_File[12] = "*JM War 10";
    DefIR_File[13] = "*BoGo Sheba";
    DefIR_File[14] = "*Orangutan";
    DefIR_File[15] = "*JoSoX";
    DefIR_File[16] = "*Reverend Red";
    DefIR_File[17] = "*Crank It";
    DefIR_File[18] = "*515 III";
    DefIR_File[19] = "*Gold 800";
    for (int t = 20; t < 200; t++) DefIR_File[20] = "";

    //R4.00 Read in the Amp and Speaker IR database.
    juce::File f = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName() + "\\MakoMiniD\\ExternalData.txt";

    if (f.existsAsFile())
    {
        int Frev = 0;
        bool FileValid = false;
        juce::FileInputStream Input(f);
        {
            Input.setPosition(0);

            //R4.00 Get the file revision.
            auto b = Input.readNextLine();        
            if (b == "MINID_SAVE_400") Frev = 400;

            //R4.00 If we have a valid file, read the data.
            if (Frev < 400)
                FileValid = false;
            else
            {                
                b = Input.readNextLine();    //R4.00 Do not edit this file notice.

                //R4.00 Read in the AMPLIFIER DATABASE.
                if (399 < Frev)
                {                    
                    b = Input.readNextLine();      //R4.00 AMP header.
                    for (int t = 0; t < 200; t++)
                    {
                        b = Input.readNextLine();
                        if (19 < t)
                        {
                            DefAmp_Path[t] = b;
                            DefAmp_File[t] = DefAmp_Path[t].fromLastOccurrenceOf("\\", false, true);
                        }
                        
                        b = Input.readNextLine();
                        if (19 < t) DefAmp_GainSetting[t] = Clip_Int(b.getFloatValue(), 0, 2);
                        
                        b = Input.readNextLine();
                        if (19 < t)
                        {
                            DefAmp_PreGain[t] = false;
                            if (Clip_Int(b.getFloatValue(), 0, 1))DefAmp_PreGain[t] = true;
                        }
                    }
                    b = Input.readNextLine();
                }

                //R4.00 Read in the IR DATABASE.
                if (399 < Frev)
                {
                    b = Input.readNextLine(); //R3.00 IR header.
                    for (int t = 0; t < 200; t++)
                    {
                        b = Input.readNextLine();
                        if (19 < t)
                        {
                            DefIR_Path[t] = b;
                            DefIR_File[t] = DefIR_Path[t].fromLastOccurrenceOf("\\", false, true);
                        }
                    }
                    b = Input.readNextLine();

                }

                //R4.00 We have successfully loaded the file.
                FileValid = true;
            }
        }
                
    }

    return;
}

//R4.00 Save the Amp and IR database file.
void MakoDist01AudioProcessor::Mako_ExternalData_Save()
{   
    //R2.23 We need to create a data directory to save our large STRING data. Strings are NOT suitable for host parameters (bool,float).
    juce::File fDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName() + "\\MakoMiniD";
    juce::Result tRes(fDir.createDirectory());

    //R2.23 Did our directory get created?
    if (!fDir.isDirectory()) return;

    //R2.23 We have a working data directory to store data, save data.
    juce::File f = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName() + "\\MakoMiniD\\ExternalData.txt";
    juce::FileOutputStream output(f);
    {
        //R4.00 Over write any data.
        output.setPosition(0);
        output.truncate();

        //R4.00 Mako IR PATHS file, version 0.
        output.writeText("MINID_SAVE_400\r\n", false, false, nullptr);
        output.writeText("DO NOT EDIT THIS FILE - Initial IR and WAVE file data that is not stored in VST data.\r\n", false, false, nullptr);

        //R4.00 Store Amplifier List.
        output.writeText(std::string("AMPLIFIER DATABASE") + "\r\n", false, false, nullptr);
        for (int t = 0; t < 200; t++)
        {
            output.writeText(DefAmp_Path[t] + "\r\n", false, false, nullptr);
            output.writeText(std::to_string(DefAmp_GainSetting[t]) + "\r\n", false, false, nullptr);
            output.writeText(std::to_string(DefAmp_PreGain[t]) + "\r\n", false, false, nullptr);
        }
        output.writeText("\r\n", false, false, nullptr);

        //R4.00 Store Speaker List.
        output.writeText(std::string("IR DATABASE") + "\r\n", false, false, nullptr);
        for (int t = 0; t < 200; t++)
        {
            output.writeText(DefIR_Path[t] + "\r\n", false, false, nullptr);
        }
        output.writeText("\r\n", false, false, nullptr);
       
    }
}

//R4.00 Tool to limit a float value to a certain range of numbers.
float MakoDist01AudioProcessor::Clip_Float(float val, float min, float max)
{
    float val2 = val;
    if (val < min)
        val2 = min;
    else if (max < val)
        val2 = max;

    return val2;
}

//R4.00 Tool to limit an integer value to a certain range of numbers.
int MakoDist01AudioProcessor::Clip_Int(int val, int min, int max)
{
    int val2 = val;
    if (val < min)
        val2 = min;
    else if (max < val)
        val2 = max;

    return val2;
}

//R4.00 Set our current Amp IR to a preprogrammed Amp IR or a user defined IR from InputEQ_FromFile.
void MakoDist01AudioProcessor::Mako_Amp_Set()
{
    int Idx = int(*Setting[e_AmpChan]);

    if (Idx < 20)
    {
        //R3.00 Load the IR array with the selected Bult-in amp or file data.
        switch (Idx)
        {
        case 0: //R2.22 Not used. Code here just in case to catch a bug.        
            for (int t = 0; t < 1024; t++) InputEQ[t] = 0.0f;
            InputEQ[0] = .9f;
            InputEQ_IRGain = .9f;
            InputEQ_GainRange = 20.0f;
            InputEQ_PreEQ = false;
            break;
        case 1://R4.00 Assman
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_Assman[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 400.0f;
            InputEQ_PreEQ = true;
            break;
        case 2://R4.00 Trained
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_Train[t];
            InputEQ_IRGain = .05f;
            InputEQ_GainRange = 2000.0f;
            InputEQ_PreEQ = true;
            break;
        case 3://R4.00 Tweedle
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_Tweedle[t];
            InputEQ_IRGain = .05f;
            InputEQ_GainRange = 2000.0f;
            InputEQ_PreEQ = true;
            break;
        case 4://R4.00 Divine
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_Div13[t];
            InputEQ_IRGain = .05f;
            InputEQ_GainRange = 10000.0f;
            InputEQ_PreEQ = true;
            break;
        case 5://R4.00 CL30
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_CL30[t];
            InputEQ_IRGain = .05f;
            InputEQ_GainRange = 5000.0f;
            InputEQ_PreEQ = true;
            break;
        case 6://R4.00 Matches
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_Matches[t];
            InputEQ_IRGain = .05f;
            InputEQ_GainRange = 10000.0f;
            InputEQ_PreEQ = true;
            break;
        case 7://R4.00 Dr Zzzs
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_DrZzzs[t];
            InputEQ_IRGain = .04f;
            InputEQ_GainRange = 20000.0f;
            InputEQ_PreEQ = true;
            break;
        case 8://R4.00 Dumbo
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_Dumbo[t];
            InputEQ_IRGain = .04f;
            InputEQ_GainRange = 50000.0f;
            InputEQ_PreEQ = true;
            break;
        case 9://R4.00 AC30c
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_AC30c[t];
            InputEQ_IRGain = .04f;
            InputEQ_GainRange = 20000.0f;
            InputEQ_PreEQ = true;
            break;
        case 10://R4.00 BritJ45
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_AC30c[t];
            InputEQ_IRGain = .04f;
            InputEQ_GainRange = 50000.0f;
            InputEQ_PreEQ = true;
            break;
        case 11://R4.00 England 15
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_England15[t];
            InputEQ_IRGain = .04f;
            InputEQ_GainRange = 20000.0f;
            InputEQ_PreEQ = true;
            break;
        case 12://R4.00 JM War 10
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_JMWar10[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 1000.0f;
            InputEQ_PreEQ = false;
            break;
        case 13://R4.00 Bogo Sheba
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_BogoSheba[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 1000.0f;
            InputEQ_PreEQ = false;
            break;
        case 14://R4.00 Bogo Orangutan
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_Orangutan[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 1000.0f;
            InputEQ_PreEQ = false;
            break;
        case 15: //R4.00 JoSox
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_JoSoX[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 1000.0f;
            InputEQ_PreEQ = false;
            break;
        case 16: //R4.00 Reverend Red
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_RevRed[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 1000.0f;
            InputEQ_PreEQ = false;
            break;
        case 17: //R4.00 CrankIt
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_CrankIt[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 1000.0f;
            InputEQ_PreEQ = false;
            break;
        case 18: //R4.00 5153
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_515III[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 1000.0f;
            InputEQ_PreEQ = false;
            break;
        case 19: //R5.00 Gold 800
            for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_Gold800[t];
            InputEQ_IRGain = .3f;
            InputEQ_GainRange = 5000.0f;
            InputEQ_PreEQ = false;
            break;        
        }
    }
    else
    {
        //R5.00 Load a file IR from a file.
        FILE_Mako_Amp_Load_Wave(DefAmp_Path[Idx]);
        for (int t = 0; t < 1024; t++) InputEQ[t] = InputEQ_FromFile[t];

        //R5.00 Setup some vars based on PRE AMP or POST AMP option.
        InputEQ_PreEQ = DefAmp_PreGain[Idx];
        if (InputEQ_PreEQ)
        {
            switch (int(DefAmp_GainSetting[Idx]))
            {
            case 0: InputEQ_GainRange = 200; InputEQ_IRGain = .8f; break;
            case 1: InputEQ_GainRange = 40000; InputEQ_IRGain = .04f; break;
            case 2: InputEQ_GainRange = 400000; InputEQ_IRGain = .02f; break;
            }
            InputEQ_IRGain = InputEQ_IRGain * (1.0f - (InputEQ_VOffset * .01f));
        }
        else
        {
            switch (int(DefAmp_GainSetting[Idx]))
            {
            case 0: InputEQ_GainRange = 20; InputEQ_IRGain = .7f; break;
            case 1: InputEQ_GainRange = 500; InputEQ_IRGain = .3f; break;
            case 2: InputEQ_GainRange = 5000; InputEQ_IRGain = .2f; break;
            }
        }
        
    }

    //R4.00 Clear the IR buffers or we get clicks.
    for (int t = 0; t < 1024; t++)
    {
        InEQ_AudioBuffer[0][t] = 0.0f;
        InEQ_AudioBuffer[1][t] = 0.0f;
    }

    //R4.00 Flag the editor it needs to update the PRE EQ indicator.
    makoEditorNeedsUpdated = 1;
}


//R5.00 Load an 1024 sample IR wave file and store in InputEQ_FromFile.
void MakoDist01AudioProcessor::FILE_Mako_Amp_Load_Wave(juce::String tFile)
{
    float max = 0.0f;
    int maxIdx = 0;
    int LIS = 0;

    //R5.00 Setup JUCE variables to read the WAVE file.
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::File jFile = juce::File::File(tFile);
    formatManager.registerBasicFormats();
    auto fileBuffer = juce::AudioBuffer < float >::AudioBuffer();
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(jFile));

    //R5.00 Read the WAVE file.
    if (reader.get() != nullptr)
    {
        auto SampCount = (float)reader->lengthInSamples;
        if ((SampCount < 3000) && (reader->sampleRate < 50000))
        {
            fileBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);
            reader->read(&fileBuffer, 0, (int)reader->lengthInSamples, 0, true, false);

            LIS = (int)reader->lengthInSamples;
            if (1024 < LIS) LIS = 1024;           //R5.00 Limit to 1024 samples.

            //R5.00 Load data up to our end point, fill 0s after that.
            for (int t = 0; t < LIS; t++) InputEQ_FromFile[t] = fileBuffer.getSample(0, t);
            for (int t = LIS; t < 1024; t++) InputEQ_FromFile[t] = 0.0f;

            float Avg = 0.0f;
            for (int t = 0; t < 1024; t++) Avg += abs(InputEQ_FromFile[t]);
            InputEQ_VOffset = Avg;
        }
        else
        {
            //R5.00 Handle File size issues.
            ERR_Message1 = "ERR: Invalid Amp IR file (1024 samples).";
        }
    }
    else
    {
        //R5.00 Handle File errors.
        ERR_Message1 = "ERR: Amp IR file is missing or invalid.";
    }
}

//R4.00 Initialize the Mod1 Flanger effect.
void MakoDist01AudioProcessor::Mako_FX_Setup_Mod1()
{        
    Mod1_B_Offset_Min[0] = .001f * makoSampleRate;                  //R4.10 Set to 1 mS delay.
    Mod1_B_Offset_Max[0] = 20 + int(750.0f * *Setting[e_Mod1Depth]);
}

//R4.00 Initialize the Mod2 Mod Filter effect.
void MakoDist01AudioProcessor::Mako_FX_Setup_Mod2()
{
    Mod2_Wave_Ramp[0] = Mod2_B_Rate_Up * *Setting[e_Mod2Rate] * 10.0f;
    Mod2_Wave_Ramp[1] = Mod2_Wave_Ramp[0];
}

//R4.00 Initialize the Mod3 Chorus effect.
void MakoDist01AudioProcessor::Mako_FX_Setup_Mod3()
{
    int Baseline = .015f * makoSampleRate; //R4.10 15 mS
    Mod3_B_Rate[0] = Mod3_B_Rate_Up;
    Mod3_B_Rate[1] = Mod3_B_Rate_Up;
    Mod3_B_Offset_Min[0] = Baseline - 10.0f - (*Setting[e_Mod3Depth] * Baseline * .9f);
    Mod3_B_Offset_Min[1] = Mod3_B_Offset_Min[0];
    Mod3_B_Offset_Max[0] = Baseline + 10.0f + (*Setting[e_Mod3Depth] * Baseline * .9f);  //R4.00 Chorus buffer End position
    Mod3_B_Offset_Max[1] = Mod3_B_Offset_Max[0];
    
    //R5.00 Verify our Min,Max Modulation limits are within the buffer size.
    //R5.00 Should not be needed since our buffer is 2048 samples.
    for (int t = 0; t < 2; t++)
    {
        if (Mod3_B_Offset_Min[t] < 0) Mod3_B_Offset_Min[t] = 0;
        if (Mod3_B_Ring1_Max[t] < Mod3_B_Offset_Max[t]) Mod3_B_Offset_Max[t] = Mod3_B_Ring1_Max[t];
    }
    Mod3_B_Offset[0] = Mod3_B_Offset_Min[0];
    Mod3_B_Offset[1] = Baseline;  //R4.00 Spread out offsets for Stereo effect.


    //R4.00 Stereo effect so adjust balance.
    if (*Setting[e_Mod3Bal] < .5f)
    {
        Mod3_BalLR[0] = 1.0f;
        Mod3_BalLR[1] = *Setting[e_Mod3Bal] * 2;
    }
    else
    {
        Mod3_BalLR[0] = 1.0f - ((*Setting[e_Mod3Bal] - .5f) * 2.0f);
        Mod3_BalLR[1] = 1.0f;
    }
}

//R4.00 Initialize the Mod4 Wide Load effect.
void MakoDist01AudioProcessor::Mako_FX_Setup_Mod4()
{
    //R4.00 Apply FREQ Offset by making a stereo Mid F adjustment.
    Filter_BP_Coeffs((*Setting[e_Mod4Foff]) * 6.0f, 500.0f, .35f, &filMod4[0]);
    Filter_BP_Coeffs((0.0f - *Setting[e_Mod4Foff]) * 6.0f, 500.0f, .35f, &filMod4[1]);

    //R4.10 
    Mod4_Depth[0] = abs(*Setting[e_Mod4Doff]) * (4096 - 1);
    Mod4_Depth[1] = Mod4_Depth[1];

    //R4.00 Stereo effect so adjust balance.
    if (*Setting[e_Mod4Bal] < .5f)
    {
        Mod4_BalLR[0] = 1.0f;
        Mod4_BalLR[1] = *Setting[e_Mod4Bal] * 2;
    }
    else
    {
        Mod4_BalLR[0] = 1.0f - ((*Setting[e_Mod4Bal] - .5f) * 2.0f);
        Mod4_BalLR[1] = 1.0f;
    }
}


// ORIGINAL
//R4.00 The heart of MD2. This func applies an IR to incoming samples that represents
//R4.00 the amplifier preamp EQ. It also applies gain, thinning, etc.
float MakoDist01AudioProcessor::Mako_Amp_InputIR(float tSample, int channel)
{
    int Index;
    int BitMask = Amp_Qual_Max - 1;
    float V = 0.0f;
    float tS = tSample; 
    
    //R4.00 Get our current buffer index and store the new sample in our buffer.
    Index = InEQB_Ring1[channel];
    InEQ_AudioBuffer[channel][Index] = tS;

    //R2.22 Apply input EQ IR to the incoming sample.
    for (int t = 0; t < Amp_Qual_Max; t++)
    {
        V += (InEQ_AudioBuffer[channel][Index] * InputEQ[t]);

        //R1.00 Increment index, mask off bits past 1023 to keep our index between 0-1023.
        Index = (Index + 1) & BitMask;
    }

    //R1.00 Decrement our ring buffer index and loop around at 0. 
    InEQB_Ring1[channel]--;
    if (InEQB_Ring1[channel] < 0) InEQB_Ring1[channel] = BitMask;
        
    //R2.00 Apply Drive.
    if (0.0f < *Setting[e_AmpGain])
    {
        float V2;
        float AG = *Setting[e_AmpGain] * *Setting[e_AmpGain];

        //R4.00 Apply the Hard clipping gain.
        V2 = V * (1.0f + (AG * InputEQ_GainRange));
                
        //R4.00 Hard Clipping.
        if (.9999f < V2)
            V2 = .9999f;
        else if (V2 < -.9999f)
            V2 = -.9999f;

        //R5.00 Save CPU cycles if not using SOFT slope. 
        if (0.0f < *Setting[e_AmpSlope])
        {
            //R4.00 Apply Soft clipping.
            V = tanhf(V * (1.0f + (AG * InputEQ_GainRange)));

            //R4.00 MIX Hard and Soft clipping styles.
            V = ((1.0f - *Setting[e_AmpSlope]) * V2) + (*Setting[e_AmpSlope] * V);
        }
        else
            V = V2;
    }

    return V * InputEQ_IRGain;    
}


//R4.20 4x OVER SAMPLING version of amplifier, reduces aliasing.
//R4.20 The heart of MD2. This func applies an IR to incoming samples that represents
//R4.20 the amplifier preamp EQ. It also applies gain, thinning, etc.
float MakoDist01AudioProcessor::Mako_Amp_InputIR_4x(float tSample, int channel)
{
    int Index;
    int BitMask = Amp_Qual_Max - 1;
    float V = 0.0f;
    float tS = tSample;
    float tS_PostIR;
    
    //R4.00 Get our current buffer index and store the new sample in our buffer.
    Index = InEQB_Ring1[channel];
    InEQ_AudioBuffer[channel][Index] = tS;

    //R2.22 Apply input EQ IR to the incoming sample.
    for (int t = 0; t < Amp_Qual_Max; t++)
    {
        V += (InEQ_AudioBuffer[channel][Index] * InputEQ[t]);

        //R1.00 Increment index, mask off bits past 1023 to keep our index between 0-1023.
        Index = (Index + 1) & BitMask;
    }
    tS_PostIR = V; //R4.20 Store for LastSample.

    //R1.00 Decrement our ring buffer index and loop around at 0. 
    InEQB_Ring1[channel]--;
    if (InEQB_Ring1[channel] < 0) InEQB_Ring1[channel] = BitMask;

    //R2.00 Apply Drive.
    if (0.0f < *Setting[e_AmpGain])
    {
        float V2;
        float AG = *Setting[e_AmpGain] * *Setting[e_AmpGain];
        float tvDelta = (V - AmpSample_Last) / 4.0f;

        for (int SampX = 0; SampX < 4; SampX++)
        {
            //R4.20 Need to interpolate between the samples here!!!
            V = AmpSample_Last + (tvDelta * SampX);

            //R4.00 Apply the Hard clipping gain.
            V2 = V * (1.0f + (AG * InputEQ_GainRange));
          
            //R4.00 Hard Clipping.
            if (.9999f < V2)
                V2 = .9999f;
            else if (V2 < -.9999f)
                V2 = -.9999f;

            //R5.00 Save CPU cycles if not using SOFT slope.
            if (0.0f < *Setting[e_AmpSlope])
            {
                //R4.00 Apply Soft clipping.
                V = tanhf(V * (1.0f + (AG * InputEQ_GainRange)));

                //R4.00 MIX Hard and Soft clipping styles.
                V = ((1.0f - *Setting[e_AmpSlope]) * V2) + (*Setting[e_AmpSlope] * V);
            }
            else
                V = V2;

            //R4.20 4x Over Sampling filter.
            V = Filter_Calc_BiQuad(V, channel, &fil_AmpOverSamp);
        }
    }

    AmpSample_Last = tS_PostIR;

    return V * InputEQ_IRGain;
}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MakoDist01AudioProcessor();
}

//R4.00 Slightly optimized filter coefficients code for WAH since it gets called constantly.
void MakoDist01AudioProcessor::Mako_Calc_coeffs_Wah(float Q, float fc, int chan)
{    
    float K = pi2 * (fc * .5f) / makoSampleRate;
    float K2 = K * K;
    float V0 = 32.0f; // LinearGain_20dB;
    
    float dd = 1.0f / (1.0f + K / Q + K2);

    WAH_coeffs[chan].a0 = (1.0f + (V0 * K) / Q + K2) * dd;   //a
    WAH_coeffs[chan].a1 = (2.0f * (K2 - 1.0f)) * dd;         //b  
    WAH_coeffs[chan].a2 = (1.0f - (V0 * K) / Q + K2) * dd;   //g
    WAH_coeffs[chan].b1 = WAH_coeffs[chan].a1;               //b
    WAH_coeffs[chan].b2 = (1.0f - K / Q + K2) * dd;          //d
    //WAH_coeffs.c0 = 1.0f;
    //WAH_coeffs.d0 = 0.0f;
}

//R4.00 Slightly optimized filter coefficients code for Modulated Filter (MWAH) since it gets called constantly.
void MakoDist01AudioProcessor::Mako_Calc_coeffs_MWah(float Q, float fc, int chan)
{
    //R4.00 This filter code is slightly optimized for speed since it gets called constantly.
    float K = pi2 * (fc * .5f) / makoSampleRate;
    float K2 = K * K;
    float V0 = 32.0f; // LinearGain_20dB;

    float dd = 1.0f / (1.0f + K / Q + K2);

    MWAH_coeffs[chan].a0 = (1.0f + (V0 * K) / Q + K2) * dd;   //a
    MWAH_coeffs[chan].a1 = (2.0f * (K2 - 1.0f)) * dd;         //b  
    MWAH_coeffs[chan].a2 = (1.0f - (V0 * K) / Q + K2) * dd;   //g
    MWAH_coeffs[chan].b1 = MWAH_coeffs[chan].a1;               //b
    MWAH_coeffs[chan].b2 = (1.0f - K / Q + K2) * dd;          //d
    //MWAH_coeffs.c0 = 1.0f;
    //MWAH_coeffs.d0 = 0.0f;
}


//***************************************************************************************************************
//R4.00 GENERIC FILTER FUNCTIONS
//***************************************************************************************************************
//R4.00 This func applies the filter to a sample using coefficients calculated else where.
float MakoDist01AudioProcessor::Filter_Calc_BiQuad(float tSample, int channel, tp_filter* fn)
{
    float tS = tSample;

    fn->xn0[channel] = tS;
    tS = fn->a0 * fn->xn0[channel] + fn->a1 * fn->xn1[channel] + fn->a2 * fn->xn2[channel] - fn->b1 * fn->yn1[channel] - fn->b2 * fn->yn2[channel];
    fn->xn2[channel] = fn->xn1[channel]; fn->xn1[channel] = fn->xn0[channel]; fn->yn2[channel] = fn->yn1[channel]; fn->yn1[channel] = tS;

    return tS;
}

//R4.00 This version reduces calcs for when the filter is not being applied but the buffers should still be updated.
float MakoDist01AudioProcessor::Filter_Calc_BiQuad_Bypass(bool DoCalcs, float tSample, int channel, tp_filter* fn)
{
    float tS = tSample;

    fn->xn0[channel] = tS;
    if (DoCalcs) tS = fn->a0 * fn->xn0[channel] + fn->a1 * fn->xn1[channel] + fn->a2 * fn->xn2[channel] - fn->b1 * fn->yn1[channel] - fn->b2 * fn->yn2[channel];
    fn->xn2[channel] = fn->xn1[channel]; fn->xn1[channel] = fn->xn0[channel]; fn->yn2[channel] = fn->yn1[channel]; fn->yn1[channel] = tS;

    return tS;
}

//R4.00 Second order parametric/peaking boost filter with constant-Q
void MakoDist01AudioProcessor::Filter_BP_Coeffs(float Gain_dB, float Fc, float Q, tp_filter* fn)
{
    float K = pi2 * (Fc * .5f) / makoSampleRate;
    fn->K = K;
    float K2 = K * K;
    float V0 = pow(10.0, Gain_dB / 20.0);
    float GF = (V0 * K) / Q;
    float KQ = K / Q;

    float a = (1.0f + GF) + K2;
    float b = 2.0f * (K2 - 1.0f);
    float g = (1.0f - GF) + K2;
    float d = (1.0f - KQ) + K2;
    float dd = 1.0f / (1.0f + KQ + K2);

    fn->a0 = a * dd;
    fn->a1 = b * dd;
    fn->a2 = g * dd;
    fn->b1 = b * dd;
    fn->b2 = d * dd;
    fn->c0 = 1.0f;
    fn->d0 = 0.0f;
}

void MakoDist01AudioProcessor::Filter_BP_Coeffs_AmpSag(float Gain, tp_filter* fn)
{
    //R4.00 Second order parametric/peaking boost filter with constant-Q
    float K2 = fn->K * fn->K;
    float GF = (Gain * fn->K) / Amp_Comp_Q; //Q = .25f
    float KQ = fn->K / Amp_Comp_Q;

    float dd = 1.0f / (1.0f + KQ + K2);

    fn->a0 = ((1.0f + GF) + K2) * dd;
    fn->a2 = ((1.0f - GF) + K2) * dd;
}

//R4.00 Optimized for speed when being called continuously.
void MakoDist01AudioProcessor::Filter_BP_Coeffs_GainOnly(float Gain_dB, float Fc, float Q, tp_filter* fn)
{
    //R4.00 Second order parametric/peaking boost filter with constant-Q
    float K2 = fn->K * fn->K;
    float V0 = pow(10.0, Gain_dB / 20.0);
    float GF = (V0 * fn->K) / Q;
    float KQ = fn->K / Q;

    float a = (1.0f + GF) + K2;
    float g = (1.0f - GF) + K2;
    float dd = 1.0f / (1.0f + KQ + K2);

    fn->a0 = a * dd;
    fn->a2 = g * dd;    
}

//R4.00 Second order LOW PASS filter. 
void MakoDist01AudioProcessor::Filter_LP_Coeffs(float fc, tp_filter* fn)
{ 
    float c = 1.0f / (tanf(pi * fc / makoSampleRate));
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = 2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * (1.0f - (c * c));
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}

//R4.20 Second order LOW PASS filter. 
void MakoDist01AudioProcessor::Filter_LP_Coeffs_4xOS(float fc, tp_filter* fn)
{
    float c = 1.0f / (tanf(pi * fc / (makoSampleRate * 4)));     //R4.20 4x sample Rate.
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = 2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * (1.0f - (c * c));
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}


//F4.00 Second order butterworth High Pass.
void MakoDist01AudioProcessor::Filter_HP_Coeffs(float fc, tp_filter* fn)
{    
    float c = tanf(pi * fc / makoSampleRate);
    fn->a0 = 1.0f / (1.0f + sqrt2 * c + (c * c));
    fn->a1 = -2.0f * fn->a0;
    fn->a2 = fn->a0;
    fn->b1 = 2.0f * fn->a0 * ((c * c) - 1.0f);
    fn->b2 = fn->a0 * (1.0f - sqrt2 * c + (c * c));
}

//R4.00 Shelfs do not work in normal fashion.
void MakoDist01AudioProcessor::Filter_Shelf_HP_Coeffs(float Gain_dB, float fc, tp_filter* fn)
{
    float th = 2.0 * pi * fc / makoSampleRate;
    float m = pow(10.0, Gain_dB / 20.0);
    float b = (1.0 + m) / 4.0;
    float d = b * tan(th / 2.0);
    float g = (1.0 - d) / (1.0 + d);
    fn->a0 = (1.0 + g) / 2.0;
    fn->a1 = -((1.0 + g) / 2.0);
    fn->a2 = 0.0;
    fn->b1 = -g;
    fn->b2 = 0.0;
    fn->c0 = m - 1.0;
    fn->d0 = 1.0;    
}

//R4.00 Shelfs do not work in normal fashion.
void MakoDist01AudioProcessor::Filter_Shelf_LP_Coeffs(float Gain_dB, float fc, tp_filter* fn)
{
    float th = 2.0 * pi * fc / makoSampleRate;
    float m = pow(10.0, Gain_dB / 20.0);
    float b = 4.0 / (1.0 + m);
    float d = b * tan(th / 2.0);
    float g = (1.0 - d) / (1.0 + d);
    fn->a0 = (1.0 - g) / 2.0;
    fn->a1 = (1.0 - g) / 2.0;
    fn->a2 = 0.0;
    fn->b1 = -g;
    fn->b2 = 0.0;
    fn->c0 = m - 1.0;
    fn->d0 = 1.0;    
}

//R4.00 First Order High Pass.
void MakoDist01AudioProcessor::Filter_FO_HP_Coeffs(float fc, tp_filter* fn)
{
        float th = 2.0 * pi * fc / makoSampleRate;
        float g = cos(th) / (1.0 + sin(th));
        fn->a0 = (1.0 + g) / 2.0;
        fn->a1 = -((1.0 + g) / 2.0);
        fn->a2 = 0.0f;
        fn->b1 = -g;
        fn->b2 = 0.0f;
        fn->c0 = 0.0f;
        fn->d0 = 0.0f;
}

//R4.00 First Order Low Pass.
void MakoDist01AudioProcessor::Filter_FO_LP_Coeffs(float fc, tp_filter* fn)
{
    float th = 2.0 * pi * fc / makoSampleRate;
    float g = cos(th) / (1.0 + sin(th));
    fn->a0 = (1.0 - g) / 2.0;
    fn->a1 = (1.0 - g) / 2.0;
    fn->a2 = 0.0;
    fn->b1 = -g;
    fn->b2 = 0.0;
    fn->c0 = 0.0f;
    fn->d0 = 0.0f;
}

//*****************************************************************
// F X   S E T T I N G   P R O C E S S I N G
//*****************************************************************
//R4.00 Pedal 1 - NOISE GATE limit calculations.
void MakoDist01AudioProcessor::Mako_FX_Setup_NoiseGate(int pos)
{
    //R4.00 Precalc some LIMITs to speed up processing.
    makoPedal[pos].Var3[0] = *Setting[e_GateAvg] * *Setting[e_GateAvg] * *Setting[e_GateAvg];     //R4.00 AVG Limit.  
    makoPedal[pos].Var4[0] = *Setting[e_GateFreq] * *Setting[e_GateFreq] * *Setting[e_GateFreq];  //R4.00 Freq Limit.
    makoPedal[pos].Var7[0] = 100.0f + (*Setting[e_GateWahRange] * 50000.0f);                      //R4.00 Range.
}

//R4.00 Pedal 2 - COMPRESSOR frequency definitions.
void MakoDist01AudioProcessor::Mako_FX_Setup_Compressor(int pos)
{
    //R4.00 Calc the THRESHOLD limit.
    makoPedal[pos].Var3[0] = *Setting[e_CompThr] * *Setting[e_CompThr] * *Setting[e_CompThr];

    //R4.00 Future Use.
    //Filter_FO_LP_Coeffs(50.0f + (*Setting[id + 9] * 2000.0f), &makoPedal[pos].Filter1);
    //Filter_FO_HP_Coeffs(50.0f + (*Setting[id + 10] * 5000.0f), &makoPedal[pos].Filter2);
}

//R4.00 Pedal 3 - DISTORTION 1 frequency definitions.
void MakoDist01AudioProcessor::Mako_FX_Setup_Distortion1(int pos)
{
    //R4.20 Changed Mid filter gain.
    Filter_BP_Coeffs((-.5f + *Setting[e_Dist1Mid]) * 30.0f, *Setting[e_Dist1MidF], *Setting[e_Dist1MidQ], &makoPedal[pos].Filter1);
    Filter_LP_Coeffs(*Setting[e_Dist1Cut], &makoPedal[pos].Filter2);
    //Filter_FO_HP_Coeffs(250.0f, &makoPedal[pos].Filter3);
}

//R4.00 Pedal 4 - DISTORTION 2 frequency definitions.
void MakoDist01AudioProcessor::Mako_FX_Setup_Distortion2(int pos)
{
    //R4.20 Changed Mid filter gain.
    Filter_BP_Coeffs((-.5f + *Setting[e_Dist2Mid]) * 30.0f, *Setting[e_Dist2MidF], *Setting[e_Dist2MidQ], &makoPedal[pos].Filter1);
    Filter_LP_Coeffs(*Setting[e_Dist2Cut], &makoPedal[pos].Filter2);
    Filter_BP_Coeffs(*Setting[e_Dist2High] * 15.0f, 1500.0f, .35f, &makoPedal[pos].Filter3);
}

//R4.00 Pedal 5 - STUDIO EQ frequency definitions.
void MakoDist01AudioProcessor::Mako_FX_Setup_EQ(int pos)
{
    Filter_BP_Coeffs(*Setting[e_EQF1A], *Setting[e_EQF1F], *Setting[e_EQF1Q], &makoPedal[pos].Filter1); //R4.00 Bass
    Filter_BP_Coeffs(*Setting[e_EQF2A], *Setting[e_EQF2F], *Setting[e_EQF2Q], &makoPedal[pos].Filter2); //R4.00 Low Mid
    Filter_BP_Coeffs(*Setting[e_EQF3A], *Setting[e_EQF3F], *Setting[e_EQF3Q], &makoPedal[pos].Filter3); //R4.00 Low Mid
    Filter_BP_Coeffs(*Setting[e_EQF4A], *Setting[e_EQF4F], *Setting[e_EQF4Q], &makoPedal[pos].Filter4); //R4.00 Mid 2 
}

//*****************************************************************
// FX   A U D I O   P R O C E S S I N G
//*****************************************************************
//R4.00 Pedal 1 - Apply the triple NOISE GATE and AUTOWAH effects to the incoming sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_NoiseGate(float tSample, int channel, int pos)
{
    float tS = tSample;
    float Gain;
    
    //R5.00 Apply Downward expander style gate.
    if (*Setting[e_GateExp])
    {
        float tS_Sqaured = tS * tS;
        if (tS < 0.0f) tS_Sqaured = -tS_Sqaured;

        //R5.00 Mix between normal and squared values to reduce low amplitude values.
        tS = (tS * (1.0f - *Setting[e_GateExp])) + (tS_Sqaured * *Setting[e_GateExp]);
    }
        
    //R4.00 Apply Average volume gate.
    makoPedal[pos].Var1[channel] = (makoPedal[pos].Var1[channel] * RATI50ms) + (abs(tS) * RAT50ms);

    //R4.00 AutoWAH.
    //if (0.001f < Setting[e_GateWah]) tS = Mako_FX_Audio_AutoWah_Avg(tS, makoPedal[idx].Var1[channel], channel);
    if (0.001f < *Setting[e_GateWah]) tS = Mako_FX_Audio_AutoWah_Peak(tS, channel, 1);

    //R4.00 FREQUENCY filter gate. Var4 is modified Parameter setting.
    if (makoPedal[pos].Var1[channel] < makoPedal[pos].Var4[0])
    {
        Gain = 1.0f - ((makoPedal[pos].Var4[0] - makoPedal[pos].Var1[channel]) / makoPedal[pos].Var4[0]);  //R4.00 Calc how much to filter.
        if (Gain < 0.0f) Gain = 0.0f;                                                                      //R4.00 Verify we dont go negative.
        tS = (tS * Gain) + (makoPedal[pos].Var2[channel] * (1.0f - Gain));                                 //R4.00 Apply Low Pass filter.
        makoPedal[pos].Var2[channel] = tS;                                                                 //R4.00 Store our last sample for filter calc.
    }

    //R4.00 AVERAGE signal level gate. Var3 is modified Parameter setting.
    if (makoPedal[pos].Var1[channel] < makoPedal[pos].Var3[0])
    {
        Gain = 1.0f;
        Gain = 1.0f - ((makoPedal[pos].Var3[0] - makoPedal[pos].Var1[channel]) / makoPedal[pos].Var3[0]);
        tS *= Gain;
    }
            

    //R4.00 Volume/Gain adjust. Should be first so we can control input vol, but then
    //R4.00 changes will affect Gate settings and throw everything off.
    return tS * *Setting[e_GateVol] * 5.0f;
}

//R4.00 Pedal 1 - Apply the GATE AUTO WAH effect to the incoming sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_AutoWah_Peak(float tSample, int channel, int pos)
{
    float tS = abs(tSample);
    float Temp_F;
    int c = channel;

    //R4.00 Track our peak signal. Fast On / Slow Off.
    //R4.00 makoPedal[1].Var5[c] = Peak
    //R4.00 makoPedal[1].Var6[c] = Last Peak, Only used to track changes in sample to trigger recalcs.
    //R4.00 makoPedal[1].Var7[c] = The range of freqs to sweep.
    if (makoPedal[pos].Var5[c] < tS)
    {
        //R4.00 Fast On
        makoPedal[pos].Var5[c] = tS;
        if (1.0f < makoPedal[pos].Var5[c]) makoPedal[pos].Var5[c] = 1.0f;   //R1.00 Clip here so we do not do broken math below. 
    }
    else
    {
        //R4.00 Slow Off.
        makoPedal[pos].Var5[c] -= Release_500mS;
        if (makoPedal[pos].Var5[c] < 0.0f) makoPedal[pos].Var5[c] = 0.0f;
    }

    //R4.00 Try not to do calcs so often. Trigger off the size of the signal change.
    //R4.00 If the WAH is warbling, get rid of this check and calc every sample. 
    if (.00005f < abs(makoPedal[pos].Var5[c] - makoPedal[pos].Var6[c]))
    {
        makoPedal[pos].Var6[c] = makoPedal[pos].Var5[c];
        Temp_F = 400.0f + (makoPedal[pos].Var5[c] * makoPedal[pos].Var7[c]);
        if (15000 < Temp_F) Temp_F = 15000;
        Mako_Calc_coeffs_Wah(*Setting[e_GateWahQ], Temp_F, c);
    }

    WAH_xn[c] = tSample;
    tS = WAH_coeffs[c].a0 * WAH_xn[c] + WAH_coeffs[c].a1 * WAH_xnz1[c] + WAH_coeffs[c].a2 * WAH_xnz2[c] - WAH_coeffs[c].b1 * WAH_ynz1[c] - WAH_coeffs[c].b2 * WAH_ynz2[c];
    WAH_xnz2[c] = WAH_xnz1[c]; WAH_xnz1[c] = WAH_xn[c]; WAH_ynz2[c] = WAH_ynz1[c]; WAH_ynz1[c] = tS;

    return (tSample * (1.0f - *Setting[e_GateWah])) + (tS * *Setting[e_GateWah] * .2f);
}

float MakoDist01AudioProcessor::Mako_FX_Audio_AutoWah_Avg(float tSample, float avg, int channel, int pos)
{
    float tS = abs(tSample);
    float Temp_F;
    int c = channel;

    //R4.00 Track our peak signal. Fast On / Slow Off.
    //R4.00 makoPedal[1].Var5[c] = Peak
    //R4.00 makoPedal[1].Var6[c] = Last Peak, Only used to track changes in sample to trigger recalcs.
    //R4.00 makoPedal[1].Var7[c] = The range of freqs to sweep.
    if (makoPedal[pos].Var5[c] < avg)
    {
        //R4.00 Fast On
        makoPedal[pos].Var5[c] = tS;
        if (1.0f < makoPedal[pos].Var5[c]) makoPedal[pos].Var5[c] = 1.0f;   //R1.00 Clip here so we do not do broken math below. 
    }
    else
    {
        //R4.00 Slow Off.
        makoPedal[pos].Var5[c] -= Release_500mS;
        if (makoPedal[pos].Var5[c] < 0.0f) makoPedal[pos].Var5[c] = 0.0f;
    }

    //R4.00 Try not to do calcs so often. Trigger off the size of the signal change. 
    if (.00005f < abs(makoPedal[pos].Var5[c] - makoPedal[pos].Var6[c]))
    {
        makoPedal[pos].Var6[c] = makoPedal[pos].Var5[c];
        Temp_F = 400.0f + (makoPedal[pos].Var5[c] * makoPedal[pos].Var7[c]);
        if (15000 < Temp_F) Temp_F = 15000;
        Mako_Calc_coeffs_Wah(*Setting[e_GateWahQ], Temp_F, c);
    }

    //R4.00 Apply the Wah filter to the incoming signal.
    WAH_xn[c] = tSample;
    tS = WAH_coeffs[c].a0 * WAH_xn[c] + WAH_coeffs[c].a1 * WAH_xnz1[c] + WAH_coeffs[c].a2 * WAH_xnz2[c] - WAH_coeffs[c].b1 * WAH_ynz1[c] - WAH_coeffs[c].b2 * WAH_ynz2[c];
    WAH_xnz2[c] = WAH_xnz1[c]; WAH_xnz1[c] = WAH_xn[c]; WAH_ynz2[c] = WAH_ynz1[c]; WAH_ynz1[c] = tS;

    return (tSample * (1.0f - *Setting[e_GateWah])) + (tS * *Setting[e_GateWah] * .2f);
}

//R4.00 Pedal 2 - Apply the COMPRESSOR to the incoming sample.
float MakoDist01AudioProcessor::Mako_Amp_Comp(float tSample, int channel)
{
    //Var1 = Current actual compression.
    //Var2 = Compression calculated point. Var1 always works toward Var2.
        
    float diff;
    float tSa = abs(tSample);

    //R4.30 Used for normal compression. Using it now for Sag.
    //float tS = Filter_Calc_BiQuad(tSample, channel, &fil_Amp_Comp);

    //R1.00 If our signal is above the Threshold.
    if (Amp_Comp_Threshold < tSa)
    {
        //R1.00 Get Difference in Gain and Threshold.    
        diff = tSa - Amp_Comp_Threshold;

        //R3.00 Calc what our new gain reduction value should be.
        Amp_Comp_Var2[channel] = (Amp_Comp_Threshold + (diff * *Setting[e_AmpCRatio])) / tSa;

        //R3.00 Slowly modify our GAIN adjuster (Var1) to the new gain value (Var2). 
        if (Amp_Comp_Var2[channel] < Amp_Comp_Var1[channel])
        {
            //R4.30 To have a comp attack we need a 2nd var that we adjust up to the actual max. So the comp slowly begins working.
            Amp_Comp_Var1[channel] -= (Release_500mS * 33.0f); //R4.30 was 33.0f.
            if (Amp_Comp_Var1[channel] < 0.0f) Amp_Comp_Var1[channel] = 0.0f;
        }
        else
        {
            //R4.30 Reduce the Gain adjust back to 1.0f.
            Amp_Comp_Var1[channel] += (Release_500mS * 25.0f); //R4.30 was 18.0f.
            if (1.0f < Amp_Comp_Var1[channel]) Amp_Comp_Var1[channel] = 1.0f;
        }

        //R4.00 Let Editor know we are triggered.
        makoAmpCompLight = true;
    }
    else
    {
        //R4.30 Signal is BELOW the threshold, reduce the Gain adjust back 1.0f.
        Amp_Comp_Var1[channel] += (Release_500mS * 25.0f); //R4.30 was 18.0f.
        if (1.0f < Amp_Comp_Var1[channel]) Amp_Comp_Var1[channel] = 1.0f;

        //R4.00 Flag the editor that our compressor is NOT triggered.
        makoAmpCompLight = false;
    }

    //R2.09 Apply GAIN and DRIVE if needed. Normal compression.
    //return tSample * Amp_Comp_Var1[channel];
    
    //R4.30 AMP COMP/SAG. Use compress value 0 - 1 for filter gain adjust. Using linear vals for speed. 
    Filter_BP_Coeffs_AmpSag(Amp_Comp_Var1[channel], &fil_Amp_Comp); 
    float tS = Filter_Calc_BiQuad(tSample, channel, &fil_Amp_Comp);

    return tS;

}

//R4.00 Pedal 2 - Apply the COMPRESSOR to the incoming sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_Compressor(float tSample, int channel, int pos)
{
    //Var2 = Max signal Buffer
    //4 = Release
    //7 = Threshold
    //5 = Attack
    //6 = Drive
    //8 = Ratio
    //9 = High Pass Detection filter

    float tThresh = makoPedal[pos].Var3[0]; // Setting[e_CompThr] * Setting[e_CompThr] * Setting[e_CompThr]; //R1.00 Square THRESH to give us more range on the knob.
    float diff;
    float tSa;

    //R4.00 Put our detection signal thru a Low/High Pass. 
    tSa = tSample;

    //R4.00 Optional filtering of trigger signal.
    //if (*Setting[xxx] < .999f) tSa = Filter_Calc_BiQuad(tSa, channel, &makoPedal[idx].Filter1);  //R4.00 Future use.
    //if (0.001f < *Setting[xxx]) tSa = Filter_Calc_BiQuad(tSa, channel, &makoPedal[idx].Filter2); //R4.00 Future use.
    
    tSa = abs(tSa);

    //R1.00 If our signal is above the Threshold.
    if (tThresh < tSa)
    {
        //R1.00 Get Difference in Gain and Threshold.    
        diff = tSa - tThresh;

        //R3.00 Calc what our new gain reduction value should be.
        makoPedal[pos].Var2[channel] = (tThresh + (diff * *Setting[e_CompRat])) / tSa;

        //R3.00 Slowly modify our GAIN adjuster (Var1) to the new gain value (Var2). 
        if (makoPedal[pos].Var2[channel] < makoPedal[pos].Var1[channel])
        {
            //R1.00 To have a comp attack we need a 2nd var that we adjust up to the actual max. So the comp slowly begins working.
            makoPedal[pos].Var1[channel] -= (Release_500mS * (1.0f + (1.0f - *Setting[e_CompAtt]) * 20.0f));
            if (makoPedal[pos].Var1[channel] < 0.0f) makoPedal[pos].Var1[channel] = 0.0f;
        }
        else
        {
            //R1.00 Reduce the Gain adjust back to 1.0f.
            makoPedal[pos].Var1[channel] += (Release_500mS * (1.0f + (1.0f - *Setting[e_CompRel]) * 10.0f));
            if (1.0f < makoPedal[pos].Var1[channel]) makoPedal[pos].Var1[channel] = 1.0f;
        }

        //R4.00 Flag the editor that our compressor is triggered.
        makoPedal2CompLight = true;
    }
    else
    {
        //R1.00 Signal is BELOW the threshold, reduce the Gain adjust back 1.0f.
        makoPedal[pos].Var1[channel] += (Release_500mS * (1.0f + (1.0f - *Setting[e_CompRel]) * 10.0f));
        if (1.0f < makoPedal[pos].Var1[channel]) makoPedal[pos].Var1[channel] = 1.0f;

        //R4.00 Flag the editor that our compressor is NOT triggered.
        makoPedal2CompLight = false;
    }

    //R2.09 Apply GAIN and DRIVE if needed.
    if (*Setting[e_CompDrv] < .0001f)
        return tSample * makoPedal[pos].Var1[channel] * *Setting[e_CompVol] * 4.0f;
    else
        return tanh(tSample * makoPedal[pos].Var1[channel] * (1.0f + (*Setting[e_CompDrv] * 10.0f))) * *Setting[e_CompVol] * 2.0f;

}

//R4.00 Pedal 3 - Apply the DISTORTION 1 to the incoming sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_Distortion1(float tSample, int channel, int pos)
{
    float tS = tSample;

    //R2.00 BP - Mid boost.
    tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[pos].Filter1);
    
    //R5.00 Apply Drive.
    tS = tanhf(tS * (1.0f + (*Setting[e_Dist1Drv] * *Setting[e_Dist1Drv] * 100.0f)));
    tS *= .1f;

    //R4.30 LP - Fizz filter. May want after the clean/dist mix.
    if (*Setting[e_Dist1Cut] < 7000.0f) tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[pos].Filter2);

    //R5.00 Mix the Clean and Distorted samples.
    tS = ((tSample * (1.0f - *Setting[e_Dist1Vol])) + (tS * *Setting[e_Dist1Vol]));

    return tS;
}


//R4.00 Pedal 4 - Apply the DISTORTION 2 to the incoming sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_Distortion2(float tSample, int channel, int pos)
{
    float tS = tSample;
    float Gain = 1.0f + (*Setting[e_Dist2Drv] * *Setting[e_Dist2Drv] * 500.0f);

    //R4.00 Low Cut can help if there is too much flubby bass.
    //tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[pos].Filter3);

    //R2.00 BP - Mid/Bright boost.
    tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[pos].Filter1);
    tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[pos].Filter3);

    //R5.00 4x Over Sampling.
    float tvDelta = (tS - Dist2_Sample_Last) / 4.0f;
    for (int SampX = 0; SampX < 4; SampX++)
    {
        //R4.20 Need to interpolate between the samples here!!!
        tS = Dist2_Sample_Last + (tvDelta * SampX);

        //R4.00 Apply the Hard clipping gain.
        tS *= Gain;

        //R4.00 Hard Clipping.
        if (.9999f < tS)
            tS = .9999f;
        else if (tS < -.9999f)
            tS = -.9999f;

        //R5.00 4x Up Sampling filter.
        tS = Filter_Calc_BiQuad(tS, channel, &fil_DistOverSamp);
    }

    //R5.00 Store last sample for next interpolation.
    Dist2_Sample_Last = tSample;

    //R2.00 LP - Fizz filter.
    if (*Setting[e_Dist2Cut] < 7000.0f) tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[pos].Filter2);

    //Volume/Gain adjust. Reduction only.
    tS *= *Setting[e_Dist2Vol];

    return tS;
}


//R4.00 Pedal 5 - Apply the STUDIO EQ Pedal to the incoming sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_EQ(float tSample, int channel, int Pos)
{
    float tS = tSample;
    if (DoEQ1) tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[Pos].Filter1); 
    if (DoEQ2) tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[Pos].Filter2); 
    if (DoEQ3) tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[Pos].Filter3); 
    if (DoEQ4) tS = Filter_Calc_BiQuad(tS, channel, &makoPedal[Pos].Filter4); 
    
    //Volume/Gain adjust. Reduction only.
    return tS * *Setting[e_EQVol];
}

//R1.00 Clip signal to -1 and 1.
float MakoDist01AudioProcessor::Mako_Clip(float tSample, int ClipStage)
{
    float tS = tSample;

    if (.9999f < tS)
    {
        tS = .9999f;
        makoClipStage = ClipStage;
    }
    else if (tS < -.9999f)
    {
        tS = -.9999f;
        makoClipStage = ClipStage;
    }
   
    return tS;
}


//R4.00 Apply the Amplifier EQ to incoming audio sample.
float MakoDist01AudioProcessor::Mako_Amp_EQ(float tSample, int channel)
{   
    float tS = tSample;

    //R4.00 Do EQ calcs, only do buffer moves if EQ is not used. 
    //R4.00 Need buffer updates or get crackles when automated/spun.
    tS = Filter_Calc_BiQuad_Bypass(DoAmpEQ1, tS, channel, &fil_Amp_EQ1);
    tS = Filter_Calc_BiQuad_Bypass(DoAmpEQ2, tS, channel, &fil_Amp_EQ2);
    tS = Filter_Calc_BiQuad_Bypass(DoAmpEQ3, tS, channel, &fil_Amp_EQ3);
    tS = Filter_Calc_BiQuad_Bypass(DoAmpEQ4, tS, channel, &fil_Amp_EQ4);
    tS = Filter_Calc_BiQuad_Bypass(DoAmpEQ5, tS, channel, &fil_Amp_EQ5);
    tS = Filter_Calc_BiQuad_Bypass(DoAmpEQ6, tS, channel, &fil_Amp_EQ6);

    return tS;
}

//R1.00 1024 sample IMPULSE RESPONSE calcs.
float MakoDist01AudioProcessor::Mako_CabSim(float tSample, int channel)
{
    int T1;
    float V = 0.0f;
    
    //R1.00 NO CAB selected. 
    if (!*Setting[e_IR] && (channel == 0)) return tSample;
   
    T1 = IRB_Ring1[channel];
    IR_AudioBuffer[channel][T1] = tSample;
        
    //R1.00 Apply Left cab (0) to ALL OTHER channels. Should only ever be 0.
    for (int t = 0; t < 1024; t++)
    {
        V += (IR_AudioBuffer[channel][T1] * IR_Speaker[t]);

        //R1.00 Increment index, mask off bits past 1023 to keep our index between 0-1023.
        T1 = (T1 + 1) & 0x3FF;            
    }
    V *= *Setting[e_IRVol];
  
    //R1.00 Decrement our ring buffer index and loop around at 0. 
    IRB_Ring1[channel]--;
    if (IRB_Ring1[channel] < 0) IRB_Ring1[channel] = 1023;
    
    //R1.00 We gain volume here so reduce it to some random level. Need good math here.
    return V * .25f;  
}

//R1.00 DIGITAL DELAY.
float MakoDist01AudioProcessor::Mako_FX_Delay(float tSample, int channel)
{
    float tS;
    float tS_Delay;


    //R5.00 Keep track of our volume for ducking. Values .99 and .01 should be sample rate dependent. 
    Delay_Duck_Peak[channel] *= RATI1000ms;
    Delay_Duck_Peak[channel] = (Delay_Duck_Peak[channel] * .99f) + abs(tSample * .01f);

    //R5.00 get the delay/echo array index.
    long idx = Delay_B_Ring1[channel];
    
    //R5.00 Get Delay signal, apply ducking if required.
    if (*Setting[e_DelDuck])
        tS_Delay = Mako_FX_Delay_Ducking(Delay_B[channel][idx] * Delay_Wet, channel);
    else
        tS_Delay = Delay_B[channel][idx] * Delay_Wet;

    //R5.00 Get the final Sample/Delay mix.
    tS = (tSample * Delay_Dry) + (tS_Delay * Delay_BalLR[channel]);
    
    //R5.00 Write the new sample/echo into the delay buffer.
    Delay_B[channel][idx] = .5f * (tSample + (Delay_B[channel][idx] * *Setting[e_DelRepeat]));

    //R1.00 Update echo ring buffer position. 
    Delay_B_Ring1[channel]++;
    if (Delay_B_Ring1_Max[channel] < Delay_B_Ring1[channel]) Delay_B_Ring1[channel] = 0;

    return tS;
}

//R4.00 Calc signal average and apply ducking.
float MakoDist01AudioProcessor::Mako_FX_Delay_Ducking(float tSample, int channel)
{
    //R2.00 Create a volume envelope based on Signal Average.
    Delay_Duck_Fac[channel] = 1.0f - (Delay_Duck_Peak[channel] * 10.0f * *Setting[e_DelDuck]);
    if (Delay_Duck_Fac[channel] < 0.0f) Delay_Duck_Fac[channel] = 0.0f;

    return tSample * Delay_Duck_Fac[channel];
}

//****************************************************************
// M O D U L A T I O N   E F F E C T S
//****************************************************************
//R4.00 Apply the CHORUS effect to incoming audio sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_Chorus(float tSample, int channel)
{
    float tS;
    float idxf;  //R5.00 Get fractional value (x.12345).
    int idx;     //R5.00 Array indexes to interpolate between.
    int idx2;
    float tS_Mod;

    //R1.00 Store new sample in Ring buffer. 
    float LP = *Setting[e_Mod3Tone];
    if (LP < .001f) LP = .01f;
    Mod3_B[channel][Mod3_B_Ring1[channel]] = (tSample * LP) + (Mod3_B_Last[channel] * (1.0f - LP));
    Mod3_B_Last[channel] = Mod3_B[channel][Mod3_B_Ring1[channel]];

    //R5.00 Calc modulating index and the next sample to interpolate with.
    idxf = Mod3_B_Ring1[channel] - Mod3_B_Offset[channel];
    idx = int(idxf);
    float idxDiff = idxf - idx;  //R5.00 Get the remainder/remove the integer portion.
    idx = idx & 0x07FF;          //R5.00 Could use if (idx < 0) idx = idx + 2048;    
    idx2 = (idx - 1) & 0x07FF;   //R5.00 Could use if (idx2 < 0) idx2 = idx2 + 2048;

    //R5.00 Interpolate between samples to remove crackle/disconinuity in modulated signal.
    tS_Mod = (Mod3_B[channel][idx] * idxDiff) + (Mod3_B[channel][idx2] * (1.0f - idxDiff));

    //R5.00 Get the final output sample. Sample mixed with Modulated sample.
    tS = (tSample * (1.0f - *Setting[e_Mod3Mix])) + ((tS_Mod * *Setting[e_Mod3Mix]) * Mod3_BalLR[channel]);

    //R1.00 Update our rotating index.
    Mod3_B_Offset[channel] *= (1.0f + (Mod3_B_Rate[channel] * *Setting[e_Mod3Rate]));

    //R1.00 Alter our delay direction if needed.
    if (Mod3_B_Offset[channel] < Mod3_B_Offset_Min[channel])
        Mod3_B_Rate[channel] = Mod3_B_Rate_Up;
    else if (Mod3_B_Offset_Max[channel] < Mod3_B_Offset[channel])
        Mod3_B_Rate[channel] = Mod3_B_Rate_Down;

    //R5.00 Update ring buffer position. Use 7FF bit mask to keep values between 0-2047.
    Mod3_B_Ring1[channel] = (Mod3_B_Ring1[channel] + 1) & 0x07FF;

    return tS * 2.5f * *Setting[e_Mod3Vol];
}

//R4.00 Apply the MOD WAH effect to incoming audio sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_ModWah(float tSample, int channel)
{
    float tS;
    int c = channel;

    //R4.00 Update our oscillating waveform.
    Mod2_Wave[c] += Mod2_Wave_Ramp[c];
    if (Mod2_Wave[c] < .01f)
    {
        Mod2_Wave[c] = .01f;
        Mod2_Wave_Ramp[c] = Mod2_B_Rate_Up * *Setting[e_Mod2Rate] * 20.0f;   //R1.00 10 Hz max rate.
    }
    if (.99f < Mod2_Wave[c])
    {
        Mod2_Wave[c] = .99f;
        Mod2_Wave_Ramp[c] = Mod2_B_Rate_Up * *Setting[e_Mod2Rate] * -20.0f;  //R1.00 10 Hz max rate.
    }

    //R4.00 Calc filter coeffs. This is expensive and should be optimized more if possible. Like call it every X samples.
    Mako_Calc_coeffs_MWah(.707f, *Setting[e_Mod2FStart] + (Mod2_Wave[c] * *Setting[e_Mod2Range]), c);
    
    //R4.00 Apply the Wah filter to the incoming signal.
    MWAH_xn[channel] = tSample;
    tS = MWAH_coeffs[c].a0 * MWAH_xn[c] + MWAH_coeffs[c].a1 * MWAH_xnz1[c] + MWAH_coeffs[c].a2 * MWAH_xnz2[c] - MWAH_coeffs[c].b1 * MWAH_ynz1[c] - MWAH_coeffs[c].b2 * MWAH_ynz2[c];
    MWAH_xnz2[c] = MWAH_xnz1[c]; MWAH_xnz1[c] = MWAH_xn[c]; MWAH_ynz2[c] = MWAH_ynz1[c]; MWAH_ynz1[c] = tS;

    //R4.00 Mix the incoming sample with the Wah effected sample.
    tS = (tSample * (1.0f - *Setting[e_Mod2Mix])) + (tS * *Setting[e_Mod2Mix] * .075f);

    return tS * 2.0f * *Setting[e_Mod2Vol];
}

//R4.00 Apply the FLANGE effect to incoming audio sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_Flange(float tSample, int channel)
{
    float tS = tSample;
    float idxf;          //R5.00 Get fractional value (x.12345).
    int idx;             //R5.00 Array indexes to interpolate between.
    int idx2;
    float tS_Mod;

    //R5.00 Calc modulating index and the next sample to interpolate with.
    idxf = Mod1_B_Ring1[channel] - Mod1_B_Offset[channel];
    idx = int(idxf);
    float idxDiff = idxf - idx;   //R5.00 Get the fractional remainder.
    idx = idx & 0x03FF;           //R5.00 Could use if (idx < 0) idx = idx + 1024;
    idx2 = (idx - 1) & 0x03FF;    //R5.00 Could use if (idx2 < 0) idx2 = idx2 + 1024;
    
    //R5.00 Interpolate between samples to remove crackle/disconinuity in flange signal.
    tS_Mod = (Mod1_B[channel][idx] * idxDiff) + (Mod1_B[channel][idx2] * (1.0f - idxDiff));

    //R5.00 Store new sample in Ring buffer. Add some negative feedback. Could just use tS. 
    Mod1_B[channel][Mod1_B_Ring1[channel]] = (tS - tS_Mod) * .5f;

    //R5.00 Get the final output sample. Sample mixed with modulated sample.
    tS = (tS * (1.0f - *Setting[e_Mod1Mix])) - (tS_Mod * *Setting[e_Mod1Mix]);

    //R5.00 Update our rotating index.
    Mod1_B_Offset[channel] += (Mod1_B_Rate[channel] * *Setting[e_Mod1Rate]);

    //R5.00 Alter our delay direction, stay between MIN and MAX.
    if (Mod1_B_Offset[channel] < Mod1_B_Offset_Min[channel])
        Mod1_B_Rate[channel] = Mod1_B_Rate_Up;    
    else if (Mod1_B_Offset_Max[channel] < Mod1_B_Offset[channel])
        Mod1_B_Rate[channel] = Mod1_B_Rate_Down;
       
    //R5.00 Update ring buffer position. Bit mask it to stay within 0-1023.
    Mod1_B_Ring1[channel] = (Mod1_B_Ring1[channel] + 1) & 0x03FF; //R5.00 3FF mask = 1023
    

    //R4.00 Apply Synthesizer effect. Squared to get more usable knob range.
    if (*Setting[e_Mod1Synth])
    {
        tS = sin(tS * *Setting[e_Mod1Synth] * *Setting[e_Mod1Synth] * 500.0f) * .2f;
    }

    //R4.00 Apply rectified Bronz effect (tS * tS). Squaring the signal creates interesting even order harmonic effects.
    if (*Setting[e_Mod1Bronz])
    {
        tS = (tS * (1.0f - *Setting[e_Mod1Bronz])) + (2.0f * (tS * tS) * *Setting[e_Mod1Bronz]);

        //R4.00 Apply DC Offset filter.
        tS = Filter_Calc_BiQuad(tS, channel, &fil_Mod1_DCRemoval);     
    }

    return tS * 2.0f * *Setting[e_Mod1Vol];
}

//R4.00 Apply the WIDE LOAD effect to incoming audio sample.
float MakoDist01AudioProcessor::Mako_FX_Audio_WideLoad(float tSample, int channel)
{    
    int depth = Mod4_Depth[0]; //R4.10 Start with the PreCalc Depth value.
    float tS = tSample;
    
    //R4.00 Apply stereo filter and delays based on channel and Delay Offset.
    if (*Setting[e_Mod4Doff] <= .0f)
    {
        if (channel == 0)
        {
            if (0.0f < *Setting[e_Mod4Foff])
            {
                tS = Filter_Calc_BiQuad(tS, channel, &filMod4[0]); //R4.00 Apply Freq Offset filter.
                tS *= 1.0f - (*Setting[e_Mod4Foff] * .8f);          //R4.00 Try to adjust gain for new filter.
            }
        }
        else
        {
            if (0.0f < *Setting[e_Mod4Foff]) tS = Filter_Calc_BiQuad(tS, channel, &filMod4[1]);
            if (.999f < *Setting[e_Mod1Mix])
                depth = 0;
            else
                depth = depth / 2;
        }
    }
    if (.0f < *Setting[e_Mod4Doff])
    {
        if (channel == 1)
        {
            if (0.0f < *Setting[e_Mod4Foff])
            {
                tS = Filter_Calc_BiQuad(tS, channel, &filMod4[0]); //R4.00 Apply Freq Offset filter.
                tS *= 1.0f - (*Setting[e_Mod4Foff] * .8f);          //R4.00 Try to adjust gain for new filter.
            }
        }
        else
        {
            if (0.0f < *Setting[e_Mod4Foff]) tS = Filter_Calc_BiQuad(tS, channel, &filMod4[1]);
            if (.999f < *Setting[e_Mod4Mix])
                depth = 0;
            else
                depth = depth / 2;
        }
    }

    //R4.00 Apply TONE filter and then store the new sample in Delay Ring buffer. 
    float LP = (*Setting[e_Mod4Tone] * *Setting[e_Mod4Tone]) + .01f;
    Mod4_B[channel][Mod4_B_Ring1[channel]] = (tS * LP) + (Mod4_B_Last[channel] * (1.0f - LP));
    Mod4_B_Last[channel] = Mod4_B[channel][Mod4_B_Ring1[channel]];

    //R5.00 Calc rotating index.
    int idx = (Mod4_B_Ring1[channel] - depth) & 0x0FFF;
    //if (idx < 0) //idx = idx + 4096;
    
    //R5.00 Update ring buffer position. 
    Mod4_B_Ring1[channel] = (Mod4_B_Ring1[channel] + 1) & 0x0FFF;
    
    //R4.00 Apply clean/effect mix and effect balance.
    tS = (tSample * (1.0f - *Setting[e_Mod4Mix])) + (Mod4_B[channel][idx] * *Setting[e_Mod4Mix]) * Mod4_BalLR[channel];

    return tS * 4.0f * *Setting[e_Mod4Vol];
}

//R4.00 LOW CUT and removal of DC offset from Asymmetry code!!! 
float MakoDist01AudioProcessor::Mako_EQ_Output_Filters(float tSample, int channel)
{
    return Filter_Calc_BiQuad(tSample, channel, &fil_Out_HP);
}


//****************************************************************
// R E V E R B   E F F E C T S
//****************************************************************
void MakoDist01AudioProcessor::Mako_FX_Reverb_CalcSize()
{
    //R2.00 Set the REVERB coefficients for largest room possible.
    //REVERB COUNT

    //R5.00 Get the Reverb Room Mode and setup reverb variables.
    //R5.00 Setting[e_RevMode] will be 0-19. List selectedID will be 1-20.
    int RM = int(*Setting[e_RevMode]);
    switch (RM)
    {
    case 0:Mako_FX_Reverb_SetMode0(); Reverb_Render_Mode = 1; break;    //R4.30 Basic
    case 1:Mako_FX_Reverb_SetMode1(); Reverb_Render_Mode = 1; break;    //R4.30 Slap
    case 2:Mako_FX_Reverb_SetMode2(); Reverb_Render_Mode = 1; break;    //R4.30 Haunted
    case 3:Mako_FX_Reverb_SetMode3(); Reverb_Render_Mode = 1; break;    //R4.30 Stereo
    case 4:Mako_FX_Reverb_SetMode4(); Reverb_Render_Mode = 1; break;    //R4.30 Springy
    case 5:Mako_FX_Reverb_SetMode5(); Reverb_Render_Mode = 2; break;    //R4.30 Flange Verb
    case 6:Mako_FX_Reverb_SetMode6(); Reverb_Render_Mode = 1; break;    //R4.30 Medium room
    case 7:Mako_FX_Reverb_SetMode7(); Reverb_Render_Mode = 1; break;    //R4.30 Small Room
    case 8:Mako_FX_Reverb_SetMode8(); Reverb_Render_Mode = 1; break;    //R4.30 Box
    case 9:Mako_FX_Reverb_SetMode9(); Reverb_Render_Mode = 1; break;    //R4.30 Nature
    case 10:Mako_FX_Reverb_SetMode10(); Reverb_Render_Mode = 1; break;  //R4.30 Fields
    case 11:Mako_FX_Reverb_SetMode11(); Reverb_Render_Mode = 1; break;  //R4.30 Garage
    case 12:Mako_FX_Reverb_SetMode12(); Reverb_Render_Mode = 3; break;  //R4.30 TremVerb
    case 13:Mako_FX_Reverb_SetMode13(); Reverb_Render_Mode = 2; break;  //R4.30 Flange Verb 2
    case 14:Mako_FX_Reverb_SetMode14(); Reverb_Render_Mode = 1; break;  //R4.30 Stereo 2
    case 15:Mako_FX_Reverb_SetMode15(); Reverb_Render_Mode = 1; break;  //R4.30 Garage 2
    case 16:Mako_FX_Reverb_SetMode16(); Reverb_Render_Mode = 1; break;  //R4.30 Inverted
    case 17:Mako_FX_Reverb_SetMode17(); Reverb_Render_Mode = 1; break;  //R4.30 Hanger
    case 18:Mako_FX_Reverb_SetMode18(); Reverb_Render_Mode = 2; break;  //R4.30 Inverted 2
    case 19:Mako_FX_Reverb_SetMode19(); Reverb_Render_Mode = 1; break;  //R4.30 Regent
    }

    //R4.30 Added echo volume adjustment.
    Rev_Echo_Gain = 2.0f;
    if (RM == 19) Rev_Echo_Gain = .9f;

    //R4.30 Added Buffer counts for Springy.
    Rev_Buffer_Count = 16;
    if (RM == 4) Rev_Buffer_Count = 8;

    //R5.00 Flanger values. These are based off of the Chorus value (250).
    //R5.00 If the chorus rate changes, these need adjusted.
    Rev_Flange_Rate = 150.0f / 250.0f;
    Rev_Flange_Limit = 80.0f;
    if (RM == 13)
    {
        Rev_Flange_Rate = 300.0f / 250.0f;
        Rev_Flange_Limit = 300.0f;
    }

    //R4.30 Reset our oscillator values.
    RevMod_Osc1[0] = .01f; RevMod_Osc1[1] = .01f;
    RevMod_B_Rate[0] = RevMod_B_Rate_Up; RevMod_B_Rate[1] = RevMod_B_Rate_Up;

    //R2.22 Calc our PreDelay. Max 200mS or 38400 samples. 
    PreDEnd = int(*Setting[e_RevPreD] * .001f * makoSampleRate);
    if (PreDMax < PreDEnd) PreDEnd = PreDMax;
    
    //R2.00 Calc the gain on DENSE verbs, used so we dont exceed gain of 1.
    float tG = 0.0f;
    for (int t = 0; t < 16; t++) tG = tG + abs(RevB_Gain[t]);  //R2.14 Added new Rev mode gain calcs.
    if (0.0f == tG)
        Reverb_Gain_Factor = 1.0f;
    else
        Reverb_Gain_Factor = 1.0f / tG;
    
    //R2.00 Reduce the Coeffs for smaller rooms. 
    float fac = *Setting[e_RevSize] * 4.0f;
    if (fac < .01f) fac = .01f;

    float tFac = *Setting[e_RevSize];
    if (tFac < .01f) tFac = .01f;
    for (int t = 0; t < 16; t++)
    {
        RevB_Max[0][t] = int(RevB_Max[0][t] * tFac);
        RevB_Max[1][t] = int(RevB_Max[1][t] * tFac);
    }

    //R1.00 Reset our buffer indexes.
    for (int t = 0; t < 16; t++)
    {
        RevB_Idx[0][t] = 0;
        RevB_Idx[1][t] = 0;
    }

}

//R4.00 BASIC.
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode0()
{    
    RevB_Gain[0] = .372716f;
    RevB_Max[0][0] = 4670;
    RevB_Max[1][0] = 4751;
    RevB_Gain[1] = .494920f;
    RevB_Max[0][1] = 9147;
    RevB_Max[1][1] = 9702;
    RevB_Gain[2] = .587023f;
    RevB_Max[0][2] = 12843;
    RevB_Max[1][2] = 12903;
    RevB_Gain[3] = -.684522f;
    RevB_Max[0][3] = 17554;
    RevB_Max[1][3] = 18266;
    RevB_Gain[4] = -.700000f;
    RevB_Max[0][4] = 22666;
    RevB_Max[1][4] = 22768;
    RevB_Gain[5] = .8f;
    RevB_Max[0][5] = 26336;
    RevB_Max[1][5] = 27167;
    RevB_Gain[6] = -.956161f;
    RevB_Max[0][6] = 31599;
    RevB_Max[1][6] = 31934;
    RevB_Gain[7] = .955815f;
    RevB_Max[0][7] = 35322;
    RevB_Max[1][7] = 36631;
    RevB_Gain[8] = .846753f;
    RevB_Max[0][8] = 39810;
    RevB_Max[1][8] = 40362;
    RevB_Gain[9] = -.706537f;
    RevB_Max[0][9] = 44722;
    RevB_Max[1][9] = 44254;
    RevB_Gain[10] = .607946f;
    RevB_Max[0][10] = 49358;
    RevB_Max[1][10] = 49851;
    RevB_Gain[11] = -.538731f;
    RevB_Max[0][11] = 53735;
    RevB_Max[1][11] = 53649;
    RevB_Gain[12] = .467733f;
    RevB_Max[0][12] = 59242;
    RevB_Max[1][12] = 57869;
    RevB_Gain[13] = -.349330f;
    RevB_Max[0][13] = 62550;
    RevB_Max[1][13] = 62473;
    RevB_Gain[14] = .278476f;
    RevB_Max[0][14] = 66834;
    RevB_Max[1][14] = 66767;
    RevB_Gain[15] = -.156921f;
    RevB_Max[0][15] = 71706;
    RevB_Max[1][15] = 71826;


}

//R4.00 SLAP SMEAR.
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode1()
{
    RevB_Gain[0] = -1.185651E-03f;
    RevB_Max[0][0] = 4332;
    RevB_Max[1][0] = 5132;
    RevB_Gain[1] = .1188914f;
    RevB_Max[0][1] = 8402;
    RevB_Max[1][1] = 9239;
    RevB_Gain[2] = -.3848826f;
    RevB_Max[0][2] = 13317;
    RevB_Max[1][2] = 14223;
    RevB_Gain[3] = .3194438f;
    RevB_Max[0][3] = 17367;
    RevB_Max[1][3] = 18007;
    RevB_Gain[4] = .1176973f;
    RevB_Max[0][4] = 22903;
    RevB_Max[1][4] = 22174;
    RevB_Gain[5] = -.5027322f;
    RevB_Max[0][5] = 26266;
    RevB_Max[1][5] = 26931;
    RevB_Gain[6] = .7059703f;
    RevB_Max[0][6] = 32144;
    RevB_Max[1][6] = 30791;
    RevB_Gain[7] = -.7168727f;
    RevB_Max[0][7] = 35775;
    RevB_Max[1][7] = 36704;
    RevB_Gain[8] = .992442f;
    RevB_Max[0][8] = 39802;
    RevB_Max[1][8] = 39754;
    RevB_Gain[9] = -1.0f;
    RevB_Max[0][9] = 45444;
    RevB_Max[1][9] = 45392;
    RevB_Gain[10] = .6304718f;
    RevB_Max[0][10] = 49529;
    RevB_Max[1][10] = 49081;
    RevB_Gain[11] = -.2074285f;
    RevB_Max[0][11] = 53819;
    RevB_Max[1][11] = 54077;
    RevB_Gain[12] = -4.904115E-02f;
    RevB_Max[0][12] = 58918;
    RevB_Max[1][12] = 58958;
    RevB_Gain[13] = .3219758f;
    RevB_Max[0][13] = 63431;
    RevB_Max[1][13] = 62385;
    RevB_Gain[14] = -.5606492f;
    RevB_Max[0][14] = 66935;
    RevB_Max[1][14] = 66956;
    RevB_Gain[15] = .6529667f;
    RevB_Max[0][15] = 72664;
    RevB_Max[1][15] = 71883;
        
}

//R4.00 HAUNTED.
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode2()
{
    RevB_Gain[0] = .732154f;
    RevB_Max[0][0] = 5120;
    RevB_Max[1][0] = 4996;
    RevB_Gain[1] = 1.000000f;
    RevB_Max[0][1] = 9632;
    RevB_Max[1][1] = 9089;
    RevB_Gain[2] = -.832007f;
    RevB_Max[0][2] = 13652;
    RevB_Max[1][2] = 13571;
    RevB_Gain[3] = .266124f;
    RevB_Max[0][3] = 18270;
    RevB_Max[1][3] = 18463;
    RevB_Gain[4] = -.451646f;
    RevB_Max[0][4] = 22544;
    RevB_Max[1][4] = 23226;
    RevB_Gain[5] = .425845f;
    RevB_Max[0][5] = 27680;
    RevB_Max[1][5] = 26979;
    RevB_Gain[6] = -1.000000f;
    RevB_Max[0][6] = 31133;
    RevB_Max[1][6] = 31925;
    RevB_Gain[7] = .909631f;
    RevB_Max[0][7] = 36568;
    RevB_Max[1][7] = 36737;
    RevB_Gain[8] = -.404365f;
    RevB_Max[0][8] = 39961;
    RevB_Max[1][8] = 40003;
    RevB_Gain[9] = -.325968f;
    RevB_Max[0][9] = 45518;
    RevB_Max[1][9] = 45709;
    RevB_Gain[10] = -.152019f;
    RevB_Max[0][10] = 49961;
    RevB_Max[1][10] = 49634;
    RevB_Gain[11] = -.265898f;
    RevB_Max[0][11] = 54105;
    RevB_Max[1][11] = 53382;
    RevB_Gain[12] = .393216f;
    RevB_Max[0][12] = 59009;
    RevB_Max[1][12] = 58661;
    RevB_Gain[13] = -.455110f;
    RevB_Max[0][13] = 62892;
    RevB_Max[1][13] = 62291;
    RevB_Gain[14] = -.869183f;
    RevB_Max[0][14] = 67850;
    RevB_Max[1][14] = 66811;
    RevB_Gain[15] = .700000f;
    RevB_Max[0][15] = 71294;
    RevB_Max[1][15] = 72407;

}

//R4.00 Stereo
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode3()
{
    RevB_Gain[0] = -1.000000f;
    RevB_Max[0][0] = 2207;
    RevB_Max[1][0] = 4148;
    RevB_Gain[1] = .934283f;
    RevB_Max[0][1] = 5433;
    RevB_Max[1][1] = 8914;
    RevB_Gain[2] = .798198f;
    RevB_Max[0][2] = 9496;
    RevB_Max[1][2] = 13077;
    RevB_Gain[3] = -.678104f;
    RevB_Max[0][3] = 12133;
    RevB_Max[1][3] = 17725;
    RevB_Gain[4] = -.405933f;
    RevB_Max[0][4] = 20202;
    RevB_Max[1][4] = 22917;
    RevB_Gain[5] = .368131f;
    RevB_Max[0][5] = 26615;
    RevB_Max[1][5] = 23149;
    RevB_Gain[6] = -.782931f;
    RevB_Max[0][6] = 28192;
    RevB_Max[1][6] = 29536;
    RevB_Gain[7] = -.263157f;
    RevB_Max[0][7] = 31367;
    RevB_Max[1][7] = 35442;
    RevB_Gain[8] = -.263157f;
    RevB_Max[0][8] = 35541;
    RevB_Max[1][8] = 40677;
    RevB_Gain[9] = -.882931f;
    RevB_Max[0][9] = 38252;
    RevB_Max[1][9] = 47732;
    RevB_Gain[10] = .368131f;
    RevB_Max[0][10] = 42212;
    RevB_Max[1][10] = 49420;
    RevB_Gain[11] = -.405933f;
    RevB_Max[0][11] = 45629;
    RevB_Max[1][11] = 53663;
    RevB_Gain[12] = -.678104f;
    RevB_Max[0][12] = 56612;
    RevB_Max[1][12] = 58514;
    RevB_Gain[13] = .798198f;
    RevB_Max[0][13] = 61840;
    RevB_Max[1][13] = 60425;
    RevB_Gain[14] = .934283f;
    RevB_Max[0][14] = 70520;
    RevB_Max[1][14] = 63592;
    RevB_Gain[15] = -.30000f;
    RevB_Max[0][15] = 71582;
    RevB_Max[1][15] = 72646;

}

//R4.30 Springy
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode4()
{       
RevB_Gain[0] = -.704351f;
RevB_Max[0][0] = 4798;
RevB_Max[1][0] = 4040;
RevB_Gain[1] = -.984522f;
RevB_Max[0][1] = 9266;
RevB_Max[1][1] = 9391;
RevB_Gain[2] = -1.000000f;
RevB_Max[0][2] = 13637;
RevB_Max[1][2] = 13944;
RevB_Gain[3] = .756161f;
RevB_Max[0][3] = 17666;
RevB_Max[1][3] = 18729;
RevB_Gain[4] = -.701780f;
RevB_Max[0][4] = 21887;
RevB_Max[1][4] = 21776;
RevB_Gain[5] = .603781f;
RevB_Max[0][5] = 27295;
RevB_Max[1][5] = 27338;
RevB_Gain[6] = -.417887f;
RevB_Max[0][6] = 30946;
RevB_Max[1][6] = 31312;
RevB_Gain[7] = .364175f;
RevB_Max[0][7] = 35922;
RevB_Max[1][7] = 35631;
RevB_Gain[8] = .308302f;
RevB_Max[0][8] = 40421;
RevB_Max[1][8] = 41200;
RevB_Gain[9] = -.235045f;
RevB_Max[0][9] = 44692;
RevB_Max[1][9] = 44390;
RevB_Gain[10] = .128516f;
RevB_Max[0][10] = 48807;
RevB_Max[1][10] = 48926;
RevB_Gain[11] = .083362f;
RevB_Max[0][11] = 53744;
RevB_Max[1][11] = 54419;
RevB_Gain[12] = -.075837f;
RevB_Max[0][12] = 59040;
RevB_Max[1][12] = 58146;
RevB_Gain[13] = .047945f;
RevB_Max[0][13] = 62703;
RevB_Max[1][13] = 63561;
RevB_Gain[14] = .020879f;
RevB_Max[0][14] = 67931;
RevB_Max[1][14] = 66898;
RevB_Gain[15] = -.05000f;
RevB_Max[0][15] = 71547;
RevB_Max[1][15] = 71602;

}

//R4.00 Flange verb
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode5()
{  
    RevB_Gain[0] = .747714f;
    RevB_Max[0][0] = 4820;
    RevB_Max[1][0] = 4701;
    RevB_Gain[1] = -1.000000f;
    RevB_Max[0][1] = 9601;
    RevB_Max[1][1] = 8925;
    RevB_Gain[2] = .509121f;
    RevB_Max[0][2] = 13416;
    RevB_Max[1][2] = 13457;
    RevB_Gain[3] = -.597189f;
    RevB_Max[0][3] = 18072;
    RevB_Max[1][3] = 17674;
    RevB_Gain[4] = .511270f;
    RevB_Max[0][4] = 23059;
    RevB_Max[1][4] = 22736;
    RevB_Gain[5] = -.238252f;
    RevB_Max[0][5] = 27485;
    RevB_Max[1][5] = 26555;
    RevB_Gain[6] = .380602f;
    RevB_Max[0][6] = 30927;
    RevB_Max[1][6] = 30971;
    RevB_Gain[7] = .313804f;
    RevB_Max[0][7] = 36641;
    RevB_Max[1][7] = 35888;
    RevB_Gain[8] = .321968f;
    RevB_Max[0][8] = 39908;
    RevB_Max[1][8] = 40466;
    RevB_Gain[9] = -.304256f;
    RevB_Max[0][9] = 44715;
    RevB_Max[1][9] = 44861;
    RevB_Gain[10] = -.236970f;
    RevB_Max[0][10] = 48778;
    RevB_Max[1][10] = 49363;
    RevB_Gain[11] = .197365f;
    RevB_Max[0][11] = 54377;
    RevB_Max[1][11] = 54183;
    RevB_Gain[12] = .165404f;
    RevB_Max[0][12] = 57755;
    RevB_Max[1][12] = 58975;
    RevB_Gain[13] = -.174817f;
    RevB_Max[0][13] = 63285;
    RevB_Max[1][13] = 63105;
    RevB_Gain[14] = -.144482f;
    RevB_Max[0][14] = 66932;
    RevB_Max[1][14] = 67997;
    RevB_Gain[15] = .102314f;
    RevB_Max[0][15] = 72446;
    RevB_Max[1][15] = 71318;

}

//R4.00 MEDIUM ROOM
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode6()
{
    RevB_Gain[0] = -.050745f;
    RevB_Max[0][0] = 4332;
    RevB_Max[1][0] = 3893;
    RevB_Gain[1] = -.395454f;
    RevB_Max[0][1] = 8348;
    RevB_Max[1][1] = 8723;
    RevB_Gain[2] = -.540994f;
    RevB_Max[0][2] = 13905;
    RevB_Max[1][2] = 13843;
    RevB_Gain[3] = -.617648f;
    RevB_Max[0][3] = 17359;
    RevB_Max[1][3] = 18476;
    RevB_Gain[4] = .536837f;
    RevB_Max[0][4] = 23159;
    RevB_Max[1][4] = 22413;
    RevB_Gain[5] = .713308f;
    RevB_Max[0][5] = 27025;
    RevB_Max[1][5] = 27724;
    RevB_Gain[6] = -.712319f;
    RevB_Max[0][6] = 31055;
    RevB_Max[1][6] = 31690;
    RevB_Gain[7] = -.867768f;
    RevB_Max[0][7] = 35678;
    RevB_Max[1][7] = 36690;
    RevB_Gain[8] = -.697008f;
    RevB_Max[0][8] = 40761;
    RevB_Max[1][8] = 40136;
    RevB_Gain[9] = -.357715f;
    RevB_Max[0][9] = 45025;
    RevB_Max[1][9] = 44470;
    RevB_Gain[10] = -.325792f;
    RevB_Max[0][10] = 49834;
    RevB_Max[1][10] = 49549;
    RevB_Gain[11] = .526544f;
    RevB_Max[0][11] = 54267;
    RevB_Max[1][11] = 54680;
    RevB_Gain[12] = -.568473f;
    RevB_Max[0][12] = 58774;
    RevB_Max[1][12] = 58897;
    RevB_Gain[13] = .712998f;
    RevB_Max[0][13] = 63591;
    RevB_Max[1][13] = 63475;
    RevB_Gain[14] = 1.000000f;
    RevB_Max[0][14] = 67954;
    RevB_Max[1][14] = 67531;
    RevB_Gain[15] = -.542133f;
    RevB_Max[0][15] = 72039;
    RevB_Max[1][15] = 71955;

}

//R4.00 SMALL ROOM
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode7()
{  
    RevB_Gain[0] = -.542133f;
    RevB_Max[0][0] = 4033;
    RevB_Max[1][0] = 4061;
    RevB_Gain[1] = 1.000000f;
    RevB_Max[0][1] = 8521;
    RevB_Max[1][1] = 9164;
    RevB_Gain[2] = .712998f;
    RevB_Max[0][2] = 14200;
    RevB_Max[1][2] = 14106;
    RevB_Gain[3] = -.568473f;
    RevB_Max[0][3] = 17625;
    RevB_Max[1][3] = 17514;
    RevB_Gain[4] = .526544f;
    RevB_Max[0][4] = 22038;
    RevB_Max[1][4] = 22799;
    RevB_Gain[5] = -.325792f;
    RevB_Max[0][5] = 27296;
    RevB_Max[1][5] = 27122;
    RevB_Gain[6] = -.357715f;
    RevB_Max[0][6] = 32227;
    RevB_Max[1][6] = 31573;
    RevB_Gain[7] = -.697008f;
    RevB_Max[0][7] = 36334;
    RevB_Max[1][7] = 35306;
    RevB_Gain[8] = -.867768f;
    RevB_Max[0][8] = 40153;
    RevB_Max[1][8] = 41232;
    RevB_Gain[9] = -.712319f;
    RevB_Max[0][9] = 45416;
    RevB_Max[1][9] = 44256;
    RevB_Gain[10] = .713308f;
    RevB_Max[0][10] = 49093;
    RevB_Max[1][10] = 49919;
    RevB_Gain[11] = .536837f;
    RevB_Max[0][11] = 54212;
    RevB_Max[1][11] = 54680;
    RevB_Gain[12] = -.617648f;
    RevB_Max[0][12] = 57766;
    RevB_Max[1][12] = 58945;
    RevB_Gain[13] = -.540994f;
    RevB_Max[0][13] = 63377;
    RevB_Max[1][13] = 62828;
    RevB_Gain[14] = -.395454f;
    RevB_Max[0][14] = 67762;
    RevB_Max[1][14] = 67005;
    RevB_Gain[15] = -.050745f;
    RevB_Max[0][15] = 72421;
    RevB_Max[1][15] = 71393;

}

//R4.00 BOX
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode8()
{   
    RevB_Gain[0] = -.704351f;
    RevB_Max[0][0] = 4798;
    RevB_Max[1][0] = 4040;
    RevB_Gain[1] = -.984522f;
    RevB_Max[0][1] = 9266;
    RevB_Max[1][1] = 9391;
    RevB_Gain[2] = -1.000000f;
    RevB_Max[0][2] = 13637;
    RevB_Max[1][2] = 13944;
    RevB_Gain[3] = .756161f;
    RevB_Max[0][3] = 17666;
    RevB_Max[1][3] = 18729;
    RevB_Gain[4] = -.701780f;
    RevB_Max[0][4] = 21887;
    RevB_Max[1][4] = 21776;
    RevB_Gain[5] = .603781f;
    RevB_Max[0][5] = 27295;
    RevB_Max[1][5] = 27338;
    RevB_Gain[6] = -.417887f;
    RevB_Max[0][6] = 30946;
    RevB_Max[1][6] = 31312;
    RevB_Gain[7] = .364175f;
    RevB_Max[0][7] = 35922;
    RevB_Max[1][7] = 35631;
    RevB_Gain[8] = .308302f;
    RevB_Max[0][8] = 40421;
    RevB_Max[1][8] = 41200;
    RevB_Gain[9] = -.235045f;
    RevB_Max[0][9] = 44692;
    RevB_Max[1][9] = 44390;
    RevB_Gain[10] = .128516f;
    RevB_Max[0][10] = 48807;
    RevB_Max[1][10] = 48926;
    RevB_Gain[11] = .083362f;
    RevB_Max[0][11] = 53744;
    RevB_Max[1][11] = 54419;
    RevB_Gain[12] = -.075837f;
    RevB_Max[0][12] = 59040;
    RevB_Max[1][12] = 58146;
    RevB_Gain[13] = .047945f;
    RevB_Max[0][13] = 62703;
    RevB_Max[1][13] = 63561;
    RevB_Gain[14] = .020879f;
    RevB_Max[0][14] = 67931;
    RevB_Max[1][14] = 66898;
    RevB_Gain[15] = .000000f;
    RevB_Max[0][15] = 71547;
    RevB_Max[1][15] = 71602;

}

//R4.00 Room 2 Nature
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode9()
{
    RevB_Gain[0] = .363553f;
    RevB_Max[0][0] = 4570;
    RevB_Max[1][0] = 4725;
    RevB_Gain[1] = .781771f;
    RevB_Max[0][1] = 8709;
    RevB_Max[1][1] = 9560;
    RevB_Gain[2] = -1.000000f;
    RevB_Max[0][2] = 13127;
    RevB_Max[1][2] = 13973;
    RevB_Gain[3] = .854110f;
    RevB_Max[0][3] = 17578;
    RevB_Max[1][3] = 17492;
    RevB_Gain[4] = .752091f;
    RevB_Max[0][4] = 22596;
    RevB_Max[1][4] = 22649;
    RevB_Gain[5] = -.652835f;
    RevB_Max[0][5] = 27675;
    RevB_Max[1][5] = 27695;
    RevB_Gain[6] = -.672523f;
    RevB_Max[0][6] = 31374;
    RevB_Max[1][6] = 31529;
    RevB_Gain[7] = .584632f;
    RevB_Max[0][7] = 35271;
    RevB_Max[1][7] = 36082;
    RevB_Gain[8] = -.447742f;
    RevB_Max[0][8] = 40568;
    RevB_Max[1][8] = 40761;
    RevB_Gain[9] = .400295f;
    RevB_Max[0][9] = 44510;
    RevB_Max[1][9] = 44935;
    RevB_Gain[10] = .469205f;
    RevB_Max[0][10] = 50172;
    RevB_Max[1][10] = 49330;
    RevB_Gain[11] = .524922f;
    RevB_Max[0][11] = 54031;
    RevB_Max[1][11] = 53558;
    RevB_Gain[12] = .814592f;
    RevB_Max[0][12] = 58624;
    RevB_Max[1][12] = 57894;
    RevB_Gain[13] = .994497f;
    RevB_Max[0][13] = 63459;
    RevB_Max[1][13] = 62793;
    RevB_Gain[14] = -.700032f;
    RevB_Max[0][14] = 67233;
    RevB_Max[1][14] = 67490;
    RevB_Gain[15] = .151264f;
    RevB_Max[0][15] = 71271;
    RevB_Max[1][15] = 72428;

}

//R4.00 Fields
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode10()
{
    //GOOD Fields
    RevB_Gain[0] = .009332f;
    RevB_Max[0][0] = 4343;
    RevB_Max[1][0] = 3764;
    RevB_Gain[1] = -.013183f;
    RevB_Max[0][1] = 9015;
    RevB_Max[1][1] = 8878;
    RevB_Gain[2] = .027363f;
    RevB_Max[0][2] = 12972;
    RevB_Max[1][2] = 13549;
    RevB_Gain[3] = -.042631f;
    RevB_Max[0][3] = 18523;
    RevB_Max[1][3] = 18641;
    RevB_Gain[4] = -.068719f;
    RevB_Max[0][4] = 22195;
    RevB_Max[1][4] = 22211;
    RevB_Gain[5] = -.072181f;
    RevB_Max[0][5] = 26412;
    RevB_Max[1][5] = 27219;
    RevB_Gain[6] = .096924f;
    RevB_Max[0][6] = 32099;
    RevB_Max[1][6] = 31463;
    RevB_Gain[7] = -.132279f;
    RevB_Max[0][7] = 36640;
    RevB_Max[1][7] = 35961;
    RevB_Gain[8] = .220779f;
    RevB_Max[0][8] = 40977;
    RevB_Max[1][8] = 40892;
    RevB_Gain[9] = .295687f;
    RevB_Max[0][9] = 44537;
    RevB_Max[1][9] = 45088;
    RevB_Gain[10] = .422045f;
    RevB_Max[0][10] = 49012;
    RevB_Max[1][10] = 48921;
    RevB_Gain[11] = -.451307f;
    RevB_Max[0][11] = 53432;
    RevB_Max[1][11] = 53339;
    RevB_Gain[12] = .743298f;
    RevB_Max[0][12] = 57827;
    RevB_Max[1][12] = 58642;
    RevB_Gain[13] = 1.000000f;
    RevB_Max[0][13] = 63136;
    RevB_Max[1][13] = 62496;
    RevB_Gain[14] = -.710352f;
    RevB_Max[0][14] = 67422;
    RevB_Max[1][14] = 67533;
    RevB_Gain[15] = -.263777f;
    RevB_Max[0][15] = 72315;
    RevB_Max[1][15] = 71783;
   
}

//R4.00 Garage
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode11()
{
    RevB_Gain[0] = -1.185651E-03f;
    RevB_Max[0][0] = 4332;
    RevB_Max[1][0] = 5132;
    RevB_Gain[1] = .1188914f;
    RevB_Max[0][1] = 8402;
    RevB_Max[1][1] = 9239;
    RevB_Gain[2] = -.3848826f;
    RevB_Max[0][2] = 13317;
    RevB_Max[1][2] = 14223;
    RevB_Gain[3] = .3194438f;
    RevB_Max[0][3] = 17367;
    RevB_Max[1][3] = 18007;
    RevB_Gain[4] = .1176973f;
    RevB_Max[0][4] = 22903;
    RevB_Max[1][4] = 22174;
    RevB_Gain[5] = -.5027322f;
    RevB_Max[0][5] = 26266;
    RevB_Max[1][5] = 26931;
    RevB_Gain[6] = .7059703f;
    RevB_Max[0][6] = 32144;
    RevB_Max[1][6] = 30791;
    RevB_Gain[7] = -.7168727f;
    RevB_Max[0][7] = 35775;
    RevB_Max[1][7] = 36704;
    RevB_Gain[8] = .992442f;
    RevB_Max[0][8] = 39802;
    RevB_Max[1][8] = 39754;
    RevB_Gain[9] = -1.0f;
    RevB_Max[0][9] = 45444;
    RevB_Max[1][9] = 45392;
    RevB_Gain[10] = .6304718f;
    RevB_Max[0][10] = 49529;
    RevB_Max[1][10] = 49081;
    RevB_Gain[11] = -.2074285f;
    RevB_Max[0][11] = 53819;
    RevB_Max[1][11] = 54077;
    RevB_Gain[12] = -4.904115E-02f;
    RevB_Max[0][12] = 58918;
    RevB_Max[1][12] = 58958;
    RevB_Gain[13] = .3219758f;
    RevB_Max[0][13] = 63431;
    RevB_Max[1][13] = 62385;
    RevB_Gain[14] = -.5606492f;
    RevB_Max[0][14] = 66935;
    RevB_Max[1][14] = 66956;
    RevB_Gain[15] = .6529667f;
    RevB_Max[0][15] = 72664;
    RevB_Max[1][15] = 71883;
}

//R4.00 TremVerb
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode12()
{       
    RevB_Gain[0] = -1.185651E-03f;
    RevB_Max[0][0] = 4332;
    RevB_Max[1][0] = 5132;
    RevB_Gain[1] = .1188914f;
    RevB_Max[0][1] = 8402;
    RevB_Max[1][1] = 9239;
    RevB_Gain[2] = -.3848826f;
    RevB_Max[0][2] = 13317;
    RevB_Max[1][2] = 14223;
    RevB_Gain[3] = .3194438f;
    RevB_Max[0][3] = 17367;
    RevB_Max[1][3] = 18007;
    RevB_Gain[4] = .1176973f;
    RevB_Max[0][4] = 22903;
    RevB_Max[1][4] = 22174;
    RevB_Gain[5] = -.5027322f;
    RevB_Max[0][5] = 26266;
    RevB_Max[1][5] = 26931;
    RevB_Gain[6] = .7059703f;
    RevB_Max[0][6] = 32144;
    RevB_Max[1][6] = 30791;
    RevB_Gain[7] = -.7168727f;
    RevB_Max[0][7] = 35775;
    RevB_Max[1][7] = 36704;
    RevB_Gain[8] = .992442f;
    RevB_Max[0][8] = 39802;
    RevB_Max[1][8] = 39754;
    RevB_Gain[9] = -1.0f;
    RevB_Max[0][9] = 45444;
    RevB_Max[1][9] = 45392;
    RevB_Gain[10] = .6304718f;
    RevB_Max[0][10] = 49529;
    RevB_Max[1][10] = 49081;
    RevB_Gain[11] = -.2074285f;
    RevB_Max[0][11] = 53819;
    RevB_Max[1][11] = 54077;
    RevB_Gain[12] = -4.904115E-02f;
    RevB_Max[0][12] = 58918;
    RevB_Max[1][12] = 58958;
    RevB_Gain[13] = .3219758f;
    RevB_Max[0][13] = 63431;
    RevB_Max[1][13] = 62385;
    RevB_Gain[14] = -.5606492f;
    RevB_Max[0][14] = 66935;
    RevB_Max[1][14] = 66956;
    RevB_Gain[15] = .6529667f;
    RevB_Max[0][15] = 72664;
    RevB_Max[1][15] = 71883;

}

//R4.00 MEDIUM TILE
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode13()
{
    RevB_Gain[0] = -.3509227f;
    RevB_Max[0][0] = 5117;
    RevB_Max[1][0] = 4444;
    RevB_Gain[1] = -.1050734f;
    RevB_Max[0][1] = 8476;
    RevB_Max[1][1] = 8995;
    RevB_Gain[2] = 1.0f;
    RevB_Max[0][2] = 14120;
    RevB_Max[1][2] = 13104;
    RevB_Gain[3] = -.6315676f;
    RevB_Max[0][3] = 18359;
    RevB_Max[1][3] = 17634;
    RevB_Gain[4] = -2.099007E-02f;
    RevB_Max[0][4] = 22219;
    RevB_Max[1][4] = 22141;
    RevB_Gain[5] = .2094507f;
    RevB_Max[0][5] = 26623;
    RevB_Max[1][5] = 26586;
    RevB_Gain[6] = -.1916206f;
    RevB_Max[0][6] = 30998;
    RevB_Max[1][6] = 31768;
    RevB_Gain[7] = .3985071f;
    RevB_Max[0][7] = 35725;
    RevB_Max[1][7] = 35703;
    RevB_Gain[8] = -.4198231f;
    RevB_Max[0][8] = 40251;
    RevB_Max[1][8] = 40072;
    RevB_Gain[9] = 9.180727E-02f;
    RevB_Max[0][9] = 45498;
    RevB_Max[1][9] = 45205;
    RevB_Gain[10] = .0670765f;
    RevB_Max[0][10] = 50160;
    RevB_Max[1][10] = 49975;
    RevB_Gain[11] = -3.344628E-02f;
    RevB_Max[0][11] = 53268;
    RevB_Max[1][11] = 53831;
    RevB_Gain[12] = -4.036823E-02f;
    RevB_Max[0][12] = 57851;
    RevB_Max[1][12] = 58572;
    RevB_Gain[13] = -6.195487E-02f;
    RevB_Max[0][13] = 63347;
    RevB_Max[1][13] = 63363;
    RevB_Gain[14] = .1139222f;
    RevB_Max[0][14] = 67821;
    RevB_Max[1][14] = 67838;
    RevB_Gain[15] = -.08f;
    RevB_Max[0][15] = 71401;
    RevB_Max[1][15] = 71525;
}

//R4.00 Dinner Plate
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode14()
{
    RevB_Gain[0] = .882653f;
    RevB_Max[0][0] = 2797;
    RevB_Max[1][0] = 8253;
    RevB_Gain[1] = 1.000000f;
    RevB_Max[0][1] = 4495;
    RevB_Max[1][1] = 19103;
    RevB_Gain[2] = .834467f;
    RevB_Max[0][2] = 9303;
    RevB_Max[1][2] = 24031;
    RevB_Gain[3] = -.827415f;
    RevB_Max[0][3] = 13646;
    RevB_Max[1][3] = 32061;
    RevB_Gain[4] = .657762f;
    RevB_Max[0][4] = 17070;
    RevB_Max[1][4] = 42189;
    RevB_Gain[5] = .668796f;
    RevB_Max[0][5] = 21673;
    RevB_Max[1][5] = 47350;
    RevB_Gain[6] = .502950f;
    RevB_Max[0][6] = 23964;
    RevB_Max[1][6] = 51095;
    RevB_Gain[7] = .478121f;
    RevB_Max[0][7] = 26564;
    RevB_Max[1][7] = 56727;
    RevB_Gain[8] = -.904287f; //3
    RevB_Max[0][8] = 29061;
    RevB_Max[1][8] = 60993;
    RevB_Gain[9] = .236036f;
    RevB_Max[0][9] = 33681;
    RevB_Max[1][9] = 64820;
    RevB_Gain[10] = -.182936f;
    RevB_Max[0][10] = 36324;
    RevB_Max[1][10] = 69460;
    RevB_Gain[11] = .090437f;
    RevB_Max[0][11] = 43384;
    RevB_Max[1][11] = 70272;
    RevB_Gain[12] = -.059889f;
    RevB_Max[0][12] = 47660;
    RevB_Max[1][12] = 75748;
    RevB_Gain[13] = .030914f;
    RevB_Max[0][13] = 59515;
    RevB_Max[1][13] = 62274;
    RevB_Gain[14] = .016609f;
    RevB_Max[0][14] = 69846;
    RevB_Max[1][14] = 66988;
    RevB_Gain[15] = -.020000f;
    RevB_Max[0][15] = 72771;
    RevB_Max[1][15] = 72454;
}

//R4.00 Garage 2
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode15()
{
    RevB_Gain[0] = 1.000000f;
    RevB_Max[0][0] = 4582;
    RevB_Max[1][0] = 4125;
    RevB_Gain[1] = -.789672f;
    RevB_Max[0][1] = 9198;
    RevB_Max[1][1] = 9032;
    RevB_Gain[2] = -.257851f;
    RevB_Max[0][2] = 13401;
    RevB_Max[1][2] = 14091;
    RevB_Gain[3] = .237319f;
    RevB_Max[0][3] = 17413;
    RevB_Max[1][3] = 17424;
    RevB_Gain[4] = -.310135f;
    RevB_Max[0][4] = 22990;
    RevB_Max[1][4] = 22406;
    RevB_Gain[5] = .129198f;
    RevB_Max[0][5] = 27092;
    RevB_Max[1][5] = 27383;
    RevB_Gain[6] = .164248f;
    RevB_Max[0][6] = 30997;
    RevB_Max[1][6] = 32097;
    RevB_Gain[7] = -.072691f;
    RevB_Max[0][7] = 35253;
    RevB_Max[1][7] = 35574;
    RevB_Gain[8] = .021599f;
    RevB_Max[0][8] = 40780;
    RevB_Max[1][8] = 41063;
    RevB_Gain[9] = .125907f;
    RevB_Max[0][9] = 45292;
    RevB_Max[1][9] = 45028;
    RevB_Gain[10] = -.083202f;
    RevB_Max[0][10] = 50082;
    RevB_Max[1][10] = 49038;
    RevB_Gain[11] = -.005068f;
    RevB_Max[0][11] = 53597;
    RevB_Max[1][11] = 54298;
    RevB_Gain[12] = .014442f;
    RevB_Max[0][12] = 58091;
    RevB_Max[1][12] = 58928;
    RevB_Gain[13] = -.023028f;
    RevB_Max[0][13] = 63543;
    RevB_Max[1][13] = 63478;
    RevB_Gain[14] = .013807f;
    RevB_Max[0][14] = 66868;
    RevB_Max[1][14] = 68024;
    RevB_Gain[15] = .003245f;
    RevB_Max[0][15] = 71359;
    RevB_Max[1][15] = 71604;
}

//R4.00 INVERTED
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode16()
{   
    RevB_Gain[0] = -.002118f;
    RevB_Max[0][0] = 4076;
    RevB_Max[1][0] = 3934;
    RevB_Gain[1] = -.059591f;
    RevB_Max[0][1] = 8904;
    RevB_Max[1][1] = 9517;
    RevB_Gain[2] = .100794f;
    RevB_Max[0][2] = 13141;
    RevB_Max[1][2] = 12783;
    RevB_Gain[3] = .008219f;
    RevB_Max[0][3] = 18593;
    RevB_Max[1][3] = 17609;
    RevB_Gain[4] = .122832f;
    RevB_Max[0][4] = 21753;
    RevB_Max[1][4] = 22156;
    RevB_Gain[5] = -.049918f;
    RevB_Max[0][5] = 27725;
    RevB_Max[1][5] = 27334;
    RevB_Gain[6] = -.237285f;
    RevB_Max[0][6] = 31590;
    RevB_Max[1][6] = 31984;
    RevB_Gain[7] = .035933f;
    RevB_Max[0][7] = 36600;
    RevB_Max[1][7] = 36011;
    RevB_Gain[8] = -.184627f;
    RevB_Max[0][8] = 41039;
    RevB_Max[1][8] = 40074;
    RevB_Gain[9] = .062678f;
    RevB_Max[0][9] = 44392;
    RevB_Max[1][9] = 44328;
    RevB_Gain[10] = .420404f;
    RevB_Max[0][10] = 48878;
    RevB_Max[1][10] = 49065;
    RevB_Gain[11] = 1.000000f;
    RevB_Max[0][11] = 53774;
    RevB_Max[1][11] = 54494;
    RevB_Gain[12] = -.245439f;
    RevB_Max[0][12] = 58324;
    RevB_Max[1][12] = 58770;
    RevB_Gain[13] = -.010151f;
    RevB_Max[0][13] = 62400;
    RevB_Max[1][13] = 62759;
    RevB_Gain[14] = -.453049f;
    RevB_Max[0][14] = 67969;
    RevB_Max[1][14] = 67886;
    RevB_Gain[15] = -.241916f;
    RevB_Max[0][15] = 71280;
    RevB_Max[1][15] = 71816;
}

//R2.14 HANGAR
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode17()
{   
    RevB_Gain[0] = .747714f;
    RevB_Max[0][0] = 4820;
    RevB_Max[1][0] = 4701;
    RevB_Gain[1] = -1.000000f;
    RevB_Max[0][1] = 9601;
    RevB_Max[1][1] = 8925;
    RevB_Gain[2] = .509121f;
    RevB_Max[0][2] = 13416;
    RevB_Max[1][2] = 13457;
    RevB_Gain[3] = -.597189f;
    RevB_Max[0][3] = 18072;
    RevB_Max[1][3] = 17674;
    RevB_Gain[4] = .511270f;
    RevB_Max[0][4] = 23059;
    RevB_Max[1][4] = 22736;
    RevB_Gain[5] = -.238252f;
    RevB_Max[0][5] = 27485;
    RevB_Max[1][5] = 26555;
    RevB_Gain[6] = .680602f; //3
    RevB_Max[0][6] = 30927;
    RevB_Max[1][6] = 30971;
    RevB_Gain[7] = -.813804f; //3
    RevB_Max[0][7] = 36641;
    RevB_Max[1][7] = 35888;
    RevB_Gain[8] = .321968f;
    RevB_Max[0][8] = 39908;
    RevB_Max[1][8] = 40466;
    RevB_Gain[9] = -.304256f;
    RevB_Max[0][9] = 44715;
    RevB_Max[1][9] = 44861;
    RevB_Gain[10] = -.236970f;
    RevB_Max[0][10] = 48778;
    RevB_Max[1][10] = 49363;
    RevB_Gain[11] = .197365f;
    RevB_Max[0][11] = 54377;
    RevB_Max[1][11] = 54183;
    RevB_Gain[12] = .165404f; 
    RevB_Max[0][12] = 57755;
    RevB_Max[1][12] = 58975;
    RevB_Gain[13] = -.174817f;
    RevB_Max[0][13] = 63285;
    RevB_Max[1][13] = 63105;
    RevB_Gain[14] = -.144482f;
    RevB_Max[0][14] = 66932;
    RevB_Max[1][14] = 67997;
    RevB_Gain[15] = .102314f;
    RevB_Max[0][15] = 72446;
    RevB_Max[1][15] = 71318;
}

//R4.00 INVERTED 2
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode18()
{    
    RevB_Gain[0] = .102314f;
    RevB_Max[0][0] = 3969;
    RevB_Max[1][0] = 4816;
    RevB_Gain[1] = -.144482f;
    RevB_Max[0][1] = 9071;
    RevB_Max[1][1] = 9577;
    RevB_Gain[2] = -.174817f;
    RevB_Max[0][2] = 12774;
    RevB_Max[1][2] = 12947;
    RevB_Gain[3] = .165404f;
    RevB_Max[0][3] = 17914;
    RevB_Max[1][3] = 18709;
    RevB_Gain[4] = .197365f;
    RevB_Max[0][4] = 22765;
    RevB_Max[1][4] = 22680;
    RevB_Gain[5] = -.236970f;
    RevB_Max[0][5] = 27465;
    RevB_Max[1][5] = 27177;
    RevB_Gain[6] = -.304256f;
    RevB_Max[0][6] = 31873;
    RevB_Max[1][6] = 31697;
    RevB_Gain[7] = .321968f;
    RevB_Max[0][7] = 35774;
    RevB_Max[1][7] = 36050;
    RevB_Gain[8] = .313804f;
    RevB_Max[0][8] = 40734;
    RevB_Max[1][8] = 40053;
    RevB_Gain[9] = .380602f;
    RevB_Max[0][9] = 44267;
    RevB_Max[1][9] = 45266;
    RevB_Gain[10] = -.238252f;
    RevB_Max[0][10] = 48874;
    RevB_Max[1][10] = 49639;
    RevB_Gain[11] = .511270f;
    RevB_Max[0][11] = 53364;
    RevB_Max[1][11] = 53285;
    RevB_Gain[12] = -.597189f;
    RevB_Max[0][12] = 58762;
    RevB_Max[1][12] = 58017;
    RevB_Gain[13] = .509121f;
    RevB_Max[0][13] = 62972;
    RevB_Max[1][13] = 63106;
    RevB_Gain[14] = -1.000000f;
    RevB_Max[0][14] = 68240;
    RevB_Max[1][14] = 67634;
    RevB_Gain[15] = .747714f;
    RevB_Max[0][15] = 71849;
    RevB_Max[1][15] = 72172;
}

//R4.00 REGENT
void MakoDist01AudioProcessor::Mako_FX_Reverb_SetMode19()
{   
    
    RevB_Gain[0] = 0.5f;
    RevB_Max[0][0] = 1210;
    RevB_Max[1][0] = 2210;
    RevB_Gain[1] = 6.0f;
    RevB_Max[0][1] = 210;
    RevB_Max[1][1] = 410;
    RevB_Gain[2] = .75f;
    RevB_Max[0][2] = 158;
    RevB_Max[1][2] = 358;
    RevB_Gain[3] = .625f;
    RevB_Max[0][3] = 561;
    RevB_Max[1][3] = 761;
    RevB_Gain[4] = .625f;
    RevB_Max[0][4] = 410;
    RevB_Max[1][4] = 610;
    RevB_Gain[5] = .7f;
    RevB_Max[0][5] = 908;
    RevB_Max[1][5] = 1108;
    RevB_Gain[6] = .7f;
    RevB_Max[0][6] = 1343;
    RevB_Max[1][6] = 1543;
    RevB_Gain[7] = .3f;
    RevB_Max[0][7] = 995;
    RevB_Max[1][7] = 1195;
    RevB_Gain[8] = .3f;
    RevB_Max[0][8] = 6590;
    RevB_Max[1][8] = 6790;
    RevB_Gain[9] = .5f;
    RevB_Max[0][9] = 3931;
    RevB_Max[1][9] = 4131;
    RevB_Gain[10] = .5f;
    RevB_Max[0][10] = 2664;
    RevB_Max[1][10] = 2864;
    RevB_Gain[11] = .3f;
    RevB_Max[0][11] = 4641;
    RevB_Max[1][11] = 4841;
    RevB_Gain[12] = .3f;
    RevB_Max[0][12] = 5505;
    RevB_Max[1][12] = 5705;
    RevB_Gain[13] = .25f;
    RevB_Max[0][13] = 9300;
    RevB_Max[1][13] = 9500;
    RevB_Gain[14] = .25f;
    RevB_Max[0][14] = 12300;
    RevB_Max[1][14] = 12500;
    RevB_Gain[15] = .2f;
    RevB_Max[0][15] = 14500;
    RevB_Max[1][15] = 14700;

    for (int t = 0; t < 16; t++)
    {
        RevB_Max[0][t] *= 5;
        RevB_Max[1][t] *= 5;
    }
    

}

//R4.00 Apply reverb to the incoming sample.
float MakoDist01AudioProcessor::Mako_FX_Reverb(float tSample, int channel)
{
    float tS;
        
    //R2.22 Deal with PreDelay. 
    tS = PreDBuff[channel][PreDCurrIdx[channel]];
    PreDBuff[channel][PreDCurrIdx[channel]] = tSample;
    PreDCurrIdx[channel]++; if (PreDEnd < PreDCurrIdx[channel]) PreDCurrIdx[channel] = 0;

    //R5.00 Keep track of our volume for ducking. Values .99 and .01 should be sample rate dependent. 
    Reverb_Duck_Peak[channel] *= RATI1000ms; 
    Reverb_Duck_Peak[channel] = (Reverb_Duck_Peak[channel] * .99f) + abs(tSample * .01f);

    //R4.30 Render the reverb, passing in both the sample and the delayed sample (tS).
    switch (Reverb_Render_Mode)
    {
        case 1: tS = Mako_FX_Reverb_Chorus(tSample, tS, channel); break;
        case 2: tS = Mako_FX_Reverb_Flange(tSample, tS, channel); break;
        case 3: tS = Mako_FX_Reverb_Mod(tSample, tS, channel); break;        
    }

    return tS;
}

//R5.00 Moved setting check out of the func.
float MakoDist01AudioProcessor::Mako_FX_Reverb_Ducking(float tSample, int channel)
{
    //R5.00 Create a volume envelope based on Signal Average.
    //R5.00 Set ducking gain to 10.0f to adjust to new peak averaging.
    Reverb_Duck_Fac[channel] = 1.0f - (Reverb_Duck_Peak[channel] * 10.0f * *Setting[e_RevDuck]);
    if (Reverb_Duck_Fac[channel] < 0.0f) Reverb_Duck_Fac[channel] = 0.0f;

    return tSample * Reverb_Duck_Fac[channel];
}

//R4.30 16 echo buffer reverb.
float MakoDist01AudioProcessor::Mako_FX_Reverb_Chorus(float tSample, float tSVerb, int channel)
{
    float Echoes = 0.0f;

    //R2.22 Apply filters.
    float tS = Filter_Calc_BiQuad(tSVerb, channel, &fil_Rev_LP);
    if (50.001f < *Setting[e_RevHP]) tS = Filter_Calc_BiQuad(tS, channel, &fil_Rev_HP);

    //R4.30 Apply chorus.
    tS = Mako_FX_ReverbChorus(tS, channel) * .2f;

    //R4.30 Get current echoes. Store new echoes. Precalc Reverb Time and scaling.
    float Buffer_Fac = *Setting[e_RevTime] * .8f;
    for (int t = 0; t < Rev_Buffer_Count; t++)
    {
        //R4.30 Get current Echoes.
        Echoes += (Rev_Buffer[channel][t][RevB_Idx[channel][t]] * RevB_Gain[t]);

        //R4.30 Mix Buffer + New(tS)
        Rev_Buffer[channel][t][RevB_Idx[channel][t]] = (Rev_Buffer[channel][t][RevB_Idx[channel][t]] * Buffer_Fac) + tS;

        //R2.14 Inc buffer Index.
        RevB_Idx[channel][t]++;
        if (RevB_Max[channel][t] < RevB_Idx[channel][t]) RevB_Idx[channel][t] = 0;
    }
        
    //R5.00 Do ducking if required.
    if (*Setting[e_RevDuck]) Echoes = Mako_FX_Reverb_Ducking(Echoes, channel);

    //R4.30 Mix the dry and wet (reverb) signals. Apply Ducking.
    return (tSample * Reverb_Dry) + (Echoes * Rev_Echo_Gain * Reverb_Wet) * Reverb_BalLR[channel];
}


float MakoDist01AudioProcessor::Mako_FX_Reverb_Flange(float tSample, float tSVerb, int channel)
{
    float Echoes = 0.0f;

    //R4.30 Apply filters.
    float tS = Filter_Calc_BiQuad(tSVerb, channel, &fil_Rev_LP);
    if (50.001f < *Setting[e_RevHP]) tS = Filter_Calc_BiQuad(tS, channel, &fil_Rev_HP);

    //R4.30 Scale for mixing with the delay buffers. tS + Buff should be 1.0 or less.
    tS *= .2f;

    //R4.30 Store the new signal in the delay buffers. Accumulate echoes as we go.
    float Buffer_Fac = *Setting[e_RevTime] * .8f;
    for (int t = 0; t < 16; t++)
    {
        //R4.30 Accumulate echoes.
        Echoes += Rev_Buffer[channel][t][RevB_Idx[channel][t]];

        //R4.30 Evenly mix Buffer + New(tS)
        Rev_Buffer[channel][t][RevB_Idx[channel][t]] = (Rev_Buffer[channel][t][RevB_Idx[channel][t]] * Buffer_Fac) + tS; 

        //R2.14 Inc buffer Index.
        RevB_Idx[channel][t]++;
        if (RevB_Max[channel][t] < RevB_Idx[channel][t]) RevB_Idx[channel][t] = 0;
    }

    //R4.30 Apply Reverb Flange.
    Echoes = Mako_FX_ReverbFlange(Echoes, channel);

    //R5.00 Do ducking if required.
    if (*Setting[e_RevDuck]) Echoes = Mako_FX_Reverb_Ducking(Echoes, channel);

    //R4.30 Mix the dry and wet (reverb) signals. We need to add some vol for long decay times (x20).
    return (tSample * Reverb_Dry) + (Echoes * Reverb_Wet) * Reverb_BalLR[channel];
}


float MakoDist01AudioProcessor::Mako_FX_Reverb_Mod(float tSample, float tSVerb, int channel)
{
    float Echoes = 0.0f;

    //R2.22 Apply filters.
    float tS = tSVerb; //R2.22 Added PreDelay.
    tS = Filter_Calc_BiQuad(tS, channel, &fil_Rev_LP);
    if (50.001f < *Setting[e_RevHP]) tS = Filter_Calc_BiQuad(tS, channel, &fil_Rev_HP);

    //R4.30 Scale to (tS + Echo) to equal 1.0 with buffer below (.2 + .8).
    tS *= .2f;

    //R5.00 Store new echoes. Reduced to only 8 echoes instead of the full 16.
    float Buffer_Fac = *Setting[e_RevTime] * .8f;
    for (int t = 0; t < 8; t++)
    {
        //R4.30 Get current echoes.
        Echoes += Rev_Buffer[channel][t][RevB_Idx[channel][t]];

        //R4.30 Mix Buffer + New(tS).
        Rev_Buffer[channel][t][RevB_Idx[channel][t]] = (Rev_Buffer[channel][t][RevB_Idx[channel][t]] * Buffer_Fac) + tS; 

        //R2.14 Inc buffer Index.
        RevB_Idx[channel][t]++;
        if (RevB_Max[channel][t] < RevB_Idx[channel][t]) RevB_Idx[channel][t] = 0;
    }
    
    //R4.30 Update the oscillator.
    RevMod_Osc1[channel] += RevMod_Osc1_Rate[channel] * *Setting[e_RevChor];

    //R4.30 Alter our oscillation direction if needed.
    if (RevMod_Osc1[channel] < 0.0f)
        RevMod_Osc1_Rate[channel] = RevMod_B_Rate_Up * .1f;   //R5.00 Based on Reverb Chorus Rate!
    else if (1.0f < RevMod_Osc1[channel])
        RevMod_Osc1_Rate[channel] = RevMod_B_Rate_Down * .1f; //R5.00 Based on Reverb Chorus Rate!
    
    //R4.30 Osciallate the echoes.
    Echoes = Echoes * RevMod_Osc1[channel];

    //R5.00 Do ducking if required.
    if (*Setting[e_RevDuck]) Echoes = Mako_FX_Reverb_Ducking(Echoes, channel);

    //R4.30 Mix the dry and wet (reverb) signals.
    return (tSample * Reverb_Dry) + (Echoes * Reverb_Wet) * Reverb_BalLR[channel];
}

//R4.00 Fixed setting Chorus used in the reverb block.
float MakoDist01AudioProcessor::Mako_FX_ReverbChorus(float tSample, int channel)
{
    float idxf;
    int idx;
    int idx2;

    //R1.00 Store new sample in Ring buffer. 
    RevMod_B[channel][RevMod_B_Ring1[channel]] = tSample;

    //R5.00 Calc modulating index and the next sample to interpolate with.
    idxf = RevMod_B_Ring1[channel] - RevMod_B_Offset[channel];
    idx = int(idxf);
    float idxDiff = idxf - idx;
    if (idx < 0) idx = idx + 2048;
    idx2 = idx - 1;
    if (idx2 < 0) idx2 = idx2 + 2048;

    //R5.00 Interpolate between samples to remove crackle/disconinuity in modulated signal.
    float tModulated = (RevMod_B[channel][idx] * idxDiff) + (RevMod_B[channel][idx2] * (1.0f - idxDiff));
            
    //R1.00 Update our rotating index.
    RevMod_B_Offset[channel] += RevMod_B_Rate[channel];

    //R1.00 Alter our delay direction if needed.
    if (RevMod_B_Offset[channel] < RevMod_B_Offset_Min[channel])
        RevMod_B_Rate[channel] = RevMod_B_Rate_Up;
    else if (RevMod_B_Offset_Max[channel] < RevMod_B_Offset[channel])
        RevMod_B_Rate[channel] = RevMod_B_Rate_Down;

    //R5.00 Update ring buffer position. Mask the values to keep it to 0-2048.
    RevMod_B_Ring1[channel] = (RevMod_B_Ring1[channel] + 1) & 0x7FF;
    
    //R4.00 Mix the dry and wet (chorus) signals. 
    return (tSample * (1.0f - *Setting[e_RevChor])) + (tModulated * *Setting[e_RevChor]);
}


//R4.30 Fixed setting Flanger used in the reverb block.
float MakoDist01AudioProcessor::Mako_FX_ReverbFlange(float tSample, int channel)
{
    float idxf;
    int idx;
    int idx2;

    //R4.30 Store new sample in Ring buffer. 
    RevMod_B[channel][RevMod_B_Ring1[channel]] = tSample;

    //R5.00 Calc modulating index and the next sample to interpolate with.
    idxf = RevMod_B_Ring1[channel] - RevMod_B_Offset[channel];
    idx = int(idxf);
    float idxDiff = idxf - idx;
    if (idx < 0) idx = idx + 2048;
    idx2 = idx - 1;
    if (idx2 < 0) idx2 = idx2 + 2048;

    //R5.00 Interpolate between samples to remove crackle/disconinuity in modulated signal.
    float tModulated = (RevMod_B[channel][idx] * idxDiff) + (RevMod_B[channel][idx2] * (1.0f - idxDiff));

    //R4.30 Update our rotating index.
    RevMod_B_Offset[channel] += (RevMod_B_Rate[channel] * Rev_Flange_Rate); 

    //R4.30 Alter our delay direction if needed.
    if (RevMod_B_Offset[channel] < 0.0f)
        RevMod_B_Rate[channel] = RevMod_B_Rate_Up;
    else if (Rev_Flange_Limit < RevMod_B_Offset[channel])  
        RevMod_B_Rate[channel] = RevMod_B_Rate_Down;

    //R4.30 Update ring buffer position. Wrap around to 0 when we exceed the buffer size.
    RevMod_B_Ring1[channel] = (RevMod_B_Ring1[channel] + 1) & 0x07FF;
   
    //R4.30 Mix the dry and wet (chorus) signals. 
    return (tSample * (1.0f - *Setting[e_RevChor])) + (-tModulated * *Setting[e_RevChor]);
}



//**********************************************************************
//S P E A K E R   I M P U L S E   R E S P O N S E 
//**********************************************************************
void MakoDist01AudioProcessor::Mako_IR_Set()
{   
    Mako_SetIR_Fill(int(*Setting[e_IR]), &IR_Speaker[0]);
}

//R4.00 Fill an IR buffer with the selected IR.
void MakoDist01AudioProcessor::Mako_SetIR_Fill(int Idx, float IR[1024])
{   
    //R5.00 Set IR to a preset IR (0 - 19) or the current database IR (IR_Buffer) from the editor.
    if (Idx < 20)
    {
        switch (Idx)
        {
        case 0: IR_Build[0] = 1.0f; for (int t = 1; t < 1024; t++) IR_Build[t] = 0.0f; break;
        case 1:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR1[t]; break;
        case 2:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR2[t]; break;
        case 3:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR3[t]; break;
        case 4:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR4[t]; break;
        case 5:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR5[t]; break;
        case 6:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR6[t]; break;
        case 7:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR7[t]; break;
        case 8:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR8[t]; break;
        case 9:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR9[t]; break;
        case 10:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR10[t]; break;
        case 11:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR11[t]; break;
        case 12:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR12[t]; break;
        case 13:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR13[t]; break;
        case 14:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR14[t]; break;
        case 15:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR15[t]; break;
        case 16:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR16[t]; break;
        case 17:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR17[t]; break;
        case 18:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR18[t]; break;
        case 19:  for (int t = 0; t < 1024; t++) IR_Build[t] = TIR19[t]; break;
        //default:  for (int t = 0; t < 1024; t++) IR_Build[t] = IR_FromFile[t]; break;
        }
    }
    else
    {
        FILE_Mako_IR_Load_Wave(DefIR_Path[Idx]);
        for (int t = 0; t < 1024; t++) IR_Build[t] = IR_FromFile[t];
    }

    //R5.00 Resize and normalize the current IR (IR_Build).
    Mako_SetIR_Size(&IR[0]);    
}

//R5.00 Resample and Normalize the current build IR to alter its "size".
void MakoDist01AudioProcessor::Mako_SetIR_Size(float IR[1024])
{
    float Max = 0.0f;

    //R1.00 Apply speaker size if selected. 
    if ((*Setting[e_IRSize] < .0f) || (.0f < *Setting[e_IRSize]))
    {
        int t = 0;
        float t2 = 0.0f;
        int t2i = 0;
        float t2diff = 0.0f;
        float t2s = (1024.0f + ((*Setting[e_IRSize] * -.5f) * 1000.0f)) / 1024.0f;

        //R1.00 Resample the IR using new specs.
        while ((t < 1024) && (t2 < 1023.0001f))
        {
            t2i = int(t2);
            t2diff = t2 - t2i;
            IR[t] = (IR_Build[t2i] * (1.0f - t2diff)) + (IR_Build[t2i + 1] * t2diff);
            if (Max < abs(IR[t])) Max = abs(IR[t]);
            t++;
            t2 += t2s;
        }

        //R1.00 Clear the unused values or the volume explodes when Maxed/Normalized.
        if (t < 1024)
        {
            while (t < 1024)
            {
                IR[t] = 0.0f;
                t++;
            }
        }
    }
    else
    {
        //R1.00 Default of 0 is selected.
        //R1.00 Do not apply Speaker Size/Time stretching.
        for (int t = 0; t < 1024; t++)
        {
            IR[t] = IR_Build[t];
            if (Max < abs(IR[t])) Max = abs(IR[t]);
        }
    }

    //R1.00 Normalize.
    if ((0.0f < Max) && (Max < 1.0f))
        Max = 1.0f / Max;
    else
        Max = 1.0f;

    for (int t = 0; t < 1024; t++) IR[t] *= Max;

}

