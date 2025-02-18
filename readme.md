Modules for VCVRack v2.0 or higher.

Included modules:

    Scanning Clock Multiplier v2

    Scanning Frequency Division Oscillator

    Phase-Driven Multihit Envelope

    Melody Transformer


**Scanning Clock Multiplier v2**

![image](https://github.com/user-attachments/assets/4bea70ba-ca9f-4e53-acf1-e3d180a48c95)

The Scanning Clock Multiplier is a clock generator or clock modulator that operates on the concept of a user-defined list of "active ratios." The buttons and their corresponding gate inputs determine whether a given ratio is active or not, and the scan parameter allows users to scan through that list via the knob and/or 0-10v CV. By selecting your active ratios thoughtfully, you can orchestrate whole rhythm sections with just one or two of these modules.

clock rate and reset section:

![image](https://github.com/user-attachments/assets/82680d2c-ac55-4f28-8543-73fabdf2c7af)

With no rate input, the rate is defined by the knob, and ranges from 1-700 bpm, defaulting to 120. The rate input can accept either clock triggers (with the direct phase button off) or 0-10 phase input (with the direct phase button on). Direct phase is useful for daisy-chaining modules, and ensures the tightest sync; if clock pulses are sent instead, the module will require two pulses before it can calculate the rate, and any tempo changes will be delayed by a pulse before they take effect.
The reset in, out, and button work exactly like every other module, resetting the phase to 0 while the gate is held.

active ratio definition section:

![image](https://github.com/user-attachments/assets/d47c433b-3b66-4cd5-bce5-35111ced31ab)

The raison of the module. Each active ratio is added to a list of ratios from which the scanner and the various other outputs select. In this example, the 1:1, 16:5, 32:5, and 32:1 ratios are active. This means the scanner will scan through 4 regions, each containing one active ratio. If the scan value is <.25, it will return 1:1; if scan <.5, it will return 16:5; and so on. The low output will return the lowest ratio (1:1), the high output returns the highest, and the middle the middle. More on the outputs in a second. The inputs below each button are gates with a 1v threshold, and have no effect if the button is already on.

The non-integer ratios are unique in that they have a tail-end phase that doesn't complete before the cycle resets; they have familiar and useful rhythmic qualities.

Note that unlike the more common clock divider modules, each ratio here *increases* the rate of the clock. For that reason it often makes sense to have a very slow master clock.

scan section:

![image](https://github.com/user-attachments/assets/5089fddd-cf85-4481-bd73-90588be471b0)

scan: 

Think of the scan knob's range as being divided into a number of regions equal to the number of active ratios (as it quite literally is in the code). As the knob (or the input 0-10v CV) passes into each new region, the scan output equals that new value. If a sawtooth lfo is fed in, then it will spend the same amount of time in each region, but of course you're not limited to sawtooth waves. Stepped random voltage, s&h outputs, triangle and sine waves, and even v/oct (given a big enough keyboard) can all yield interesting results.

skew: 

applies an exponential function to the input phase, resulting in outputs that bunch up toward the beginning or end of the phase. for a 2:1 ratio, low amounts can sound like swing. At high ratios this can create interesting enveloping or burst effects.

![image](https://github.com/user-attachments/assets/6fece706-b6b8-4c1a-8988-703592efac1b)

skewed phase vs. normal phase.

phase: 

pushes the start and end of the phase farther "back" relative to the input phase.

![image](https://github.com/user-attachments/assets/8f6a6321-460d-4abf-86d5-27b0e6a994b9)

In the first oscilloscope, two phases with no phase applied. You can see they are identical. In the second, a phase shift of .5 is applied to the second signal.
Useful in cases where syncopation is desired, as in the case of a bass groove sitting in the pocket of a kick drum, for example.
Note that phase is applied based on the active ratio in the scanner, which can result in odd behavior from the ratios present at other outputs.


output section:

![image](https://github.com/user-attachments/assets/ba622242-e702-40ac-8cba-fb6206090f4c)

The outputs on the left are triggers, the outputs on the right are phasors.

rate:

The first outputs are through-puts of the rate, with no modulation applied.

scan:

Can be thought of as the "main" output of the module, its output is determined by the list of active ratios and the value of the scan knob summed with the scan cv input.

refl. scan:

Returns the ratio at the mirror image of the knob's position. At scan = 0, it returns the value of scan = 1. At scan = .75, it returns the value of scan = .25. And so on.

low:

Returns the lowest active ratio.

high:

Returns the highest active ratio.

middle:

Returns the middlemost active ratio. If there are an even number of active ratio, it chooses the higher of the two middlemost values.





**Phase-Driven Multihit Envelope**

![{D60FBC3D-B95D-4A34-9EB5-A035D62D94B7}](https://github.com/user-attachments/assets/c59d697c-cf23-40d4-ab76-3df02630fc9e)

The Phase-Driven Multihit Envelope takes either a 0-10v phase signal (basically, a sawtooth wave) or a clock signal, and outputs a series of 1-16 envelopes. It is not an envelope generator in the traditional sense of modulating a gate from a keyboard; instead, it is tempo-driven. It might be useful to think of it as a function generator.

The graphic display: 

![Rack_43ze0qgopO](https://github.com/user-attachments/assets/cf9dc82a-8e61-4121-a590-63e56bd2c172)

With a little bit of CV, things can quickly get interesting!

The gray triangle (or blob, if the spread value is low) represents the current state of the phase after applying skew and spread. The phase output will look the same through an oscilloscope. The gray vertical line, which is only active when an input is present at rate, indicates the current position of the phase. The white line depicts the current state of the envelope--here there are four hits on both sides of the skew, affected by moderate spread and phase->envelope. It isn't perfect--see below at attack and phase->envelope--but it gives a good approximation.

rate: 

input signal. With the rate button turned on (glowing white), the output values will be the envelope calculation times the current voltage of the input--a sawtooth wave (rising from 0-10v) will scan linearly through the envelopes; a triangle wave will scan linearly forwards, then linearly backwards; and so on. With the rate button turned off, the module will calculate the length of a phase based on the period of time between two clock triggers, and then scan linearly through that period(behaving as though the clock were a sawtooth). The primary benefit of using a phase wave rather than a clock is that the module will always be synced to the input--you can change the frequency of the input on the fly and the module adapts in real time. With a standard clock signal, it will always be one cycle behind any rate changes, as it has to recalculate the phase. It also takes two clock pulses to initialize, instead of beginning instantly. But it's fine if you expect the tempo to stay steady and find the word "phase-driven" intimidating.

vca in: 

the Phase-Driven Multihit Envelope has an internal VCA normalled to the envelope, so you can modulate an incoming signal (like a sound generator) without needing an external VCA. Not important in the world of VCVRack, where modules are free, but it's easy to implement, so why not include it.

attack: 

behaves similarly to attack on a normal envelope, except that it's calculated as a percentage of the phase divided by the number of hits, rather than as a length of time. The exponential knob to the left of the slider works as expected, although its effects are not rendered on the graphics widget. To visualize the effect it's having on the envelope, send the envelope output to an oscilloscope.

![{28A2E803-5051-4EE2-B026-8A596657F16D}](https://github.com/user-attachments/assets/7a3dab78-de2b-430d-a8ed-d57de5c3300a)

the exponential knobs on attack and decay are not reflected in the graphics widget, but they work as expected


sustain: 

the sustain level is always 100% (relative to phase->env--see below); unlike most envelope modules, this slider determines the DURATION of the envelope, again calculated as a percentage of the phase divided by the number of hits.

decay: 

like attack.

phase->env: 

reduces the amplitude of the envelope by an amount proportionate to the phase value. Easier to see than to describe:

![{44710D33-F918-40B1-BFE2-662FE0B00BD6}](https://github.com/user-attachments/assets/c9a313b8-a2a6-43e2-9dfd-d96e0afd5e74)

The top scope shows the envelope output with phase->env at 100%, the bottom at 0%. You can also compare the output of the first module to the graphics widget to see how the widget misrepresents the actual output signal.



hits: 

this controls how many times the envelope triggers per phase cycle. By default it ranges from 1-16, but there are several other options selectable from the context menu, which may be useful in different rhythmic contexts, particularly when CV is applied. It works by dividing the y-value of the phase into equal portions; as the phase rises through those portions, an envelope is output.

![{A72A9254-6F29-4841-98E9-1AADA895D991}](https://github.com/user-attachments/assets/d2d2619e-0630-486d-b0b4-ce411460ed5e)

context menu providing several options for hit scaling


skew: 

![{7AF39E20-07EC-438C-ABAE-345B2CD6233C}](https://github.com/user-attachments/assets/5706ebcb-0a1b-4ab1-83d4-42b1ad04b850)

skew at 1., skew at .66, and skew at 0.

An unusual consequence of the mirroring effect of skew is that during the descending phase, the attack and decay values are swapped; attack becomes decay, and vice versa. This can create interesting and complex effects, but it may not always be desirable, e.g. in percussive contexts. To compensate for this behavior, there is a trigger output available--you can use the module to calculate complex timings, but then send triggers to a concrete gate- or trigger-based envelope.

spread: 

![{E2B3F275-A4BB-4D95-B663-07EC62D61438}](https://github.com/user-attachments/assets/c643cce0-836f-4d7d-8b53-5f2c36918a1a)

no spread, logarithmic spread, exponential spread

spread applies a variable exponential or logarithmic function to the phase--the effect this has on the envelope is to cause it to linger longer on some envelopes, and move more quickly through others. Extreme values may cause some envelopes to disappear entirely. Again, this effect is easier to understand visually, and is accurately depicted on the graphics widget. And again, because this effect distorts the envelopes--although the effect can be quite organic--you can compensate by using the trigger output to trigger an external envelope generator.

env: 

the main envelope output. 0-10v.

trig: 

outputs triggers at the beginning of each attack phase. Useful if you want to use the unique timing effects of the module, but without the attendant envelope distortions.

vca: 

outputs the value of the vca input, multiplied by the envelope.

phase: 

outputs the phase after applying skew and spread--the shape indicated by a gray triangle on the widget.
