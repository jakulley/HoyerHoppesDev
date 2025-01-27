#include "plugin.hpp"
#include <vector>


struct Scanning_clock_multiplier_v2 : Module {
	enum ParamId {
		PHASE_POSITION_PARAM,
		RATE_PARAM,
		RESET_PARAM,
		SKEW_PARAM,
		SCAN_PARAM,
		_1_1_PARAM,
		_8_5_PARAM,
		_2_1_PARAM,
		_8_3_PARAM,
		_3_1_PARAM,
		_16_5_PARAM,
		_4_1_PARAM,
		_5_1_PARAM,
		_16_3_PARAM,
		_6_1_PARAM,
		_32_5_PARAM,
		_7_1_PARAM,
		_8_1_PARAM,
		_32_3_PARAM,
		_16_1_PARAM,
		_32_1_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SKEW_CV_INPUT,
		RATE_IN_INPUT,
		RESET_INPUT,
		SCAN_CV_INPUT,
		_1_1_INPUT,
		_8_5_INPUT,
		_2_1_INPUT,
		_8_3_INPUT,
		_3_1_INPUT,
		_16_5_INPUT,
		_4_1_INPUT,
		_5_1_INPUT,
		_16_3_INPUT,
		_6_1_INPUT,
		_32_5_INPUT,
		_7_1_INPUT,
		_8_1_INPUT,
		_32_3_INPUT,
		_16_1_INPUT,
		_32_1_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		RATE_OUTPUT,
		RESET_OUTPUT,
		RATE_PHASE_OUTPUT,
		MAIN_OUTPUT,
		SCAN_PHASE_OUTPUT,
		REFL_SCAN_PHASE_OUTPUT,
		HIGHEST_OUTPUT,
		REFL_SCAN_OUTPUT,
		LOWEST_OUTPUT,
		MIDDLE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Scanning_clock_multiplier_v2() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PHASE_POSITION_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RATE_PARAM, 1.f, 700.f, 120.f, "1 to 700 bpm. cv in overrides");
		configButton(RESET_PARAM, "");
		configParam(SKEW_PARAM, 0.11f, 10.f, 1.f, "adds exponential or logarithmic curve to output");
		configParam(SCAN_PARAM, 0.f, 1.f, 0.f, "scan active ratios. sums with cv in");
		configSwitch(_1_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_8_5_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_2_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_8_3_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_3_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_16_5_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_4_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_5_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_16_3_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_6_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_32_5_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_7_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_8_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_32_3_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_16_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_32_1_PARAM, 0.f, 1.f, 0.f, "");
		configInput(SKEW_CV_INPUT, "expects 0-10v");
		configInput(RATE_IN_INPUT, "");
		configInput(RESET_INPUT, "");
		configInput(SCAN_CV_INPUT, "expects 0-10v");
		configInput(_1_1_INPUT, "inverts button value");
		configInput(_8_5_INPUT, "inverts button value");
		configInput(_2_1_INPUT, "inverts button value");
		configInput(_8_3_INPUT, "inverts button value");
		configInput(_3_1_INPUT, "inverts button value");
		configInput(_16_5_INPUT, "inverts button value");
		configInput(_4_1_INPUT, "inverts button value");
		configInput(_5_1_INPUT, "inverts button value");
		configInput(_16_3_INPUT, "inverts button value");
		configInput(_6_1_INPUT, "inverts button value");
		configInput(_32_5_INPUT, "inverts button value");
		configInput(_7_1_INPUT, "inverts button value");
		configInput(_8_1_INPUT, "inverts button value");
		configInput(_32_3_INPUT, "inverts button value");
		configInput(_16_1_INPUT, "inverts button value");
		configInput(_32_1_INPUT, "inverts button value");
		configOutput(RATE_OUTPUT, "rate pass-thru");
		configOutput(RESET_OUTPUT, "reset pass-thru");
		configOutput(RATE_PHASE_OUTPUT, "0-10v phasor based on rate");
		configOutput(MAIN_OUTPUT, "trigs calculated by scanner and skew");
		configOutput(SCAN_PHASE_OUTPUT, "phasor calculated by scanner and skew");
		configOutput(LOWEST_OUTPUT, "trigs based on the lowest active ratio");
		configOutput(MIDDLE_OUTPUT, "trigs based on the ratio active when scan is at noon");
		configOutput(HIGHEST_OUTPUT, "trigs based on the highest active ratio");
		configOutput(REFL_SCAN_PHASE_OUTPUT, "reflects scan value around .5");
		configOutput(REFL_SCAN_OUTPUT, "reflects scan value around .5");
	}

	rack::dsp::SchmittTrigger ratioButtonTriggers[16];
	rack::dsp::SchmittTrigger ratioInputTriggers[16];
	rack::dsp::SchmittTrigger rateTrigger;
	rack::dsp::SchmittTrigger outPhaseTrigger;
	rack::dsp::SchmittTrigger highPhaseTrigger;
	rack::dsp::SchmittTrigger middlePhaseTrigger;
	rack::dsp::SchmittTrigger lowPhaseTrigger;
	rack::dsp::SchmittTrigger flipPhaseTrigger;
	rack::dsp::SchmittTrigger invScanTrigger;
	rack::dsp::PulseGenerator invScanPulse;
	rack::dsp::PulseGenerator flipPhasePulse;
	rack::dsp::PulseGenerator ratePulse;
	rack::dsp::PulseGenerator outPhasePulse;
	rack::dsp::PulseGenerator highPhasePulse;
	rack::dsp::PulseGenerator middlePhasePulse;
	rack::dsp::PulseGenerator lowPhasePulse;
	float phase = 0.f;
	float rate = 0.f;
	float period = 0.f;
	float scanValue = 0.f;
	//it's silly to make these doubles; still, floats might start returning imprecise values past 6 or so hours of runtime.
	double lastTrigTime = 0.0f;
	double currentTime = 0.f;
	float phasePos = 0.f;
	float phaseHolder = 0.f;
	bool resetHold = false;
	float skew = 0.f;

	float allRatios[16] = {1.f, 1.6f, 2.f, 2.6666667f, 3.f, 3.2f, 4.f, 5.f, 5.3333333f, 6.f, 10.6f, 7.f, 8.f, 10.6666667f, 16.f, 32.f};
	std::vector<int> activeRatios;

	float scanReturn(float allRatios[16], std::vector<int> activeRatios, float scanValue) {
		if (activeRatios.size() != 0) {
			//divide the scan range into equal sections according to the number of active ratios
			float scanInc = 1.f / (float)activeRatios.size();
			long long unsigned int i = 0;
			if (scanValue < scanInc) {
				return allRatios[activeRatios[i]];
			} else {
				while (scanValue >= scanInc * (i+1) && i < activeRatios.size()-1) {
					i++;
				}
				return allRatios[activeRatios[i]];
			}
		} else {
			return 1.f;
		}
	}

	void process(const ProcessArgs& args) override {
		//this code would be much more CPU-friendly than the for-loop in scanReturn(), if I could just get it to WORK! The problem is the buttonsOff and inputsOff bools never output true.
		//I FIGURED IT OUT! They were supposed to be Events, not bools! The buttonsOn and inputsOn bools were happening, but they got in the way of the off bools ever getting processed! 
		for (int i = 0; i < 16; i++) {
			rack::dsp::SchmittTrigger::Event buttons = ratioButtonTriggers[i].processEvent(getParam(ParamId(i+5)).getValue(), 0.1f, 0.9f);
			rack::dsp::SchmittTrigger::Event inputs = ratioInputTriggers[i].processEvent(getInput(InputId(i+4)).getVoltage(), 0.1f, 0.9f);

			if ((buttons == 1 && (!ratioInputTriggers[i].isHigh())) || ((!ratioButtonTriggers[i].isHigh()) && inputs == 1)) {
				activeRatios.push_back(i);
			}
			if ((buttons == -1 && (!ratioInputTriggers[i].isHigh())) || (inputs == -1 && (!ratioButtonTriggers[i].isHigh()))) {
      			activeRatios.erase(std::remove(activeRatios.begin(), activeRatios.end(), i), activeRatios.end());
			}
		}

		//set scan value used to determine active ratio
		scanValue = getParam(SCAN_PARAM).getValue();
		scanValue += getInput(SCAN_CV_INPUT).getVoltage()/10.f;
		//scan values exceeding 1 break the scanReturn function by accessing out-of-bounds
		float clampScan = clamp(scanValue, 0.f, 1.f);
		//reflectedScan allows you to extract a second value from a single scan knob
		float reflectedScan = 0.5f - clampScan + 0.5f;

		//set rate
		if (getInput(RATE_IN_INPUT).isConnected()) {
			if (rateTrigger.process(getInput(RATE_IN_INPUT).getVoltage())) {
				//currentTime and lastTrigTime may overflow if you leave it running for too many billions of years.
				currentTime = args.frame;
				period = currentTime - lastTrigTime;
				lastTrigTime = currentTime;
				rate = 60.f*(args.sampleRate/period);
				//resetting phaseHolder on rateTrigger=true keeps daisy-chained modules in sync
				phaseHolder = 0.f;
			}
		} else {
			rate = getParam(RATE_PARAM).getValue();
		}

		//divide phase position by active ratio to get correct phase positioning.
		phasePos = getParam(PHASE_POSITION_PARAM).getValue() / scanReturn(allRatios, activeRatios, clampScan);
		//phaseHolder is necessary to implement the phase position parameter, otherwise there's no way to increment the phase without adding an increment of phasePos every time.
		phaseHolder += (rate/60.f) * args.sampleTime;
		phase = phaseHolder + phasePos;
		//implement reset
		if (getParam(RESET_PARAM).getValue() == 1.f || getInput(RESET_INPUT).getVoltage() > 0.f) {
			phase = 0.f;
			phaseHolder = 0.f;
			getOutput(RESET_OUTPUT).setVoltage(10.f);
			resetHold = true;
		} else {
			getOutput(RESET_OUTPUT).setVoltage(0.f);
			resetHold = false;
		}
		//reset phase and phaseHolder when they reach the threshold value
		if (phaseHolder >= 1.f) {
			phaseHolder = 0.f;
			phase = 0.f;
			//threshold also serves as trigger for rate out pulse
			ratePulse.trigger();
		}
		//rate phase is the baseline phase, before any skew or multiplication
		getOutput(RATE_PHASE_OUTPUT).setVoltage(phase * 10.f);
		skew = (getInput(SKEW_CV_INPUT).isConnected() ? (getInput(SKEW_CV_INPUT).getVoltage() + 0.11f) : getParam(SKEW_PARAM).getValue());
		float skewPhase = pow(phase, skew);
		while (skewPhase >= 1.f) {
			skewPhase -= 1.f;
		}
		//pow implements the Skew param, scanReturn multiplies the phase by the selected ratio
		float outPhase = skewPhase * scanReturn(allRatios, activeRatios, clampScan);
		float invScanPhase = skewPhase * scanReturn(allRatios, activeRatios, reflectedScan);
		float highPhase = skewPhase * scanReturn(allRatios, activeRatios, 1.f);
		float middlePhase = skewPhase * scanReturn(allRatios, activeRatios, 0.5f);
		float lowPhase = skewPhase * scanReturn(allRatios, activeRatios, 0.f);
		//just like the raw phase, the calculated phases need to be trimmed past a 1.f threshold; even though phase never exceeds 1 for more than 1 sample, the pow and scanReturn functions can easily cause it to exceed 1.
		while (outPhase >= 1.f) {
			outPhase -= 1.f;
		}
		while (invScanPhase >= 1.f) {
			invScanPhase -= 1.f;
		}
		while (highPhase >= 1.f) {
			highPhase -= 1.f;
		}
		while (middlePhase >= 1.f) {
			middlePhase -= 1.f;
		}
		while (lowPhase >= 1.f) {
			lowPhase -= 1.f;
		}

		bool outPhaseTrig = outPhaseTrigger.process(outPhase, 0.01f, 0.99f);
		if (outPhaseTrig) {
			outPhasePulse.trigger();
		}
		bool highPhaseTrig = highPhaseTrigger.process(highPhase, 0.01f, 0.99f);
		if (highPhaseTrig) {
			highPhasePulse.trigger();
		}
		bool middlePhaseTrig = middlePhaseTrigger.process(middlePhase, 0.01f, 0.99f);
		if (middlePhaseTrig) {
			middlePhasePulse.trigger();
		}
		bool lowPhaseTrig = lowPhaseTrigger.process(lowPhase, 0.01f, 0.99f);
		if (lowPhaseTrig) {
			lowPhasePulse.trigger();
		}
		//outPhase can only be inverted *after* it has been calculated
		//Potentially confusing: the FLIPPED value is passed to the MAIN_OUTPUT, and the UNFLIPPED or outPhase value is passed to the INV_OUT. This is so that the main out is HIGH at the beginning of the step, rather than at the end. Important for synchronization.
		float flipPhase = 1.f - outPhase;
		bool flipPhaseTrig = flipPhaseTrigger.process(flipPhase, 0.01f, .99f);
		if (flipPhaseTrig) {
			flipPhasePulse.trigger();
		}
		bool highPulse = highPhasePulse.process(args.sampleTime);
		bool middlePulse = middlePhasePulse.process(args.sampleTime);
		bool lowPulse = lowPhasePulse.process(args.sampleTime);
		bool scanPulse = invScanPulse.process(args.sampleTime);
		getOutput(HIGHEST_OUTPUT).setVoltage(highPulse? 10.f : 0.f);
		getOutput(MIDDLE_OUTPUT).setVoltage(middlePulse? 10.f : 0.f);
		getOutput(LOWEST_OUTPUT).setVoltage(lowPulse? 10.f : 0.f);
		getOutput(REFL_SCAN_OUTPUT).setVoltage(scanPulse? 10.f : 0.f);
		getOutput(REFL_SCAN_PHASE_OUTPUT).setVoltage(invScanPhase * 10.f);
		getOutput(SCAN_PHASE_OUTPUT).setVoltage(flipPhase * 10.f);
		bool pulse = flipPhasePulse.process(args.sampleTime);
		getOutput(MAIN_OUTPUT).setVoltage(pulse ? 10.f : 0.f);
		bool rateOut = ratePulse.process(args.sampleTime);
		getOutput(RATE_OUTPUT).setVoltage(resetHold ? 0.f : (rateOut ? 10.f : 0.f));
	}
};


