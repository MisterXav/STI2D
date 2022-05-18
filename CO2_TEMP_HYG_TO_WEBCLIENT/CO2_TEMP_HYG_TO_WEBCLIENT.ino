/* Programme réalisé par le groupe de Xavier Valck Breton, Thomas Badana et Isaié Donies
 * Le but est de récupérer la température, l'humidité et le C02 présent dans la salle, et de l'afficher sur un écran LCD
 * Le programme fonctionne le 24/02/2022
 * Le but final est d'envoyer les données sur une page web afin qu'elles y soient stockés
 */ 


// ///////////////////////// SHIELD ETHERNET /////////////////////////////

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0xBA, 0xCF };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(195,221,155,10);  // numeric IP for Google (no DNS)
//char server[] = "nargeot.alwaysdata.net";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(0, 0, 0, 0);
IPAddress myDns(0, 0, 0, 0);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement


// ///////////////////////// CAPTEUR /////////////////////////////
#include <SoftwareSerial.h>
 SoftwareSerial s_serial(2, 3);      // TX, RX
 
#define sensor s_serial
 
const unsigned char cmd_get_sensor[] =
{
    0xff, 0x01, 0x86, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x79
};
 
unsigned char dataRevice[9];
int CO2PPM; 

// ///////////////////////// DHT /////////////////////////////
#include "DHT.h"
#define DHTPIN 4 // broche ou l'on a branché le capteur
#define DHTTYPE DHT22 // préciser le modéle de notre capteur
DHT dht(DHTPIN, DHTTYPE);// déclaration du capteur

// ///////////////////////// LCD /////////////////////////////
#include <Wire.h>
#include "rgb_lcd.h"
rgb_lcd lcd;

void setup()
{
  // initialise le nombre de caractères et de lignes du LCD
  lcd.begin(16, 2);
  dht.begin();
  sensor.begin(9600);
  Serial.begin(115200);
  Serial.println("Programme : CO2, temp et humidite affichee sur LCD cree le 06/01/2022 par Xavier VB et Thomas B");
  Serial.println("********************************************************");
  Serial.println();

  // ///////////////////////// SHIELD ETHERNET /////////////////////////////

  Ethernet.init(10);  // Initialisation Shield

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialisation Ethernet avec DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Impossible de configurer l'Ethernet avec DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Shied Ethernet non trouvé. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Cable Ethernet non connecté.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connection à ");
  Serial.print(server);
  Serial.println("...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.print("connecté à ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println("GET /capteur_htco/memo.php?temp=VARIABLETEMP&hum=VARIABLEHUM&co=VARIABLECO2&envoyer=Valider HTTP/1.1"); //Trouver un moyen d'envoyer ça en insérant les variables affichées sur le LCD
    client.println("Host: 195.221.155.10"); //mettre l'addresse IP de UWAMP
    client.println("Connexion: fermé");
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connexion échoué");
  }
  beginMicros = micros(); 
}
void loop()
{
  Mesurer();
  temp();
  screen();
  internet();
}

void Mesurer(){
   if(dataRecieve())
    {
        Serial.print("  CO2: ");
        Serial.print(CO2PPM);
        Serial.println("");
    }
    delay(1000);
}

bool dataRecieve(void)
{
    byte data[9];
    int i = 0;
 
    //transmit command data
    for(i=0; i<sizeof(cmd_get_sensor); i++)
    {
        sensor.write(cmd_get_sensor[i]);
    }
    delay(10);
    //begin reveiceing data
    if(sensor.available())
    {
        while(sensor.available())
        {
            for(int i=0;i<9; i++)
            {
                data[i] = sensor.read();
            }
        }
    }
    CO2PPM = (int)data[2] * 256 + (int)data[3];
 
    return true;
}

void temp(){
  delay(2000);

  // La lecture du capteur prend 250ms
  // Les valeurs lues peuvent être vieilles de jusqu'à 2 secondes (le capteur est lent)
  float h = dht.readHumidity(); // Lecture du taux d'humidité
  float t = dht.readTemperature(); // Lecture de la température en degrés Celsius
}


void screen(){
  float h = dht.readHumidity(); // Lecture du taux d'humidité
  float t = dht.readTemperature(); // Lecture de la température en degrés Celsius
  //Affichages :
  // HUMIDITE
  lcd.setCursor(0, 0); // positionne le curseur à la colonne 1 et à la ligne 1
  lcd.print("H: ");
  lcd.setCursor(3, 0);
  lcd.print(h);
  lcd.setCursor(8, 0);
  lcd.print("%");
  // TEMPERATURE
  lcd.setCursor(11, 0); // positionne le curseur à la colonne 1 et à la ligne 2
  lcd.print("T: ");
  lcd.setCursor(0, 1);
  lcd.print(t);
  lcd.setCursor(5, 1);
  lcd.print("C");
  // CO2
  lcd.setCursor(7, 1);
  lcd.print("C:");
  lcd.setCursor(9, 1);
  lcd.print(CO2PPM);
}

void internet(){
    // if there are incoming bytes available
  // from the server, read them and print them:
  int len = client.available();
  if (len > 0) {
    byte buffer[80];
    if (len > 80) len = 80;
    client.read(buffer, len);
    if (printWebData) {
      Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    endMicros = micros();
    Serial.println();
    Serial.println("déconnection.");
    client.stop();
    Serial.print("Received ");
    Serial.print(byteCount);
    Serial.print(" bytes in ");
    float seconds = (float)(endMicros - beginMicros) / 1000000.0;
    Serial.print(seconds, 4);
    float rate = (float)byteCount / seconds / 1000.0;
    Serial.print(", rate = ");
    Serial.print(rate);
    Serial.print(" kbytes/second");
    Serial.println();

    // do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
}
