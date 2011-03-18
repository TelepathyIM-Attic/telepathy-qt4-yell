/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2010 Collabora Ltd. <http://www.collabora.co.uk/>
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

#include <TelepathyQt4Yell/Models/ContactModelItem>

#include "TelepathyQt4Yell/Models/_gen/contact-model-item.moc.hpp"

#include <TelepathyQt4Yell/Models/AccountsModel>
#include <TelepathyQt4Yell/CallChannel>

#include <TelepathyQt4/AvatarData>
#include <TelepathyQt4/ContactCapabilities>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/RequestableChannelClassSpec>
#include <TelepathyQt4/ContactCapabilities>

#include <QImage>

namespace {

class CallRequestableChannelClassSpec : public Tp::RequestableChannelClassSpec {
public:
    static Tp::RequestableChannelClassSpec call();
    static Tp::RequestableChannelClassSpec audioCallAllowed();
    static Tp::RequestableChannelClassSpec audioCallFixed();
    static Tp::RequestableChannelClassSpec videoCallAllowed();
    static Tp::RequestableChannelClassSpec videoCallFixed();
    static Tp::RequestableChannelClassSpec videoCallAllowedWithAudioAllowed();
    static Tp::RequestableChannelClassSpec videoCallAllowedWithAudioFixed();
    static Tp::RequestableChannelClassSpec videoCallFixedWithAudioAllowed();
    static Tp::RequestableChannelClassSpec videoCallFixedWithAudioFixed();
};

class CallContactCapabilities {
public:
    bool calls() const;
    bool audioCalls() const;
    bool videoCalls() const;
    bool videoCallsWithAudio() const;
    bool upgradingCalls() const;

public:
    CallContactCapabilities(bool specificToContact);
    CallContactCapabilities(const Tp::RequestableChannelClassSpecList &rccs,
            bool specificToContact);

    Tp::RequestableChannelClassSpecList rccSpecs;
    bool specificToContact;
};

CallContactCapabilities::CallContactCapabilities(bool pSpecificToContact)
    : specificToContact(pSpecificToContact)
{
}

CallContactCapabilities::CallContactCapabilities(
        const Tp::RequestableChannelClassSpecList &pRccSpecs,
        bool pSpecificToContact)
    : rccSpecs(pRccSpecs),
      specificToContact(pSpecificToContact)
{
}

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::call()
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

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::audioCallAllowed()
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

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::audioCallFixed()
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

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::videoCallFixed()
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

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::videoCallAllowed()
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

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::videoCallAllowedWithAudioAllowed()
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

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::videoCallAllowedWithAudioFixed()
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

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::videoCallFixedWithAudioAllowed()
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

Tp::RequestableChannelClassSpec CallRequestableChannelClassSpec::videoCallFixedWithAudioFixed()
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

bool CallContactCapabilities::calls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, rccSpecs) {
        if (rccSpec.supports(CallRequestableChannelClassSpec::call())) {
            return true;
        }
    }
    return false;
}

bool CallContactCapabilities::audioCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, rccSpecs) {
        if (rccSpec.supports(CallRequestableChannelClassSpec::audioCallAllowed()) ||
            rccSpec.supports(CallRequestableChannelClassSpec::audioCallFixed()) ) {
            return true;
        }
    }
    return false;
}

bool CallContactCapabilities::videoCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, rccSpecs) {
        if (rccSpec.supports(CallRequestableChannelClassSpec::videoCallAllowed()) ||
            rccSpec.supports(CallRequestableChannelClassSpec::videoCallFixed())) {
            return true;
        }
    }
    return false;
}

bool CallContactCapabilities::videoCallsWithAudio() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, rccSpecs) {
        if (rccSpec.supports(CallRequestableChannelClassSpec::videoCallAllowedWithAudioAllowed()) ||
            rccSpec.supports(CallRequestableChannelClassSpec::videoCallAllowedWithAudioFixed()) ||
            rccSpec.supports(CallRequestableChannelClassSpec::videoCallFixedWithAudioAllowed()) ||
            rccSpec.supports(CallRequestableChannelClassSpec::videoCallFixedWithAudioFixed())) {
            return true;
        }
    }
    return false;
}

bool CallContactCapabilities::upgradingCalls() const
{
    foreach (const Tp::RequestableChannelClassSpec &rccSpec, rccSpecs) {
        if (rccSpec.channelType() == TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL &&
            rccSpec.allowsProperty(TP_QT4_YELL_IFACE_CHANNEL_TYPE_CALL + QLatin1String(".MutableContents"))) {
            return true;
        }
    }
    return false;
}

}

namespace Tpy
{

struct TELEPATHY_QT4_YELL_MODELS_NO_EXPORT ContactModelItem::Private
{
    Private(const Tp::ContactPtr &contact)
        : mContact(contact),
          mCallContactCaps(contact->capabilities().allClassSpecs(),
              contact->capabilities().isSpecificToContact())
    {
    }

