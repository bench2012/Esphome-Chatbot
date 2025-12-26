# Esphome-Chatbot
Esphome based chatbot using esp32s3, oled, i2s microphone and a speaker 

Find this on Aliexpress whichis the cheapest all-in-one ESP32S3 AI chatbot board. It basically consists of an XH-S3E, which accounting to this web site:

https://fcc.report/FCC-ID/2BCLR-XH-S3E/6776438.pdf

is just an ESP32S3-N16R8 and some peripherals; an i2s Micophone, an i2s Mono D-Amp with speaker connector, three buttons an WS2812 (GPIO. It has pin out for 3.3v/GND, i2c (SDA/GPIO41 and SCL/GPIO42) , VIn (5V??), TX/RX.  

GPIO Allocation was as below:

| **GPIO** | **Function**  | **Board Marking** |
|----------|---------------|-------------------|
|    40    | Button        | Volume +          |
|    39    | Button        | Volume -          |
|    48    | WS2812 DIN    | RGB LED           |
|    41    | I2C SDA       | SDA               |
|    42    | I2C SCL       | SCL               |
|     4    | I2S  WS/LRCLK | On Chip           |
|     5    | I2S SCK/BCK   | On Chip           |
|    16    | i2S LRC/LRCLK | On Chip           |
|    15    | I2S BLCK/BCLK | On Chip           |
|     6    | I2S Din       | Mic Dout          |
|     7    | I2S Dout      | D-Amp Din         |
|     0    | Boot          | Boot              |
|    RST   | Reset         | RST               |


