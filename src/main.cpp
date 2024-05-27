#include <Arduino.h>
#include <Fuzzy.h>
#include <MQUnifiedsensor.h>
#include <DHT.h>


#define MQPIN A0
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define enA 11
#define in1 10
#define in2 9

#define enB 6
#define in3 5
#define in4 4

boolean inital = true;
int output = 0;
Fuzzy *fuzzy = new Fuzzy();

//Fuzzy Input Suhu
FuzzySet *dingin= new FuzzySet(0, 0, 20, 33);
FuzzySet *hangat = new FuzzySet(20, 33, 48, 55);
FuzzySet *panas = new FuzzySet(48, 55, 100, 100);

//Fuzzy Input Asap
FuzzySet *rendah = new FuzzySet(0, 0, 200, 700);
FuzzySet *sedang = new FuzzySet(200, 700, 4500, 5500);
FuzzySet *tinggi = new FuzzySet(4500, 5500, 10000, 10000);

//Fuzzy Output PWN DC Motor 1 Kipas
FuzzySet *lambat1 = new FuzzySet(0, 0, 70, 100);
FuzzySet *sedang1 = new FuzzySet(70, 100, 140, 170);
FuzzySet *cepat1 = new FuzzySet(140, 170, 255, 255);

//Fuzzy Output PWN DC Motor 2 Pompa Air
FuzzySet *mati2 = new FuzzySet(0, 0, 2.5, 5);
FuzzySet *sedang2 = new FuzzySet(2.5, 7.5, 140, 170);
FuzzySet *cepat2 = new FuzzySet(140, 170, 255, 255);

