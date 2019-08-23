//Писчалка для Треккона 2019.
//Ринс. Москва 2019.

#define buz_pin 10
#define pot_pin A2

int tn;

void setup() {
  
  pinMode (buz_pin, OUTPUT);
  pinMode (pot_pin, INPUT);

  digitalWrite (buz_pin, LOW);

}

void loop() {

  tn = analogRead (buz_pin);
  tone (buz_pin, tn*3);
}
