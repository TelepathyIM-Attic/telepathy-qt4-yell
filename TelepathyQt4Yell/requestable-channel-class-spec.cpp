/**
 * This file is part of TelepathyQt4Yell
 *
 * @copyright Copyright (C) 2011 Collabora Ltd. <http://www.collabora.co.uk/>
 * @license LGPL 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <TelepathyQt4Yell/Constants>
#include <TelepathyQt4Yell/RequestableChannelClassSpec>

namespace Tpy
{

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::mediaCall()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::audioCallAllowed()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        rcc.allowedProperties.append(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialAudio"));
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::audioCallFixed()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        rcc.fixedProperties.insert(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialAudio"),
                true);
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::videoCallFixed()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        rcc.fixedProperties.insert(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialVideo"),
                true);
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::videoCallAllowed()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        rcc.allowedProperties.append(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialVideo"));
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::videoCallAllowedWithAudioAllowed()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        rcc.allowedProperties.append(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialAudio"));
        rcc.allowedProperties.append(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialVideo"));
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::videoCallAllowedWithAudioFixed()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        rcc.fixedProperties.insert(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialAudio"),
                true);
        rcc.allowedProperties.append(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialVideo"));
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::videoCallFixedWithAudioAllowed()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        rcc.fixedProperties.insert(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialVideo"),
                true);
        rcc.allowedProperties.append(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialAudio"));
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

Tp::RequestableChannelClassSpec RequestableChannelClassSpec::videoCallFixedWithAudioFixed()
{
    static Tp::RequestableChannelClassSpec spec;

    if (!spec.isValid()) {
        Tp::RequestableChannelClass rcc;
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".ChannelType"),
                TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL);
        rcc.fixedProperties.insert(TP_QT4_IFACE_CHANNEL + QLatin1String(".TargetHandleType"),
                (uint) Tp::HandleTypeContact);
        rcc.fixedProperties.insert(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialVideo"),
                true);
        rcc.fixedProperties.insert(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".InitialAudio"),
                true);
        spec = Tp::RequestableChannelClassSpec(rcc);
    }

    return spec;
}

} // Tpy
