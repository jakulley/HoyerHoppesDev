#include "plugin.hpp"

struct Scanning_frequency_division_oscillator : Module {
	enum ParamId {
		FREQ_PARAM,
		SCAN_PARAM,
		WAVE_SHAPE_PARAM,
		FM_DEPTH_PARAM,
		SYNC_WINDOW_PARAM,
		WINDOWING_ON_PARAM,
		BY_1_PARAM,
		BY_2_PARAM,
		BY_3_PARAM,
		BY_4_PARAM,
		BY_5_PARAM,
		BY_6_PARAM,
		BY_7_PARAM,
		BY_8_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SCAN_CV_INPUT,
		VOCT_INPUT,
		WAVE_SHAPE_INPUT,
		FM_INPUT,
		FM_DEPTH_INPUT,
		SYNC_INPUT,
		BY_1_INPUT,
		BY_2_INPUT,
		BY_3_INPUT,
		BY_4_INPUT,
		BY_5_INPUT,
		BY_6_INPUT,
		BY_7_INPUT,
		BY_8_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		FREQ_OUTPUT,
		SCAN_OUTPUT,
		REFL_OUTPUT,
		LOW_OUTPUT,
		TRIG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		WINDOWING_ON_LIGHT,
		LIGHTS_LEN
	};

