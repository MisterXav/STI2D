// Auteur du Programme : MisterXav
// Programme écrit le : 21/09/2021
// Fonctionnel le : 23/09/2021


// ////////////////////  PRESENTATION DU PROGRAMME //////////////////// 

// -------- Que fait ce programme ? ---------
// Ce programme permet de commander l'angle d'un servo moteur a l'aide d'un caractère envoyé dans le moniteur série

// --- Fonctionnalités utilisées --- 

// /////////////////////////////// 1. Entête déclarative /////////////////////// 
// A ce niveau sont déclarées les librairies incluses, les constantes, les variables, les objets utiles...

// --- Inclusion des librairies ---
#include <Servo.h>

// --- Déclaration des constantes ---
Servo servo; // On crée l'objet "servo"

int Angle = 0; // Valeur que le servo devra récupérer pour se positionner
String readSerial;

// ////////////////////////// 2. FONCTION SETUP = Code d'initialisation ////////////////////////// 
// La fonction setup() est exécutée en premier et 1 seule fois, au démarrage du programme

void setup()   { // debut de la fonction setup()

// --- ici instructions à exécuter 1 seule fois au démarrage du programme --- 

servo.attach(5); // Port du servo moteur
Serial.begin(9600); // Démmarrage liaison série
servo.write(0); //Position du servo sur 0°
Serial.println("Programme : Arduino_servo_moniteur cree le 21/09/2021 par Xavier VB"); // Indication du programme dans la carte arduino

} // fin de la fonction setup()
// ********************************************************************************


////////////////////////////////// 3. FONCTION LOOP = Boucle sans fin = coeur du programme //////////////////
// la fonction loop() s'exécute sans fin en boucle aussi longtemps que l'Arduino est sous tension

void loop(){ // debut de la fonction loop()
// --- ici instructions à exécuter par le programme principal en boucle --- 
  while (Serial.available()) { //On regarde le moniteur série
    char serialChar = Serial.read(); //On récupère la valeur reçue
    readSerial += serialChar; //On prend bien tout les caractères reçu
    delay(2);   // On attend un court instant, que l'opération se termine
  }

  if (readSerial.length() >0) { // Valeur au dessus de 0
    Serial.print("Angle choisi: ");
    Serial.println(readSerial); // On indique la position choisi
    Angle = readSerial.toInt();
    readSerial = ""; // On reset la valeur readSerial
    servo.write(Angle); // On fait tourner le servo a sa position demandé
    Serial.print("Angle du servo: ");
    Serial.println(Angle); // On indique la position du servo
  }
  
} // fin de la fonction loop() - le programme recommence au début de la fonction loop sans fin
// ********************************************************************************

// ////////////////////////// Fin du programme //////////////////// 
