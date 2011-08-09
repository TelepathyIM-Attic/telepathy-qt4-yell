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

#include <TelepathyQt4Yell/Models/AvatarImageProvider>

#include <TelepathyQt4/Account>
#include <TelepathyQt4/AccountManager>

namespace Tpy
{

struct TELEPATHY_QT4_YELL_MODELS_NO_EXPORT AvatarImageProvider::Private
{
    Private(const Tp::AccountManagerPtr &am)
        : mAM(am)
    {
    }

    Tp::AccountManagerPtr mAM;
};

/**
 * \class AvatarImageProvider
 * \ingroup models
 * \headerfile TelepathyQt4Yell/avatar-image-provider.h <TelepathyQt4Yell/AvatarImageProvider>
 *
 * \brief This This class provides the avatars for Telepathy accounts and contacts in a suitable format for QML applications
 *
 */

/**
  * Construct an AvatarImageProvider object
  * \param am A valid AccountManager pointer
  */
AvatarImageProvider::AvatarImageProvider(const Tp::AccountManagerPtr &am)
    : QDeclarativeImageProvider(Image),
      mPriv(new Private(am))
{
}

AvatarImageProvider::~AvatarImageProvider()
{
    delete mPriv;
}

/**
  * Returns a URL for the avatar of a given account
  */
QString AvatarImageProvider::urlFor(const Tp::AccountPtr &account)
{
    return QString::fromLatin1("image://avatars/") + account->uniqueIdentifier();
}

/**
  * Create an instance and register it to serve as a provider in QDeclarative (QML)
  * \param engine The application QtDeclarative engine
  * \param am A valid AccountManager pointer
  */
void AvatarImageProvider::registerProvider(QDeclarativeEngine *engine, const Tp::AccountManagerPtr &am)
{
    engine->addImageProvider(QString::fromLatin1("avatars"), new AvatarImageProvider(am));
}

/**
  * Returns the avatar image for a given account id
  * \param id An account id
  * \param size If a valid pointer, it will be set to the image size
  * \param requestedSize This is not implemented
  */
QImage AvatarImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString path = QString::fromLatin1(TELEPATHY_ACCOUNT_OBJECT_PATH_BASE "/") + id;
    Tp::AccountPtr account = mPriv->mAM->accountForPath(path);
    QImage image;
    image.loadFromData(account->avatar().avatarData);
    if (size) {
        *size = image.size();
    }
    return image;
}

}
