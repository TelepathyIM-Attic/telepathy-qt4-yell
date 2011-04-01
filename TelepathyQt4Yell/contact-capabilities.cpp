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
#include <TelepathyQt4Yell/ContactCapabilities>
#include <TelepathyQt4Yell/RequestableChannelClassSpec>

namespace Tpy
{

/**
 * \class ContactCapabilities
 * \ingroup clientconn
 * \headerfile TelepathyQt4Yell/contact-capabilities.h <TelepathyQt4Yell/ContactCapabilities>
 *
 * \brief The ContactCapabilities class provides an object representing the
 * capabilities of a Contact.
 */

/**
 * Construct a new ContactCapabilities object.
 */
ContactCapabilities::ContactCapabilities()
    : Tp::ContactCapabilities()
{
}

/**
 * Construct a new ContactCapabilities object.
 */
ContactCapabilities::ContactCapabilities(bool specificToContact)
    : Tp::ContactCapabilities(specificToContact)
{
}

/**
 * Construct a new ContactCapabilities object using the give \a rccs.
 *
 * \param rccs RequestableChannelClassList representing the capabilities of a
 *             contact.
 */
ContactCapabilities::ContactCapabilities(const Tp::RequestableChannelClassList &rccs,
        bool specificToContact)
    : Tp::ContactCapabilities(rccs, specificToContact)
{
}

/**
 * Construct a new ContactCapabilities object using the give \a rccSpecs.
 *
 * \param rccSpecs RequestableChannelClassList representing the capabilities of a
 *                 contact.
 */
ContactCapabilities::ContactCapabilities(const Tp::RequestableChannelClassSpecList &rccSpecs,
        bool specificToContact)
    : Tp::ContactCapabilities(rccSpecs, specificToContact)
{
}

/**
 * Class destructor.
 */
ContactCapabilities::~ContactCapabilities()
{
}

bool ContactCapabilities::mediaCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.supports(Tpy::RequestableChannelClassSpec::mediaCall())) {
            return true;
        }
    }
    return false;
}

bool ContactCapabilities::audioCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.supports(Tpy::RequestableChannelClassSpec::audioCallAllowed()) ||
            rccSpec.supports(Tpy::RequestableChannelClassSpec::audioCallFixed()) ) {
            return true;
        }
    }
    return false;
}

bool ContactCapabilities::videoCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.supports(Tpy::RequestableChannelClassSpec::videoCallAllowed()) ||
            rccSpec.supports(Tpy::RequestableChannelClassSpec::videoCallFixed())) {
            return true;
        }
    }
    return false;
}

bool ContactCapabilities::videoCallsWithAudio() const
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

bool ContactCapabilities::upgradingCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, allClassSpecs()) {
        if (rccSpec.channelType() == TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL &&
            rccSpec.allowsProperty(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".MutableContents"))) {
            return true;
        }
    }
    return false;
}

} // Tpy
