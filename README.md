# Audio File Analyzer

<p>This tool allows you to analyze one or more audio files to extract their fundamental frequency and determine the corresponding note in the correct octave.</p><br>
<p>Once the note is found, the program renames the audio files by appending the corresponding fundamental note.</p>

<h2>Usage</h2>
<ol>
  <li>Make sure you have C++ installed on your system.</li>
  <li>Compile the program with <strong>make</strong></li>
  <li>Then:</li>
    <img src="https://github.com/ryu-75/audio_analysis/assets/79263476/f5a6e80f-aad5-4919-ba3f-85d416df1d08"></img><br>
    <span>Replace <strong>PATH</strong> with the absolute or relative path to the folder containing the audio files to be analyzed.</span>
  </li>
</ol>

<h2>Dependencies</h2>
<h3>FFTW3 Library</h3>
<ul>
  <li>This program uses the <strong>fftw3</strong> library, a C library that allows for powerful computations using the Fourier theorem.</li>
</ul>
<h3>Libsndfile Library</h3>
<ul>
  <li>This program also makes use of the <strong>libsndfile</strong> library, another C library allowing for reading and writing audio files containing audio samples.</li>
</ul>

<h2>Signal Processing</h2>
<p>The audio signal is processed using low-pass and high-pass filters to eliminate undesirable frequencies that may disturb the analysis.</p>

<h3>Low-pass Filter</h3>
<img src="https://github.com/ryu-75/audio_analysis/assets/79263476/2e729310-2fec-4f0e-89e4-8a396256a22a"></img>

<h3>High-pass Filter</h3>
<img src="https://github.com/ryu-75/audio_analysis/assets/79263476/acab36db-bad0-40ff-b26e-de23b1ff3dcf"></img>

<h3>Finding the Fundamental Frequency</h3>
<p>The fundamental frequency is determined by searching for the highest amplitude peak in the audio signal.</p>
<img src="https://github.com/ryu-75/audio_analysis/assets/79263476/7ceeb8f3-63ab-4948-9af9-8cfea3b8c0b7"></img>

<h3>Converting to Musical Note</h3>
<p>Once the fundamental frequency is found, it is converted into the corresponding musical note based on a frequency range from 30 Hz to 20,000 Hz after signal processing. The reference pitch used for this conversion is A4 at 440 Hz.</p>
<img src="https://github.com/ryu-75/audio_analysis/assets/79263476/1a402502-e9c4-4d0e-afbc-28bf93359c3e"></img>

<h2>Example</h2>
<p>Suppose you have a folder <strong>audio_files</strong> containing the following files:</p>
<ul>
  <li><strong>sample1.wav</strong></li>
  <li><strong>sample2.wav</strong></li>
</ul>
<p>After compiling the program with the <strong>audio_files</strong>strong> folder as an argument, the files will be renamed as follows:</p>
<ul>
  <li><strong>sample1.wav</strong></li>
  <li><strong>sample2.wav</strong></li>
</ul>
<h2>Notes</h2>
<ul>
  <li>The program only supports <strong>WAV</strong> files.</li>
  <li>Ensure that the audio files are properly recorded and that the fundamental frequency can be accurately extracted.</li>
</ul>
