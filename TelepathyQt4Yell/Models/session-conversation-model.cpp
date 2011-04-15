/*
 * This file is part of TelepathyQt4Yell Models
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

#include <TelepathyQt4Yell/Models/SessionConversationModel>

#include "TelepathyQt4Yell/Models/_gen/session-conversation-model.moc.hpp"

#include <TelepathyQt4Yell/Models/TextEventItem>
#include <TelepathyQt4Yell/Models/CustomEventItem>

#include <TelepathyQt4/AvatarData>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ReceivedMessage>

#include <QPixmap>
#include <QtAlgorithms>

namespace Tpy
{

struct TELEPATHY_QT4_YELL_MODELS_NO_EXPORT SessionConversationModel::Private
{
    Private(const Tp::ContactPtr &self, const Tp::TextChannelPtr &channel)
        : mSelf(self),
          mChannel(channel)
    {
    }

    Tp::ContactPtr mSelf;
    Tp::TextChannelPtr mChannel;
};

SessionConversationModel::SessionConversationModel(const Tp::ContactPtr &self, const Tp::TextChannelPtr &channel, QObject *parent)
    : AbstractConversationModel(parent),
      mPriv(new Private(self,channel))
{
    connect(mPriv->mChannel.data(),
            SIGNAL(chatStateChanged(Tp::ContactPtr,Tp::ChannelChatState)),
            SLOT(onChatStateChanged(Tp::ContactPtr,Tp::ChannelChatState)));
}

SessionConversationModel::~SessionConversationModel()
{
    delete mPriv;
}

void SessionConversationModel::sendMessage(const QString &text)
{
    // FIXME get receiver ?
    TextEventItem *item = new TextEventItem(mPriv->mSelf, Tp::ContactPtr(),
        QDateTime::currentDateTime(), text, Tp::ChannelTextMessageTypeNormal, this);
    addItem(item);

    mPriv->mChannel->send(item->messageText());
}

Tp::ContactPtr SessionConversationModel::selfContact() const
{
    return mPriv->mSelf;
}

void SessionConversationModel::onMessageReceived(const Tp::ReceivedMessage &message)
{
    // TODO: For the moment skip if the message is a delivery report
    // Later they could be used to report status on sent messages
    if (message.messageType() != Tp::ChannelTextMessageTypeDeliveryReport) {
        TextEventItem *item = new TextEventItem(message.sender(), mPriv->mSelf,
            message.sent(), message.text(), message.messageType(), this);
        addItem(item);
    }
    mPriv->mChannel->acknowledge(QList<Tp::ReceivedMessage>() << message);
}

void SessionConversationModel::onChatStateChanged(const Tp::ContactPtr &contact, Tp::ChannelChatState state)
{
    // ignore events originating from self
    if (contact == mPriv->mSelf) {
        return;
    }

    if (state == Tp::ChannelChatStateGone) {
        QString message = QString::fromLatin1("%1 left the chat").arg(contact->alias());
        CustomEventItem *item = new CustomEventItem(contact, mPriv->mSelf,
            QDateTime::currentDateTime(), message, CustomEventItem::CustomEventUserLeftChat, this);
        addItem(item);
    }
}

/**
  * Disconnect the model from the channel messages queue so that messages on the queue will not
  * be acknowledged and entered into the model automatically
  */
void SessionConversationModel::disconnectChannelQueue()
{
    disconnect(mPriv->mChannel.data(), SIGNAL(messageReceived(Tp::ReceivedMessage)),
               this, SLOT(onMessageReceived(Tp::ReceivedMessage)));
}

/**
  * Reconnect the model to the channel queue and acknowledge messages on the queue
  */
void SessionConversationModel::connectChannelQueue()
{
    // reconnect the signal
    connect(mPriv->mChannel.data(),
                SIGNAL(messageReceived(Tp::ReceivedMessage)),
                SLOT(onMessageReceived(Tp::ReceivedMessage)));

    // flush the queue and enter all messages into the model
    // display messages already in queue
    foreach (Tp::ReceivedMessage message, mPriv->mChannel->messageQueue()) {
        onMessageReceived(message);
    }
}

}
