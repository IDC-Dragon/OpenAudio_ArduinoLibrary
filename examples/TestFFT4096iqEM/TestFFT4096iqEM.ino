// TestFFT2048iqEM.ino  for Teensy 4.x
// Bob Larkin 9 March 2021

// Generate Sin and Cosine pair and input to IQ FFT.
// Serial Print out powers of all 4096 bins in
// dB relative to Sine Wave Full Scale
//       EXTERNAL MEMORY FFT
// Public Domain

#include "OpenAudio_ArduinoLibrary.h"
#include "AudioStream_F32.h"

// Memory for IQ FFT
float32_t  fftOutput[4096];  // Array to allow fftBuffer[] to be available for new in data
float32_t  window[2048];     // Half size window storage
float32_t  fftBuffer[8192];  // Used for FFT, 4096 real, 4096 imag, interleaved
float32_t  sumsq[4096];      // Required if power averaging is being done

int jj;

// GUItool: begin automatically generated code
AudioSynthSineCosine_F32   sine_cos1;       //xy=76,532
//                                                                         Optional
// (float32_t* _pOutput, float32_t* _pWindow, float32_t* _pFFT_buffer, float32_t* _pSumsq)
//AudioAnalyzeFFT4096_IQEM_F32 FFT4096iqEM1(fftOutput, window, fftBuffer);      //xy=243,532
AudioAnalyzeFFT4096_IQEM_F32 FFT4096iqEM1(fftOutput, window, fftBuffer, sumsq);  // w/ power ave
AudioOutputI2S_F32         audioOutI2S1;    //xy=246,591
AudioConnection_F32        patchCord1(sine_cos1, 0, FFT4096iqEM1, 0);
AudioConnection_F32        patchCord2(sine_cos1, 1, FFT4096iqEM1, 1);
// GUItool: end automatically generated code

void setup(void) {

  Serial.begin(9600);
  delay(1000);

  // The 4096 complex FFT needs 32 F32 memory for real and 32 for imag.
  // Set memory to more than 64, depending on other useage.
  AudioMemory_F32(100);
  Serial.println("FFT4096IQem Test");

  sine_cos1.amplitude(1.0f); // Initialize Waveform Generator

  // Pick T4.x bin center
  //sine_cos1.frequency(689.0625f);

  // or pick any old frequency
  sine_cos1.frequency(1000.0f);

  // elect the output format, FFT_RMS, FFT_POWER, or FFT_DBFS
  FFT4096iqEM1.setOutputType(FFT_DBFS);

  // Select the wndow function, designed by FFT object
  //FFT4096iqEM1.windowFunction(AudioWindowNone);
  //FFT4096iqEM1.windowFunction(AudioWindowHanning4096);
  //FFT4096iqEM1.windowFunction(AudioWindowKaiser4096, 55.0f);
  FFT4096iqEM1.windowFunction(AudioWindowBlackmanHarris4096);

  // Uncomment to Serial print window function
  // for (int i=0; i<2048; i++) Serial.println(*(window+i), 7);

  // xAxis, bit 0 left/right;  bit 1 low to high;  default 0X03
  FFT4096iqEM1.setXAxis(0X01);

  // In order to average powers, a buffer for sumsq[4096] must be
  // globally declared and that pointer, sumsq, set as the last
  // parameter in the object creation.  Then the following will
  // cause averaging of 4 powers:
  FFT4096iqEM1.setNAverage(20);

  jj = 0;   // This is todelay data gathering to get steady state
  }

void loop(void)  {
  static bool doPrint=true;
  float *pPwr;

  delay(10);

  // Print output, once
  if( FFT4096iqEM1.available() && doPrint )  {
	  if(jj++ < 3)return;
      for(int i=0; i<4096; i++)
        {
        Serial.print((int)((float32_t)i * 44100.0/4096.0));
        Serial.print(" ");
        Serial.println(*(fftOutput + i), 8 );
	    }
      doPrint = false;
      }
  Serial.print(" Audio MEM Float32 Peak: ");
  Serial.println(AudioMemoryUsageMax_F32());
  delay(500);
  }
