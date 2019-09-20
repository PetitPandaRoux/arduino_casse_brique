#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

//Definit le nombre de NeoPixels attachés à l'arduino
#define NUMPIXELS 25

//Definit la pin utilisé pour l'envoi du signal
#define PIN 6

#define BRIGHTNESS 255



Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

//tableau des calculs
uint8_t tableau[5][5] = {
  {1, 0, 0, 0, 1},
  {1, 2, 2, 2, 1},
  {1, 0, 0, 0, 1},
  {1, 0, 3, 0, 1},
  {1, 0, 0, 0, 1}
};

//tableau final
uint8_t tableau_1d[25];

uint8_t briques[3][2] = {
  {1,1}, {1,2}, {1,3}
};

int led = 13;

//Nous initialisons le positionnemnet du joueur ici
int joueurX = 1;
int joueurY = 1;

//Nous initialisons la position de la balle ici
uint8_t balleX = 3;
uint8_t balleY = 2;

//Les vitesses initiales de la balle
int vitesseX = 1;
int vitesseY = 1;

//initialisation des boutons de déplacement
const int buttonPlus = 2;
const int buttonMoins = 3;

int etatButtonPlus = 0;
int etatButtonPlusPrec = 0;
int etatButtonMoins = 0;
int etatButtonMoinsPrec = 0;  

//Obtenir la longueur d'un tableau (nombre d'éléments)
size_t array_length = sizeof(tableau) / sizeof(tableau[0]);


void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  Serial.begin(9600);
  pinMode(buttonPlus, INPUT);
  pinMode(buttonMoins, INPUT);
  pinMode(led, OUTPUT);
  pixels.begin();
  pixels.show(); // Initialize all pixels to 'off'
  pixels.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)

}

void loop() {

//Lecture des deux boutons de déplacement
  etatButtonPlus = digitalRead(buttonPlus);
  etatButtonMoins = digitalRead(buttonMoins);

// Déplacement du joueur
  if (etatButtonPlus == HIGH && etatButtonPlusPrec == LOW) {
    deplacerJoueur("gauche");
    delay(5);
  } else {
    etatButtonPlusPrec = LOW;
  }

  if (etatButtonMoins == HIGH && etatButtonMoinsPrec == LOW) {
    deplacerJoueur("droite");
    delay(5);
  } else {
    etatButtonMoinsPrec = LOW;
  }


// Tranformation du tableau 
  int i, j, k = 0;

  for(j = 0 ; j < 5 ; j++){
    if ( j % 2 != 0) {
      for ( i = 4 ; i >= 0 ; i--) {
        tableau_1d[k] = tableau[i][j];
        k++ ;
      }
    } else {
      for( i = 0 ; i < 5 ; i++){
        tableau_1d[k] = tableau[i][j];
        k++ ;  
      }
    }
  }
 
  
  size_t array_length = sizeof(tableau_1d) / sizeof(tableau_1d[0]);
  
  deplacerBalle();
  
  setMatriceCouleur(tableau_1d, array_length);
  
  montrerSerial(); // Permet de vérifier les positions
  
  delay(2000);

}


void setMatriceCouleur(uint8_t matrice[], int longueurTableau) {

  for (byte i = 0 ; i < longueurTableau ; i++) {
    if (matrice[i] == 1 ) { 
      pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Les murs rouges
    } else if (matrice[i] == 2) { 
      pixels.setPixelColor(i, pixels.Color(255, 255, 0)); // Le joueur 
    } else if (matrice[i] == 3) { 
      pixels.setPixelColor(i, pixels.Color(255, 0, 0)); //La balle
    } else if (matrice[i] == 4) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 255)); // Les briques
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Rien
    }
  }
  pixels.show();                          //  Update strip to match
}

void deplacerBalle() {

  //Efface la lumière du dernier positionnement de la balle
  tableau[balleX][balleY] = 0 ;

  balleX = balleX + vitesseX ;
  balleY = balleY + vitesseY ;

//Si la balle sort horizontalement
  if (balleX >= 4 || balleX <= 0) {
    vitesseX = -vitesseX ;
  } else {
    vitesseX = vitesseX;
  }

//Si la balle sort verticalement
  if (balleY >= 4 || balleY <= 0) {
    vitesseY = -vitesseY;
  } else {
    vitesseY = vitesseY;
  }

int b,c = 0 ;
for (b = 0 ; b < 3 ; b ++){
  if (balleX == briques[b][0] && balleY == briques[b][1]){
      tableau[briques[b][0]][briques[b][1]] = 0 ;
      briques[b][0] = -1;
      briques[b][1] = -1;
      vitesseY = -vitesseY;          
      vitesseX = -vitesseX ;
  } 
}


  // Place la lumière sur la nouvelle position de la balle 
  tableau[balleX][balleY] = 3;
  
}

void updatePosition(){

}

void montrerSerial() {
  Serial.println("");
  Serial.print("position balle X: ");
  Serial.println(balleX);
  Serial.print("position balle Y: ");
  Serial.println(balleY);
  Serial.print("vitesseX: ");
  Serial.println(vitesseX);
  Serial.print("vitesseY: ");
  Serial.println(vitesseY);
  Serial.println("");

  for (int i = 0; i < 5; i++) {
    for (int j = 0 ; j < 5 ; j++) {
      Serial.print(tableau[i][j]);
    }
    Serial.println("");
  }
}

void deplacerJoueur(String sens) {
  if (sens == "gauche") {
    tableau[joueurY][joueurX] = 0;
    joueurX++;
    tableau[joueurY][joueurX] = 1;
    etatButtonPlusPrec = HIGH;
  } else if (sens == "droite") {
    tableau[joueurY][joueurX] = 0;
    joueurX--;
    tableau[joueurY][joueurX] = 1;
    etatButtonMoinsPrec = HIGH;
  }
}