void setup(){
  dht.begin();
  Serial.begin(9600);
  randomSeed(analogRead(0));
 
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  FuzzyInput *suhu = new FuzzyInput(1);

  suhu->addFuzzySet(dingin);
  suhu->addFuzzySet(hangat);
  suhu->addFuzzySet(panas);
  fuzzy->addFuzzyInput(suhu);

  FuzzyInput *asap = new FuzzyInput(2);

  asap->addFuzzySet(rendah);
  asap->addFuzzySet(sedang);
  asap->addFuzzySet(tinggi);
  fuzzy->addFuzzyInput(asap);

  FuzzyOutput *fanSpeed = new FuzzyOutput(1);

  fanSpeed->addFuzzySet(lambat1);
  fanSpeed->addFuzzySet(sedang1);
  fanSpeed->addFuzzySet(cepat1);
  fuzzy->addFuzzyOutput(fanSpeed);
  
  FuzzyOutput *pam = new FuzzyOutput(2);

  pam->addFuzzySet(mati2);
  pam->addFuzzySet(sedang2);
  pam->addFuzzySet(cepat2);
  fuzzy->addFuzzyOutput(pam);


  // Fuzzy Rule 1
  FuzzyRuleAntecedent *dingin_rendah = new FuzzyRuleAntecedent();
  dingin_rendah->joinWithAND(dingin, rendah);

  FuzzyRuleConsequent *aturan1 = new FuzzyRuleConsequent;
  aturan1->addOutput(lambat1);
  aturan1->addOutput(mati2);

  FuzzyRule *rule1 = new FuzzyRule(1, dingin_rendah, aturan1);
  fuzzy->addFuzzyRule(rule1);


  //fuzzy Rule 2
  FuzzyRuleAntecedent *dingin_sedang = new FuzzyRuleAntecedent();
  dingin_sedang->joinWithAND(dingin, sedang);

  FuzzyRuleConsequent *aturan2 = new FuzzyRuleConsequent;
  aturan2->addOutput(sedang1);
  aturan2->addOutput(mati2);

  FuzzyRule *rule2 = new FuzzyRule(2, dingin_sedang, aturan2);
  fuzzy->addFuzzyRule(rule2);

  //fuzzy Rule 3
  FuzzyRuleAntecedent *dingin_tinggi = new FuzzyRuleAntecedent();
  dingin_tinggi->joinWithAND(dingin, tinggi);

  FuzzyRuleConsequent *aturan3 = new FuzzyRuleConsequent;
  aturan3->addOutput(cepat1);
  aturan3->addOutput(mati2);

  FuzzyRule *rule3 = new FuzzyRule(3, dingin_tinggi, aturan3);
  fuzzy->addFuzzyRule(rule3);

  //fuzzy Rule 4
  FuzzyRuleAntecedent *hangat_rendah = new FuzzyRuleAntecedent();
  hangat_rendah->joinWithAND(hangat, rendah);

  FuzzyRuleConsequent *aturan4 = new FuzzyRuleConsequent;
  aturan4->addOutput(lambat1);
  aturan4->addOutput(sedang2);

  FuzzyRule *rule4 = new FuzzyRule(4, hangat_rendah, aturan4);
  fuzzy->addFuzzyRule(rule4);

  //fuzzy Rule 5
  FuzzyRuleAntecedent *hangat_sedang = new FuzzyRuleAntecedent();
  hangat_sedang->joinWithAND(hangat, sedang);

  FuzzyRuleConsequent *aturan5 = new FuzzyRuleConsequent;
  aturan5->addOutput(sedang1);
  aturan5->addOutput(sedang2);

  FuzzyRule *rule5 = new FuzzyRule(5, hangat_sedang, aturan5);
  fuzzy->addFuzzyRule(rule5);

  //fuzzy Rule 6
  FuzzyRuleAntecedent *hangat_tinggi = new FuzzyRuleAntecedent();
  hangat_tinggi->joinWithAND(hangat, tinggi);

  FuzzyRuleConsequent *aturan6 = new FuzzyRuleConsequent;
  aturan6->addOutput(cepat1);
  aturan6->addOutput(sedang2);

  FuzzyRule *rule6 = new FuzzyRule(6, hangat_tinggi, aturan6);
  fuzzy->addFuzzyRule(rule6);

  //Fuzzy Rule 7
  FuzzyRuleAntecedent *panas_rendah = new FuzzyRuleAntecedent();
  panas_rendah->joinWithAND(panas, rendah);

  FuzzyRuleConsequent *aturan7 = new FuzzyRuleConsequent;
  aturan7->addOutput(lambat1);
  aturan7->addOutput(cepat2);

  FuzzyRule *rule7 = new FuzzyRule(7, panas_rendah, aturan7);
  fuzzy->addFuzzyRule(rule7);

  //Fuzzy Rule 8
  FuzzyRuleAntecedent *panas_sedang = new FuzzyRuleAntecedent();
  panas_sedang->joinWithAND(panas, sedang);

  FuzzyRuleConsequent *aturan8 = new FuzzyRuleConsequent;
  aturan8->addOutput(sedang1);
  aturan8->addOutput(cepat2);

  FuzzyRule *rule8 = new FuzzyRule(8, panas_sedang, aturan8);
  fuzzy->addFuzzyRule(rule8);

  //Fuzzy Rule 9
  FuzzyRuleAntecedent *panas_tinggi = new FuzzyRuleAntecedent();
  panas_tinggi->joinWithAND(panas, tinggi);

  FuzzyRuleConsequent *aturan9 = new FuzzyRuleConsequent;
  aturan9->addOutput(cepat1);
  aturan9->addOutput(cepat2);

  FuzzyRule *rule9 = new FuzzyRule(9, panas_tinggi, aturan9);
  fuzzy->addFuzzyRule(rule9);

}

void loop() {
  // int input1 = random(0, 100);
  // int input2 = random(0, 10000);

  // int input1 = 80;
  // int input2 = 4555;
  
  
  int input1 = dht.readTemperature();
  int input2 = analogRead(MQPIN);
  input2 = map(input2, 0, 900, 0, 10000);
  

  Serial.print("Suhu : ");
  Serial.println(input1);

  Serial.print("Asap : ");
  Serial.println(input2);

  fuzzy->setInput(1, input1);
  fuzzy->setInput(2, input2);

  fuzzy->fuzzify();

  // Fuzzy output for the fan (enA)
  int fanOutput = fuzzy->defuzzify(1);

  // Fuzzy output for the water pump (enB)
  int pumpOutput = fuzzy->defuzzify(2);

  Serial.print("Kecepatan PWM Fan : ");
  Serial.println(fanOutput);
  Serial.print("Kecepatan PWM Pump : ");
  Serial.println(pumpOutput);
  Serial.println(" ");

  // Set the fan speed based on the fuzzy output
  analogWrite(enA, fanOutput);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  // Set the water pump speed based on the fuzzy output
  analogWrite(enB, pumpOutput);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  

  delay(5000); 
}


