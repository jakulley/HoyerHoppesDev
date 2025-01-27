#include "plugin.hpp"
#include <set>
#include <vector>
using namespace std;


struct Scanning_clock_multiplier : Module {
	enum ParamId {
		SKEW_PARAM,
		BPM_PARAM,
		SCAN_PARAM,
		_1_1_PARAM,
		_2_1_PARAM,
		_3_1_PARAM,
		_3_2_PARAM,
		_3_4_PARAM,
		_4_1_PARAM,
		_4_3_PARAM,
		_4_5_PARAM,
		_8_1_PARAM,
		_8_3_PARAM,
		_8_5_PARAM,
		_16_1_PARAM,
		_16_3_PARAM,
		_16_5_PARAM,
		_32_1_PARAM,
		_32_3_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SKEW_CV_INPUT,
		BPM_IN_INPUT,
		SCAN_CV_INPUT,
		_1_1_INPUT,
		_2_1_INPUT,
		_3_1_INPUT,
		_3_2_INPUT,
		_3_4_INPUT,
		_4_1_INPUT,
		_4_3_INPUT,
		_4_5_INPUT,
		_8_1_INPUT,
		_8_3_INPUT,
		_8_5_INPUT,
		_16_1_INPUT,
		_16_3_INPUT,
		_16_5_INPUT,
		_32_1_INPUT,
		_32_3_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		MAIN_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Scanning_clock_multiplier() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SKEW_PARAM, 0.1f, 10.f, 1.f, "adds exponential or logarithmic curve to output");
		configParam(BPM_PARAM, 1.f, 700.f, 120.f, "1 to 700 bpm. cv in overrides.");
		configParam(SCAN_PARAM, 0.f, 1.f, 0.f, "scan active ratios. sums with cv in.");
		configSwitch(_1_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_2_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_3_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_3_2_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_3_4_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_4_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_4_3_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_4_5_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_8_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_8_3_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_8_5_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_16_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_16_3_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_16_5_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_32_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(_32_3_PARAM, 0.f, 1.f, 0.f, "");
		configInput(SKEW_CV_INPUT, "expects 0-10");
		configInput(BPM_IN_INPUT, "");
		configInput(SCAN_CV_INPUT, "expects 0-10");
		configInput(_1_1_INPUT, "inverts button value");
		configInput(_2_1_INPUT, "inverts button value");
		configInput(_3_1_INPUT, "inverts button value");
		configInput(_3_2_INPUT, "inverts button value");
		configInput(_3_4_INPUT, "inverts button value");
		configInput(_4_1_INPUT, "inverts button value");
		configInput(_4_3_INPUT, "inverts button value");
		configInput(_4_5_INPUT, "inverts button value");
		configInput(_8_1_INPUT, "inverts button value");
		configInput(_8_3_INPUT, "inverts button value");
		configInput(_8_5_INPUT, "inverts button value");
		configInput(_16_1_INPUT, "inverts button value");
		configInput(_16_3_INPUT, "inverts button value");
		configInput(_16_5_INPUT, "inverts button value");
		configInput(_32_1_INPUT, "inverts button value");
		configInput(_32_3_INPUT, "inverts button value");
		configOutput(MAIN_OUTPUT, "Output");
	};

	rack::dsp::SchmittTrigger ratioButtonTriggers[16];
	rack::dsp::SchmittTrigger ratioInputTriggers[16];
	rack::dsp::SchmittTrigger schmittTrigger;
	rack::dsp::SchmittTrigger outPhaseTrigger;
	rack::dsp::PulseGenerator pulseGenerator;
	float phase = 0.f;
	float bpm = 0.f;
	float period = 0.f;
	float scanValue = 0.f;
	//it's silly to make these doubles; still, floats might start returning imprecise values past 6 or so hours of runtime.
	double lastTrigTime = 0.0f;
	double currentTime = 0.f;

	float allRatios[16] = {1.f, 2.f, 3.f, 1.5f, 0.75f, 4.f, 1.33333333333f, 0.8f, 8.f, 2.66666666667f, 1.6f, 16.f, 5.33333333333f, 3.2f, 32.f, 10.6666666667f};
	vector<int> activeRatios;

