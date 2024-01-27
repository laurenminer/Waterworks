/* Write an array of firing frequencies and pulse widths and change them with a computer
 
 Clumsily rewritten and built into monstrous spaghetti code over many iterations.
 My apologies to anyone else reading this. Perhaps one day I'll clean it up.
 
 For the below code, the following convention is used:
 Frequencies are in Hz
 Pulse widths are in milliseconds
 
 The LED array is indexed as follows:
 -----------------
||  1     2    3  ||
||                ||
||  4     5    6  ||
||                ||
||  7     8    9  ||
||----------------||
 
 
 NOTE: The Buckpuck interpets a high control voltage as
 off and a low control voltage as on. It's stupid, but it
 probably made life easier for the guy who designed it. Oh
 well. Anyways, that explains why the digital write commands
 seem backwards.
 
 Stephen Thornquist March 22, 2015
 */
 
const int numPins = 12;

int onVal = 210;
const int offVal = 255;

// This line is to map the schematized well number above onto the right entry in the pin array
// wellMap's nth entry is the index of Well #n in the array pin. It basically
// exists to make up for my sloppy wiring job. When I make a PCB I can fix this
// to make it better.
// our input looks like: {0,1,2,3,4,5,6,7,8,9,10,11,12}
int wellMap[numPins+1] = {12,0,1,2,3,4,5 ,6,7,8,9,10,11};
//int wellMap[numPins+1] = {0,1 ,2 ,3 ,4,5 ,6, 7, 8,9,10,11,12};
//int wellMap[numPins+1] = {12,11, 9, 2, 0,10,7, 4, 1, 6,8,3, 5};
int gpin[numPins] = {2,4,5,3,6,7,8,9,10,11,12,13};
int rpin[numPins] = {24,26,27,25,28,29,30,31,32,33,34,35};
int bpin[numPins] = {36,38,39,37,40,41,42,43,44,45,46,47};
// Timescale says how many of our selected timescale are in a second
// so if we use microseconds, we should switch this to 1000000
const unsigned long timescale = 1000;
// startTime is how long until it should start
unsigned long startTime = 00000;
// duration is how long the Arduino should run its protocol.
unsigned long duration = 12000000; // 200 minutes
// endOfDays is when the protocol should end
unsigned long endOfDays = startTime + duration;

