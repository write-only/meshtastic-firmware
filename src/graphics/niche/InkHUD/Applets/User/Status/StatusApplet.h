#ifdef MESHTASTIC_INCLUDE_INKHUD

/*

Shows lora, gps and other device stats on a single page.

*/

#pragma once

#include "configuration.h"
#include "main.h"
#include "graphics/niche/InkHUD/Applet.h"

#include "PowerStatus.h"
#include "GPSStatus.h"
#include "airtime.h"

namespace NicheGraphics::InkHUD
{

  class StatusApplet : public Applet
  {
  public:
    StatusApplet();

    void onRender() override;
    void statusUpdated(bool forceUpdate = false);

    uint32_t minUpdateInterval = 30000;

    int onPowerStatusUpdate(const meshtastic::Status *status);
    int onGpsStatusUpdate(const meshtastic::Status *status);
    int onNodeStatusUpdate(const meshtastic::Status *status);
    int onBluetoothStatusUpdate(const meshtastic::Status *status);

  private:
    CallbackObserver<StatusApplet, const meshtastic::Status *> powerStatusObserver =
        CallbackObserver<StatusApplet, const meshtastic::Status *>(this, &StatusApplet::onPowerStatusUpdate);

    CallbackObserver<StatusApplet, const meshtastic::Status *> gpsStatusObserver =
        CallbackObserver<StatusApplet, const meshtastic::Status *>(this, &StatusApplet::onGpsStatusUpdate);

    CallbackObserver<StatusApplet, const meshtastic::Status *> nodeStatusObserver =
        CallbackObserver<StatusApplet, const meshtastic::Status *>(this, &StatusApplet::onNodeStatusUpdate);

    CallbackObserver<StatusApplet, const meshtastic::Status *> bluetoothStatusObserver =
        CallbackObserver<StatusApplet, const meshtastic::Status *>(this, &StatusApplet::onBluetoothStatusUpdate);

    void drawProgressBar(int16_t x0, int16_t y0, int16_t x1, int16_t y1, float progress);
    void formatBatteryVoltage(char *buf, size_t bufLen);
    void formatNodeCount(char *buf, size_t bufLen);
    void formatPercent(char *buf, size_t bufLen, float_t value);
    uint32_t lastUpdate = 0;
    meshtastic::PowerStatus *powerData;
    meshtastic::GPSStatus *gpsData;
    meshtastic::BluetoothStatus *btData;
    meshtastic::NodeStatus *nodeData;
  };

} // namespace NicheGraphics::InkHUD

#endif