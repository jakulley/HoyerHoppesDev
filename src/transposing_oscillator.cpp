#include "plugin.hpp"

using namespace std;


struct Transposing_oscillator : Module {
	enum ParamId {
		FINE_PITCH_PARAM,
		PITCH_PARAM,
		FOLLOW_SWITCH_PARAM,
		AR_DURATION_PARAM,
		FM_ATTENUVERTER_PARAM,
		AR_RATIO_PARAM,
		VCA_PARAM,
		VCA_INPUT_ATTENUVERTER_PARAM,
		PWM_PARAM,
		PWM_INPUT_ATTENUVERTER_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		PITCH_INPUT,
		AR_DURATION_INPUT,
		FM_INPUT,
		FM_DEPTH_INPUT,
		AR_RATIO_INPUT,
		VCA_INPUT,
		PWM_INPUT,
		SYNC_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		SQUARE_OUTPUT,
		AR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Transposing_oscillator() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(FINE_PITCH_PARAM, -0.5f, 0.5f, 0.f, "Fine pitch");
		configParam(PITCH_PARAM, -5.f, 5.f, 0.f, "Pitch");
		configSwitch(FOLLOW_SWITCH_PARAM, 0.f, 1.f, 0.f, "If on, AR duration follows pitch in", {"Follow", "Ignore"});
		configParam(AR_DURATION_PARAM, -4.99f, 4.99f, 0.f, "");
		configParam(FM_ATTENUVERTER_PARAM, -1.f, 1.f, 0.f, "");
		configParam(AR_RATIO_PARAM, 0.01f, 0.99f, 0.5f, "");
		configParam(VCA_PARAM, 0.f, 1.f, 1.f, "");
		configParam(VCA_INPUT_ATTENUVERTER_PARAM, -1.f, 1.f, 0.5f, "");
		configParam(PWM_PARAM, 0.1f, 0.9f, 0.5f, "expects 0-10v");
		configInput(PITCH_INPUT, "Pitch input (v/oct)");
		configInput(AR_DURATION_INPUT, "essentially pitch (v/oct)");
		configInput(FM_INPUT, "");
		configInput(FM_DEPTH_INPUT, "");
		configInput(AR_RATIO_INPUT, "");
		configInput(VCA_INPUT, "");
		configInput(PWM_INPUT, "expects 0-10v");
		configInput(SYNC_INPUT, "");
		configOutput(SQUARE_OUTPUT, "");
		configOutput(AR_OUTPUT, "");
	}

	float phase = 0.f;
	float arPhase = 0.f;
	float arPitch = 0.f;
	dsp::SchmittTrigger schmittTrigger;

	float pwmWave(float phase, float pulseWidth) {
		if (phase < pulseWidth) {
			return 0.5f;
		} else {
			return -0.5f;
		}
	}

	float arWave(float arPhase, float arRatio) {
		if (arPhase < arRatio) {
			return (arPhase / arRatio);
		}else {
			return (1-arPhase) / (1-arRatio);
		}
	}

	void process(const ProcessArgs& args) override {

		float pitch = getParam(PITCH_PARAM).getValue() + getParam(FINE_PITCH_PARAM).getValue();
		pitch += getInput(PITCH_INPUT).getVoltage();
		pitch += getInput(FM_INPUT).getVoltage() * getParam(FM_ATTENUVERTER_PARAM).getValue();
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);
		phase += freq * args.sampleTime;
		if (phase >= 1.f) {
			phase -= 1.f;
		}
		if (getParam(FOLLOW_SWITCH_PARAM).getValue() == 1.) {
			arPitch = pitch / (std::trunc(getParam(AR_DURATION_PARAM).getValue() + getInput(AR_DURATION_INPUT).getVoltage())+5.f);
		} else {
			arPitch = getParam(AR_DURATION_PARAM).getValue();
			arPitch += getInput(AR_DURATION_INPUT).getVoltage();
		}
		float arRatio = getParam(AR_RATIO_PARAM).getValue();
		arRatio += (getInput(AR_RATIO_INPUT).getVoltage()/10);
		float arFreq = dsp::FREQ_C4 * std::pow(2.f, arPitch);
		arPhase += arFreq * args.sampleTime;
		if (arPhase >= 1.f) {
			arPhase -= 1.f;
		}

		float pulseWidth = getParam(PWM_PARAM).getValue();
		if (getInput(PWM_INPUT).isConnected()) {
			//the PWM_INPUT formula prevents low input values from breaking up the signal--the expression never evaluates to less than .1 or more than 1, and maintains linearity.
			pulseWidth = pulseWidth * ((((getInput(PWM_INPUT).getVoltage() + 1.f)*.9f))/10);
		}

		getOutput(AR_OUTPUT).setVoltage((10.f * arWave(arPhase, arRatio)) - 5.f);
		getOutput(SQUARE_OUTPUT).setVoltage(10.f * pwmWave(phase, pulseWidth));
		
	}
};


struct Transposing_oscillatorWidget : ModuleWidget {
	Transposing_oscillatorWidget(Transposing_oscillator* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/transposing_oscillator.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(66.927, 13.571)), module, Transposing_oscillator::FINE_PITCH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(40.64, 29.833)), module, Transposing_oscillator::PITCH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(73.05, 54.408)), module, Transposing_oscillator::FOLLOW_SWITCH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(50.272, 54.668)), module, Transposing_oscillator::AR_DURATION_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(12.017, 62.475)), module, Transposing_oscillator::FM_ATTENUVERTER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(55.487, 72.104)), module, Transposing_oscillator::AR_RATIO_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.757, 83.177)), module, Transposing_oscillator::VCA_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(24.776, 83.177)), module, Transposing_oscillator::VCA_INPUT_ATTENUVERTER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.496, 100.47)), module, Transposing_oscillator::PWM_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.579, 39.718)), module, Transposing_oscillator::PITCH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(62.508, 54.408)), module, Transposing_oscillator::AR_DURATION_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.865, 62.475)), module, Transposing_oscillator::FM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(35.714, 62.475)), module, Transposing_oscillator::FM_DEPTH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(70.325, 71.844)), module, Transposing_oscillator::AR_RATIO_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.796, 83.177)), module, Transposing_oscillator::VCA_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.316, 100.47)), module, Transposing_oscillator::PWM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.251, 118.166)), module, Transposing_oscillator::SYNC_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.554, 95.742)), module, Transposing_oscillator::SQUARE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(66.554, 109.275)), module, Transposing_oscillator::AR_OUTPUT));
	}
};


Model* modelTransposing_oscillator = createModel<Transposing_oscillator, Transposing_oscillatorWidget>("transposing_oscillator");
