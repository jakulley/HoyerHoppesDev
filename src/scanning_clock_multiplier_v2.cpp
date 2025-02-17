#include "plugin.hpp"
#include <vector>


struct Scanning_clock_multiplier_v2 : Module {
	enum ParamId {
		PHASE_POSITION_PARAM,
		RATE_PARAM,
		RESET_PARAM,
		SKEW_PARAM,
		SCAN_PARAM,
		PHASE_IN_PARAM,
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
		PHASE_POSITION_CV_INPUT,
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
		HIGHEST_PHASE_OUTPUT,
		REFL_SCAN_OUTPUT,
		LOWEST_OUTPUT,
		LOWEST_PHASE_OUTPUT,
		MIDDLE_OUTPUT,
		MIDDLE_PHASE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		PHASE_IN_LIGHT,
		LIGHTS_LEN
	};

	Scanning_clock_multiplier_v2() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PHASE_POSITION_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RATE_PARAM, 1.f, 700.f, 120.f, "1 to 700 bpm. cv in overrides");
		configButton(RESET_PARAM, "");
		configSwitch(PHASE_IN_PARAM, 0.f, 1.f, 0.f, "recieve phase directly from input");
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
		configInput(PHASE_POSITION_CV_INPUT, "expects 0-10v");
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
		configOutput(LOWEST_PHASE_OUTPUT, "outputs the phase of the lowest active ratio");
		configOutput(MIDDLE_PHASE_OUTPUT, "outputs the phase of the ratio active when scan is at noon");
		configOutput(HIGHEST_PHASE_OUTPUT, "outputs the phase of the highest active ratio");
		configOutput(MIDDLE_OUTPUT, "trigs based on the ratio active when scan is at noon");
		configOutput(HIGHEST_OUTPUT, "trigs based on the highest active ratio");
		configOutput(REFL_SCAN_PHASE_OUTPUT, "reflects scan value around .5");
		configOutput(REFL_SCAN_OUTPUT, "reflects scan value around .5");
		configLight(PHASE_IN_LIGHT, "");
	}

	rack::dsp::SchmittTrigger ratioButtonTriggers[16];
	rack::dsp::SchmittTrigger ratioInputTriggers[16];
	rack::dsp::SchmittTrigger rateTrigger;
	rack::dsp::SchmittTrigger outPhaseTrigger;
	rack::dsp::SchmittTrigger highPhaseTrigger;
	rack::dsp::SchmittTrigger middlePhaseTrigger;
	rack::dsp::SchmittTrigger lowPhaseTrigger;
	rack::dsp::SchmittTrigger invScanTrigger;
	rack::dsp::PulseGenerator invScanPulse;
	rack::dsp::PulseGenerator ratePulse;
	rack::dsp::PulseGenerator outPhasePulse;
	rack::dsp::PulseGenerator highPhasePulse;
	rack::dsp::PulseGenerator middlePhasePulse;
	rack::dsp::PulseGenerator lowPhasePulse;
	float phase = 0.f;
	float rate = 0.f;
	float period = 0.f;
	float scanValue = 0.f;
	float clampScan = 0.f;
	float reflectedScan = 0.f;
	//it's silly to make these doubles; still, floats might start returning imprecise values past 6 or so hours of runtime.
	double lastTrigTime = 0.0f;
	double currentTime = 0.f;
	float phasePos = 0.f;
	float phaseHolder = 0.f;
	bool resetHold = false;
	float skew = 0.f;
	float currentScan = 0.f;

	float allRatios[16] = {1.f, 1.6f, 2.f, 2.6666667f, 3.f, 3.2f, 4.f, 5.f, 5.3333333f, 6.f, 6.4f, 7.f, 8.f, 10.6666667f, 16.f, 32.f};
	std::vector<int> activeRatios;

	float scanReturn(float allRatios[16], const std::vector<int>& activeRatios, const float& scanValue) {
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
		//create vector of active ratios based on user input
		activeRatios.reserve(16);
		for (int i = 0; i < 16; i++) {
			rack::dsp::SchmittTrigger::Event buttons = ratioButtonTriggers[i].processEvent(getParam(ParamId(i+6)).getValue(), 0.1f, 0.9f);
			rack::dsp::SchmittTrigger::Event inputs = ratioInputTriggers[i].processEvent(getInput(InputId(i+5)).getVoltage(), 0.1f, 0.9f);

			if ((buttons == 1 && (!ratioInputTriggers[i].isHigh())) || ((!ratioButtonTriggers[i].isHigh()) && inputs == 1)) {
				activeRatios.push_back(i);
				sort(activeRatios.begin(), activeRatios.end());
			}
			if ((buttons == -1 && (!ratioInputTriggers[i].isHigh())) || (inputs == -1 && (!ratioButtonTriggers[i].isHigh()))) {
      			activeRatios.erase(std::remove(activeRatios.begin(), activeRatios.end(), i), activeRatios.end());
				sort(activeRatios.begin(), activeRatios.end());
			}
		}
		currentScan = scanReturn(allRatios, activeRatios, clampScan);

		//set rate
		if (getParam(PHASE_IN_PARAM).getValue() == 1.f) {
				getLight(PHASE_IN_LIGHT).setBrightness(1.f);
		} else {
				getLight(PHASE_IN_LIGHT).setBrightness(0.f);

		}
		if (getInput(RATE_IN_INPUT).isConnected()) {
			if (rateTrigger.process(getInput(RATE_IN_INPUT).getVoltage())) {
				//currentTime and lastTrigTime may overflow if you leave it running for too many billions of years.
				currentTime = args.frame;
				period = currentTime - lastTrigTime;
				lastTrigTime = currentTime;
				rate = 60.f*(args.sampleRate/period);
				//resetting phaseHolder on rateTrigger=true keeps daisy-chained modules in sync
				phaseHolder = 0.f;
				//phase never exceeds 1.f when receiving an external rate, so calculate scanValue on trig
				scanValue = getParam(SCAN_PARAM).getValue();
				scanValue += getInput(SCAN_CV_INPUT).getVoltage()/10.f;
				clampScan = clamp(scanValue, 0.f, 1.f);
				reflectedScan = 0.5f - clampScan + 0.5f;
			}
		} else {
			rate = getParam(RATE_PARAM).getValue();
		}

		//divide phase position by active ratio to get correct phase positioning.
		phasePos = getInput(PHASE_POSITION_CV_INPUT).isConnected() ? ((getInput(PHASE_POSITION_CV_INPUT).getVoltage()/10)/currentScan) : (getParam(PHASE_POSITION_PARAM).getValue()/currentScan);
		//phaseHolder is necessary to implement the phase position parameter, otherwise there's no way to increment the phase without adding an increment of phasePos every time.
		if (getParam(PHASE_IN_PARAM).getValue() == 1.f) {
			phaseHolder = getInput(RATE_IN_INPUT).getVoltage()/10;
			scanValue = getParam(SCAN_PARAM).getValue();
			scanValue += getInput(SCAN_CV_INPUT).getVoltage()/10.f;
			clampScan = clamp(scanValue, 0.f, 1.f);
			reflectedScan = 0.5f - clampScan + 0.5f;
		} else {
			phaseHolder += (rate/60.f) * args.sampleTime;
		}
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
			//set scan value used to determine active ratio
			//setting scanValue inside phase reset prevents half-phases and jittery responses--any phase that starts will finish, and a new phase can't start before the old phase is finished.
			scanValue = getParam(SCAN_PARAM).getValue();
			scanValue += getInput(SCAN_CV_INPUT).getVoltage()/10.f;
			//scan values exceeding 1 break the scanReturn function by accessing out-of-bounds
			clampScan = clamp(scanValue, 0.f, 1.f);
			//reflectedScan allows you to extract a second value from a single scan knob
			reflectedScan = 0.5f - clampScan + 0.5f;
		}
		//rate phase is the baseline phase, before any skew or multiplication
		getOutput(RATE_PHASE_OUTPUT).setVoltage(phase * 10.f);
		skew = (getInput(SKEW_CV_INPUT).isConnected() ? (getInput(SKEW_CV_INPUT).getVoltage() + 0.11f) : getParam(SKEW_PARAM).getValue());
		float skewPhase = pow(phase, skew);
		while (skewPhase >= 1.f) {
			skewPhase -= 1.f;
		}
		float outPhase = skewPhase * currentScan;
		float reflectedScanPhase = skewPhase * scanReturn(allRatios, activeRatios, reflectedScan);
		float highPhase = skewPhase * scanReturn(allRatios, activeRatios, 1.f);
		float middlePhase = skewPhase * scanReturn(allRatios, activeRatios, 0.5f);
		float lowPhase = skewPhase * scanReturn(allRatios, activeRatios, 0.f);
		//just like the raw phase, the calculated phases need to be trimmed past a 1.f threshold; even though phase never exceeds 1 for more than 1 sample, the pow and scanReturn functions can easily cause it to exceed 1.
		while (outPhase >= 1.f) {
			scanValue = getParam(SCAN_PARAM).getValue();
			scanValue += getInput(SCAN_CV_INPUT).getVoltage()/10.f;
			clampScan = clamp(scanValue, 0.f, 1.f);
			reflectedScan = 0.5f - clampScan + 0.5f;
			outPhase -= 1.f;
		}
		while (reflectedScanPhase >= 1.f) {
			reflectedScanPhase -= 1.f;
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
		bool invScanTrig = invScanTrigger.process(reflectedScanPhase, 0.01f, 0.99f);
		if (invScanTrig) {
			invScanPulse.trigger();
		}

		bool highPulse = highPhasePulse.process(args.sampleTime);
		bool middlePulse = middlePhasePulse.process(args.sampleTime);
		bool lowPulse = lowPhasePulse.process(args.sampleTime);
		bool scanPulse = invScanPulse.process(args.sampleTime);
		bool pulse = outPhasePulse.process(args.sampleTime);
		bool rateOut = ratePulse.process(args.sampleTime);
		getOutput(HIGHEST_PHASE_OUTPUT).setVoltage(highPhase * 10.f);
		getOutput(HIGHEST_OUTPUT).setVoltage(highPulse? 10.f : 0.f);
		getOutput(MIDDLE_PHASE_OUTPUT).setVoltage(middlePhase *10.f);
		getOutput(MIDDLE_OUTPUT).setVoltage(middlePulse? 10.f : 0.f);
		getOutput(LOWEST_PHASE_OUTPUT).setVoltage(lowPhase * 10.f);
		getOutput(LOWEST_OUTPUT).setVoltage(lowPulse? 10.f : 0.f);
		getOutput(REFL_SCAN_OUTPUT).setVoltage(scanPulse? 10.f : 0.f);
		getOutput(REFL_SCAN_PHASE_OUTPUT).setVoltage(reflectedScanPhase * 10.f);
		getOutput(SCAN_PHASE_OUTPUT).setVoltage(outPhase * 10.f);
		getOutput(MAIN_OUTPUT).setVoltage(pulse ? 10.f : 0.f);
		getOutput(RATE_OUTPUT).setVoltage(resetHold ? 0.f : (rateOut ? 10.f : 0.f));
	}

	json_t* dataToJson() override {
        json_t* rootJ = json_object();

        // Save the activeRatios vector to JSON
        json_t* ratiosJ = json_array();
        for (int ratio : activeRatios) {
            json_array_append_new(ratiosJ, json_integer(ratio));
        }
        json_object_set_new(rootJ, "activeRatios", ratiosJ);

        return rootJ;
    }
    void dataFromJson(json_t* rootJ) override {

        // Load the activeRatios vector from JSON
        json_t* ratiosJ = json_object_get(rootJ, "activeRatios");
        if (ratiosJ && json_is_array(ratiosJ)) {
            activeRatios.clear();
            size_t index;
            json_t* value;
            json_array_foreach(ratiosJ, index, value) {
                activeRatios.push_back((int)json_integer_value(value));
            }
        }
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

		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(62.0, 16.75)), module, Scanning_clock_multiplier_v2::PHASE_POSITION_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(10.44, 23.84)), module, Scanning_clock_multiplier_v2::RATE_PARAM));
		addParam(createParamCentered<BefacoPush>(mm2px(Vec(21.37, 23.84)), module, Scanning_clock_multiplier_v2::RESET_PARAM));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(10.44, 15.3)), module, Scanning_clock_multiplier_v2::PHASE_IN_PARAM, Scanning_clock_multiplier_v2::PHASE_IN_LIGHT));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(37.0, 21.81)), module, Scanning_clock_multiplier_v2::SKEW_PARAM));
		addParam(createParamCentered<RoundBigBlackKnob>(mm2px(Vec(53.55, 29.815)), module, Scanning_clock_multiplier_v2::SCAN_PARAM));
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

		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(44.62, 16.56)), module, Scanning_clock_multiplier_v2::SKEW_CV_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(69.47, 21.62)), module, Scanning_clock_multiplier_v2::PHASE_POSITION_CV_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(10.44, 33.7)), module, Scanning_clock_multiplier_v2::RATE_IN_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(21.37, 33.7)), module, Scanning_clock_multiplier_v2::RESET_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(41.68, 38.18)), module, Scanning_clock_multiplier_v2::SCAN_CV_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(9.5, 59.27)), module, Scanning_clock_multiplier_v2::_1_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(23.75, 59.168)), module, Scanning_clock_multiplier_v2::_8_5_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(37.9, 59.27)), module, Scanning_clock_multiplier_v2::_2_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(52.1, 59.27)), module, Scanning_clock_multiplier_v2::_8_3_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(9.5, 79.27)), module, Scanning_clock_multiplier_v2::_3_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(23.75, 79.27)), module, Scanning_clock_multiplier_v2::_16_5_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(37.9, 79.27)), module, Scanning_clock_multiplier_v2::_4_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(52.1, 79.27)), module, Scanning_clock_multiplier_v2::_5_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(9.5, 99.168)), module, Scanning_clock_multiplier_v2::_16_3_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(23.75, 99.168)), module, Scanning_clock_multiplier_v2::_6_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(37.9, 99.27)), module, Scanning_clock_multiplier_v2::_32_5_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(52.1, 99.168)), module, Scanning_clock_multiplier_v2::_7_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(9.5, 119.168)), module, Scanning_clock_multiplier_v2::_8_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(23.75, 119.168)), module, Scanning_clock_multiplier_v2::_32_3_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(37.9, 119.27)), module, Scanning_clock_multiplier_v2::_16_1_INPUT));
		addInput(createInputCentered<DarkPJ301MPort>(mm2px(Vec(52.1, 119.168)), module, Scanning_clock_multiplier_v2::_32_1_INPUT));

		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(63.85, 54.65)), module, Scanning_clock_multiplier_v2::RATE_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(21.4, 15.3)), module, Scanning_clock_multiplier_v2::RESET_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(75.0, 54.65)), module, Scanning_clock_multiplier_v2::RATE_PHASE_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(63.85, 66.25)), module, Scanning_clock_multiplier_v2::MAIN_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(75.0, 66.25)), module, Scanning_clock_multiplier_v2::SCAN_PHASE_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(63.85, 92.4)), module, Scanning_clock_multiplier_v2::LOWEST_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(75.0, 117.77)), module, Scanning_clock_multiplier_v2::MIDDLE_PHASE_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(75.0, 92.4)), module, Scanning_clock_multiplier_v2::LOWEST_PHASE_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(75.0, 105.25)), module, Scanning_clock_multiplier_v2::HIGHEST_PHASE_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(63.85, 105.25)), module, Scanning_clock_multiplier_v2::HIGHEST_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(63.85, 79.5)), module, Scanning_clock_multiplier_v2::REFL_SCAN_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(75.0, 79.5)), module, Scanning_clock_multiplier_v2::REFL_SCAN_PHASE_OUTPUT));
		addOutput(createOutputCentered<DarkPJ301MPort>(mm2px(Vec(63.85, 117.77)), module, Scanning_clock_multiplier_v2::MIDDLE_OUTPUT));

	}
};


Model* modelScanning_clock_multiplier_v2 = createModel<Scanning_clock_multiplier_v2, Scanning_clock_multiplier_v2Widget>("scanning_clock_multiplier_v2");