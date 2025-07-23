#ifdef MESHTASTIC_INCLUDE_INKHUD

#include "./StatusApplet.h"

#include "RTC.h"
using namespace NicheGraphics;

InkHUD::StatusApplet::StatusApplet()
{
    powerStatusObserver.observe(&powerStatus->onNewStatus);
    gpsStatusObserver.observe(&gpsStatus->onNewStatus);
    nodeStatusObserver.observe(&nodeStatus->onNewStatus);
    bluetoothStatusObserver.observe(&bluetoothStatus->onNewStatus);
}

void InkHUD::StatusApplet::statusUpdated(bool forceUpdate)
{
    uint32_t currentMillis = millis();
    if (forceUpdate || currentMillis - this->lastUpdate > minUpdateInterval)
    {
        requestUpdate();
        lastUpdate = currentMillis;
    }
}

void InkHUD::StatusApplet::onRender()
{
    uint16_t height = 0;

    char nodeCount[10];
    formatNodeCount(nodeCount, sizeof(nodeCount));

    char voltage[7];
    formatBatteryVoltage(voltage, sizeof(voltage));

    setFont(fontMedium);
    height += fontMedium.lineHeight() * 1.1f;

    if (getRTCQuality() != RTCQualityNone)
    {
        std::string clockString = getTimeString();
        if (clockString.length() > 0)
        {
            setFont(fontMedium);
            printAt(X(0.5f), Y(0), clockString, CENTER, TOP);
        }
        printAt(1, 0, nodeCount, LEFT);
        printAt(X(0.5f), 0, getTimeString(), CENTER);
    }
    else
    {
        printAt(X(0.5f), 0, "Time Unset", CENTER);
    }

    drawLine(X(0), height, X(1), height, BLACK);
    height += 4;

    // TODO find out how to get battery icon width
    if (powerData->getHasBattery())
        printAt(X(0.9f), Y(0), voltage, RIGHT);

    // print mesh name

    setFont(fontLarge);
    printAt(X(0.5), height, parse(owner.short_name), CENTER);
    height += fontLarge.lineHeight() * 1.1;
    setFont(fontSmall);
    printAt(X(0.5), height, parse(owner.long_name), CENTER);
    height += fontSmall.lineHeight() * 1.1f;
    setFont(fontMedium);

    // define bar offsets
    int16_t barLeft = X(0.2f);
    int16_t barWidth = X(0.8f) - barLeft;

    float chUtil = airTime->channelUtilizationPercent();
    char chUtilStr[12];
    formatPercent(chUtilStr, sizeof(chUtilStr), chUtil);

    float txUtil = airTime->utilizationTXPercent();
    char txUtilStr[12];
    formatPercent(txUtilStr, sizeof(txUtilStr), txUtil);

    // draw channel util bar
    height = Y(1) - fontSmall.lineHeight() * 3;
    setFont(fontSmall);
    printAt(X(0.2f), height, "Ch. Util: ", RIGHT);
    printAt(barLeft + barWidth + 5, height, chUtilStr);
    drawProgressBar(barLeft, height + 2, barWidth, fontSmall.lineHeight() - 4, chUtil / 100);
    height += fontSmall.lineHeight() * 1.5;

    // draw tx util bar
    printAt(X(0.2f), height, "TX Util: ", RIGHT);
    printAt(barLeft + barWidth + 5, height, txUtilStr);
    drawProgressBar(barLeft, height + 2, barWidth, fontSmall.lineHeight() - 4, txUtil / 100);
    height += fontSmall.lineHeight() * 1.5;
}

void InkHUD::StatusApplet::drawProgressBar(int16_t x0, int16_t y0, int16_t w, int16_t h, float progress)
{
    const uint8_t padding = 2;

    // draw outline
    drawRect(x0, y0, w, h, BLACK);

    // draw bar
    int16_t maxLen = w - 2 * padding;

    // calculate progress bar length
    // if progress < 0, length is 0;
    // if progress > 1, length is maxLen.
    int16_t progressLen = 0;
    if (progress > 1)
    {
        progressLen = maxLen;
    }
    else if (progress > 0)
    {
        progressLen = maxLen * progress;
    }

    if (progressLen > 1)
    {
        hatchRegion(x0 + padding, y0 + padding, progressLen, h - 2 * padding, 2, BLACK);
        drawRect(x0 + padding, y0 + padding, progressLen, h - 2 * padding, BLACK);
    }
}

int InkHUD::StatusApplet::onPowerStatusUpdate(const meshtastic::Status *status)
{
    assert(status->getStatusType() == STATUS_TYPE_POWER);

    meshtastic::PowerStatus *powerStatus = (meshtastic::PowerStatus *)status;
    this->powerData = powerStatus;
    this->statusUpdated();
    return 0;
}

int InkHUD::StatusApplet::onGpsStatusUpdate(const meshtastic::Status *status)
{
    assert(status->getStatusType() == STATUS_TYPE_GPS);

    meshtastic::GPSStatus *gpsStatus = (meshtastic::GPSStatus *)status;
    this->gpsData = gpsStatus;
    this->statusUpdated();
    return 0;
}

int InkHUD::StatusApplet::onNodeStatusUpdate(const meshtastic::Status *status)
{
    assert(status->getStatusType() == STATUS_TYPE_NODE);

    meshtastic::NodeStatus *nodeStatus = (meshtastic::NodeStatus *)status;
    this->nodeData = nodeStatus;
    this->statusUpdated();
    return 0;
}

int InkHUD::StatusApplet::onBluetoothStatusUpdate(const meshtastic::Status *status)
{
    assert(status->getStatusType() == STATUS_TYPE_BLUETOOTH);

    meshtastic::BluetoothStatus *btStatus = (meshtastic::BluetoothStatus *)status;
    this->btData = btStatus;
    this->statusUpdated();
    return 0;
}

void InkHUD::StatusApplet::formatNodeCount(char *buf, size_t bufLen)
{
    snprintf(buf, bufLen, "%u/%u", this->nodeData->getNumOnline(), this->nodeData->getNumTotal());
}

void InkHUD::StatusApplet::formatPercent(char *buf, size_t bufLen, float_t value)
{
    if (value >= 1000 || value <= -1000)
    {
        strncpy(buf, "- %", bufLen);
        buf[bufLen - 1] = '\0';
    }
    else
    {
        snprintf(buf, bufLen, "%2.0f%%", value);
    }
}

void InkHUD::StatusApplet::formatBatteryVoltage(char *buf, size_t bufLen)
{
    float_t batteryVoltage = powerData->getBatteryVoltageMv() / 1000.0f;
    if (batteryVoltage >= 10.0f)
    {
        strncpy(buf, ">9.99V", bufLen);
        buf[bufLen - 1] = '\0';
    }
    else
    {
        snprintf(buf, bufLen, "%.2fV", batteryVoltage);
    }
}

#endif