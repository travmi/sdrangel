///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 Edouard Griffiths, F4EXB                                   //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
// (at your option) any later version.                                           //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#ifndef INCLUDE_INTERFEROMETER_H
#define INCLUDE_INTERFEROMETER_H

#include <vector>
#include <QNetworkRequest>

#include "dsp/mimochannel.h"
#include "dsp/spectrumvis.h"
#include "dsp/scopevis.h"
#include "channel/channelapi.h"
#include "util/messagequeue.h"
#include "util/message.h"

#include "interferometersettings.h"

class QThread;
class DeviceAPI;
class InterferometerBaseband;
class QNetworkReply;
class QNetworkAccessManager;
class ObjectPipe;
class Interferometer: public MIMOChannel, public ChannelAPI
{
public:
    class MsgConfigureInterferometer : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        const InterferometerSettings& getSettings() const { return m_settings; }
        bool getForce() const { return m_force; }

        static MsgConfigureInterferometer* create(const InterferometerSettings& settings, bool force)
        {
            return new MsgConfigureInterferometer(settings, force);
        }

    private:
        InterferometerSettings m_settings;
        bool m_force;

        MsgConfigureInterferometer(const InterferometerSettings& settings, bool force) :
            Message(),
            m_settings(settings),
            m_force(force)
        { }
    };

    class MsgBasebandNotification : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        static MsgBasebandNotification* create(int sampleRate, qint64 centerFrequency) {
            return new MsgBasebandNotification(sampleRate, centerFrequency);
        }

        int getSampleRate() const { return m_sampleRate; }
        qint64 getCenterFrequency() const { return m_centerFrequency; }

    private:

        MsgBasebandNotification(int sampleRate, qint64 centerFrequency) :
            Message(),
            m_sampleRate(sampleRate),
            m_centerFrequency(centerFrequency)
        { }

        int m_sampleRate;
        qint64 m_centerFrequency;
    };

    Interferometer(DeviceAPI *deviceAPI);
	virtual ~Interferometer();
	virtual void destroy() { delete this; }
    virtual void setDeviceAPI(DeviceAPI *deviceAPI);
    virtual DeviceAPI *getDeviceAPI() { return m_deviceAPI; }

	virtual void startSinks(); //!< thread start()
	virtual void stopSinks();  //!< thread exit() and wait()
    virtual void startSources() {}
    virtual void stopSources() {}
	virtual void feed(const SampleVector::const_iterator& begin, const SampleVector::const_iterator& end, unsigned int sinkIndex);
    virtual void pull(SampleVector::iterator& begin, unsigned int nbSamples, unsigned int sourceIndex);
    virtual void pushMessage(Message *msg) { m_inputMessageQueue.push(msg); }
    virtual QString getMIMOName() { return objectName(); }

    virtual void getIdentifier(QString& id) { id = objectName(); }
    virtual QString getIdentifier() const { return objectName(); }
    virtual void getTitle(QString& title) { title = "Interferometer"; }
    virtual qint64 getCenterFrequency() const { return m_frequencyOffset; }
    virtual void setCenterFrequency(qint64) {}
    uint32_t getDeviceSampleRate() const { return m_deviceSampleRate; }

    virtual QByteArray serialize() const;
    virtual bool deserialize(const QByteArray& data);

    virtual int getNbSinkStreams() const { return 2; }
    virtual int getNbSourceStreams() const { return 0; }

    virtual qint64 getStreamCenterFrequency(int streamIndex, bool sinkElseSource) const
    {
        (void) streamIndex;
        (void) sinkElseSource;
        return m_frequencyOffset;
    }

    virtual void setMessageQueueToGUI(MessageQueue *queue) { m_guiMessageQueue = queue; }
    MessageQueue *getMessageQueueToGUI() { return m_guiMessageQueue; }

    SpectrumVis *getSpectrumVis() { return &m_spectrumVis; }
    ScopeVis *getScopeVis() { return &m_scopeSink; }
    void applyChannelSettings(uint32_t log2Decim, uint32_t filterChainHash);

    virtual int webapiSettingsGet(
            SWGSDRangel::SWGChannelSettings& response,
            QString& errorMessage);

    virtual int webapiSettingsPutPatch(
            bool force,
            const QStringList& channelSettingsKeys,
            SWGSDRangel::SWGChannelSettings& response,
            QString& errorMessage);

    virtual int webapiWorkspaceGet(
            SWGSDRangel::SWGWorkspaceInfo& query,
            QString& errorMessage);

    static void webapiFormatChannelSettings(
        SWGSDRangel::SWGChannelSettings& response,
        const InterferometerSettings& settings);

    static void webapiUpdateChannelSettings(
            InterferometerSettings& settings,
            const QStringList& channelSettingsKeys,
            SWGSDRangel::SWGChannelSettings& response);

    static const char* const m_channelIdURI;
    static const char* const m_channelId;
    static const int m_fftSize;

private:
    DeviceAPI *m_deviceAPI;
    QThread *m_thread;
    SpectrumVis m_spectrumVis;
    ScopeVis m_scopeSink;
    InterferometerBaseband* m_basebandSink;
    InterferometerSettings m_settings;
    MessageQueue *m_guiMessageQueue;  //!< Input message queue to the GUI

    QNetworkAccessManager *m_networkManager;
    QNetworkRequest m_networkRequest;

    int64_t m_frequencyOffset;
    uint32_t m_deviceSampleRate;
    int m_count0, m_count1;

	virtual bool handleMessage(const Message& cmd); //!< Processing of a message. Returns true if message has actually been processed
    void applySettings(const InterferometerSettings& settings, bool force = false);
    static void validateFilterChainHash(InterferometerSettings& settings);
    void calculateFrequencyOffset();
    void webapiReverseSendSettings(QList<QString>& channelSettingsKeys, const InterferometerSettings& settings, bool force);
    void sendChannelSettings(
        const QList<ObjectPipe*>& pipes,
        QList<QString>& channelSettingsKeys,
        const InterferometerSettings& settings,
        bool force
    );
    void webapiFormatChannelSettings(
        QList<QString>& channelSettingsKeys,
        SWGSDRangel::SWGChannelSettings *swgChannelSettings,
        const InterferometerSettings& settings,
        bool force
    );

private slots:
    void handleInputMessages();
    void networkManagerFinished(QNetworkReply *reply);
};

#endif // INCLUDE_INTERFEROMETER_H
