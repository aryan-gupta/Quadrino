# Quadrino

## Intro
I was deciding on a project that I could work on over the summer,
somthing that was not to hard and not to easy, somthing that would
complement my Major of Computer Engineering, somthing that would 
make me thing and grow as a person. I started out thinking of
creating a 3D printed moving, talking, tracking model of Portal's
turrent, but after many design considerations I realized that the
arms of the turrent are actually physically incapable of folding in
I then decided to work on a drone. It would be somthing fun and useful
and somthing that I could expand later on. I decied that it was too
easy to just buy the parts and put them together, no; I wanted to
program my own flight controller using a uC. After watching the
tutorial by Joop Brokking: https://www.youtube.com/user/MacPuffdog
And that was the birth of Quadino. 

## Parts


## Modules
The code is split into seperate modules that were tested indivisually
 - PWM output
 - PWM input
 - iBus
 - I<sup>2</sup>C (named I2C)
 - MPU
 - PID
You can review each modeule with its test code seperately

### Reciver input
The original idea for this was to use similar code that Joop Brokking
used: PWM. The uC would record the time a signal would turn high then
use that to get high time when the signal would turn low. There were
many disadvantages to this: first, the inturupt rutine was very long
and it was very inaccurate and prone to jitter, lastly, each channel
would need its own signal wire to the uC. Inherently, PWM is an analog
signal where the leagth of the pulse is the data. I debated on whether 
to use PPM, however I realized that the frame of a PPM signal is very
long and would take a max time of 2ms per channel plus the frame
seperation time (about 20ms). Our uC is suposed to update every
6ms meaning if we used PPM, we would get new data every 4 updates.
Then I read about a  propriatary signaling meathod called iBus. 
This communication protocol is spesific to FlySky recivers and took
some googling to find resorces that had reverse engineered the
protocol. You can read about it here:
 - https://github.com/povlhp/iBus2PPM/blob/master/iBus2PPM.ino
 - https://basejunction.wordpress.com/2015/08/23/en-flysky-i6-14-channels-part1/
The iBus protocol sends new data every 7ms, meaning we would only need
a little over 1 update to get new data. 
