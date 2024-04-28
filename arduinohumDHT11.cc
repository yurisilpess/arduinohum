#include <LiquidCrystal.h>
#include <DHT.h>

// Define os pinos as serem usados
#define LED_VERDE 4
#define LED_AMARELO 3
#define LED_VERMELHO 2
#define PIEZZO 1
#define DHTPIN 13  
#define DHTTYPE DHT11 // Define o tipo do sensor DHT como DHT11
DHT dht(DHTPIN, DHTTYPE); // Cria uma instância do sensor DHT
#define RS 7
#define E 6
#define D4 12
#define D5 10
#define D6 9
#define D7 8
#define LDR A0
LiquidCrystal lcd(RS, E, D4, D5, D6, D7); // Cria uma instância do LCD

// Define as variaveis 
unsigned long lastUpdateTime = 0;
const int LUMINOSIDADE_MIN = 33;
const int LUMINOSIDADE_MAX = 50;
const int TEMPERATURA_MIN = 10;
const int TEMPERATURA_MAX = 15;
const int UMIDADE_MIN = 50;
const int UMIDADE_MAX = 70;

// Define o desenho dos ícones a serem exibidos no LCD
byte umidadeIcon[8] = {B00100, B00100, B01010, B01010, B10001, B10001, B10001, B01110,};
byte temperaturaIcon[8] = {B00100, B01010, B01010, B01110, B01110, B11111, B11111, B01110,};
byte luzIcon[8] = {B10001, B01110, B11111, B01110, B10001, B00000, B00000, B00000,};
byte aviaoIcon1[8] = {B00000, B00000, B00001, B00011, B11111, B00011, B00001, B00000,};
byte aviaoIcon2[8] = {B00000, B00000, B10000, B11000, B11111, B11000, B10000, B00000,};

void setup() {
  // Configura os pinos como saída ou entrada
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LDR, INPUT);
  // Inicia a comunicação serial
  Serial.begin(115200);
  Serial.println(F("Print dos valores!"));
  // Inicia o sensor DHT22
  dht.begin();
  // Inicia o LCD com 16 colunas e 2 linhas
  lcd.begin(16, 2);
  // Cria os caracteres personalizados para o LCD
  lcd.createChar(0, umidadeIcon); 
  lcd.createChar(1, temperaturaIcon); 
  lcd.createChar(2, luzIcon); 
  lcd.createChar(3, aviaoIcon1); 
  lcd.createChar(4, aviaoIcon2); 
  // Exibe uma animação inicial no LCD
  for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    lcd.clear();
    lcd.setCursor(positionCounter, 0);
    lcd.write(byte(3)); 
    lcd.setCursor(positionCounter + 1, 0);
    lcd.write(byte(4)); 
    lcd.setCursor(positionCounter, 1);
    lcd.print("PCD's CodeX");
    delay(500); 
  }
  lcd.clear();
}

