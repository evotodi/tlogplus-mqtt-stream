#include "MqttlogStream.h"
#include <PubSubClient.h>
namespace TLogPlusStream {
    void MqttStream::begin() {
        if (!_mqtt) {
            if (!_mqttServer || !_mqttTopic || !_mqttPort) {
                TLogPlus::Log.printf("Missing%s%s%s for MQTT Logging\n",
                           _mqttServer ? "" : " server", _mqttTopic ? "" : " topic", _mqttPort ? "" : " port");
                return;
            }

            PubSubClient *psc = new PubSubClient(*_client);
            psc->setServer(_mqttServer, _mqttPort);
            psc->setBufferSize(sizeof(logbuff) + 9 + strlen(_mqttTopic));

            TLogPlus::Log.printf("Opened log on mqtt:://%s:%d/%s\n", _mqttServer, _mqttPort, _mqttTopic);
            _mqtt = psc;
            _mqtt->connect(_identifier.c_str());
        } else {
            if (!_mqttTopic) {
                TLogPlus::Log.printf("Missing topic for MQTT Logging\n");
                return;
            };
            TLogPlus::Log.printf("Opened mqtt log on topic %s\n", _mqttTopic);
        };
        reconnect();
        loop();
    }

    void MqttStream::reconnect() {
    }

    void MqttStream::loop() {
        static unsigned long lst = 0;
        if (!_mqtt)
            return;

        // When we do not have the client handle - we're sharing a connection
        // with something else. Let that take the lead.
        if (!_client)
            return;

        _mqtt->loop();

        if (_mqtt->connected())
            return;

        if (lst && millis() - lst < 15000)
            return;

        TLogPlus::Log.printf("Log::MQTT connection state: %d (not connected)\n", _mqtt->state());

        if (_mqtt->connect(_mqttTopic)) {
            TLogPlus::Log.println("Log:: (re)connecting to MQTT");
            lst = 0;
            return;
        };

        TLogPlus::Log.println("Log:: MQTT (re)connection failed. Will retry");
        lst = millis();
    }

    size_t MqttStream::write(uint8_t c) {
        if (at >= sizeof(logbuff) - 1) {
            Serial.println("Purged logbuffer (should never happen)");
            at = 0;
        };

        if (c >= 32 && c < 128)
            logbuff[at++] = c;

        if (c == '\n' || at >= sizeof(logbuff) - 1) {
            logbuff[at++] = 0;
            at = 0;
            // perhaps we should buffer this - and do this in the main loop().
            if (_mqtt && _mqtt->connected()) {
                if (_mqttTopic == NULL)
                    _mqtt->publish("debug", logbuff);
                else
                    _mqtt->publish(_mqttTopic, logbuff);
            };
            yield();

        };
        return 1;
    }
}