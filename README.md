# ScenarioX
Second Year EEE Scenario X project: Real-time audio descrambler

Scenario Context:

A gang of criminals are planning to rob a security van carrying a large amount of gold.  To ensure their plan will succeed, they have been practising their act before the planned robbery.  They are concerned that the police might discover their secret plan because they communicate with each other over insecure telephone lines. 

 
To stop the police from intercepting their communication, they ask Professor Peach, who is a computer expert with a particular interest in traffic control systems, to find a way to secure their telephone conversations.  Professor Peach proposes to make a number of audio scramblers to scramble the telephone conversations so even if the police intercept their communication, the audio message will still be unintelligible.


However, Professor Peach has himself been arrested by the police for his lewd act in public shortly before the planned robbery.  At Professor Peach’s home, the police find a highly sophisticated audio scrambler and a recorded scrambled message.  When the police replay the scrambled message, they cannot understand it at all because it is scrambled and Professor Peach is not collaborating with the police.  They need your help to descramble the audio message and stop the robbery.




Objectives:

1) work out how the scrambling is done by recording your own voice and analysing the scrambled message produced by the scrambling program “The scrambler file” written in the protected Matlab code.

2) design and build a digital real-time audio descrambler to work out what the secret message is which will be made available on Thursday.  





System Block Diagram:

Scrambled signal (+ DC offset) -> MSP432 (ADC + DSP*) -> DAC (R-2R ladder) -> Low-pass filter (anti-aliasing) -> Speaker 
