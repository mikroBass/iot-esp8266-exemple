/*
  MOOC IMT Fabriquer un objet connecté : TP semaine 3

 Implémentation d'un capteur angulaire

 Lit la valeur analogique du servo moteur sur la broche et envoie
 cette valeur sur la plateforme TOM 
 (serveur HTTP mis en place par l'IMT pour le Mooc - utilisation de requête PATCH )

 Toutes les 3 secondes, la valeur est transmise à TOM afin de voir si le servo a bougé

 Réalisation de l'exercice à partir du squelette de code fourni par l'équipe de l'IMT
 
 Ce code est sous licence CC0 1.0

 François Jarriges 2017

 */
 
// librairies WIFI et HTTP adaptées à notre ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// librairie pour servo moteur
#include <Servo.h>

// valeurs limites de lecture brute du potentiomètre du servomoteur
#define KNOB_RAW_MIN 39
#define KNOB_RAW_MAX 750

// création de l'objet servo issu du moule Servo
Servo myservo;

// variables liées à la position de notre Nelson
const int tolerance = 2; // précision de la lecture de position
int pos             = 0; // position actuelle
int lastPos         = 0; // position précédente


// initialisation des constantes spécifiques à votre installation
// n'oubliez pas d'effacer vos identifiants wifi et nelson lors du dépôt de votre TP
const String nelsonName = "lostinmorvan";
const char* ssid     = "Livebox-XXXX";
<<<<<<< HEAD
const char* password = "XXXXXXXXXXXXXXXXXXXXX";
=======
const char* password = "XXXXXXXXXX";
>>>>>>> 4da379c9f83cfb6a01b4888597dd1ed781f2ea6a

// adresse et port du serveur Tom
const String host     = "api.tom.tools"; 
const int httpPort    = 80;

// url de base des Nelsons sur Tom
const String baseUrl = "/nelsons/";


void wifiConnection(const char* ssid, const char* password) {
  
  // on se connecte au Wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // on vérifie qu'on est bien connecté
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  
  // on récupère une adresse IP
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("");
}

void sendPatch() {
  
  // on envoie une requête HTTP PATCH à Tom 
  // en transmettant la valeur de position de notre Nelson
  // afin de signaler une nouvelle manipulation de celui-ci
  
  // création d'un objet appelé http sorti du moule HTTPClient
  HTTPClient http;
  
  // et déclaration d'une variable httpCode de compte rendu des échanges 
  int httpCode;

  // chaîne de caractères contenant la trame JSON à transmettre
  String payload = "{\"position\": " + String(pos) + "}";
  
  // construction de l'URI de la ressource du Nelson sur Tom
  String uri = baseUrl + nelsonName + "/";

  // on se connecte sur le port HTTP qui est traditionnellement le port 80
  http.begin(host, 80, uri);
      
  // on démarre la connexion et on envoie les entêtes HTTP
  http.addHeader("Content-type", "application/json");
  httpCode = http.sendRequest("PATCH", (uint8_t *) payload.c_str(), payload.length());

  // on affiche la trame JSON portée par la requête PATCH
  Serial.print("[HTTP] PATCH... payload:");
  Serial.println(payload);
  
  // httpCode sera négatif si on rencontre une erreur
  if(httpCode > 0) {

    // tout va bien, les entêtes HTTP ont été envoyés et on affiche la réponse du serveur
    Serial.printf("[HTTP] PATCH... code: %d\n", httpCode);
        
    // et on signale quand le serveur TOM répond par OK (cas normal)
    if(httpCode == HTTP_CODE_OK) 
      Serial.println("[HTTP] PATCH... OK!");
  }
  // attention, si la connexion HTTP échoue, on affiche le problème rencontré sur le port série
  else
      Serial.printf("[HTTP] PATCH... failed, error: %s\n", http.errorToString(httpCode).c_str());

  // dans tous les cas, nous devons fermer la connexion HTTP
  http.end();
}

void setup() {
  
  // on souhaite communiquer avec l'ordinateur
  Serial.begin(9600);
  delay(10);

  // puis on se connecte au Wifi
  wifiConnection(ssid, password);
}

void loop() {
 
  // on lit la valeur de position du servo moteur
  pos = analogRead(A0);
 
  // on convertit celle-ci entre 0 et 180 
  pos = map(pos, KNOB_RAW_MIN, KNOB_RAW_MAX, 0, 180);

  // si le servo a été manipulé
  if (abs(pos - lastPos)> tolerance) {
    Serial.println("");
    Serial.println("Servo à " + String(pos) + " degrés");

    // alors cette nouvelle position est transmise à Tom
    sendPatch();

    // mémorisation de cette nouvelle position
    lastPos = pos;
  }

  // on attend trois secondes avant la prochaine lecture du Nelson
  delay (3000);
}
