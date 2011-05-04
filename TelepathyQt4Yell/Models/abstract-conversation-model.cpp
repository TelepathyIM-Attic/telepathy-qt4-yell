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

#include <TelepathyQt4Yell/Models/AbstractConversationModel>

#include "TelepathyQt4Yell/Models/_gen/abstract-conversation-model.moc.hpp"

#include <TelepathyQt4Yell/Models/CallEventItem>
#include <TelepathyQt4Yell/Models/CustomEventItem>
#include <TelepathyQt4Yell/Models/TextEventItem>

#include <TelepathyQt4/AvatarData>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ReceivedMessage>

#include <QPixmap>
#include <QtAlgorithms>
#include <QMetaObject>

namespace Tpy
{

struct TELEPATHY_QT4_YELL_MODELS_NO_EXPORT AbstractConversationModel::Private
{
    Private()
    {
    }

    QList<const EventItem *> mItems;
};

AbstractConversationModel::AbstractConversationModel(QObject *parent)
    : QAbstractListModel(parent),
      mPriv(new Private())
{
    QHash<int, QByteArray> roles;
    roles[EventTypeRole] = "eventType";
    roles[SenderRole] = "sender";
    roles[SenderAvatarRole] = "senderAvatar";
    roles[ReceiverRole] = "receiver";
    roles[ReceiverAvatarRole] = "receiverAvatar";
    roles[DateTimeRole] = "dateTime";
    roles[ItemRole] = "item";
    roles[MessageTextRole] = "messageText";
    roles[MessageTypeRole] = "messageType";
    roles[CallDurationRole] = "callDuration";
    roles[CallEndActorRole] = "callEndActor";
    roles[CallEndActorAvatarRole] = "callEndActorAvatar";
    roles[CallEndReasonRole] = "callEndReason";
    roles[CallDetailedEndReasonRole] = "callDetailedEndReason";
    roles[MissedCallRole] = "missedCall";
    roles[RejectedCallRole] = "rejectedCall";
    roles[CustomEventTextRole] = "customEventText";
    roles[CustomEventTypeRole] = "customEventType";
    setRoleNames(roles);
}

AbstractConversationModel::~AbstractConversationModel()
{
    qDeleteAll(mPriv->mItems);
    mPriv->mItems.clear();
    delete mPriv;
}

QVariant AbstractConversationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= mPriv->mItems.count()) {
        return QVariant();
    }

    const EventItem *item = mPriv->mItems[index.row()];
    if (!item) {
        return QVariant();
    }

    switch (role) {
    case EventTypeRole:
        return QString::fromLatin1(item->metaObject()->className());
    case SenderRole:
        if (!item->sender().isNull()) {
            return item->sender()->alias();
        }
        return QVariant();
    case SenderAvatarRole:
        if (!item->sender().isNull()) {
            return item->sender()->avatarData().fileName;
        }
        return QVariant();
    case ReceiverRole:
        if (!item->receiver().isNull()) {
            return item->receiver()->alias();
        }
        return QVariant();
    case ReceiverAvatarRole:
        if (!item->receiver().isNull()) {
            return item->receiver()->avatarData().fileName;
        }
        return QVariant();
    case DateTimeRole:
        return item->dateTime();
    case ItemRole:
        return QVariant::fromValue(
                        const_cast<QObject *>(
                        static_cast<const QObject *>(item)));
    case MessageTextRole: {
        const TextEventItem *textEvent = qobject_cast<const TextEventItem*> (item);
        if (textEvent) {
            return textEvent->messageText();
        }
        return QVariant();
    }
    case MessageTypeRole: {
        const TextEventItem *textEvent = qobject_cast<const TextEventItem*> (item);
        if (textEvent) {
            return textEvent->messageType();
        }
        return QVariant();
    }
    case CallDurationRole: {
        const CallEventItem *callEvent = qobject_cast<const CallEventItem*> (item);
        if (callEvent) {
            return callEvent->duration();
        }
        return QVariant();
    }
    case CallEndActorRole: {
        const CallEventItem *callEvent = qobject_cast<const CallEventItem*> (item);
        if (callEvent && !callEvent->endActor().isNull()) {
            return callEvent->endActor()->alias();
        }
        return QVariant();
    }
    case CallEndActorAvatarRole: {
        const CallEventItem *callEvent = qobject_cast<const CallEventItem*> (item);
        if (callEvent && !callEvent->endActor().isNull()) {
            return callEvent->endActor()->avatarData().fileName;
        }
        return QVariant();
    }
    case CallEndReasonRole: {
        const CallEventItem *callEvent = qobject_cast<const CallEventItem*> (item);
        if (callEvent) {
            return callEvent->endReason();
        }
        return QVariant();
    }
    case CallDetailedEndReasonRole: {
        const CallEventItem *callEvent = qobject_cast<const CallEventItem*> (item);
        if (callEvent) {
            return callEvent->detailedEndReason();
        }
        return QVariant();
    }
    case MissedCallRole: {
        const CallEventItem *callEvent = qobject_cast<const CallEventItem*> (item);
        if (callEvent) {
            return callEvent->missedCall();
        }
        return QVariant();
    }
    case RejectedCallRole: {
        const CallEventItem *callEvent = qobject_cast<const CallEventItem*> (item);
        if (callEvent) {
            return callEvent->rejectedCall();
        }
        return QVariant();
    }
    case CustomEventTextRole: {
        const CustomEventItem *customEvent = qobject_cast<const CustomEventItem*> (item);
        if (customEvent) {
            return customEvent->customEventText();
        }
        return QVariant();
    }
    case CustomEventTypeRole: {
        const CustomEventItem *customEvent = qobject_cast<const CustomEventItem*> (item);
        if (customEvent) {
            return customEvent->customEventType();
        }
        return QVariant();
    }
    default:
        return QVariant();
    }
}

int AbstractConversationModel::rowCount(const QModelIndex &parent) const
{
    return mPriv->mItems.count();
}

void AbstractConversationModel::addItem(const EventItem *item)
{
    beginInsertRows(QModelIndex(), mPriv->mItems.count(), mPriv->mItems.count());
    mPriv->mItems.append(item);
    endInsertRows();
}

bool AbstractConversationModel::deleteItem(const EventItem *item)
{
    int num = mPriv->mItems.indexOf(item);
    if (num != -1) {
        beginRemoveRows(QModelIndex(), num, num);
        mPriv->mItems.removeAt(num);
        endRemoveRows();
        return true;
    }

    return false;
}

QModelIndex AbstractConversationModel::index(const EventItem *item) const
{
    int num = mPriv->mItems.indexOf(item);
    if (num != -1) {
        return QAbstractListModel::index(num);
    }

    return QModelIndex();
}

void AbstractConversationModel::insertItems(QList<const EventItem *> items, int index)
{
    beginInsertRows(QModelIndex(), index, index + items.count() - 1);
    const Tpy::EventItem *item;
    int i = 0;
    foreach (item, items) {
        mPriv->mItems.insert(index + i++, item);
    }
    endInsertRows();
}

bool AbstractConversationModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (row >= 0 && count > 0 && (row + count) <= mPriv->mItems.count()) {
        beginRemoveRows(parent, row, row + count - 1);
        while (count-- > 0) {
            mPriv->mItems.removeAt(row);
        }
        endRemoveRows();
        return true;
    }
    return false;
}

}
