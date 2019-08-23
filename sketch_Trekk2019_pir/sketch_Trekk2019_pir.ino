//Детектор движения для Треккона 2019.
//Ринс. Москва. 2019.


#define pir_pin       2
#define but_pin       3
#define ledR_pin      4
#define ledG_pin      5
#define buz_pin       6

#define timer         30

volatile boolean but_stat;
volatile boolean pir_stat;
unsigned long tm;

void setup() {

  pinMode (ledR_pin, OUTPUT);
  pinMode (ledG_pin, OUTPUT);
  pinMode (buz_pin, OUTPUT);
  pinMode (but_pin, INPUT_PULLUP);
  pinMode (pir_pin, INPUT_PULLUP);
  
  while (true){
    
    digitalWrite (ledG_pin, HIGH);
    
    tone (buz_pin, 1500);
    
    if (digitalRead (pir_pin)){digitalWrite (ledR_pin, HIGH);}
    else {digitalWrite (ledR_pin, LOW);}

    if (!digitalRead (but_pin)){
      noTone(buz_pin);
      digitalWrite (ledG_pin, LOW);
      digitalWrite (ledR_pin, LOW);
      break;
    }
  }
}

void loop() {
  
  
  if (!digitalRead (but_pin)){
    digitalWrite (ledG_pin, HIGH);
    digitalWrite (ledR_pin, LOW);
    noTone(buz_pin);
    delay (timer*1000);
  }
  else{
    digitalWrite (ledG_pin, LOW);
    digitalWrite (ledR_pin, HIGH);
  }

  if (digitalRead (pir_pin)){
    tone (buz_pin, 1500); 
  }
  else{
    noTone(buz_pin);
  }
}
