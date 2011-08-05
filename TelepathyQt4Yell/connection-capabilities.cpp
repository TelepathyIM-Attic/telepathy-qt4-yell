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
#include <TelepathyQt4Yell/ConnectionCapabilities>
#include <TelepathyQt4Yell/RequestableChannelClassSpec>

namespace Tpy
{

/**
 * \class ConnectionCapabilities
 * \ingroup clientconn
 * \headerfile TelepathyQt4Yell/connection-capabilities.h <TelepathyQt4Yell/ConnectionCapabilities>
 *
 * \brief The ConnectionCapabilities class provides an object representing the
 * capabilities of a Connection.
 */

/**
 * Construct a new ConnectionCapabilities object.
 */
ConnectionCapabilities::ConnectionCapabilities()
    : Tp::ConnectionCapabilities()
{
}

/**
 * Construct a new ConnectionCapabilities object using the give \a rccs.
 *
 * \param rccs RequestableChannelClassList representing the capabilities of a
 *             contact.
 */
ConnectionCapabilities::ConnectionCapabilities(const Tp::RequestableChannelClassList &rccs)
    : Tp::ConnectionCapabilities(rccs)
{
}

/**
 * Construct a new ConnectionCapabilities object using the give \a rccSpecs.
 *
 * \param rccSpecs RequestableChannelClassList representing the capabilities of a
 *                 contact.
 */
ConnectionCapabilities::ConnectionCapabilities(const Tp::RequestableChannelClassSpecList &rccSpecs)
    : Tp::ConnectionCapabilities(rccSpecs)
{
}

/**
 * Construct a new ConnectionCapabilities object using the other \a rccSpecs.
 *
 * \param rccSpecs RequestableChannelClassList representing the capabilities of a
 *                 contact.
 */
ConnectionCapabilities::ConnectionCapabilities(const Tp::ConnectionCapabilities &other)
    : Tp::ConnectionCapabilities(other.allClassSpecs())
{
}

/**
 * Class destructor.
 */
ConnectionCapabilities::~ConnectionCapabilities()
{
}

bool ConnectionCapabilities::mediaCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.supports(Tpy::RequestableChannelClassSpec::mediaCall())) {
            return true;
        }
    }
    return false;
}

bool ConnectionCapabilities::audioCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.supports(Tpy::RequestableChannelClassSpec::audioCallAllowed()) ||
            rccSpec.supports(Tpy::RequestableChannelClassSpec::audioCallFixed()) ) {
            return true;
        }
    }
    return false;
}

bool ConnectionCapabilities::videoCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.supports(Tpy::RequestableChannelClassSpec::videoCallAllowed()) ||
            rccSpec.supports(Tpy::RequestableChannelClassSpec::videoCallFixed())) {
            return true;
        }
    }
    return false;
}

bool ConnectionCapabilities::videoCallsWithAudio() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.supports(Tpy::RequestableChannelClassSpec::videoCallAllowedWithAudioAllowed()) ||
            rccSpec.supports(Tpy::RequestableChannelClassSpec::videoCallAllowedWithAudioFixed()) ||
            rccSpec.supports(Tpy::RequestableChannelClassSpec::videoCallFixedWithAudioAllowed()) ||
            rccSpec.supports(Tpy::RequestableChannelClassSpec::videoCallFixedWithAudioFixed())) {
            return true;
        }
    }
    return false;
}

bool ConnectionCapabilities::upgradingCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.channelType() == TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL &&
            rccSpec.allowsProperty(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".MutableContents"))) {
            return true;
        }
    }
    return false;
}

bool ConnectionCapabilities::fileTransfers() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.supports(Tp::RequestableChannelClassSpec::fileTransfer())) {
            return true;
        }
    }
    return false;
}

} // Tpy
