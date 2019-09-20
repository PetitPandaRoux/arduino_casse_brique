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
// Argument 1 = Number of pixels in NeoPixel pixels
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

//tableau des calculs
uint8_t tableau[5][5] = {
  {0, 0, 2, 0, 0},
  {0, 2, 0, 2, 0},
  {0, 0, 2, 0, 0},
  {0, 0, 0, 3, 0},
  {0, 0, 4, 0, 0}
};

//tableau final
uint8_t tableau_1d[25];

uint8_t briques[4][2] = {
  {0,2}, {1,1}, {1,3},{2,2}
};

int led = 13;

//Nous initialisons le positionnemnet du joueur ici
int joueurX = 2;
int joueurY = 4;

//Nous initialisons la position de la balle ici
uint8_t balleX = 3;
uint8_t balleY = 3;

//Les vitesses initiales de la balle
int vitesseX = -1;
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
  feuDartifice();
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



 
  
  size_t array_length = sizeof(tableau_1d) / sizeof(tableau_1d[0]);
  
  deplacerBalle();

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
  
  setMatriceCouleur(tableau_1d, array_length);
  
  montrerSerial(); // Permet de vérifier les positions
  
  delay(500);

}


void whiteOverRainbow(int whiteSpeed, int whiteLength) {

  if(whiteLength >= pixels.numPixels()) whiteLength = pixels.numPixels() - 1;

  int      head          = whiteLength - 1;
  int      tail          = 0;
  int      loops         = 3;
  int      loopNum       = 0;
  uint32_t lastTime      = millis();
  uint32_t firstPixelHue = 0;

  for(;;) { // Repeat forever (or until a 'break' or 'return')
    for(int i=0; i<pixels.numPixels(); i++) {  // For each pixel in pixels...
      if(((i >= tail) && (i <= head)) ||      //  If between head & tail...
         ((tail > head) && ((i >= tail) || (i <= head)))) {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0, 255)); // Set white
      } else {                                             // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
        pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
      }
    }

    pixels.show(); // Update pixels with new contents
    // There's no delay here, it just runs full-tilt until the timer and
    // counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    if((millis() - lastTime) > whiteSpeed) { // Time to update head/tail?
      if(++head >= pixels.numPixels()) {      // Advance head, wrap around
        head = 0;
        if(++loopNum >= loops) return;
      }
      if(++tail >= pixels.numPixels()) {      // Advance tail, wrap around
        tail = 0;
      }
      lastTime = millis();                   // Save time of last movement
    }
  }
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<pixels.numPixels(); i++) { // For each pixel in pixels...
    pixels.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    pixels.show();                          //  Update pixels to match
    delay(wait);                           //  Pause for a moment
  }
}

void feuDartifice(){
  colorWipe(pixels.Color(255,   0,   0)     , 50); // Red
  colorWipe(pixels.Color(  0, 255,   0)     , 50); // Green
  colorWipe(pixels.Color(  0,   0, 255)     , 50); // Blue
  colorWipe(pixels.Color(  0,   0,   0, 255), 50); // True white (not RGB white)

  whiteOverRainbow(75, 5);
}
  

void setMatriceCouleur(uint8_t matrice[], int longueurTableau) {

  for (byte i = 0 ; i < longueurTableau ; i++) {
    if (matrice[i] == 1 ) { 
      pixels.setPixelColor(i, pixels.Color(0, 255, 0)); // Les murs rouges
    } else if (matrice[i] == 2) { 
      pixels.setPixelColor(i, pixels.Color(255, 255, 0)); // La briques
    } else if (matrice[i] == 3) { 
      pixels.setPixelColor(i, pixels.Color(255, 0, 255)); //La balle
    } else if (matrice[i] == 4) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 255)); // Le joueur
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // Rien
    }
  }
  pixels.show();                          //  Update pixels to match
}

void deplacerBalle() {

  //Efface la lumière du dernier positionnement de la balle
  tableau[balleX][balleY] = 0 ;

  //On déplace la balle
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

//Si la balle rencontre une brique
int b,c = 0 ;
for (b = 0 ; b < 4 ; b ++){
  if (balleX == briques[b][0] && balleY == briques[b][1]){
      tableau[briques[b][0]][briques[b][1]] = 0 ;
      briques[b][0] = -1;
      briques[b][1] = -1;
      vitesseY = -vitesseY;          
      vitesseX = -vitesseX ;
  } 
}

//Si la balle rencontre le joueur 
if (balleX == joueurX && balleY == joueurY) {
    vitesseY = -vitesseY;          
    vitesseX = -vitesseX ;
}

if (balleX < 0) {
 // restart()
}

  // Place la lumière sur la nouvelle position de la balle 
  tableau[balleX][balleY] = 3;
  
}


/*void restart() {
  tableau= {
    {1, 0, 0, 0, 1},
    {1, 2, 2, 2, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 3, 0, 1},
    {1, 0, 0, 0, 1}
  }

  //tableau final
  tableau_1d[25];

  briques= {
    {1,1}, {1,2}, {1,3}
  };


  //Nous initialisons le positionnemnet du joueur ici
  joueurX = 1;
  joueurY = 1;

  //Nous initialisons la position de la balle ici
  balleX = 3;
  balleY = 2;

}*/

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
    tableau[joueurY][joueurX] = 4;
    etatButtonPlusPrec = HIGH;
  } else if (sens == "droite") {
    tableau[joueurY][joueurX] = 0;
    joueurX--;
    tableau[joueurY][joueurX] = 4;
    etatButtonMoinsPrec = HIGH;
  }
}
