rendersound
===========

Render waveforms from sound files as png images.

About
-----

Rendersound is a small utility that draws the waveform of a soundfile as a .png image.
I.e. given a file sound.wav, the following command:

    rendersound sound.wav sound.png

Would create an image looking something like this:

![Image](/test/out.png)

There are options to control the width, height and color of the generated image, as well as some other options, 
try rendersound -h for a full list of command line options.

A possible use is for batch processing on sound related websites, but there might be other uses for this little tool, 
who knows...

Input formats
-------------

Rendersound uses ffmpeg, http://www.ffmpeg.org/, so it supports all input formats of ffmpeg supports.

Installation
------------

Download and install like so:

    git clone 
