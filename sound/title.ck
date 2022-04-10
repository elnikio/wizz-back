.5::second => dur T;
T - (now % T) => now;

SinOsc s => JCRev r => dac;
.05 => s.gain;
.25 => r.mix;

// scale (in semitones)
[ 0, 2, 4, 7, 9 ] @=> int scale[];

// infinite time loop
while( true )
{
    // get note class
    scale[ Math.random2(0,4) ] => float freq;
    // get the final freq
    Std.mtof( 69 + (Math.random2(0,3)*12 + freq) ) => s.freq;
    // reset phase for extra bandwidth
    0 => s.phase;

    // advance time
    // note: Math.randomf() returns value between 0 and 1
    if( Math.randomf() > .25 ) .25::T => now;
    else .5::T => now;
}
