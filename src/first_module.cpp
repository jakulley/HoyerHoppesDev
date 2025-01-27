#include "plugin.hpp"


struct First_module : Module {
	enum ParamId {
		CVATTEN_PARAM,
		FREQUENCY_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CV1_INPUT,
		CV2_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	First_module() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CVATTEN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(FREQUENCY_PARAM, 0.f, 1.f, 0.f, "");
		configInput(CV1_INPUT, "");
		configInput(CV2_INPUT, "");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
		
	}
};


struct First_moduleWidget : ModuleWidget {
	First_moduleWidget(First_module* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/first_module.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(52.443, 10.624)), module, First_module::CVATTEN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.915, 25.455)), module, First_module::FREQUENCY_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.845, 111.665)), module, First_module::CV1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.845, 119.426)), module, First_module::CV2_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.928, 111.665)), module, First_module::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.928, 119.526)), module, First_module::OUT2_OUTPUT));
	}
};


Model* modelFirst_module = createModel<First_module, First_moduleWidget>("first_module");