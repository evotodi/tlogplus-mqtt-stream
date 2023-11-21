#ifndef _H_MQTT_STREAM_PLUS
#define _H_MQTT_STREAM_PLUS

#include <TLogPlus.h>
#include <Print.h>
#include <PubSubClient.h>
namespace TLogPlusStream {
    class MqttStream : public TLogPlus::TLog {
    public:
        MqttStream(Client *client, char *mqttServer = NULL, char *mqttTopic = NULL, const uint16_t mqttPort = 1883) :
                _client(client), _mqttPort(mqttPort) {
            if (mqttServer) _mqttServer = strdup(mqttServer);
            if (mqttTopic) _mqttTopic = strdup(mqttTopic);
        };

        MqttStream(PubSubClient *pubsub, char *mqttTopic = NULL) :
                _mqtt(pubsub) {
            if (mqttTopic) _mqttTopic = strdup(mqttTopic);
            _client = NULL; // used to detect the case where we're not resposible for the connection.
        };

        void setPort(uint16_t port) {
            _mqttPort = port;
        }

        void setTopic(const char *topic) {
            if (topic)
                _mqttTopic = strdup(topic);
        }

        void setServer(const char *server) {
            if (server)
                _mqttServer = strdup(server);
        }

        virtual size_t write(uint8_t c);

        virtual void begin();

        virtual void loop();

        virtual void reconnect();

    private:
        Client *_client = NULL;
        PubSubClient *_mqtt = NULL;
        const char *_mqttServer = NULL, *_mqttTopic = NULL;
        uint16_t _mqttPort = 0;
        char logbuff[512];
        size_t at = 0;
    protected:
    };
}
#endif