/*
  communication mirroir grace au protocole LoRa par Paul Albouy

  Programme Maitre

*/
//librairies utilisées:
//#include <analogShield.h>
#include <SPI.h>
#include <RH_RF95.h>
RH_RF95 rf95;

char string[10];
char string1[10];
int ValeurLuDigital = 0;
float ForceFiltre = 0.992;
float ValeurSortieFiltre = 0;
int test = 6;
int boucle = 2;
String PointeurString = "";
uint8_t StringEmisible[20];


void setup() {
  // initialisation de la modulation LoRa (868MHz, 500KHz, 4/5, SF7)
  Serial.begin(115200);
  if (!rf95.init())
    Serial.println("l'initialisation a échoué");
  rf95.setTxPower(20);
  Serial.println("Starting at 20 dBm");
  Serial.println("");
  rf95.setModemConfig(RH_RF95::Bw125Cr48Sf2048);
  Serial.println("lancement du test :  BW 125   CR 4/8   SF 11    ***");

}

void Rst() // Restarts program from beginning but does not reset the peripherals and registers
{
  Serial.end();
  asm volatile ("  jmp 0");
}

void loop() {
  int i = 0;
  int pin[] = {3,4,5};
  Serial.print("Information envoyée : ");
  for (i = 2; i >= 0; i--) {
    pinMode(pin[i],INPUT);
    bitWrite(ValeurLuDigital, i, digitalRead(pin[i]));
  }
  ValeurSortieFiltre = (1-ForceFiltre)*(analogRead(A0))+ForceFiltre*(ValeurSortieFiltre);
  //transformation de la valeur float en valeur String pour envoi:
  PointeurString += dtostrf(ValeurLuDigital, 2, 0, string1);
  PointeurString += dtostrf(round(ValeurSortieFiltre), 6, 0, string);
  strcat(string,string1);
  strcpy((char *)StringEmisible, string);
  Serial.println(string);
  rf95.send(StringEmisible, sizeof(StringEmisible));
  rf95.waitPacketSent();
  uint8_t dataIn[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(dataIn);
  if (rf95.waitAvailableTimeout(3000))
  {
    if (rf95.recv(dataIn, &len))
    {
      Serial.println("");
      Serial.print("puissance du siganl reçu par l'esclave : ");
      Serial.println((char*)dataIn);
      Serial.println("");
      Serial.print("puissance du signal reçu par le maitre : ");
      Serial.println(rf95.spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE) - 30);
      Serial.println("");
      boucle = 0;
    }
    else
    {
      Serial.println("la réception a raté");
    }
  }
  else
  {
    Serial.println("Le module oposé ne répond pas");
  }
  boucle += 1;
  if (boucle > 4) {
    test = test + 1;
    boucle = 1;
  }
  switch (test) {
    case 0:
      Serial.println("lancement du test :  BW 125   CR 4/8   SF 11    ***");
      break;
    case 1:
      rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
      Serial.println("lancement du test :  BW 500   CR 4/5   SF 7");
      break;
    case 2:
      rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
      Serial.println("lancement du test :  BW 125   CR 4/5   SF 7    ***");
      break;
    case 3:
      rf95.setModemConfig(RH_RF95::Bw125Cr48Sf256);
      Serial.println("lancement du test :  BW 125   CR 4/8   SF 8");
      break;
    case 4:
      rf95.setModemConfig(RH_RF95::Bw125Cr48Sf512);
      Serial.println("lancement du test :  BW 125   CR 4/8   SF 9    ***");
      break;
    case 5:
      rf95.setModemConfig(RH_RF95::Bw125Cr48Sf1024);
      Serial.println("lancement du test :  BW 125   CR 4/8   SF 10");
      break;
    case 6:
      rf95.setModemConfig(RH_RF95::Bw125Cr48Sf2048);
      Serial.println("lancement du test :  BW 125   CR 4/8   SF 11    ***");
      test = 0;
      break;
    case 7:
      rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
      Serial.println("La communication est en limite de portée ...");
      Serial.println("");
      Serial.println("");
      Serial.println("lancement du test :  BW 125   CR 4/8   SF 12");
      break;
    case 8:
      Serial.println("reboot");
      Rst();
      break;
  }
}
