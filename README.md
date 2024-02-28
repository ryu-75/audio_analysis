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
<ul>
  <li>This program uses the <strong>fftw3</strong> library, a C library that allows for powerful computations using the Fourier theorem.</li>
</ul>
<h2>Signal Processing</h2>
<p>The audio signal is processed using low-pass and high-pass filters to eliminate undesirable frequencies that may disturb the analysis.</p>
<h3>Low-pass Filter</h3>
<img src="https://github.com/ryu-75/audio_analysis/assets/79263476/2e729310-2fec-4f0e-89e4-8a396256a22a"></img>
<h3>High-pass Filter</h3>
<img src="https://github.com/ryu-75/audio_analysis/assets/79263476/acab36db-bad0-40ff-b26e-de23b1ff3dcf"></img>