    Tp::ContactPtr mContact;
    CallContactCapabilities mCallContactCaps;
};

ContactModelItem::ContactModelItem(const Tp::ContactPtr &contact)
    : mPriv(new Private(contact))
{

    connect(contact.data(),
            SIGNAL(aliasChanged(QString)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(avatarTokenChanged(QString)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(avatarDataChanged(Tp::AvatarData)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(presenceChanged(Tp::Presence)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(capabilitiesChanged(Tp::ContactCapabilities)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(locationUpdated(Tp::LocationInfo)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(infoFieldsChanged(Tp::Contact::InfoFields)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(subscriptionStateChanged(Tp::Contact::PresenceState)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(publishStateChanged(Tp::Contact::PresenceState,QString)),
            SLOT(onChanged()));
    connect(contact.data(),
            SIGNAL(blockStatusChanged(bool)),
            SLOT(onChanged()));
}

ContactModelItem::~ContactModelItem()
{
    delete mPriv;
}

QVariant ContactModelItem::data(int role) const
{
    switch (role)
    {
        case AccountsModel::ItemRole:
            return QVariant::fromValue(
                const_cast<QObject *>(
                    static_cast<const QObject *>(this)));
        case AccountsModel::IdRole:
            return mPriv->mContact->id();
        case Qt::DisplayRole:
        case AccountsModel::AliasRole:
            return mPriv->mContact->alias();
        case AccountsModel::PresenceStatusRole:
            return mPriv->mContact->presence().status();
        case AccountsModel::PresenceTypeRole:
            return mPriv->mContact->presence().type();
        case AccountsModel::PresenceMessageRole:
            return mPriv->mContact->presence().statusMessage();
        case AccountsModel::SubscriptionStateRole:
            return mPriv->mContact->subscriptionState();
        case AccountsModel::PublishStateRole:
            return mPriv->mContact->publishState();
        case AccountsModel::BlockedRole:
            return mPriv->mContact->isBlocked();
        case AccountsModel::GroupsRole:
            return mPriv->mContact->groups();
        case AccountsModel::AvatarRole:
            return mPriv->mContact->avatarData().fileName;
        case Qt::DecorationRole:
            return QImage(mPriv->mContact->avatarData().fileName);
        case AccountsModel::TextChatCapabilityRole:
            return mPriv->mContact->capabilities().textChats();
        case AccountsModel::StreamedMediaCallCapabilityRole:
            return mPriv->mContact->capabilities().streamedMediaCalls();
        case AccountsModel::StreamedMediaAudioCallCapabilityRole:
            return mPriv->mContact->capabilities().streamedMediaAudioCalls();
        case AccountsModel::StreamedMediaVideoCallCapabilityRole:
            return mPriv->mContact->capabilities().streamedMediaVideoCalls();
        case AccountsModel::StreamedMediaVideoCallWithAudioCapabilityRole:
            return mPriv->mContact->capabilities().streamedMediaVideoCallsWithAudio();
        case AccountsModel::StreamedMediaUpgradeCallCapabilityRole:
            return mPriv->mContact->capabilities().upgradingStreamedMediaCalls();
        case AccountsModel::CallCapabilityRole:
            return mPriv->mCallContactCaps.calls();
        case AccountsModel::AudioCallCapabilityRole:
            return mPriv->mCallContactCaps.audioCalls();
        case AccountsModel::VideoCallCapabilityRole:
            return mPriv->mCallContactCaps.videoCalls();
        case AccountsModel::VideoCallWithAudioCapabilityRole:
            return mPriv->mCallContactCaps.videoCallsWithAudio();
        case AccountsModel::UpgradeCallCapabilityRole:
            return mPriv->mCallContactCaps.upgradingCalls();
        case AccountsModel::FileTransferCapabilityRole: {
            foreach (const Tp::RequestableChannelClassSpec &rccSpec, mPriv->mContact->capabilities().allClassSpecs()) {
                if (rccSpec.supports(Tp::RequestableChannelClassSpec::fileTransfer())) {
                    return true;
                }
            }
            return false;
        }
        default:
            break;
    }

    return QVariant();
}

bool ContactModelItem::setData(int role, const QVariant &value)
{
    switch (role) {
        case AccountsModel::PublishStateRole: {
            Tp::Contact::PresenceState state;
            state = (Tp::Contact::PresenceState) value.toInt();
            switch (state) {
                case Tp::Contact::PresenceStateYes:
                    // authorize the contact and request its presence publication
                    mPriv->mContact->authorizePresencePublication();
                    mPriv->mContact->requestPresenceSubscription();
                    return true;
                case Tp::Contact::PresenceStateNo: {
                    // reject the presence publication and remove the contact
                    mPriv->mContact->removePresencePublication();
                    QList<Tp::ContactPtr> contacts;
                    contacts << mPriv->mContact;
                    mPriv->mContact->manager()->removeContacts(contacts);
                    return true;
                }
                default:
                    return false;
            }
        }
        default:
            return false;
    }
}

void ContactModelItem::onChanged()
{
    emit changed(this);
}

Tp::ContactPtr ContactModelItem::contact() const
{
    return mPriv->mContact;
}

}
