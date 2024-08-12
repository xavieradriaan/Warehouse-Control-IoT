#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <NewPing.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Reemplaza con tus credenciales de red
const char* ssid = "G.OCHOA-NETLIFE";
const char* password = "0915858088";

// Pines
const int ledPin = 2; // LED conectado al pin GPIO 2
const int trigPin = 4; // Pin TRIG del sensor de proximidad
const int echoPin = 5; // Pin ECHO del sensor de proximidad

// Configuración del sensor de proximidad
#define MAX_DISTANCE 200 // Distancia máxima en cm
NewPing sonar(trigPin, echoPin, MAX_DISTANCE);

// Configuración del cliente NTP
WiFiUDP ntpUDP;
const long utcOffsetInSeconds = -5 * 3600; // Nueva York está en UTC-5
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, 60000);

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Declaración de la función PrintToLCD
void PrintToLCD(const String& phrase, int XOffset, int YOffset);

void setup() {
  // Inicializa la comunicación serial
  Serial.begin(115200);

  // Configura el pin del LED como salida
  pinMode(ledPin, OUTPUT);

  // Conéctate a la red WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Inicializa el cliente NTP
  Serial.println("Inicializando NTP...");
  timeClient.begin();

  // Inicializa la pantalla OLED
  Serial.println("Inicializando pantalla OLED...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Dirección I2C 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.display();
  delay(500);
  display.clearDisplay();
}

void loop() {
  // Actualiza la hora desde el servidor NTP
  timeClient.update();

  // Medir la distancia usando el sensor de proximidad
  unsigned int distance = sonar.ping_cm();

  // Verificar si la distancia es menor o igual a 5 cm
  if (distance > 0 && distance <= 5) {
    // Enciende el LED
    digitalWrite(ledPin, HIGH);
    Serial.println("¡Alerta! Objeto detectado a menos de 5 cm");

    // Muestra la alerta en la pantalla OLED
    PrintToLCD("¡Alerta!\nObjeto detectado\na menos de 5 cm", 0, 0);
  } else {
    // Apaga el LED
    digitalWrite(ledPin, LOW);

    // Muestra la hora en la pantalla OLED
    String timeString = timeClient.getFormattedTime();
    PrintToLCD("Hora actual:\n" + timeString, 0, 0);
  }

  delay(1000); // Espera 1 segundo antes de verificar nuevamente
}

void PrintToLCD(const String& phrase, int XOffset, int YOffset) {
  display.clearDisplay();
  display.setCursor(XOffset, YOffset);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println(phrase);
  display.display();
}