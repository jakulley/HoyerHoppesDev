#include "plugin.hpp"


struct Scanning_frequency_division_oscillator : Module {
	enum ParamId {
		FREQ_PARAM,
		SCAN_PARAM,
		WAVE_SHAPE_PARAM,
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
		BY_1_INPUT,
		BY_2_INPUT,
		FM_INPUT,
		FM_DEPTH_INPUT,
		BY_3_INPUT,
		BY_4_INPUT,
		PWM_INPUT,
		SYNC_INPUT,
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
		HIGH_OUTPUT,
		MID_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Scanning_frequency_division_oscillator() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FREQ_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SCAN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(WAVE_SHAPE_PARAM, 0.f, 1.f, 0.f, "");
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
		configInput(WAVE_SHAPE_INPUT, "");
		configInput(BY_1_INPUT, "");
		configInput(BY_2_INPUT, "");
		configInput(FM_INPUT, "");
		configInput(FM_DEPTH_INPUT, "");
		configInput(BY_3_INPUT, "");
		configInput(BY_4_INPUT, "");
		configInput(PWM_INPUT, "");
		configInput(SYNC_INPUT, "");
		configInput(BY_5_INPUT, "");
		configInput(BY_6_INPUT, "");
		configInput(BY_7_INPUT, "");
		configInput(BY_8_INPUT, "");
		configOutput(FREQ_OUTPUT, "");
		configOutput(SCAN_OUTPUT, "");
		configOutput(REFL_OUTPUT, "");
		configOutput(LOW_OUTPUT, "");
		configOutput(HIGH_OUTPUT, "");
		configOutput(MID_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
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

		addParam(createParamCentered<RoundExtraBigBlackKnob>(mm2px(Vec(46.468, 20.682)), module, Scanning_frequency_division_oscillator::FREQ_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.762, 23.626)), module, Scanning_frequency_division_oscillator::SCAN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(48.874, 42.655)), module, Scanning_frequency_division_oscillator::WAVE_SHAPE_PARAM));
		addParam(createParamCentered<By1Button>(mm2px(Vec(9.983, 44.107)), module, Scanning_frequency_division_oscillator::BY_1_PARAM));
		addParam(createParamCentered<By2Button>(mm2px(Vec(22.496, 44.107)), module, Scanning_frequency_division_oscillator::BY_2_PARAM));
		addParam(createParamCentered<By3Button>(mm2px(Vec(9.983, 65.071)), module, Scanning_frequency_division_oscillator::BY_3_PARAM));
		addParam(createParamCentered<By4Button>(mm2px(Vec(22.496, 65.071)), module, Scanning_frequency_division_oscillator::BY_4_PARAM));
		addParam(createParamCentered<By5Button>(mm2px(Vec(9.983, 85.637)), module, Scanning_frequency_division_oscillator::BY_5_PARAM));
		addParam(createParamCentered<By6Button>(mm2px(Vec(22.496, 85.637)), module, Scanning_frequency_division_oscillator::BY_6_PARAM));
		addParam(createParamCentered<By7Button>(mm2px(Vec(9.983, 106.394)), module, Scanning_frequency_division_oscillator::BY_7_PARAM));
		addParam(createParamCentered<By8Button>(mm2px(Vec(22.496, 106.394)), module, Scanning_frequency_division_oscillator::BY_8_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.465, 29.842)), module, Scanning_frequency_division_oscillator::SCAN_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(34.595, 29.842)), module, Scanning_frequency_division_oscillator::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.953, 45.094)), module, Scanning_frequency_division_oscillator::WAVE_SHAPE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.983, 53.466)), module, Scanning_frequency_division_oscillator::BY_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.496, 53.466)), module, Scanning_frequency_division_oscillator::BY_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.953, 60.497)), module, Scanning_frequency_division_oscillator::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.953, 60.497)), module, Scanning_frequency_division_oscillator::FM_DEPTH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.983, 74.444)), module, Scanning_frequency_division_oscillator::BY_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.496, 74.444)), module, Scanning_frequency_division_oscillator::BY_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.953, 76.172)), module, Scanning_frequency_division_oscillator::PWM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.953, 76.172)), module, Scanning_frequency_division_oscillator::SYNC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.983, 94.991)), module, Scanning_frequency_division_oscillator::BY_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.496, 94.991)), module, Scanning_frequency_division_oscillator::BY_6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.983, 115.923)), module, Scanning_frequency_division_oscillator::BY_7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.496, 115.923)), module, Scanning_frequency_division_oscillator::BY_8_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.953, 93.437)), module, Scanning_frequency_division_oscillator::FREQ_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(49.953, 93.437)), module, Scanning_frequency_division_oscillator::SCAN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.953, 106.391)), module, Scanning_frequency_division_oscillator::REFL_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(49.953, 106.391)), module, Scanning_frequency_division_oscillator::LOW_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.953, 118.032)), module, Scanning_frequency_division_oscillator::HIGH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(49.953, 118.032)), module, Scanning_frequency_division_oscillator::MID_OUTPUT));
	}
};


Model* modelScanning_frequency_division_oscillator = createModel<Scanning_frequency_division_oscillator, Scanning_frequency_division_oscillatorWidget>("scanning_frequency_division_oscillator");