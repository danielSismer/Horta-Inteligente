#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SOIL_SENSOR_PIN 34
#define LDR_SENSOR_PIN 35
#define WATER_PUMP_PIN 23
#define GROW_LIGHT_PIN 19
#define BUZZER_PIN 18

LiquidCrystal_I2C lcd(0x27, 16, 2);
byte lcdAddress = 0;

int calcularPorcentagem(int valorAnalogico) {
  return (valorAnalogico * 100) / 4095;
}

void scanI2C() {
  byte error, address;
  int nDevices = 0;
  Serial.println("Escaneando barramento I2C...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo I2C encontrado no endereco 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      lcdAddress = address;
      nDevices++;
    }
  }
  if (nDevices == 0)
    Serial.println("Nenhum dispositivo I2C encontrado");
  else
    Serial.println("Scan I2C finalizado");
}

void atualizarSolo(int soil) {
  int soilPct = calcularPorcentagem(soil);

  lcd.setCursor(0, 0);
  lcd.print("Solo:      ");
  lcd.setCursor(6, 0);
  lcd.print(soilPct);
  lcd.print("% ");

  if (soil < 2000) {
    digitalWrite(WATER_PUMP_PIN, HIGH);
    tone(BUZZER_PIN, 1000, 300);
    lcd.print("Seco");
    lcd.setCursor(0, 1);
    lcd.print("Irrigando...    ");
  } else {
    digitalWrite(WATER_PUMP_PIN, LOW);
    noTone(BUZZER_PIN);
    lcd.print("Ok  ");
    lcd.setCursor(0, 1);
    lcd.print("Irrigacao off   ");
  }

  Serial.print("Umidade: ");
  Serial.print(soilPct);
  Serial.print("% | ");
}

void atualizarLuz(int light) {
  int lightPct = calcularPorcentagem(light);

  lcd.setCursor(0, 0);
  lcd.print("Luz:       ");
  lcd.setCursor(5, 0);
  lcd.print(lightPct);
  lcd.print("% ");

  if (light < 2000) {
    digitalWrite(GROW_LIGHT_PIN, HIGH);
    lcd.print("Baixa");
    lcd.setCursor(0, 1);
    lcd.print("Luz ligada      ");
  } else {
    digitalWrite(GROW_LIGHT_PIN, LOW);
    lcd.print("Boa ");
    lcd.setCursor(0, 1);
    lcd.print("Luz desligada   ");
  }

  Serial.print("Luminosidade: ");
  Serial.print(lightPct);
  Serial.println("%");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  pinMode(WATER_PUMP_PIN, OUTPUT);
  pinMode(GROW_LIGHT_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  scanI2C();

  if (lcdAddress != 0) {
    lcd = LiquidCrystal_I2C(lcdAddress, 16, 2);
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("LCD endereco:");
    lcd.setCursor(0, 1);
    lcd.print("0x");
    lcd.print(lcdAddress, HEX);
    delay(3000);
    lcd.clear();
  } else {
    Serial.println("LCD nao encontrado. Verifique conexoes e endereco.");
  }
}

void loop() {
  if (lcdAddress == 0) return;

  int soil = analogRead(SOIL_SENSOR_PIN);
  int light = analogRead(LDR_SENSOR_PIN);

  atualizarSolo(soil);
  delay(4000);

  lcd.clear();

  atualizarLuz(light);
  delay(4000);

  lcd.clear();
}
