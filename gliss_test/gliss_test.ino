const int positionPin = 33;
const int channel = 1;
const int noteMin = 72;
const int noteMax = 84;

const int pitchBendMin = 6144;
const int pitchBendDefault = 8192;
const int pitchBendMax = 10240;
const int positionMax = 1023;
const int positionMin = 0;
const int notePositions[15] = {0, 73, 146, 219, 292, 365, 438, 512, 585, 658, 731, 804, 877, 950, 1023};
const float positionStep = 1023/28.0;
//[73, 146, 219, 292, 365, 438, 512, 585, 658, 731, 804, 877, 950, 1023]
//[37, 110, 183, 256, 329, 402, 475, 548, 621, 694, 767, 840, 913, 986]

bool bendUp = false;
int rawPosition = 0;
int bend = pitchBendDefault;
int prevNote = 0;
int note = 0;

void setup() {
  Serial.begin(9600);
//  usbMIDI.sendNoteOff(75, 127, channel);
//  usbMIDI.sendNoteOn(45, 127, channel);
}

void loop() {
  rawPosition = analogRead(positionPin);

  note = map(rawPosition, positionMin, positionMax, noteMin, noteMax);
  if (note != prevNote) {
    usbMIDI.sendNoteOff(prevNote, 0, channel);
    usbMIDI.sendNoteOn(note, 127, channel);
  }
  int ceilThing = round(ceil(rawPosition/positionStep)*positionStep);
  int floorThing = round(floor(rawPosition/positionStep)*positionStep);
  Serial.print("Raw Position: "); Serial.println(rawPosition);
  Serial.print("Ceil: "); Serial.println(ceilThing);
  Serial.print("Bend Dirn: "); Serial.println(isBendDown(ceilThing));
  // up by num - [ceil(num/40)*40 - 40]
  // down by [floor(num/40)*40 + 40] - num
  int mid = 0;
  if (isBendDown(ceilThing)) {
    mid = ceilThing;
  } else {
    mid = floorThing;
  }
  bend = map(rawPosition, round(mid - positionStep), round(mid + positionStep), pitchBendMin, pitchBendMax);
  usbMIDI.sendPitchBend(bend, channel);
  
//  int mappedPosition = map(rawPosition, 0, 1023, 4096, 12288);
//  Serial.println(rawPosition);
//  usbMIDI.sendPitchBend(mappedPosition, channel);
//  usbMIDI.sendNoteOff(75, 0, channel);
//  usbMIDI.sendNoteOff(76, 0, channel);
//  if (rawPosition < 400) {
//    Serial.println("Slurp");
//    usbMIDI.sendNoteOn(75, 127, channel);
//    usbMIDI.sendPitchBend(8192, channel);
//  } else if (rawPosition < 800) {
//    Serial.println("Gooch");
//    usbMIDI.sendNoteOn(75, 127, channel);
//    usbMIDI.sendPitchBend(9557, channel);
//  } else {
//    Serial.println("Social Capital");
//    usbMIDI.sendNoteOn(76, 127, channel);
//    usbMIDI.sendPitchBend(8192, channel);
//  }
  delay(100);
}

bool isBendDown(int pos) {
  for (int i = 0; i < 15; i++) {
    if (notePositions[i] == pos) {
      return true;
    }
  }
  return false;
}

