#include "I2S.h"

int i2s_num = 0; // i2s port number
int i2s_num_1 = 1;
headerState_t state = HEADER_RIFF;
wavProperties_t wavProps;

void I2S_Init(i2s_mode_t MODE, i2s_bits_per_sample_t BPS) {
  //i2s_driver_uninstall(I2S_NUM_0);
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | MODE ),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = BPS,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S |I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 64,
    .use_apll = false
  };
  i2s_pin_config_t pin_config;
  pin_config.bck_io_num = PIN_I2S_BCLK;
  pin_config.ws_io_num = PIN_I2S_LRC;
  if (MODE == I2S_MODE_RX) {
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    pin_config.data_in_num = PIN_I2S_DIN;
  }
  else if (MODE == I2S_MODE_TX) {
    pin_config.data_out_num = PIN_I2S_DOUT;
    pin_config.data_in_num = I2S_PIN_NO_CHANGE;
  }
  i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_1, &pin_config);
  i2s_set_clk(I2S_NUM_1, SAMPLE_RATE, BPS, I2S_CHANNEL_STEREO);
}


int I2S_Read(char* data, int numData) {
  return i2s_read_bytes(I2S_NUM_1, (char *)data, numData, portMAX_DELAY);
}

void I2S0_Write(char* data, int numData) {
    i2s_write_bytes(I2S_NUM_0, (const char *)data, numData, portMAX_DELAY);
}
void I2S1_Write(char* data, int numData) {
    i2s_write_bytes(I2S_NUM_1, (const char *)data, numData, portMAX_DELAY);
}
//************************************************//
void Play_I2S_Init(int samplerate){
  //i2s_driver_uninstall(I2S_NUM_0);
  i2s_config_t i2s_config = {
       .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
       .sample_rate = samplerate,
       .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, /* the DAC module will only take the 8bits from MSB */
       .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
       .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
       .intr_alloc_flags = 0, // default interrupt priority
       .dma_buf_count = 16,
       .dma_buf_len = 64,
       .use_apll = false
      };
  i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)i2s_num, NULL);
  i2s_set_clk((i2s_port_t)i2s_num, samplerate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
}
//i2s_pin_config_t pin_config = {
//    .bck_io_num = 26, //this is BCK pin
//    .ws_io_num = 25, // this is LRCK pin
//    .data_out_num = 22, // this is DATA output pin
//    .data_in_num = -1   //Not used
//};

void debug(uint8_t *buf, int len){
  for(int i=0;i<len;i++){
    Serial.print(buf[i], HEX);
    Serial.print("\t");
  }
  Serial.println();
}

/* write sample data to I2S */
int i2s_write_sample_nb(uint8_t sample,int bufSize){
  return i2s_write_bytes((i2s_port_t)i2s_num, (const char *)&sample, bufSize, portMAX_DELAY);
}
/* read 4 bytes of data from wav file */
int read4bytes(File file, uint32_t *chunkId){
  int n = file.read((uint8_t *)chunkId, sizeof(uint32_t));
  return n;
}

int readbyte(File file, uint8_t *chunkId){
  int n = file.read((uint8_t *)chunkId, sizeof(uint8_t));
  return n;
}

/* these are function to process wav file */
int readRiff(File file, wavRiff_t *wavRiff){
  int n = file.read((uint8_t *)wavRiff, sizeof(wavRiff_t));
  return n;
}
int readProps(File file, wavProperties_t *wavProps){
  int n = file.read((uint8_t *)wavProps, sizeof(wavProperties_t));
  return n;
}
