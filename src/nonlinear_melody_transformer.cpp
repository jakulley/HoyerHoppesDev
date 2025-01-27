#include "plugin.hpp"


struct Nonlinear_melody_transformer : Module {
	enum ParamId {
		FLOOR_MODE_PARAM,
		CEILING_MODE_PARAM,
		FLOOR_HEIGHT_PARAM,
		CEILING_HEIGHT_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		FLOOR_HEIGHT_CV_INPUT,
		CEILING_HEIGHT_CV_INPUT,
		MAIN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		FLOOR_GATE_OUTPUT,
		CEILING_GATE_OUTPUT,
		MAIN_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Nonlinear_melody_transformer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(FLOOR_MODE_PARAM, 0.f, 4.f, 0.f, "Floor mode", {"fold up", "to ceiling", "wrap up", "hard clip", "soft clip (50%)"});
		configSwitch(CEILING_MODE_PARAM, 0.f, 4.f, 0.f, "Ceiling mode", {"fold down", "to floor", "wrap down", "hard clip", "soft clip (50%)"});
		configParam(FLOOR_HEIGHT_PARAM, 0.f, 10.f, 0.f, "");
		configParam(CEILING_HEIGHT_PARAM, 0.f, 10.f, 10.f, "");
		configInput(FLOOR_HEIGHT_CV_INPUT, "");
		configInput(CEILING_HEIGHT_CV_INPUT, "");
		configInput(MAIN_INPUT, "");
		configOutput(FLOOR_GATE_OUTPUT, "");
		configOutput(CEILING_GATE_OUTPUT, "");
		configOutput(MAIN_OUTPUT, "");
	}
	
	
	
	bool detectLow(float input, float floor) {
		if (input<floor) {
			return true;
		} else {
			return false;
		}
	}
	
	bool detectHigh(float input, float ceiling) {
		if (input>ceiling) {
			return true;
		} else {
			return false;
		}
	}
	
	float foldUp(float input, float floor) {
		if (detectLow(input, floor)) {
			float difference = floor - input;
			float output = floor + difference;
			return output;
		} else {
			return input;
		}
	}
	
	float foldDown(float input, float ceiling) {
		if (detectHigh(input, ceiling)) {
			float difference = input - ceiling;
			float output = ceiling - difference;
			return output;
		} else {
			return input;
		}
	}
	
	float toFloor(float input, float ceiling, float floor) {
		if (detectHigh(input, ceiling)) {
			return floor;
		} else {
			return input;
		}
	}
	
	float toCeiling(float input, float floor, float ceiling) {
		if (detectLow(input, floor)) {
			return ceiling;
		} else {
			return input;
		}
	}
	
	float wrapDown(float input, float ceiling) {
		while (input > ceiling) {
			input = input - 1;
		}
		return input;
	}
	
	float wrapUp(float input, float floor) {
		while (input < floor) {
			input = input + 1;
		}
		return input;
	}
	
	float ceilingClip(float input, float ceiling) {
		while (input > ceiling) {
			input = ceiling;
		}
		return input;
	}
	
	float floorClip(float input, float floor) {
		while (input < floor) {
			input = floor;
		}
		return input;
	}
	
	float ceilingSoftClip(float input, float ceiling) {
		float difference = input - ceiling;
		if (input > ceiling) {
			input = input - (difference/2);
		}
		return input;
	}
	
	float floorSoftClip(float input, float floor) {
		float difference = floor - input;
		if (input < floor) {
			input = input + (difference/2);
		}
		return input;
	}
	
	


	void process(const ProcessArgs& args) override {
		float input = getInput(MAIN_INPUT).getVoltage();
		float floor = getInput(FLOOR_HEIGHT_CV_INPUT).getVoltage() + (getParam(FLOOR_HEIGHT_PARAM).getValue());
		float ceiling = getInput(CEILING_HEIGHT_CV_INPUT).getVoltage() + (getParam(CEILING_HEIGHT_PARAM).getValue());
		float floorGate = 0.f;
		float ceilingGate = 0.f;
		int ceilingMode = int(getParam(CEILING_MODE_PARAM).getValue());
		int floorMode = int(getParam(FLOOR_MODE_PARAM).getValue());
		
		
		if (getInput(MAIN_INPUT).isConnected()) {
			if (detectLow(input, floor)) {
				floorGate = 10.f;
				ceilingGate = 0.f;
				switch (floorMode) {
					case 0: outputs[MAIN_OUTPUT].setVoltage(foldUp(input, floor));
					break;
					case 1: outputs[MAIN_OUTPUT].setVoltage(toCeiling(input, floor, ceiling));
					break;
					case 2: outputs[MAIN_OUTPUT].setVoltage(wrapUp(input, floor));
					break;
					case 3: outputs[MAIN_OUTPUT].setVoltage(floorClip(input, floor));
					break;
					case 4: outputs[MAIN_OUTPUT].setVoltage(floorSoftClip(input, floor));
					break;
					default: outputs[MAIN_OUTPUT].setVoltage(0.);
				}
			} else if (detectHigh(input, ceiling)) {
				floorGate = 0.f;
				ceilingGate = 10.f;
				switch (ceilingMode) {
					case 0: outputs[MAIN_OUTPUT].setVoltage(foldDown(input, ceiling));
					break;
					case 1: outputs[MAIN_OUTPUT].setVoltage(toFloor(input, ceiling, floor));
					break;
					case 2: outputs[MAIN_OUTPUT].setVoltage(wrapDown(input, ceiling));
					break;
					case 3: outputs[MAIN_OUTPUT].setVoltage(ceilingClip(input, ceiling));
					break;
					case 4: outputs[MAIN_OUTPUT].setVoltage(ceilingSoftClip(input, ceiling));
					break;
					default: outputs[MAIN_OUTPUT].setVoltage(0.);
				}
			} else {
			outputs[MAIN_OUTPUT].setVoltage(input);
			floorGate = 0.f;
			ceilingGate = 0.f;
		}
		outputs[FLOOR_GATE_OUTPUT].setVoltage(floorGate);
		outputs[CEILING_GATE_OUTPUT].setVoltage(ceilingGate);
		
		
 		}
 	}
};


struct Nonlinear_melody_transformerWidget : ModuleWidget {
	Nonlinear_melody_transformerWidget(Nonlinear_melody_transformer* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/nonlinear_melody_transformer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.76, 29.28)), module, Nonlinear_melody_transformer::FLOOR_MODE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(36.05, 29.28)), module, Nonlinear_melody_transformer::CEILING_MODE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.76, 55.00)), module, Nonlinear_melody_transformer::FLOOR_HEIGHT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(36.05, 55.00)), module, Nonlinear_melody_transformer::CEILING_HEIGHT_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.76, 73.15)), module, Nonlinear_melody_transformer::FLOOR_HEIGHT_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.05, 73.15)), module, Nonlinear_melody_transformer::CEILING_HEIGHT_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.76, 113.27)), module, Nonlinear_melody_transformer::MAIN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.76, 90.9)), module, Nonlinear_melody_transformer::FLOOR_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.05, 90.9)), module, Nonlinear_melody_transformer::CEILING_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.05, 113.27)), module, Nonlinear_melody_transformer::MAIN_OUTPUT));
	}
};


Model* modelNonlinear_melody_transformer = createModel<Nonlinear_melody_transformer, Nonlinear_melody_transformerWidget>("nonlinear_melody_transformer");