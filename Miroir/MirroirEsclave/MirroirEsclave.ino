/*
  test de portée pour différentes modulations LoRa par Paul Albouy

  Programme Esclave

*/
//librairies utilisées:
//#include <analogShield.h>
#include <SPI.h>
#include <RH_RF95.h>
RH_RF95 rf95;

//définition des variables :

char string1[40];
char string[40];
float Valeurremise;
double Valeurrecu;
String PointeurString = "";
uint8_t StringEmisible[20];
int test = 1;
double boucle = -60000;
int nombremanque = 1;
int premiertest = 1;
void setup() {
  // initialisation de la modulation LoRa (868MHz, 500KHz, 4/5, SF7)
  Serial.begin(115200);
  if (!rf95.init())
    Serial.println("l'initialisation a échoué");
  rf95.setModemConfig(RH_RF95::Bw125Cr48Sf2048);
  Serial.println("lancement du test :  BW 125   CR 4/8   SF 11");
//  rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
//  Serial.println("lancement du test :  BW 125   CR 4/8   SF 12");
  Serial.println("");
  Serial.println("synchronisation en cours");
  Serial.println("si la synchronisation prend plus de deux minutes, le module opposé est trop éloigné ou éteint.");
  rf95.setTxPower(20);
}

void Rst() // Restarts program from beginning but does not reset the peripherals and registers
{
  asm volatile ("  jmp 0");
}

void loop() {
  if (boucle > 12000)
  {
    boucle = 0;
    test += 1;
    switch (test) {
      case 1:
        rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
        Serial.println("lancement du test :  BW 500   CR 4/5   SF 7");
        break;
      case 2:
        rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
        Serial.println("lancement du test :  BW 125   CR 4/5   SF 7");
        break;
      case 3:
        rf95.setModemConfig(RH_RF95::Bw125Cr48Sf256);
        Serial.println("lancement du test :  BW 125   CR 4/8   SF 8");
        break;
      case 4:
        rf95.setModemConfig(RH_RF95::Bw125Cr48Sf512);
        Serial.println("lancement du test :  BW 125   CR 4/8   SF 9");
        break;
      case 5:
        rf95.setModemConfig(RH_RF95::Bw125Cr48Sf1024);
        Serial.println("lancement du test :  BW 125   CR 4/8   SF 10");
        break;
      case 6:
        rf95.setModemConfig(RH_RF95::Bw125Cr48Sf2048);
        Serial.println("lancement du test :  BW 125   CR 4/8   SF 11");
        break;
      case 7:
        Serial.println("reboot");
        delay(1);
        Rst();
        rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
        Serial.println("lancement du test :  BW 125   CR 4/8   SF 12");
        break;
      default:
        Serial.println("reboot");
        delay(1);
        Rst();

    }

  }
  delay(1);
  boucle = boucle + 1;
  if (rf95.available())
  {
    boucle = 0;
    uint8_t dataIn[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(dataIn);
    if (rf95.recv(dataIn, &len))
    {
      Serial.print("message reçu : ");
      Serial.println((char*)dataIn);
      strcpy(string,(char*) dataIn);
      string1[0]=string[6];
      string1[1]=string[7];
      strncpy(string,string,5);
      int Valeurrecu = strtod(string1, 0);
      int ana = strtod(string, 0);
      Serial.println(Valeurrecu);
      int i=0;
      int pin[]={3,4};
      for (i = 1;i>=0;i--){
        byte B = bitRead(Valeurrecu,i);
        pinMode(pin[i],OUTPUT);
        digitalWrite(pin[i],B);
        Serial.print(pin[i]);
        Serial.print(" : ");
        Serial.println(B);
        Serial.println(" ");
      }
      pinMode(5,OUTPUT);
      analogWrite(5,ana/4);
      Serial.print("envoi de la réponse : ");
      Valeurremise = rf95.spiRead(RH_RF95_REG_1A_PKT_RSSI_VALUE) - 30;
      //Valeurremise = Valeurrecu;
      //transformation de la valeur float en valeur String pour envoi:
      PointeurString += dtostrf(Valeurremise, 1, 0, string);
      strcpy((char *)StringEmisible, string);
      Serial.println(string);
      Serial.println("");
      rf95.send(StringEmisible, sizeof(StringEmisible));
      rf95.waitPacketSent();
      boucle = 0;
      if (premiertest == 1){
        premiertest = 0;
        test = 0;
        boucle = 12001;
        delay(12000);
      }
      switch (test) {
        case 1:
          rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
          Serial.println("lancement du test :  BW 500   CR 4/5   SF 7");
          break;
        case 2:
          rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
          Serial.println("lancement du test :  BW 125   CR 4/5   SF 7");
          break;
        case 3:
          rf95.setModemConfig(RH_RF95::Bw125Cr48Sf256);
          Serial.println("lancement du test :  BW 125   CR 4/8   SF 8");
          break;
        case 4:
          rf95.setModemConfig(RH_RF95::Bw125Cr48Sf512);
          Serial.println("lancement du test :  BW 125   CR 4/8   SF 9");
          break;
        case 5:
          rf95.setModemConfig(RH_RF95::Bw125Cr48Sf1024);
          Serial.println("lancement du test :  BW 125   CR 4/8   SF 10");
          break;
        case 6:
          rf95.setModemConfig(RH_RF95::Bw125Cr48Sf2048);
          Serial.println("lancement du test :  BW 125   CR 4/8   SF 11");
          test = 0;
          break;
        case 7:
          rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);
          Serial.println("lancement du test :  BW 125   CR 4/8   SF 12");
          break;
        case 0:
          Serial.println("synchronisation réussi, recherche de la meilleur transmission");
          break;
        default:
          Serial.println("reboot");
          delay(1);
          Rst();
      }
    }
    else
    {
      Serial.println("la réception a raté");
      Serial.println("reboot");
      delay(1);
      Rst();
    }
  }
}