struct Scanning_clock_multiplier_v2Widget : ModuleWidget {
	Scanning_clock_multiplier_v2Widget(Scanning_clock_multiplier_v2* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/scanning_clock_multiplier_v2.svg"),
			asset::plugin(pluginInstance, "res/scanning_clock_multiplier_v2_dark_theme.svg")
		));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(54.719, 16.872)), module, Scanning_clock_multiplier_v2::PHASE_POSITION_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(10.44, 23.84)), module, Scanning_clock_multiplier_v2::RATE_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(21.37, 23.84)), module, Scanning_clock_multiplier_v2::RESET_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(32.706, 20.344)), module, Scanning_clock_multiplier_v2::SKEW_PARAM));
		addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(47.653, 29.435)), module, Scanning_clock_multiplier_v2::SCAN_PARAM));
		addParam(createParamCentered<_1_1Button>(mm2px(Vec(9.5, 49.525)), module, Scanning_clock_multiplier_v2::_1_1_PARAM));
		addParam(createParamCentered<_8_5Button>(mm2px(Vec(23.75, 49.525)), module, Scanning_clock_multiplier_v2::_8_5_PARAM));
		addParam(createParamCentered<_2_1Button>(mm2px(Vec(37.9, 49.525)), module, Scanning_clock_multiplier_v2::_2_1_PARAM));
		addParam(createParamCentered<_8_3Button>(mm2px(Vec(52.1, 49.525)), module, Scanning_clock_multiplier_v2::_8_3_PARAM));
		addParam(createParamCentered<_3_1Button>(mm2px(Vec(9.5, 69.525)), module, Scanning_clock_multiplier_v2::_3_1_PARAM));
		addParam(createParamCentered<_16_5Button>(mm2px(Vec(23.75, 69.525)), module, Scanning_clock_multiplier_v2::_16_5_PARAM));
		addParam(createParamCentered<_4_1Button>(mm2px(Vec(37.9, 69.525)), module, Scanning_clock_multiplier_v2::_4_1_PARAM));
		addParam(createParamCentered<_5_1Button>(mm2px(Vec(52.1, 69.525)), module, Scanning_clock_multiplier_v2::_5_1_PARAM));
		addParam(createParamCentered<_16_3Button>(mm2px(Vec(9.5, 89.525)), module, Scanning_clock_multiplier_v2::_16_3_PARAM));
		addParam(createParamCentered<_6_1Button>(mm2px(Vec(23.75, 89.525)), module, Scanning_clock_multiplier_v2::_6_1_PARAM));
		addParam(createParamCentered<_32_5Button>(mm2px(Vec(37.9, 89.525)), module, Scanning_clock_multiplier_v2::_32_5_PARAM));
		addParam(createParamCentered<_7_1Button>(mm2px(Vec(52.1, 89.525)), module, Scanning_clock_multiplier_v2::_7_1_PARAM));
		addParam(createParamCentered<_8_1Button>(mm2px(Vec(9.5, 109.525)), module, Scanning_clock_multiplier_v2::_8_1_PARAM));
		addParam(createParamCentered<_32_3Button>(mm2px(Vec(23.75, 109.525)), module, Scanning_clock_multiplier_v2::_32_3_PARAM));
		addParam(createParamCentered<_16_1Button>(mm2px(Vec(37.9, 109.525)), module, Scanning_clock_multiplier_v2::_16_1_PARAM));
		addParam(createParamCentered<_32_1Button>(mm2px(Vec(52.1, 109.525)), module, Scanning_clock_multiplier_v2::_32_1_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.215, 15.099)), module, Scanning_clock_multiplier_v2::SKEW_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.44, 33.7)), module, Scanning_clock_multiplier_v2::RATE_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.37, 33.7)), module, Scanning_clock_multiplier_v2::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(35.748, 37.713)), module, Scanning_clock_multiplier_v2::SCAN_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.5, 59.27)), module, Scanning_clock_multiplier_v2::_1_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.75, 59.168)), module, Scanning_clock_multiplier_v2::_8_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.9, 59.27)), module, Scanning_clock_multiplier_v2::_2_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52.1, 59.27)), module, Scanning_clock_multiplier_v2::_8_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.5, 79.27)), module, Scanning_clock_multiplier_v2::_3_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.75, 79.27)), module, Scanning_clock_multiplier_v2::_16_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.9, 79.27)), module, Scanning_clock_multiplier_v2::_4_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52.1, 79.27)), module, Scanning_clock_multiplier_v2::_5_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.5, 99.168)), module, Scanning_clock_multiplier_v2::_16_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.75, 99.168)), module, Scanning_clock_multiplier_v2::_6_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.9, 99.27)), module, Scanning_clock_multiplier_v2::_32_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52.1, 99.168)), module, Scanning_clock_multiplier_v2::_7_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.5, 119.168)), module, Scanning_clock_multiplier_v2::_8_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.75, 119.168)), module, Scanning_clock_multiplier_v2::_32_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.9, 119.27)), module, Scanning_clock_multiplier_v2::_16_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52.1, 119.168)), module, Scanning_clock_multiplier_v2::_32_1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.44, 15.3)), module, Scanning_clock_multiplier_v2::RATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.37, 15.3)), module, Scanning_clock_multiplier_v2::RESET_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.6, 12.17)), module, Scanning_clock_multiplier_v2::RATE_PHASE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.6, 26.35)), module, Scanning_clock_multiplier_v2::MAIN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.6, 40.6)), module, Scanning_clock_multiplier_v2::SCAN_PHASE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.6, 83.2)), module, Scanning_clock_multiplier_v2::LOWEST_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.6, 97.5)), module, Scanning_clock_multiplier_v2::HIGHEST_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.6, 54.7)), module, Scanning_clock_multiplier_v2::REFL_SCAN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.6, 69.1)), module, Scanning_clock_multiplier_v2::REFL_SCAN_PHASE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.6, 111.9)), module, Scanning_clock_multiplier_v2::MIDDLE_OUTPUT));

	}
};


Model* modelScanning_clock_multiplier_v2 = createModel<Scanning_clock_multiplier_v2, Scanning_clock_multiplier_v2Widget>("scanning_clock_multiplier_v2");