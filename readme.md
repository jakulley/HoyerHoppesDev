Modules for VCVRack v2.0 or higher.

Included modules:

    Scanning Clock Multiplier v2

    Scanning Frequency Division Oscillator

    Phase-Driven Multihit Envelope

    Melody Transformer



Phase-Driven Multihit Envelope

![{D60FBC3D-B95D-4A34-9EB5-A035D62D94B7}](https://github.com/user-attachments/assets/c59d697c-cf23-40d4-ab76-3df02630fc9e)

The Phase-Driven Multihit Envelope takes either a 0-10v phase signal (basically, a sawtooth wave) or a clock signal, and outputs a series of 1-16 envelopes. It is not an envelope generator in the traditional sense of modulating a gate from a keyboard; instead, it is tempo-driven. It might be useful to think of it as a function generator.

The graphic display: 

![{64A5233C-E701-440C-AA55-52950F95F360}](https://github.com/user-attachments/assets/770e72df-0827-47b3-8c9c-8bfdcfd06091)

Here we see how the calculations of the multihit envelope can quickly get interesting. The gray triangle represents the current state of the phase after applying skew and spread. The phase output will look the same through an oscilloscope. The gray vertical line, which is only active when an input is present at rate, indicates the current position of the phase. The white line depicts the current state of the envelope--here there are four hits on both sides of the skew, affected by moderate spread and phase->envelope. It isn't perfect--see below at attack and phase->envelope--but it gives a good approximation.

rate: 
input signal. With the rate button turned on (glowing white), the output values will be the envelope calculation times the current voltage of the input--a sawtooth wave (rising from 0-10v) will scan linearly through the envelopes; a triangle wave will scan linearly forwards, then linearly backwards; and so on. With the rate button turned off, the module will calculate the length of a phase based on the period of time between two clock triggers, and then scan linearly through that period(behaving as though the clock were a sawtooth). The primary benefit of using a phase wave rather than a clock is that the module will always be synced to the input--you can change the frequency of the input on the fly and the module adapts in real time. With a standard clock signal, it will always be one cycle behind any rate changes, as it has to recalculate the phase. It also takes two clock pulses to initialize, instead of beginning instantly. But it's fine if you expect the tempo to stay steady and find the word "phase-driven" intimidating.

vca in: 
the Phase-Driven Multihit Envelope has an internal VCA normalled to the envelope, so you can modulate an incoming signal (like a sound generator) without needing an external VCA. Not important in the world of VCVRack, where modules are free, but it's easy to implement, so why not include it.

attack: 
behaves similarly to attack on a normal envelope, except that it's calculated as a percentage of the phase divided by the number of hits, rather than as a length of time. The exponential knob to the left of the slider works as expected, although its effects are not rendered on the graphics widget. To visualize the effect it's having on the envelope, send the envelope output to an oscilloscope.

sustain: 
the sustain level is always 100% (relative to phase->env--see below); unlike most envelope modules, this slider determines the DURATION of the envelope, again calculated as a percentage of the phase divided by the number of hits.

decay: 
like attack.

phase->env: 
reduces the amplitude of the envelope by an amount proportionate to the phase value. Easier to see than to describe:

![{44710D33-F918-40B1-BFE2-662FE0B00BD6}](https://github.com/user-attachments/assets/c9a313b8-a2a6-43e2-9dfd-d96e0afd5e74)

The top scope shows the envelope output with phase->env at 100%, the bottom at 0%. You can also compare the output of the first module to the graphics widget to see how the widget misrepresents the actual output signal.



hits: this controls how many times the envelope triggers per phase cycle. By default it ranges from 1-16, but there are several other options selectable from the context menu, which may be useful in different rhythmic contexts, particularly when CV is applied. It works by dividing the y-value of the phase into equal portions; as the phase rises through those portions, an envelope is output.

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
