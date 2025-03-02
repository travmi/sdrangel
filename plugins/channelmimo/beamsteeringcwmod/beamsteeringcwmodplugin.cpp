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


#include <QtPlugin>
#include "plugin/pluginapi.h"

#ifndef SERVER_MODE
#include "beamsteeringcwmodgui.h"
#endif
#include "beamsteeringcwmod.h"
#include "beamsteeringcwmodwebapiadapter.h"
#include "beamsteeringcwmodplugin.h"

const PluginDescriptor BeamSteeringCWModPlugin::m_pluginDescriptor = {
    BeamSteeringCWMod::m_channelId,
    QStringLiteral("BeamSteeringCWMod"),
    QStringLiteral("7.3.0"),
    QStringLiteral("(c) Edouard Griffiths, F4EXB"),
    QStringLiteral("https://github.com/f4exb/sdrangel"),
    true,
    QStringLiteral("https://github.com/f4exb/sdrangel")
};

BeamSteeringCWModPlugin::BeamSteeringCWModPlugin(QObject* parent) :
    QObject(parent),
    m_pluginAPI(nullptr)
{
}

const PluginDescriptor& BeamSteeringCWModPlugin::getPluginDescriptor() const
{
    return m_pluginDescriptor;
}

void BeamSteeringCWModPlugin::initPlugin(PluginAPI* pluginAPI)
{
    m_pluginAPI = pluginAPI;

    // register channel MIMO
    m_pluginAPI->registerMIMOChannel(BeamSteeringCWMod::m_channelIdURI, BeamSteeringCWMod::m_channelId, this);
}

void BeamSteeringCWModPlugin::createMIMOChannel(DeviceAPI *deviceAPI, MIMOChannel **bs, ChannelAPI **cs) const
{
	if (bs || cs)
	{
		BeamSteeringCWMod *instance = new BeamSteeringCWMod(deviceAPI);

		if (bs) {
			*bs = instance;
		}

		if (cs) {
			*cs = instance;
		}
	}
}

#ifdef SERVER_MODE
ChannelGUI* BeamSteeringCWModPlugin::createMIMOChannelGUI(
        DeviceUISet *deviceUISet,
        MIMOChannel *mimoChannel) const
{
    (void) deviceUISet;
    (void) mimoChannel;
    return nullptr;
}
#else
ChannelGUI* BeamSteeringCWModPlugin::createMIMOChannelGUI(DeviceUISet *deviceUISet, MIMOChannel *mimoChannel) const
{
    return BeamSteeringCWModGUI::create(m_pluginAPI, deviceUISet, mimoChannel);
}
#endif

ChannelWebAPIAdapter* BeamSteeringCWModPlugin::createChannelWebAPIAdapter() const
{
	return new BeamSteeringCWModWebAPIAdapter();
}
