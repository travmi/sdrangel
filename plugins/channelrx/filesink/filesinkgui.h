///////////////////////////////////////////////////////////////////////////////////
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

#ifndef PLUGINS_CHANNELRX_FILESINK_FILESINKGUI_H_
#define PLUGINS_CHANNELRX_FILESINK_FILESINKGUI_H_

#include <stdint.h>

#include <QObject>

#include "dsp/channelmarker.h"
#include "channel/channelgui.h"
#include "util/messagequeue.h"
#include "settings/rollupstate.h"

#include "filesinksettings.h"

class PluginAPI;
class DeviceUISet;
class FileSink;
class BasebandSampleSink;
class SpectrumVis;

namespace Ui {
    class FileSinkGUI;
}

class FileSinkGUI : public ChannelGUI {
    Q_OBJECT
public:
    static FileSinkGUI* create(PluginAPI* pluginAPI, DeviceUISet *deviceUISet, BasebandSampleSink *rxChannel);
    virtual void destroy();

    void resetToDefaults();
    QByteArray serialize() const;
    bool deserialize(const QByteArray& data);
    virtual MessageQueue *getInputMessageQueue() { return &m_inputMessageQueue; }
    virtual void setWorkspaceIndex(int index) { m_settings.m_workspaceIndex = index; };
    virtual int getWorkspaceIndex() const { return m_settings.m_workspaceIndex; };
    virtual void setGeometryBytes(const QByteArray& blob) { m_settings.m_geometryBytes = blob; };
    virtual QByteArray getGeometryBytes() const { return m_settings.m_geometryBytes; };
    virtual QString getTitle() const { return m_settings.m_title; };
    virtual QColor getTitleColor() const  { return m_settings.m_rgbColor; };
    virtual void zetHidden(bool hidden) { m_settings.m_hidden = hidden; }
    virtual bool getHidden() const { return m_settings.m_hidden; }
    virtual ChannelMarker& getChannelMarker() { return m_channelMarker; }
    virtual int getStreamIndex() const { return m_settings.m_streamIndex; }
    virtual void setStreamIndex(int streamIndex) { m_settings.m_streamIndex = streamIndex; }

public slots:
	void channelMarkerChangedByCursor();
	void channelMarkerHighlightedByCursor();

private:
    Ui::FileSinkGUI* ui;
    PluginAPI* m_pluginAPI;
    DeviceUISet* m_deviceUISet;
    ChannelMarker m_channelMarker;
    RollupState m_rollupState;
    FileSinkSettings m_settings;
    qint64 m_deviceCenterFrequency;
    bool m_running;
    int m_fixedShiftIndex;
    int m_basebandSampleRate;
    double m_shiftFrequencyFactor; //!< Channel frequency shift factor
    bool m_fixedPosition;
    bool m_doApplySettings;

    FileSink* m_fileSink;
    SpectrumVis* m_spectrumVis;
    MessageQueue m_inputMessageQueue;

    uint32_t m_tickCount;

    explicit FileSinkGUI(PluginAPI* pluginAPI, DeviceUISet *deviceUISet, BasebandSampleSink *rxChannel, QWidget* parent = nullptr);
    virtual ~FileSinkGUI();

    void blockApplySettings(bool block);
    void applySettings(bool force = false);
    void applyDecimation();
    void displaySettings();
    void displayRate();
    void displayPos();
    void setFrequencyFromPos();
    void setPosFromFrequency();
    QString displayScaled(uint64_t value, int precision);
    bool handleMessage(const Message& message);
    void makeUIConnections();
    void updateAbsoluteCenterFrequency();

    void leaveEvent(QEvent*);
    void enterEvent(QEvent*);

private slots:
    void handleSourceMessages();
    void on_deltaFrequency_changed(qint64 value);
    void on_decimationFactor_currentIndexChanged(int index);
    void on_fixedPosition_toggled(bool checked);
    void on_position_valueChanged(int value);
    void on_spectrumSquelch_toggled(bool checked);
    void on_squelchLevel_valueChanged(int value);
    void on_preRecordTime_valueChanged(int value);
    void on_postSquelchTime_valueChanged(int value);
    void on_squelchedRecording_toggled(bool checked);
    void on_record_toggled(bool checked);
    void on_showFileDialog_clicked(bool checked);
    void onWidgetRolled(QWidget* widget, bool rollDown);
    void onMenuDialogCalled(const QPoint& p);
    void tick();
};



#endif /* PLUGINS_CHANNELRX_FILESINK_FILESINKGUI_H_ */
