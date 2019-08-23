//Шапка детектор для Треккона 2019.
//Ринс. Москва 2019.
//v3.1 (В led_pin и функциях ледов поменяны местами HIGH и LOW, из за снятия транзистора)


//ИК библиотека
#include <IRremote.h>

//Отладка (0 выкл, 1 вкл)
#define port 0

//Назначаем порты
#define led_pin 11
#define vib_pin 12
#define buz_pin 10
#define ik_pin  2
#define in_pin  6

//Список сигналов с пульта Car mp3
#define CHm  0xFFA25D
#define CH   0xFF629D
#define CHp  0xFFE21D
#define PREV 0xFF22DD
#define NEXT 0xFF02FD
#define PLPA 0xFFC23D
#define VOLm 0xFFE01F
#define VOLp 0xFFA857
#define EQ   0xFF906F
#define C0   0xFF6897
#define C100 0xFF9867
#define C200 0xFFB04F
#define C1   0xFF30CF
#define C2   0xFF18E7
#define C3   0xFF7A85
#define C4   0xFF10EF
#define C5   0xFF38C7
#define C6   0xFF5AA5
#define C7   0xFF42BD
#define C8   0xFF4AB5
#define C9   0xFF52AD
//Список сигналов с пульта Ballu
#define TIMR 0x40BF10EF
#define TEMP 0x40BF50AF
#define ONOF 0x40BF08F7
#define MODE 0x40BF48B7

//Куски ИК библиотеки. Порт и переменная для записи
IRrecv irrecv(ik_pin);
decode_results results;

//Статус игрока 0 жив, 1 парализован, 2 реген, 3 ранен, 4 убит.
volatile byte stat;

//Режимы (0 - паралич от любого сигнала кроме назначеннЫХ(!), 1 - паралич от назначеннОГО(!) сигнала)
boolean allSignals;

//Таймеры
volatile unsigned long tm;
unsigned long tmBlink;
unsigned long tmChek;

//Флажки вкл\выкл
boolean led;
boolean vib;
//Флажок частот низ\выс
boolean buz;

void setup() {
  //Отладка
  if (port){Serial.begin(9600);}
  
  //Инициализация прослушку ИК
  irrecv.enableIRIn();
  
  //Инициализация пинов
  pinMode (led_pin, OUTPUT);
  pinMode (vib_pin, OUTPUT);
  pinMode (buz_pin, OUTPUT);
  pinMode (in_pin,  INPUT_PULLUP);
  
  //Задаем начальное положение пинов (HIGH из за того что стоит прямой (pnp) транзистор, 0 на базе его включает, 1 выключает) 
  digitalWrite (led_pin, LOW);  //v3.1
  digitalWrite (vib_pin, HIGH);
  digitalWrite (buz_pin, LOW);

  //Установка режима приема сигналов (все сигналы || конкретный сигнал)
  if (!digitalRead(in_pin)){
    allSignals = 1;
    digitalWrite (13, HIGH);
  }
  
  //Проверка ледов, вибро и бузера
  for (byte i=0;i<=2;i++){
    led_on();
    vib_on();
    buz_h();
    delay (250);
    led_off();
    vib_off();
    buz_l();
    delay(250);
  }
  buz_off();
}

void loop() {
  //Прерывания на пине ИК (0 = пин d2)
  attachInterrupt (0, signals, RISING);
  
  //Обработка флажков
  if (led == 1){led_on();} else {led_off();}
  if (vib == 1){vib_on();} else {vib_off();}
  
  //Игрок жив
  if (stat == 0){
    led = 0;
    vib = 0;
    buz_off();
  }
  //Игрок парализовн
  if (stat == 1){
    led = 1;
    vib = 1;
    if (millis()-tm > 10000){
      stat = 2;
    }
  }
  //Игрок востанавливается
  if (stat == 2){
    if (millis() - tmBlink > 500){
      led = !led;
      tmBlink = millis();
    }
    if (millis() - tm > 15000){
      stat = 0;
    }
  }
  //Игрок ранен
  if (stat == 3){
    if (millis()- tmBlink >= 100){
      led = !led;    
      tmBlink = millis();
    }
    if(millis() - tm >= 10000){
      stat = 1;
      tm = millis()- 5000;
    }
  }
  //Игрок мертв
  if (stat == 4){
    vib_off();
    while (true){
      //ОСТОРОЖНО! КОСТЫЛЬ! Вообще это должно быть функцией.
      if (led == 1){led_on();} else {led_off();}
      if (buz == 1){buz_h();} else {buz_l();}
      //Мигаем и писчим
      if(millis() - tmBlink > 500){
        led = !led;
        buz = !buz;
        tmBlink = millis();
      }
      //ОСТОРОЖНО! КОСТЫЛЬ! Прерывани не срабатывают, так что пишем кусок функции signals() прямо сюда. Возраждаем по сигналу.
      if (irrecv.decode(&results)){
        if (results.value == C6){
          stat = 0;
          tm = millis();
          break;
        }
        irrecv.resume();
      }
      //Отладка в цикле
      chek();
    }
  }
  //Отладка вообще
  chek();
}
//Функция управления с пульта
void signals(){
  if (irrecv.decode(&results)){
    switch (results.value){
      case TIMR:
      case CHm:
      case CH:    
      case CHp:   if(!allSignals && stat == 0 && millis() - tm > 100){stat = 1; tm = millis();} break;
      case TEMP:
      case PREV:
      case NEXT:
      case PLPA:  if(stat == 1 && millis() - tm > 100 || stat == 2 && millis() - tm > 100){stat = 3; tm = millis();} break;
      case ONOF:
      case VOLm:
      case VOLp:
      case EQ:    if(stat == 3 && millis() - tm > 100){stat = 4; tm = millis();}break;
      case C6:    stat = 0; tm = millis();break;
      default:    if(allSignals && stat == 0 && millis() - tm > 100){stat = 1; tm = millis();} break;
    }
    irrecv.resume();
  }
}

//Функции управления ледами, вибро и бузером
void led_on()   {digitalWrite (led_pin, HIGH);  led = 1;} //v3.1
void led_off()  {digitalWrite (led_pin, LOW);   led = 0;} //v3.1
void vib_on()   {digitalWrite (vib_pin, LOW);   vib = 1;}
void vib_off()  {digitalWrite (vib_pin, HIGH);  vib = 0;}
void buz_h()    {analogWrite  (buz_pin, 50);    buz = 1;}
void buz_l()    {analogWrite  (buz_pin, 20);    buz = 0;}
void buz_off()  {analogWrite  (buz_pin, 0);}

//Функция отладки
void chek() {
  if (port) {
    if (millis()- tmChek > 100){
      tmChek = millis();
      Serial.print("Status: ");
      Serial.print(stat);
      Serial.print(" | led: ");
      if (led == 0){Serial.print("OFF");}else{Serial.print("ON");}
      Serial.print(" | ledReal: ");
      if (digitalRead (led_pin) == 0){Serial.print("OFF");}else{Serial.print("ON");}  //v3.1
      Serial.print(" | vib: ");
      if (vib == 0){Serial.print("OFF");}else{Serial.print("ON");}
      Serial.print(" | vibReal: ");
      if (digitalRead (vib_pin) == 1){Serial.print("OFF");}else{Serial.print("ON");}
      Serial.print(" | buz: ");
      if (digitalRead (buz_pin) == 0){Serial.print("OFF");}else{Serial.print("ON");}
      Serial.println(" ");
    }
  }
}