	Scanning_frequency_division_oscillator() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FREQ_PARAM, -4.f, 6.f, 0.f, "");
		configParam(SCAN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WAVE_SHAPE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FM_DEPTH_PARAM, -2.f, 2.f, 0.f, "");
		configParam(SYNC_WINDOW_PARAM, 0.f, 1.f, 0.f, "controls depth of windowing effect");
		configSwitch(WINDOWING_ON_PARAM, 0.f, 1.f, 0.f, "enable window for smooth sync");
		configSwitch(BY_1_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(BY_2_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(BY_3_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(BY_4_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(BY_5_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(BY_6_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(BY_7_PARAM, 0.f, 1.f, 0.f, "");
		configSwitch(BY_8_PARAM, 0.f, 1.f, 0.f, "");
		configInput(SCAN_CV_INPUT, "");
		configInput(VOCT_INPUT, "");
		configInput(WAVE_SHAPE_INPUT, "use negative voltage to subtract from the knob position");
		configInput(BY_1_INPUT, "");
		configInput(BY_2_INPUT, "");
		configInput(FM_INPUT, "");
		configInput(FM_DEPTH_INPUT, "");
		configInput(BY_3_INPUT, "");
		configInput(BY_4_INPUT, "");
		configInput(SYNC_INPUT, "");
		configInput(BY_5_INPUT, "");
		configInput(BY_6_INPUT, "");
		configInput(BY_7_INPUT, "");
		configInput(BY_8_INPUT, "");
		configOutput(FREQ_OUTPUT, "");
		configOutput(SCAN_OUTPUT, "");
		configOutput(REFL_OUTPUT, "");
		configOutput(LOW_OUTPUT, "");
		configOutput(TRIG_OUTPUT, "");
		configLight(WINDOWING_ON_LIGHT, "");
	}

	dsp::SchmittTrigger buttons[8];
	dsp::SchmittTrigger inputs[8];
	std::vector<int> activeRatios;
	dsp::PulseGenerator scanPulse;
	dsp::SchmittTrigger scanTrigger;
	dsp::SchmittTrigger syncCheck;
	dsp::PulseGenerator syncPulse;
	float scanValue = 0.f;
	float clampScan = 0.f;
	float reflectedScan = 0.f;
	int allDivisors[8] = {1, 2, 3, 4, 5, 6, 7, 8};
	float waveBlend = 0.f;
	float fMod = 0.f;
	float modIndex = 1.f;
	float phase = 0.f;
	float scanPhase = 0.f;
	float reflectedScanPhase = 0.f;
	float lowPhase = 0.f;
	float pitch = 0.f;
	float freq = 0.f;
	float fmFreq = 0.f;
	float scanFreq = 0.f;
	float reflectFreq = 0.f;
	float lowFreq = 0.f;
	int currentScan = 0;
	int previousScan = 0;

	int scanReturn(const int allDivisors[8], const std::vector<int>& activeRatios, const float& scanValue) {
		if (activeRatios.size() != 0) {
			//divide the scan range into equal sections according to the number of active ratios
			float scanInc = 1.f / (float)activeRatios.size();
			
			long long unsigned int i = 0;
			if (scanValue < scanInc) {
				return allDivisors[activeRatios[i]];
			} else {
				while (scanValue >= scanInc * (i+1) && i < activeRatios.size()-1) {
					i++;
				}
				return allDivisors[activeRatios[i]];
			}
		} else {
			return 1;
		}
	}

	float chebyshevSin(float x) {
		x = std::fmod(x, 2 * M_PI);
		if (x > M_PI) x -= 2 * M_PI;
		if (x < -M_PI) x += 2 * M_PI;

		float x2 = x * x;
		float x3 = x2 * x;
		float x5 = x3 * x2;
		float x7 = x5 * x2;
		float x9 = x7 * x2;
		float x11 = x9 * x2;
		float x13 = x11 * x2;
		float x15 = x13 * x2;

		// 15th-degree Chebyshev approximation for sin(x)
		return x - (x3 / 6.0f) + (x5 / 120.0f) - (x7 / 5040.0f) + (x9 / 362880.0f) 
           - (x11 / 39916800.0f) + (x13 / 6227020800.0f) - (x15 / 1307674368000.0f);
	}

	float waveReturn(const float& waveBlend, float& phase) {
    	// compute common values that don’t depend on waveBlend
		float syncWindow = 4*pow((getInput(SYNC_INPUT).getVoltage()/10), 2);
    	float windowDepth = getParam(SYNC_WINDOW_PARAM).getValue();
    	float window = 1.f - (getInput(SYNC_INPUT).getVoltage() / 5.f) * windowDepth;
    	float absPhase = std::abs(phase);
		float wave = 0.f;

    	float triangle = (phase < 0.5f) ? ((absPhase * 4.f) - 1.f) : (((1.f - absPhase) * 4.f) - 1.f);
    	// normalize triangle
    	if (triangle >= 1.f) {
    	    triangle = 2.f - triangle;
    	}
    	float sine = chebyshevSin(2.f * phase * M_PI);
    	// Compute the wave based on waveBlend
    	if (waveBlend < 0.33333f) {
    	    wave = 3.f * ((sine * (0.33333f - waveBlend)) + (triangle * waveBlend));
    	} else if (waveBlend < 0.666666f) {
    	    wave = 3.f * ((triangle * (0.66666f - waveBlend)) + ((absPhase * 2.f) - 1.f) * (waveBlend - 0.33333f));
    	} else {
        	wave = 3.f * (((absPhase * 2.f) - 1.f) * (1.f - waveBlend) + (absPhase > 0.5f ? 1.f : -1.f) * (waveBlend - 0.66666f));
    	}
    	if (getInput(SYNC_INPUT).isConnected() && getParam(WINDOWING_ON_PARAM).getValue() == 1.f) {
        	wave *= syncWindow * (window - windowDepth);
    	}
    	return wave;
	}

	void process(const ProcessArgs& args) override {
		activeRatios.reserve(8);
		for (int i = 0; i < 8; i++) {
			rack::dsp::SchmittTrigger::Event buttonPress = buttons[i].processEvent(getParam(ParamId(i+6)).getValue(), 0.1f, 0.9f);
			rack::dsp::SchmittTrigger::Event inputActivate = inputs[i].processEvent(getInput(InputId(i+6)).getVoltage(), 0.1f, 0.9f);

			if ((buttonPress == 1 && (!inputs[i].isHigh())) || ((!buttons[i].isHigh()) && inputActivate == 1)) {
				activeRatios.push_back(i);
				sort(activeRatios.begin(), activeRatios.end());
			}
			if ((buttonPress == -1 && (!inputs[i].isHigh())) || (inputActivate == -1 && (!buttons[i].isHigh()))) {
      			activeRatios.erase(std::remove(activeRatios.begin(), activeRatios.end(), i), activeRatios.end());
				sort(activeRatios.begin(), activeRatios.end());
			}
		}

		scanValue = getParam(SCAN_PARAM).getValue();
		scanValue += getInput(SCAN_CV_INPUT).getVoltage()/10.f;
		clampScan = clamp(scanValue);
		reflectedScan = 0.5f - clampScan + 0.5f;

		waveBlend = getInput(WAVE_SHAPE_INPUT).isConnected()? clamp(((getInput(WAVE_SHAPE_INPUT).getVoltage()/10.f) + getParam(WAVE_SHAPE_PARAM).getValue())) : getParam(WAVE_SHAPE_PARAM).getValue();
		fMod = getInput(FM_DEPTH_INPUT).isConnected()? (getInput(FM_DEPTH_INPUT).getVoltage()/10) * getParam(FM_DEPTH_PARAM).getValue() * getInput(FM_INPUT).getVoltage() : getParam(FM_DEPTH_PARAM).getValue() * getInput(FM_INPUT).getVoltage();
		pitch = getParam(FREQ_PARAM).getValue() + getInput(VOCT_INPUT).getVoltage();
		//fmFreq = pitch + (fMod * (1.0 - 2.0 * (fMod < 0 ? 1 : 0)));
		//pitch += fMod;
		//pitch += (fMod > 0 ? fMod : -fMod);
		freq = dsp::FREQ_C4 * dsp::exp2_taylor5(pitch);
		freq += dsp::FREQ_C4 * fMod;
		phase += freq*args.sampleTime;
		previousScan = currentScan;
		currentScan = scanReturn(allDivisors, activeRatios, scanValue);
		if (previousScan != currentScan) {
			scanPulse.trigger();
		}
		scanFreq = (freq/currentScan)*args.sampleTime;
		reflectFreq = (freq/scanReturn(allDivisors, activeRatios, reflectedScan))*args.sampleTime;
		lowFreq = (freq/scanReturn(allDivisors, activeRatios, 1.f))*args.sampleTime;
		dsp::SchmittTrigger::Event syncTrigger = syncCheck.processEvent(getInput(SYNC_INPUT).getVoltage());
		if (getParam(WINDOWING_ON_PARAM).getValue() == 1.f) {
				getLight(WINDOWING_ON_LIGHT).setBrightness(1.f);
		} else {
				getLight(WINDOWING_ON_LIGHT).setBrightness(0.f);
		}

		//phase
		if (phase >= 1.f) {
			phase = 0.f;
		}
		if (syncTrigger == 1) {
			phase = 0.f;
		}
		if (phase <= -1.f) {
			phase = 0.f;
		}
		//scanPhase
		scanPhase += scanFreq;
		if (scanPhase >= 1.f) {
			scanPhase = 0.f;
		}
		if (syncTrigger == 1) {
			scanPhase = 0.f;
		}
		if (scanPhase <= -1.f) {
			scanPhase = 0.f;
		}
		//reflectedScanPhase
		reflectedScanPhase += reflectFreq;
		if (reflectedScanPhase >= 1.f) {
			reflectedScanPhase = 0.f;
		}
		if (syncTrigger == 1) {
			reflectedScanPhase = 0.f;
		}
		if (reflectedScanPhase <= -1.f) {
			reflectedScanPhase = 0.f;
		}
		//lowPhase
		lowPhase += lowFreq;
		if (lowPhase >= 1.f) {
			lowPhase = 0.f;
		}
		if (syncCheck.isHigh()) {
			lowPhase = 0.f;
		}
		if (lowPhase <= -1.f) {
			lowPhase = 0.f;
		}

		getOutput(FREQ_OUTPUT).setVoltage(5.f * waveReturn(waveBlend, phase));
		getOutput(SCAN_OUTPUT).setVoltage(5.f * waveReturn(waveBlend, scanPhase));
		getOutput(REFL_OUTPUT).setVoltage(5.f * waveReturn(waveBlend, reflectedScanPhase));
		getOutput(LOW_OUTPUT).setVoltage(5.f * waveReturn(waveBlend, lowPhase));
		bool trigOut = scanPulse.process(args.sampleTime);
		getOutput(TRIG_OUTPUT).setVoltage(trigOut? 10.f : 0.f);
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


struct Scanning_frequency_division_oscillatorWidget : ModuleWidget {
	Scanning_frequency_division_oscillatorWidget(Scanning_frequency_division_oscillator* module) {
		setModule(module);
		setPanel(createPanel(
			asset::plugin(pluginInstance, "res/scanning_frequency_division_oscillator.svg"),
			asset::plugin(pluginInstance, "res/scanning_frequency_division_oscillator_dark_theme.svg")
		));


		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundExtraBigBlackKnob>(mm2px(Vec(44.0, 24.5)), module, Scanning_frequency_division_oscillator::FREQ_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.762, 25.88)), module, Scanning_frequency_division_oscillator::SCAN_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(50.52, 46.3)), module, Scanning_frequency_division_oscillator::WAVE_SHAPE_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(36.953, 64.9)), module, Scanning_frequency_division_oscillator::FM_DEPTH_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(50.52, 87.12)), module, Scanning_frequency_division_oscillator::SYNC_WINDOW_PARAM));
		addParam(createLightParamCentered<VCVLightLatch<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(50.52, 76.9)), module, Scanning_frequency_division_oscillator::WINDOWING_ON_PARAM, Scanning_frequency_division_oscillator::WINDOWING_ON_LIGHT));
		addParam(createParamCentered<By1Button>(mm2px(Vec(9.983, 45.907)), module, Scanning_frequency_division_oscillator::BY_1_PARAM));
		addParam(createParamCentered<By2Button>(mm2px(Vec(22.496, 45.907)), module, Scanning_frequency_division_oscillator::BY_2_PARAM));
		addParam(createParamCentered<By3Button>(mm2px(Vec(9.983, 66.871)), module, Scanning_frequency_division_oscillator::BY_3_PARAM));
		addParam(createParamCentered<By4Button>(mm2px(Vec(22.496, 66.871)), module, Scanning_frequency_division_oscillator::BY_4_PARAM));
		addParam(createParamCentered<By5Button>(mm2px(Vec(9.983, 87.437)), module, Scanning_frequency_division_oscillator::BY_5_PARAM));
		addParam(createParamCentered<By6Button>(mm2px(Vec(22.496, 87.437)), module, Scanning_frequency_division_oscillator::BY_6_PARAM));
		addParam(createParamCentered<By7Button>(mm2px(Vec(9.983, 108.194)), module, Scanning_frequency_division_oscillator::BY_7_PARAM));
		addParam(createParamCentered<By8Button>(mm2px(Vec(22.496, 108.194)), module, Scanning_frequency_division_oscillator::BY_8_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.465, 32.0)), module, Scanning_frequency_division_oscillator::SCAN_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.85, 34.2)), module, Scanning_frequency_division_oscillator::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.953, 46.3)), module, Scanning_frequency_division_oscillator::WAVE_SHAPE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.983, 55.266)), module, Scanning_frequency_division_oscillator::BY_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.496, 55.266)), module, Scanning_frequency_division_oscillator::BY_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(50.52, 64.9)), module, Scanning_frequency_division_oscillator::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.953, 74.6)), module, Scanning_frequency_division_oscillator::FM_DEPTH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.983, 76.244)), module, Scanning_frequency_division_oscillator::BY_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.496, 76.244)), module, Scanning_frequency_division_oscillator::BY_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.953, 87.5)), module, Scanning_frequency_division_oscillator::SYNC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.983, 96.791)), module, Scanning_frequency_division_oscillator::BY_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.496, 96.791)), module, Scanning_frequency_division_oscillator::BY_6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.983, 117.723)), module, Scanning_frequency_division_oscillator::BY_7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.496, 117.723)), module, Scanning_frequency_division_oscillator::BY_8_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.953, 104.15)), module, Scanning_frequency_division_oscillator::FREQ_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.52, 104.15)), module, Scanning_frequency_division_oscillator::SCAN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.953, 117.1)), module, Scanning_frequency_division_oscillator::REFL_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.52, 117.1)), module, Scanning_frequency_division_oscillator::LOW_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(28.1, 32.0)), module, Scanning_frequency_division_oscillator::TRIG_OUTPUT));
	}
};


Model* modelScanning_frequency_division_oscillator = createModel<Scanning_frequency_division_oscillator, Scanning_frequency_division_oscillatorWidget>("scanning_frequency_division_oscillator");