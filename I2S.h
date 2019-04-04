#include <Arduino.h>
#include <driver/dac.h>
#include "FS.h"
#include "SPIFFS.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "esp_system.h"

#define SAMPLE_RATE (44100)
#define PIN_I2S_BCLK 35//(sck)
#define PIN_I2S_LRC 3//22//(ws)
#define PIN_I2S_DIN 36//34//(sd)
#define PIN_I2S_DOUT 22
#define CCCC(c1, c2, c3, c4)    ((c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

/* these are data structures to process wav file */
typedef enum headerState_e {
    HEADER_RIFF, HEADER_FMT, HEADER_DATA, DATA
} headerState_t;

typedef struct wavRiff_s {
    uint32_t chunkID;
    uint32_t chunkSize;
    uint32_t format;
} wavRiff_t;

typedef struct wavProperties_s {
    uint32_t chunkID;
    uint32_t chunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
} wavProperties_t;




// This I2S specification : 
//  -   LRC high is channel 2 (right).
//  -   LRC signal transitions once each word.
//  -   DATA is valid on the CLOCK rising edge.
//  -   Data bits are MSB first.
//  -   DATA bits are left-aligned with respect to LRC edge.
//  -   DATA bits are right-shifted by one with respect to LRC edges.
//        It's valid for ADMP441 (microphone) and MAX98357A (speaker). 
//        It's not valid for SPH0645LM4H(microphone) and WM8960(microphon/speaker).
//
//  -   44100Hz
//  -   stereo

/// @parameter MODE : I2S_MODE_RX or I2S_MODE_TX
/// @parameter BPS : I2S_BITS_PER_SAMPLE_16BIT or I2S_BITS_PER_SAMPLE_32BIT
void I2S_Init(i2s_mode_t MODE, i2s_bits_per_sample_t BPS);

/// I2S_Read() for I2S_MODE_RX
/// @parameter data: pointer to buffer
/// @parameter numData: buffer size
/// @return Number of bytes read
int I2S_Read(char* data, int numData);

/// I2S_Write() for I2S_MODE_TX
/// @param data: pointer to buffer
/// @param numData: buffer size
void I2S0_Write(char* data, int numData);
void I2S1_Write(char* data, int numData);
//****************************************//
void Play_I2S_Init(int samplerate);
int i2s_write_sample_nb(uint8_t sample,int bufSize);
int read4bytes(File file, uint32_t *chunkId);
int readbyte(File file, uint8_t *chunkId);
int readRiff(File file, wavRiff_t *wavRiff);
int readProps(File file, wavProperties_t *wavProps);
