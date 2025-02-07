///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2021 Jon Beniston, M7RCE                                        //
// Copyright (C) 2020 Edouard Griffiths, F4EXB                                   //
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

#ifndef INCLUDE_FEATURE_MAP_H_
#define INCLUDE_FEATURE_MAP_H_

#include <QThread>
#include <QHash>
#include <QNetworkRequest>
#include <QDateTime>
#include <QMutex>

#include "feature/feature.h"
#include "util/message.h"

#include "mapsettings.h"

class WebAPIAdapterInterface;
class QNetworkAccessManager;
class QNetworkReply;

namespace SWGSDRangel {
    class SWGDeviceState;
}

class Map : public Feature
{
    Q_OBJECT
public:
    class MsgConfigureMap : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        const MapSettings& getSettings() const { return m_settings; }
        bool getForce() const { return m_force; }

        static MsgConfigureMap* create(const MapSettings& settings, bool force) {
            return new MsgConfigureMap(settings, force);
        }

    private:
        MapSettings m_settings;
        bool m_force;

        MsgConfigureMap(const MapSettings& settings, bool force) :
            Message(),
            m_settings(settings),
            m_force(force)
        { }
    };

    class MsgFind : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        QString getTarget() const { return m_target; }

        static MsgFind* create(const QString& target) {
            return new MsgFind(target);
        }

    private:
        QString m_target;

        MsgFind(const QString& target) :
            Message(),
            m_target(target)
        {}
    };

    class MsgSetDateTime : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        QDateTime getDateTime() const { return m_dateTime; }

        static MsgSetDateTime* create(const QDateTime& dateTime) {
            return new MsgSetDateTime(dateTime);
        }

    private:
        QDateTime m_dateTime;

        MsgSetDateTime(const QDateTime& dateTime) :
            Message(),
            m_dateTime(dateTime)
        {}
    };

    class MsgReportAvailableChannelOrFeatures : public Message {
        MESSAGE_CLASS_DECLARATION

    public:
        QList<MapSettings::AvailableChannelOrFeature>& getItems() { return m_availableChannelOrFeatures; }

        static MsgReportAvailableChannelOrFeatures* create() {
            return new MsgReportAvailableChannelOrFeatures();
        }

    private:
        QList<MapSettings::AvailableChannelOrFeature> m_availableChannelOrFeatures;

        MsgReportAvailableChannelOrFeatures() :
            Message()
        {}
    };

    Map(WebAPIAdapterInterface *webAPIAdapterInterface);
    virtual ~Map();
    virtual void destroy() { delete this; }
    virtual bool handleMessage(const Message& cmd);

    virtual void getIdentifier(QString& id) const { id = objectName(); }
    virtual QString getIdentifier() const { return objectName(); }
    virtual void getTitle(QString& title) const { title = m_settings.m_title; }

    virtual QByteArray serialize() const;
    virtual bool deserialize(const QByteArray& data);

    virtual int webapiRun(bool run,
            SWGSDRangel::SWGDeviceState& response,
            QString& errorMessage);

    virtual int webapiSettingsGet(
            SWGSDRangel::SWGFeatureSettings& response,
            QString& errorMessage);

    virtual int webapiSettingsPutPatch(
            bool force,
            const QStringList& featureSettingsKeys,
            SWGSDRangel::SWGFeatureSettings& response,
            QString& errorMessage);

    virtual int webapiReportGet(
            SWGSDRangel::SWGFeatureReport& response,
            QString& errorMessage);

    virtual int webapiActionsPost(
            const QStringList& featureActionsKeys,
            SWGSDRangel::SWGFeatureActions& query,
            QString& errorMessage);

    static void webapiFormatFeatureSettings(
        SWGSDRangel::SWGFeatureSettings& response,
        const MapSettings& settings);

    static void webapiUpdateFeatureSettings(
            MapSettings& settings,
            const QStringList& featureSettingsKeys,
            SWGSDRangel::SWGFeatureSettings& response);

    void setMapDateTime(QDateTime mapDateTime, QDateTime systemDateTime, double multiplier);
    QDateTime getMapDateTime();

    static const char* const m_featureIdURI;
    static const char* const m_featureId;

private:
    QThread m_thread;
    MapSettings m_settings;
    QHash<QObject*, MapSettings::AvailableChannelOrFeature> m_availableChannelOrFeatures;

    QNetworkAccessManager *m_networkManager;
    QNetworkRequest m_networkRequest;

    void applySettings(const MapSettings& settings, bool force = false);
    void webapiFormatFeatureReport(SWGSDRangel::SWGFeatureReport& response);
    void webapiReverseSendSettings(QList<QString>& featureSettingsKeys, const MapSettings& settings, bool force);
    void registerPipe(QObject *object);
    void notifyUpdate();

    QDateTime m_mapDateTime;
    QDateTime m_systemDateTime;
    double m_multiplier;
    QMutex m_dateTimeMutex;

private slots:
    void networkManagerFinished(QNetworkReply *reply);
    void scanAvailableChannelsAndFeatures();
    void handleFeatureAdded(int featureSetIndex, Feature *feature);
    void handleChannelAdded(int deviceSetIndex, ChannelAPI *channel);
    void handleMessagePipeToBeDeleted(int reason, QObject* object);
    void handlePipeMessageQueue(MessageQueue* messageQueue);
};

#endif // INCLUDE_FEATURE_MAP_H_
