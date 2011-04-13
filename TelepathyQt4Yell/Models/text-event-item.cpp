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

#include <TelepathyQt4Yell/Models/TextEventItem>

#include "TelepathyQt4Yell/Models/_gen/text-event-item.moc.hpp"

namespace Tpy
{

struct TELEPATHY_QT4_YELL_MODELS_NO_EXPORT TextEventItem::Private
{
    Private(const QString &message, TextEventItem::MessageOrigin messageOrigin,
        Tp::ChannelTextMessageType messageType)
        : mMessage(message),
          mMessageOrigin(messageOrigin),
          mMessageType(messageType)
    {
    }

    QString mMessage;
    TextEventItem::MessageOrigin mMessageOrigin;
    Tp::ChannelTextMessageType mMessageType;
};

TextEventItem::TextEventItem(const Tp::ContactPtr &sender,
    const Tp::ContactPtr &receiver, const QDateTime &time,
    const QString &message, TextEventItem::MessageOrigin messageOrigin,
    Tp::ChannelTextMessageType messageType, QObject *parent)
    : EventItem(sender, receiver, time, parent),
      mPriv(new Private(message, messageOrigin, messageType))
{
}

TextEventItem::~TextEventItem()
{
    delete mPriv;
}

QString TextEventItem::message() const
{
    return mPriv->mMessage;
}

TextEventItem::MessageOrigin TextEventItem::messageOrigin() const
{
    return mPriv->mMessageOrigin;
}

Tp::ChannelTextMessageType TextEventItem::messageType() const
{
    return mPriv->mMessageType;
}

}