void loop() {
  // Verifica se passaram 5 segundos desde a última atualização
  if (millis() - lastUpdateTime >= 5000) {
    lastUpdateTime = millis();
    // Lê a luminosidade, temperatura e umidade
    int luminosidade = analogRead(LDR);
    int temperatura = dht.readTemperature();
    int umidade = dht.readHumidity();
    // Mapeia esses valores para uma escala de 0 a 100
    int luminosidadePercentual = map(luminosidade, 0, 1023, 0, 100);
    int temperaturaMapeada = map(temperatura, 0, 100, 0, 100);
    int umidadeMapeada = map(umidade, 0, 100, 0, 100);
    // Verifica se a leitura do sensor DHT22 foi bem-sucedida
    if (isnan(temperatura) || isnan(umidade)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    // Imprime as informações de luminosidade, umidade e temperatura no monitor serial
    Serial.print(F("Luminosidade: "));
    Serial.print(luminosidadePercentual);
    Serial.print(F(", Umidade: "));
    Serial.print(umidadeMapeada);
    Serial.print(F("%,  Temperatura: "));
    Serial.print(temperaturaMapeada);
    Serial.println(F("°C "));
    // Chama a função para desligar todos os LEDs
    desligarLEDs();
    // Chama a função para atualizar o status do sistema com base nos valores lidos
    atualizarStatus(luminosidadePercentual, temperaturaMapeada, umidadeMapeada);
  }
}
// Função para desligar todos os LEDs
void desligarLEDs() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERMELHO, LOW);
}
// Função para atualizar o status do sistema com base nos valores lidos
void atualizarStatus(int luminosidade, int temperatura, int umidade) {
  if (luminosidade <= LUMINOSIDADE_MIN && temperatura >= TEMPERATURA_MIN && temperatura <= TEMPERATURA_MAX && umidade >= UMIDADE_MIN && umidade <= UMIDADE_MAX) {
    Serial.println("Condições para LED verde ativo"); 
    digitalWrite(LED_VERDE, HIGH);
    atualizarLCD(temperatura, umidade, luminosidade);
  } 
  else if (temperatura >= 1 && temperatura < 9 || temperatura >= 16 && temperatura < 30 || luminosidade >= 34 && luminosidade < 50 || umidade >= 40 && umidade < 49 || umidade >= 71 && umidade < 80) {
    Serial.println("Condições para LED amarelo ativo"); 
    digitalWrite(LED_AMARELO, HIGH);
    atualizarLCD(temperatura, umidade, luminosidade);
    tone(PIEZZO, 1000);
    delay(1000);
  }   
  else {
    Serial.println("Condições para LED vermelho ativo"); 
    digitalWrite(LED_VERMELHO, HIGH);
    atualizarLCD(temperatura, umidade, luminosidade);
    tone(PIEZZO, 1000);
    delay(1000);
  }
}
// Função para atualizar o display LCD
void atualizarLCD(int temperatura, int umidade, int luminosidade) {
  lcd.clear();
  
  // Define a posição dos ícones na segunda linha do LCD
  lcd.setCursor(0, 1);
  lcd.write(byte(1)); // Ícone de temperatura
  lcd.print(":");
  lcd.setCursor(2, 1);
  lcd.print(int(temperatura)); // Valor da temperatura

  lcd.setCursor(6, 1);
  lcd.write(byte(0)); // Ícone de umidade
  lcd.print(":");
  lcd.setCursor(8, 1);
  lcd.print(int(umidade)); // Valor da umidade

  lcd.setCursor(12, 1);
  lcd.write(byte(2)); // Ícone de luz
  lcd.print(":");
  lcd.setCursor(14, 1);
  lcd.print(int(luminosidade)); // Valor da luminosidade

 // Verifica a temperatura e exibe a mensagem correspondente
lcd.setCursor(0, 0);
lcd.print("                "); // Limpa a linha
lcd.setCursor(0, 0);
if (temperatura < TEMPERATURA_MIN) {
  lcd.print("Temp baixa:");
  lcd.print(int(temperatura));
  tone(PIEZZO, 1000, 2000); // Adiciona som de buzzer
  delay(2000);
} else if (temperatura > TEMPERATURA_MAX) {
  lcd.print("Temp alta:");
  lcd.print(int(temperatura));
  tone(PIEZZO, 1000, 2000); // Adiciona som de buzzer
  delay(2000);
} else {
  lcd.print("Temp ok:");
  lcd.print(int(temperatura));
  delay(2000);
}

// Verifica a umidade e exibe a mensagem correspondente
lcd.setCursor(0, 0);
lcd.print("                "); // Limpa a linha
lcd.setCursor(0, 0);
if (umidade < UMIDADE_MIN) {
  lcd.print("Umidade baixa:");
  lcd.print(int(umidade));
  tone(PIEZZO, 1000, 2000); // Adiciona som de buzzer
  delay(2000);
} else if (umidade > UMIDADE_MAX) {
  lcd.print("Umidade alta:");
  lcd.print(int(umidade));
  tone(PIEZZO, 1000, 2000); // Adiciona som de buzzer
  delay(2000);
} else {
  lcd.print("Umidade ok:");
  lcd.print(int(umidade));
  delay(2000);
}

// Verifica a luminosidade e exibe a mensagem correspondente
lcd.setCursor(0, 0);
lcd.print("                "); // Limpa a linha
lcd.setCursor(0, 0);
if (luminosidade < LUMINOSIDADE_MIN) {
  lcd.print("Luz ok:");
  lcd.print(int(luminosidade));
  delay(2000);
} else if (luminosidade > LUMINOSIDADE_MIN && luminosidade <= LUMINOSIDADE_MAX) {
  lcd.print("Meia Luz:");
  lcd.print(int(luminosidade));
  tone(PIEZZO, 1000, 2000); // Adiciona som de buzzer
  delay(2000);
} else {
  lcd.print("Luz alta:");
  lcd.print(int(luminosidade));
  tone(PIEZZO, 1000, 2000); // Adiciona som de buzzer
  delay(2000);
  }
}
