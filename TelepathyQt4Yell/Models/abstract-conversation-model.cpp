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

#include <TelepathyQt4Yell/Models/AbstractConversationModel>

#include "TelepathyQt4Yell/Models/_gen/abstract-conversation-model.moc.hpp"

#include <TelepathyQt4Yell/Models/ConversationItem>

#include <TelepathyQt4/AvatarData>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ReceivedMessage>

#include <QPixmap>
#include <QtAlgorithms>

namespace Tpy
{

struct TELEPATHY_QT4_YELL_MODELS_NO_EXPORT AbstractConversationModel::Private
{
    Private()
    {
    }

    QList<const ConversationItem *> mItems;
};

AbstractConversationModel::AbstractConversationModel(QObject *parent)
    : QAbstractListModel(parent),
      mPriv(new Private())
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    roles[ContactRole] = "contact";
    roles[ContactAvatarRole] = "contactAvatar";
    roles[TimeRole] = "time";
    roles[TypeRole] = "type";
    roles[ItemRole] = "item";
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

    const ConversationItem *item = mPriv->mItems[index.row()];
    switch (role) {
    case TextRole:
        return item->text();
    case ContactRole:
        return item->contact()->alias();
    case ContactAvatarRole:
        return item->contact()->avatarData().fileName;
    case TimeRole:
        return item->time();
    case TypeRole:
        switch (item->type()) {
        case ConversationItem::INCOMING_MESSAGE:
            return QString::fromLatin1("incoming_message");
        case ConversationItem::OUTGOING_MESSAGE:
            return QString::fromLatin1("outgoing_message");
        case ConversationItem::EVENT:
            return QString::fromLatin1("event");
        default:
            return QString();
        }
    case ItemRole:
        return QVariant::fromValue(
                        const_cast<QObject *>(
                        static_cast<const QObject *>(item)));
    default:
        return QVariant();
    }

}

int AbstractConversationModel::rowCount(const QModelIndex &parent) const
{
    return mPriv->mItems.count();
}

void AbstractConversationModel::addItem(const ConversationItem *item)
{
    beginInsertRows(QModelIndex(), mPriv->mItems.count(), mPriv->mItems.count());
    mPriv->mItems.append(item);
    endInsertRows();
}

bool AbstractConversationModel::deleteItem(const ConversationItem *item)
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

QModelIndex AbstractConversationModel::index(const ConversationItem *item) const
{
    int num = mPriv->mItems.indexOf(item);
    if (num != -1) {
        return QAbstractListModel::index(num);
    }

    return QModelIndex();
}

void AbstractConversationModel::insertItems(QList<const ConversationItem *> items, int index)
{
    beginInsertRows(QModelIndex(), index, index + items.count() - 1);
    const Tpy::ConversationItem *item;
    int i = 0;
    foreach(item, items) {
        mPriv->mItems.insert(index + i++, item);
    }
    endInsertRows();
}

}