	float scanReturn(float allRatios[16], std::vector<int> activeRatios, float scanValue) {
		for (int i = 0; i<16; i++) {
			if ((getParam(ParamId(i+3)).getValue() == 1.f && getInput(InputId(i+3)).getVoltage() < 1.f) || 
			(getParam(ParamId(i+3)).getValue() == 0.f && getInput(InputId(i+3)).getVoltage() > 1.f)) {
				activeRatios.push_back(i);
			} else {
      			activeRatios.erase(std::remove(activeRatios.begin(), activeRatios.end(), i), activeRatios.end());
			}
		}
		if (activeRatios.size() != 0) {
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

	// float setScanReturn(float allRatios[16], std::set<int> activeRatios, float scanValue) {
	// 	I want to implement this outside the function call to improve performance
	// 	for (int i = 0; i<16; i++) {
	// 		if ((getParam(ParamId(i+3)).getValue() == 1.f && getInput(InputId(i+3)).getVoltage() < 1.f) || 
	// 		(getParam(ParamId(i+3)).getValue() == 0.f && getInput(InputId(i+3)).getVoltage() > 1.f)) {
	// 			activeRatios.insert(allRatios[i]);
	// 		} else {
    //   			auto it = activeRatios.find(allRatios[i]);
    //    			if (it != activeRatios.end()) {
    //         		activeRatios.erase(it);
    //     		}
	// 		}
	// 	}
	// 	if (activeRatios.size() != 0) {
	// 		float scanInc = 1.f / (float)activeRatios.size();
	// 		long long unsigned int i = 0;
	// 		auto ratio = activeRatios.begin();
	// 		if (scanValue < scanInc) {
	// 			return *ratio;
	// 		} else {
	// 			while (scanValue >= (scanInc * (i+1)) && i < activeRatios.size()-1) {
	// 				std::advance(ratio, 1);
	// 				i++;
	// 			}
	// 			return *ratio;
	// 		}
	// 	} else {
	// 		return 1.f;
	// 	}
	// }

	void process(const ProcessArgs& args) override {
		// for (int i = 0; i < 16; i++) {
		// 	bool buttonsOn = ratioButtonTriggers[i].processEvent(getParam(ParamId(i+3)).getValue(), 0.1f, 0.9f) == 1;
		// 	bool inputsOn = ratioInputTriggers[i].processEvent(getInput(InputId(i+3)).getVoltage(), 0.1f, 0.9f) == 1;
		// 	bool buttonsOff = ratioButtonTriggers[i].processEvent(getParam(ParamId(i+3)).getValue(), 0.1f, 0.9f) == -1;
		// 	bool inputsOff = ratioInputTriggers[i].processEvent(getInput(InputId(i+3)).getVoltage(), 0.1f, 0.9f) == -1;

		// 	if ((buttonsOn && (!ratioInputTriggers[i].isHigh())) || ((!ratioButtonTriggers[i].isHigh()) && inputsOn)) {
		// 		activeRatios.push_back(i);
		// 	}
		// 	if ((buttonsOff && (!ratioInputTriggers[i].isHigh())) || (inputsOff && (!ratioButtonTriggers[i].isHigh()))) {
      	// 		activeRatios.erase(std::remove(activeRatios.begin(), activeRatios.end(), i), activeRatios.end());
		// 	}
		// }
		scanValue = getParam(SCAN_PARAM).getValue();
		scanValue += getInput(SCAN_CV_INPUT).getVoltage()/10.f;
		//scan values exceeding 10 break the scanReturn function by accessing out-of-bounds
		float clampScan = clamp(scanValue, 0.f, 10.f);

		if (getInput(BPM_IN_INPUT).isConnected()) {
			if (schmittTrigger.process(getInput(BPM_IN_INPUT).getVoltage())) {
				//currentTime and lastTrigTime may overflow if you leave it running for too many billions of years.
				currentTime = args.frame;
				period = currentTime - lastTrigTime;
				lastTrigTime = currentTime;
			}
			if (period > 0.f) {
				bpm = (60.f*args.sampleRate)/period;
			} else {
				bpm = getParam(BPM_PARAM).getValue();
			}
		} else {
				bpm = getParam(BPM_PARAM).getValue();
		}
		
		phase += (bpm/60) * args.sampleTime;
		if (phase >= 1.f) {
			phase -= 1.f;
		}
		float skew = (getInput(SKEW_CV_INPUT).isConnected() ? (getInput(SKEW_CV_INPUT).getVoltage() + 0.1f) : getParam(SKEW_PARAM).getValue());
		//pow implements the Skew param
		//scanReturn multiplies the phase by the selected ratio
		float outPhase = pow(phase, skew) * scanReturn(allRatios, activeRatios, clampScan);
		while (outPhase >= 1.f) {
			outPhase -= 1.f;
		}
		//outPhase can only be inverted *after* it has been calculated
		float flipPhase = 1.f - outPhase;
		bool trigger = outPhaseTrigger.process(flipPhase, 0.1f, .9f);
		if (trigger) {
			pulseGenerator.trigger(1e-3f);
		}
		bool pulse = pulseGenerator.process(args.sampleTime);
		getOutput(MAIN_OUTPUT).setVoltage(pulse ? 10.f : 0.f);
	}
};


struct Scanning_clock_multiplierWidget : ModuleWidget {
	Scanning_clock_multiplierWidget(Scanning_clock_multiplier* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/scanning_clock_multiplier.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(28.682, 18.964)), module, Scanning_clock_multiplier::SKEW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.185, 25.429)), module, Scanning_clock_multiplier::BPM_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.869, 25.801)), module, Scanning_clock_multiplier::SCAN_PARAM));
		addParam(createParamCentered<OneOneButton>(mm2px(Vec(14.185, 48.467)), module, Scanning_clock_multiplier::_1_1_PARAM));
		addParam(createParamCentered<TwoOneButton>(mm2px(Vec(28.508, 48.467)), module, Scanning_clock_multiplier::_2_1_PARAM));
		addParam(createParamCentered<ThreeOneButton>(mm2px(Vec(42.617, 48.467)), module, Scanning_clock_multiplier::_3_1_PARAM));
		addParam(createParamCentered<ThreeTwoButton>(mm2px(Vec(56.894, 48.467)), module, Scanning_clock_multiplier::_3_2_PARAM));
		addParam(createParamCentered<ThreeFourButton>(mm2px(Vec(14.185, 68.467)), module, Scanning_clock_multiplier::_3_4_PARAM));
		addParam(createParamCentered<FourOneButton>(mm2px(Vec(28.508, 68.467)), module, Scanning_clock_multiplier::_4_1_PARAM));
		addParam(createParamCentered<FourThreeButton>(mm2px(Vec(42.617, 68.467)), module, Scanning_clock_multiplier::_4_3_PARAM));
		addParam(createParamCentered<FourFiveButton>(mm2px(Vec(56.894, 68.467)), module, Scanning_clock_multiplier::_4_5_PARAM));
		addParam(createParamCentered<EightOneButton>(mm2px(Vec(14.185, 88.467)), module, Scanning_clock_multiplier::_8_1_PARAM));
		addParam(createParamCentered<EightThreeButton>(mm2px(Vec(28.508, 88.467)), module, Scanning_clock_multiplier::_8_3_PARAM));
		addParam(createParamCentered<EightFiveButton>(mm2px(Vec(42.617, 88.467)), module, Scanning_clock_multiplier::_8_5_PARAM));
		addParam(createParamCentered<SixteenOneButton>(mm2px(Vec(56.894, 88.467)), module, Scanning_clock_multiplier::_16_1_PARAM));
		addParam(createParamCentered<SixteenThreeButton>(mm2px(Vec(14.185, 108.467)), module, Scanning_clock_multiplier::_16_3_PARAM));
		addParam(createParamCentered<SixteenFiveButton>(mm2px(Vec(28.508, 108.467)), module, Scanning_clock_multiplier::_16_5_PARAM));
		addParam(createParamCentered<ThirtytwoOneButton>(mm2px(Vec(42.617, 108.467)), module, Scanning_clock_multiplier::_32_1_PARAM));
		addParam(createParamCentered<ThirtytwoThreeButton>(mm2px(Vec(56.894, 108.467)), module, Scanning_clock_multiplier::_32_3_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.101, 14.822)), module, Scanning_clock_multiplier::SKEW_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.185, 33.71)), module, Scanning_clock_multiplier::BPM_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.963, 33.71)), module, Scanning_clock_multiplier::SCAN_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.185, 58.212)), module, Scanning_clock_multiplier::_1_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.519, 58.11)), module, Scanning_clock_multiplier::_2_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.617, 58.212)), module, Scanning_clock_multiplier::_3_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56.894, 58.212)), module, Scanning_clock_multiplier::_3_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.185, 78.212)), module, Scanning_clock_multiplier::_3_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.508, 78.212)), module, Scanning_clock_multiplier::_4_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.617, 78.212)), module, Scanning_clock_multiplier::_4_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56.894, 78.212)), module, Scanning_clock_multiplier::_4_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.185, 98.11)), module, Scanning_clock_multiplier::_8_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.519, 98.11)), module, Scanning_clock_multiplier::_8_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.617, 98.212)), module, Scanning_clock_multiplier::_8_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56.894, 98.11)), module, Scanning_clock_multiplier::_16_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.185, 118.11)), module, Scanning_clock_multiplier::_16_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.519, 118.11)), module, Scanning_clock_multiplier::_16_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.617, 118.212)), module, Scanning_clock_multiplier::_32_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(56.894, 118.11)), module, Scanning_clock_multiplier::_32_3_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(58.861, 27.162)), module, Scanning_clock_multiplier::MAIN_OUTPUT));
	}
};


Model* modelScanning_clock_multiplier = createModel<Scanning_clock_multiplier, Scanning_clock_multiplierWidget>("scanning_clock_multiplier");