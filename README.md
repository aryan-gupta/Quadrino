# Quadrino

## Intro
I was deciding on a project that I could work on over the summer,
somthing that was not to hard and not to easy, something that would
complement my Major of Computer Engineering, something that would 
make me thing and grow as a person. I started out thinking of
creating a 3D printed moving, talking, tracking model of Portal's
turret, but after many design considerations I realized that the
arms of the turret are actually physically incapable of folding in
I then decided to work on a drone. It would be something fun and useful
and something that I could expand later on. I decided that it was too
easy to just buy the parts and put them together, no; I wanted to
program my own flight controller using a uC. After watching the
tutorial by Joop Brokking: https://www.youtube.com/user/MacPuffdog
And that was the birth of Quadino. 

## Parts
 - 4x [Hobbypower 2212 920KV Brushless Motor](https://www.amazon.com/gp/product/B01DF2D3U8/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1)
 - 4x [SimonK 30A ESC](https://www.amazon.com/gp/product/B01DF2D3U8/ref=oh_aui_detailpage_o02_s00?ie=UTF8&psc=1)
 - 1x [Flysky FS-i6X Transmitter w/ FS-iA6B Receiver](https://www.amazon.com/gp/product/B0744DPPL8/ref=od_aui_detailpages00?ie=UTF8&psc=1)
 - 1x [RipaFire F450 Quadcopter Frame](https://www.amazon.com/gp/product/B00XJFXYG0/ref=od_aui_detailpages00?ie=UTF8&psc=1)
 - 1x [GY-521 MPU-6050 IMU](https://www.amazon.com/gp/product/B008BOPN40/ref=oh_aui_detailpage_o03_s00?ie=UTF8&psc=1)
 - 1x [Arduino Nano v3](https://www.amazon.com/gp/product/B0713XK923/ref=oh_aui_detailpage_o03_s00?ie=UTF8&psc=1)
 - 1x [HRB 3S LiPi Battery](https://www.amazon.com/gp/product/B06XKWPXNM/ref=oh_aui_detailpage_o01_s01?ie=UTF8&psc=1)
 - 1x [RC B3 LiPo 2S-3S Battery Charger](https://www.amazon.com/gp/product/B06XR87987/ref=oh_aui_detailpage_o01_s01?ie=UTF8&psc=1)
 - 1x [RAYCorp 1045 Propellers](https://www.amazon.com/gp/product/B01CJMJ886/ref=od_aui_detailpages00?ie=UTF8&psc=1)
 - 1x [LHI XT90 Plug set](https://www.amazon.com/gp/product/B074WYMT9Y/ref=oh_aui_detailpage_o01_s01?ie=UTF8&psc=1)
 - 1x [SUNKEE F2F Dupont cable](https://www.amazon.com/gp/product/B00AYCON8Y/ref=oh_aui_detailpage_o00_s00?ie=UTF8&psc=1)

## Modules
The code is split into separate modules that were tested individually
 - PWM output
 - PWM input
 - iBus
 - I<sup>2</sup>C (named I2C)
 - MPU
 - PID
 
You can review each module with its test code separately

## Prerequisites
Please make sure that the USART/Serial library that Arduino automatically
includes does not have `ISR(USART_RX_vect)` function or there will be 
compilation errors

## Contributing
 1. Fork it!
 2. Create your feature branch: git checkout -b my-new-feature
 3. Commit your changes: git commit -am 'Add some feature'
 4. Push to the branch: git push origin my-new-feature
 5. Submit a pull request :D

## Authors
* **Aryan Gupta** - *The Whole Shebang* - [My Site](https://theguptaempire.net)

## License
Copyright (c) 2017 The Gupta Empire - All Rights Reserved

Please see Licence.md file for more information

## Acknowledgments
* Thank you [Joop Brokking](http://http://www.brokking.net)
* Thank you Michael Ajazi for typing a 3
* Thanks for all my friends that gave me inspiration and ideas
* Thanks to anyone else that may have slipped my mind

## Design Choices
### Receiver input
The original idea for this was to use similar code that Joop Brokking
used: PWM. The uC would record the time a signal would turn high then
use that to get high time when the signal would turn low. There were
many disadvantages to this: first, the interrupt routine was very long
and it was very inaccurate and prone to jitter, lastly, each channel
would need its own signal wire to the uC. Inherently, PWM is an analog
signal where the length of the pulse is the data. I debated on whether 
to use PPM, however I realized that the frame of a PPM signal is very
long and would take a max time of 2ms per channel plus the frame
separation time (about 20ms). Our uC is supposed to update every
6ms meaning if we used PPM, we would get new data every 4 updates.
Then I read about a proprietary signaling method called iBus. 
This communication protocol is specific to FlySky receivers and took
some googling to find resources that had reverse engineered the
protocol. You can read about it here:

 - https://github.com/povlhp/iBus2PPM/blob/master/iBus2PPM.ino
 - https://basejunction.wordpress.com/2015/08/23/en-flysky-i6-14-channels-part1/
 
The iBus protocol sends new data every 7ms, meaning we would only need
a little over 1 update to get new data. 

### iBus
The original design for iBus was it used the Hardware Serial library to 
store the bytes as it came in, then cycle would read it. I decided it was
too slow so I created a separate ISR for this. I needed a way for the main
program to access the buffer without the ISR interrupting it and possibly
corrupt our data. I decided to use a tri-state double buffer (I cant stop
saying it in Dr. Heinz Doofenshmirtz's voice). This is copied from the
iBus.h file:

	There are 2 buffers, as one is getting filled the other is available
	to be used by the main program. The buffers will be protected by a 
	tri-state lock. (pretty much and atomic uint8_t). These are the states
		0. Data is available for use
		1. The data will be overwritten within 5ms
		2. Data is being used by the main program
	The basic principle is this. This interrupt set one of the buffer
	state to 1, then it will start filling up the buffer. Once the buffer
	is filled, it will change the state to 0 and set the other buffer's
	state to 1. If the main program is using this other buffer, it has about
	5ms to finish using it (See basejunction's post above why it is 5ms).
	The main program will check each buffer for state 0 and use that buffer
	as its working buffer. It will set it to state 2 and continue using it. 
	The main program will encounter only 2 states: 0 and 1. If it
	encounters 1, then it will use the other buffer (because this buffer
	is being filled). The ISR will encounter all 3 states, however it will
	only act on state 1 buffer. It will find the buffer with state 1 and
	fill it up. Once it has filled up the buffer it will change this to
	state 0 and change the other's state to 1, doest matter if its a 0 or
	a 2. The main point of state 1 is minimize the delay of the input data
	to a maximum of 7ms. We don't need to label any of these as volatile
	because only one function will be using one buffer. nobody on state 0, 
	ISR on state 1, and main function on state 2

### I<sup>2</sup>C
I started out with I2C using the Wire library, however I found that it was
too slow, I decided to write my own library. This way I was able to set the
I2C speed and remove the error checking process (ehh, we don't really need
it). I based this code from this tutorial:

 - https://embedds.com/programming-avr-i2c-interface/

### PWM
I learned how PWM woks from Joop Brokking's video, however he used a simple 
loop that brought each esc signal down when it needed to. I believed that 
this was a very inefficient solution and wanted to use the output compare
feature of the atmega328p. The problem with this is that we have 2 compare
registers and 4 output signals. My first design was to split the 4 esc's
into 2 parts. OCRA would control one set of escs and the OCRB would control
the other set. However I found this very inefficient, because the ISR would
have to first bring the signal LOW then set up the register for the other
signal. If all 4 signals were within 25us of each other then the ISR would
compete and we would get error of up to 50us, which was unacceptable. I
then had the idea of splitting the esc pulses one after the other:

	// Phase --  P1  >>  P2  >>  P3
	// Ticks -- 100  >> 4050 >> 4050
	// Int   -- iBus >> ESC1 >> ESC2
	// Exec  -- iBus >> MPU  >> PID
 
From this diagram you can see what I mean. I split the cycle into 3 phases. 
The first phase would process the iBus input data, this would elapse 100 ticks
or 50us. Then the 2nd phase would start, one set of the esc would be set high and
the OCR would be set to when the signal should be set back low. Then when Phase 3
starts, the other 2 escs would be signaled. This way only 2 esc's will be signaled
at a time. Meaning we only need 2 interrupts, OCRA and OCRB. While the esc's are
being signaled, we can process the data for the next cycle. 