// This block is for single wells with ongoing frequenices
// Array of frequencies desired (in Hz)
// To tell a controller to be constantly off, input 0 for frequency
float rfreq[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
float gfreq[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
float bfreq[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
// Array of pulse widths desired (in milliseconds)
// You don't need to worry about pulse width for constant
// LEDs.
unsigned long rpulseWidth[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long gpulseWidth[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long bpulseWidth[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};

// This next block is for paired pulses:
// an array indicating at what time you gave the signal to count down from the delay
unsigned long rpinInit[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long gpinInit[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long bpinInit[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};

// Delay until start (minutes)
double rdel[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
double gdel[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
double bdel[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
// Gap between pulses (seconds)
double rgap[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
double ggap[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
double bgap[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
// Array of pulse widths desired (in milliseconds)
// You don't need to worry about pulse width for constant
// LEDs.
// First pulse
double rp1[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
double gp1[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
double bp1[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
// Second pulse
double rp2[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
double gp2[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
double bp2[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};

// This section is for block stimuli
unsigned long rblockDur[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long rblockTime[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long rblockOn[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long gblockDur[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long gblockTime[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long gblockOn[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long bblockDur[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long bblockTime[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};
unsigned long bblockOn[numPins] = {0,0,0,0,0,0,0,0,0,0,0,0};

// Last on info for light flashing

unsigned long rlastOn[numPins]={ 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0 };
unsigned long glastOn[numPins]={ 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0 };
unsigned long blastOn[numPins]={ 0, 0, 0, 0, 0, 0, 0, 0, 0,0,0,0 };

int hasStarted = 0;

String mode = "";
char charBuffer;
/* Load setup and initialize every pin as an output.
   Further, if the pin is set to be constant, initialize that.
*/   
void setup() {
  Serial.begin(9600);
  for(int k = 0; k < numPins; k++){
    pinMode(gpin[k],OUTPUT);
    analogWrite(gpin[k],offVal);
    pinMode(rpin[k],OUTPUT);
    digitalWrite(rpin[k],HIGH);
    pinMode(bpin[k],OUTPUT);
    digitalWrite(bpin[k],HIGH);
  }
  // give the mode information time to be written to the serial port
  delay(1000);
  while(Serial.available()){
    char buffer = Serial.read();
    mode.concat(buffer);
  }
  if(mode == "") {
    Serial.write("ERROR: No mode selected!");
    delay(100000);
  }
  hasStarted = 0;
  Serial.print("Arduino on");
  Serial.print(mode);
}

// Run this loop ad electrical nauseum
void loop() {
  unsigned long currentTime = millis();
  // Check to see if it's time to start or not and then turn on the light
  if((currentTime > startTime) && hasStarted == 0) {
    hasStarted = 1;
    Serial.write("Starting expt!");
  }
  // single well with consistent stim mode
  if(mode=="singleWells") {
    if(currentTime > startTime) {
      // Step through each pin, indexed from 0
      for(int pinScan = 0; pinScan < numPins; pinScan++){
        // See if this pin is supposed to be constantly on
        if(rfreq[pinScan]!=0) {
          // Now make sure the pin is supposed to be on (not yet implemented, was buggy)
            // If it has been the pulse width since the last time the LED
            // was turned on, turn it off.
            if((unsigned long)(currentTime-rlastOn[pinScan]) > rpulseWidth[pinScan]) {
              digitalWrite(rpin[pinScan], HIGH);
            }
             
            // If it's been timescale/frequency units of time since the LED
            // was last turned on, turn it on again. Then annotate 
            // having done so by updating the lastOn array.
            if((unsigned long)((currentTime - rlastOn[pinScan])*rfreq[pinScan])>=timescale){
              digitalWrite(rpin[pinScan], LOW);
              rlastOn[pinScan] = currentTime; 
            }
        }
        else {
          digitalWrite(rpin[pinScan],HIGH);
         }
         if(gfreq[pinScan]!=0) {
          // Now make sure the pin is supposed to be on (not yet implemented, was buggy)
            // If it has been the pulse width since the last time the LED
            // was turned on, turn it off.
            if((unsigned long)(currentTime-glastOn[pinScan]) > gpulseWidth[pinScan]) {
              analogWrite(gpin[pinScan], offVal);
            }
             
            // If it's been timescale/frequency units of time since the LED
            // was last turned on, turn it on again. Then annotate 
            // having done so by updating the lastOn array.
            if((unsigned long)((currentTime - glastOn[pinScan])*gfreq[pinScan])>=timescale){
              analogWrite(gpin[pinScan], onVal);
              glastOn[pinScan] = currentTime; 
            }
        }
        else {
          analogWrite(gpin[pinScan],offVal);
        }
        if(bfreq[pinScan]!=0) {
          // Now make sure the pin is supposed to be on (not yet implemented, was buggy)
            // If it has been the pulse width since the last time the LED
            // was turned on, turn it off.
            if((unsigned long)(currentTime-blastOn[pinScan]) > bpulseWidth[pinScan]) {
              digitalWrite(bpin[pinScan], HIGH);
            }
             
            // If it's been timescale/frequency units of time since the LED
            // was last turned on, turn it on again. Then annotate 
            // having done so by updating the lastOn array.
            if((unsigned long)((currentTime -blastOn[pinScan])*bfreq[pinScan])>=timescale){
              digitalWrite(bpin[pinScan], LOW);
              blastOn[pinScan] = currentTime; 
            }
        }
        else {
          digitalWrite(bpin[pinScan],HIGH);
         }
      }
    }
  }
  // paired pulse mode
  if(mode=="pairedPulse"){
    if(currentTime > startTime) {
    // Step through each pin, indexed from 0
      for(int pinScan = 0; pinScan < numPins; pinScan++){
        // See if time delay has passed since the signal was given
        if((currentTime - rpinInit[pinScan]) > rdel[pinScan]) {
            // Are we in the regime of p1? If so, turn on the light
            if((currentTime - rpinInit[pinScan] - rdel[pinScan]) < rp1[pinScan]) {
              digitalWrite(rpin[pinScan],LOW);
            }
            // Are we in the gap? If so, light off!
            else if((currentTime - rpinInit[pinScan] - rdel[pinScan]-rp1[pinScan]) < rgap[pinScan]) {
             digitalWrite(rpin[pinScan],HIGH); 
            }
            // Are we in p2? Turn it back on
            else if((currentTime - rpinInit[pinScan] - rdel[pinScan]-rp1[pinScan]-rgap[pinScan]) < rp2[pinScan]) {
             digitalWrite(rpin[pinScan],LOW); 
            }
            else {
              digitalWrite(rpin[pinScan],HIGH);
            }
        }
        else {
           digitalWrite(rpin[pinScan],HIGH);
        }
        if((currentTime - gpinInit[pinScan]) > gdel[pinScan]) {
            // Are we in the regime of p1? If so, turn on the light
            if((currentTime - gpinInit[pinScan] - gdel[pinScan]) < gp1[pinScan]) {
              analogWrite(gpin[pinScan],onVal);
            }
            // Are we in the gap? If so, light off!
            else if((currentTime - gpinInit[pinScan] - gdel[pinScan]-gp1[pinScan]) < ggap[pinScan]) {
             analogWrite(gpin[pinScan],offVal); 
            }
            // Are we in p2? Turn it back on
            else if((currentTime - gpinInit[pinScan] - gdel[pinScan]-gp1[pinScan]-ggap[pinScan]) < gp2[pinScan]) {
             analogWrite(gpin[pinScan],onVal); 
            }
            else {
              digitalWrite(gpin[pinScan],offVal);
            }
        }
        else {
           analogWrite(gpin[pinScan],offVal);
        }
        if((currentTime - bpinInit[pinScan]) > bdel[pinScan]) {
            // Are we in the regime of p1? If so, turn on the light
            if((currentTime - bpinInit[pinScan] - bdel[pinScan]) < bp1[pinScan]) {
              digitalWrite(bpin[pinScan],LOW);
            }
            // Are we in the gap? If so, light off!
            else if((currentTime - bpinInit[pinScan] - bdel[pinScan]-bp1[pinScan]) < bgap[pinScan]) {
             digitalWrite(bpin[pinScan],HIGH); 
            }
            // Are we in p2? Turn it back on
            else if((currentTime - bpinInit[pinScan] - bdel[pinScan]-bp1[pinScan]-bgap[pinScan]) < bp2[pinScan]) {
             digitalWrite(bpin[pinScan],LOW); 
            }
            else {
              digitalWrite(bpin[pinScan],HIGH);
            }
        }
        else {
           digitalWrite(bpin[pinScan],HIGH);
        }
      }
    }
  }
   // block stim mode
  if(mode=="blocks"){
    if(currentTime > startTime) {
      // Step through each pin, indexed from 0
      for(int pinScan = 0; pinScan < numPins; pinScan++){
        // See if this pin is supposed to be constantly off
        if(rfreq[pinScan]!=0) {
          /// RED
          // Now make sure the pin is supposed to be on
            // If it has been the pulse width since the last time the LED
            // was turned on, turn it off. Likewise, if it's been more than blockTime
            // since the block started, turn the light off (but only if blockDur isn't set to 0).
            if((currentTime-rlastOn[pinScan]) > rpulseWidth[pinScan] || ((((currentTime-startTime) - rblockOn[pinScan]) > rblockDur[pinScan]))) {
              digitalWrite(rpin[pinScan], HIGH);
            }
             
            // If it's been timescale/frequency units of time since the LED
            // was last turned on, turn it on again. Then annotate 
            // having done so by updating the lastOn array.
            if((unsigned long)((currentTime - rlastOn[pinScan])*rfreq[pinScan])>=timescale && ((currentTime-startTime) - rblockOn[pinScan]) < rblockDur[pinScan]){
              digitalWrite(rpin[pinScan], LOW);
              rlastOn[pinScan] = currentTime; 
            }
            // Update the blockOn
            if (((currentTime-startTime) - rblockOn[pinScan]) > rblockTime[pinScan]) {
              rblockOn[pinScan] = currentTime;
            }
        }
           //// GREEN
            if(gfreq[pinScan]!=0) {
          // Now make sure the pin is supposed to be on
            // If it has been the pulse width since the last time the LED
            // was turned on, turn it off. Likewise, if it's been more than blockTime
            // since the block started, turn the light off (but only if blockDur isn't set to 0).
            if((currentTime-glastOn[pinScan]) > gpulseWidth[pinScan] || ((((currentTime-startTime) - gblockOn[pinScan]) > gblockDur[pinScan]))) {
              analogWrite(gpin[pinScan], offVal);
            }
             
            // If it's been timescale/frequency units of time since the LED
            // was last turned on, turn it on again. Then annotate 
            // having done so by updating the lastOn array.
            if((unsigned long)((currentTime - glastOn[pinScan])*gfreq[pinScan])>=timescale && ((currentTime-startTime) - gblockOn[pinScan]) < gblockDur[pinScan]){
              analogWrite(gpin[pinScan], onVal);
              glastOn[pinScan] = currentTime; 
            }
            // Update the blockOn
            if (((currentTime-startTime) - gblockOn[pinScan]) > gblockTime[pinScan]) {
              gblockOn[pinScan] = currentTime;
            }
        }
        /// BLUE
          // Now make sure the pin is supposed to be on
            // If it has been the pulse width since the last time the LED
            // was turned on, turn it off. Likewise, if it's been more than blockTime
            // since the block started, turn the light off (but only if blockDur isn't set to 0).
         if(bfreq[pinScan]!=0) {
            if((currentTime-blastOn[pinScan]) > bpulseWidth[pinScan] || ((((currentTime-startTime) - bblockOn[pinScan]) > bblockDur[pinScan]))) {
              digitalWrite(bpin[pinScan], HIGH);
            }
             
            // If it's been timescale/frequency units of time since the LED
            // was last turned on, turn it on again. Then annotate 
            // having done so by updating the lastOn array.
            if((unsigned long)((currentTime - blastOn[pinScan])*bfreq[pinScan])>=timescale && ((currentTime-startTime) -bblockOn[pinScan]) < bblockDur[pinScan]){
              digitalWrite(bpin[pinScan], LOW);
              blastOn[pinScan] = currentTime; 
            }
            // Update the blockOn
            if (((currentTime-startTime) - bblockOn[pinScan]) > bblockTime[pinScan]) {
              bblockOn[pinScan] = currentTime;
            }
        }
      }
    } 
  }
}


// For when something gets written to serial port
void serialEvent() {
  /* Freeze wells in the off state
  for(int k = 0; k < numPins; k++){
    digitalWrite(pin[k],HIGH);
  }*/
  char look; 
  look = char(Serial.peek());
  if (look == 'i') {
    Serial.print("Looking");
    byte dummy = Serial.read();
    onVal = (1-Serial.parseFloat())*255;
  }
    else {
  if (mode == "singleWells") {
    int inWell = Serial.parseInt();
    float rinFreq = Serial.parseFloat();
    unsigned long rinPW = Serial.parseInt();
    float ginFreq = Serial.parseFloat();
    unsigned long ginPW = Serial.parseInt();
    float binFreq = Serial.parseFloat();
    unsigned long binPW = Serial.parseInt();
    // Translate the well request into which pin to modify
    int pinInd = wellMap[inWell];
    rfreq[pinInd] = rinFreq;
    rpulseWidth[pinInd] = rinPW;
    gfreq[pinInd] = ginFreq;
    gpulseWidth[pinInd] = ginPW;
    bfreq[pinInd] = binFreq;
    bpulseWidth[pinInd] = binPW;
  }
  if (mode=="pairedPulse") {
    unsigned long timeNow = millis();
    int inWell = Serial.parseInt();
    double rinDelay = Serial.parseFloat();
    float rinP1 = Serial.parseFloat();
    double rinGap = Serial.parseFloat();
    float rinP2 = Serial.parseFloat();
    double ginDelay = Serial.parseFloat();
    float ginP1 = Serial.parseFloat();
    double ginGap = Serial.parseFloat();
    float ginP2 = Serial.parseFloat();
    double binDelay = Serial.parseFloat();
    float binP1 = Serial.parseFloat();
    double binGap = Serial.parseFloat();
    float binP2 = Serial.parseFloat();
    // Translate the well request into which pin to modify
    int pinInd = wellMap[inWell];
    rpinInit[pinInd] = timeNow;
    rdel[pinInd] = rinDelay*60000;
    rgap[pinInd] = rinGap*1000;
    rp1[pinInd] = rinP1;
    rp2[pinInd] = rinP2;
    gpinInit[pinInd] = timeNow;
    gdel[pinInd] = ginDelay*60000;
    ggap[pinInd] = ginGap*1000;
    gp1[pinInd] = ginP1;
    gp2[pinInd] = ginP2;
    bpinInit[pinInd] = timeNow;
    bdel[pinInd] = binDelay*60000;
    bgap[pinInd] = binGap*1000;
    bp1[pinInd] = binP1;
    bp2[pinInd] = binP2;
  }
  if (mode == "blocks") {
        int inWell = Serial.parseInt();
        float rinFreq = Serial.parseFloat();
        unsigned long rinPW = Serial.parseInt();
        unsigned long rbd = Serial.parseInt();
        unsigned long rbr = Serial.parseInt();
        float ginFreq = Serial.parseFloat();
        unsigned long ginPW = Serial.parseInt();
        unsigned long gbd = Serial.parseInt();
        unsigned long gbr = Serial.parseInt();
        float binFreq = Serial.parseFloat();
        unsigned long binPW = Serial.parseInt();
        unsigned long bbd = Serial.parseInt();
        unsigned long bbr = Serial.parseInt();
        int pinInd = wellMap[inWell];
        rfreq[pinInd] = rinFreq;
        rpulseWidth[pinInd] = rinPW;
        rblockDur[pinInd] = rbd;
        rblockTime[pinInd] = rbr;
        gfreq[pinInd] = ginFreq;
        gpulseWidth[pinInd] = ginPW;
        gblockDur[pinInd] = gbd;
        gblockTime[pinInd] = gbr;
        bfreq[pinInd] = binFreq;
        bpulseWidth[pinInd] = binPW;
        bblockDur[pinInd] = bbd;
        bblockTime[pinInd] = bbr;
      }
  while(Serial.available()) {
    Serial.read();
  }
    }
}

