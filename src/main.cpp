#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// Nordic UART Service — widely supported by phone apps (nRF Connect, LightBlue, etc.)
#define NUS_SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define NUS_RX_UUID      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // phone → ESP (write)
#define NUS_TX_UUID      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // ESP → phone (notify)

static constexpr char kDeviceName[] = "Pill-FW";

static BLEServer* g_server = nullptr;
static BLECharacteristic* g_tx = nullptr;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* /*p*/) override {
    Serial.println("[BLE] connected");
  }

  void onDisconnect(BLEServer* /*p*/) override {
    Serial.println("[BLE] disconnected — advertising again");
    BLEDevice::startAdvertising();
  }
};

class RxCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* ch) override {
    String v = ch->getValue();
    if (v.length() == 0) {
      return;
    }
    Serial.print("[BLE] RX: ");
    Serial.println(v);

    if (g_tx != nullptr) {
      g_tx->setValue(v);
      g_tx->notify();
    }
  }
};

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("BLE Nordic UART server starting…");

  BLEDevice::init(kDeviceName);

  g_server = BLEDevice::createServer();
  g_server->setCallbacks(new ServerCallbacks());

  BLEService* svc = g_server->createService(NUS_SERVICE_UUID);

  g_tx = svc->createCharacteristic(
      NUS_TX_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  g_tx->setValue("ok");

  BLECharacteristic* rx = svc->createCharacteristic(
      NUS_RX_UUID,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  rx->setCallbacks(new RxCallbacks());

  svc->start();

  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(NUS_SERVICE_UUID);
  adv->setScanResponse(true);
  adv->setMinPreferred(0x06);
  adv->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.print("Advertising as \"");
  Serial.print(kDeviceName);
  Serial.println("\" — open nRF Connect, connect, use Nordic UART RX/TX");
}

void loop() {
  delay(200);
}
