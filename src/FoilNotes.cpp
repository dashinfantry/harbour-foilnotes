/*
 * Copyright (C) 2018-2019 Jolla Ltd.
 * Copyright (C) 2018-2019 Slava Monich <slava@monich.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "FoilNotes.h"

#include "HarbourDebug.h"

#include <QQmlEngine>
#include <QFileSystemWatcher>

#include <ctype.h>

#define FOILNOTES_MAX_FILE_NAME (8)

#define FOILPICS_DIR    "/usr/bin"
#define FOILPICS_PATH   FOILPICS_DIR "/harbour-foilpics"

// ==========================================================================
// FoilNotes::Private
// ==========================================================================

class FoilNotes::Private : public QObject {
    Q_OBJECT

public:
    Private(FoilNotes* aParent);

    static bool foilPicsInstalled();
    FoilNotes* foilNotes() const;

public Q_SLOTS:
    void checkFoilPicsInstalled();

public:
    QFileSystemWatcher* iFileWatcher;
    bool iFoilPicsInstalled;
};

FoilNotes::Private::Private(FoilNotes* aParent) :
    QObject(aParent),
    iFileWatcher(new QFileSystemWatcher(this))
{
    connect(iFileWatcher, SIGNAL(fileChanged(QString)),
            SLOT(checkFoilPicsInstalled()));
    connect(iFileWatcher, SIGNAL(directoryChanged(QString)),
        SLOT(checkFoilPicsInstalled()));
    iFileWatcher->addPath(FOILPICS_DIR);
    iFileWatcher->addPath(FOILPICS_PATH);
    iFoilPicsInstalled = foilPicsInstalled();
}

inline FoilNotes* FoilNotes::Private::foilNotes() const
{
    return qobject_cast<FoilNotes*>(parent());
}

bool FoilNotes::Private::foilPicsInstalled()
{
    bool installed = QFile::exists(FOILPICS_PATH);
    HDEBUG("FoilPics is" << (installed ? "installed" : "not installed"));
    return installed;
}

void FoilNotes::Private::checkFoilPicsInstalled()
{
    const bool installed = foilPicsInstalled();
    if (iFoilPicsInstalled != installed) {
        iFoilPicsInstalled = installed;
        Q_EMIT foilNotes()->foilPicsInstalledChanged();
    }
}

// ==========================================================================
// FoilNotes
// ==========================================================================

FoilNotes::FoilNotes(QObject* aParent) :
    QObject(aParent),
    iPrivate(new Private(this))
{
}

// Callback for qmlRegisterSingletonType<FoilNotes>
QObject* FoilNotes::createSingleton(QQmlEngine* aEngine, QJSEngine* aScript)
{
    return new FoilNotes(aEngine);
}

QString FoilNotes::generateFileName(QString aText)
{
    char name[FOILNOTES_MAX_FILE_NAME + 1];
    QByteArray latin1 = aText.toLatin1();
    const char* latin1chars = latin1.constData();
    int i, len = 0, n = latin1.size();
    for (i = 0; i < n && len < FOILNOTES_MAX_FILE_NAME; i++) {
        if (isalnum(latin1chars[i])) {
            name[len++] = latin1chars[i];
        }
    }
    if (len == FOILNOTES_MAX_FILE_NAME) {
        name[len] = 0;
        QString str = QString::fromLatin1(name);
        HDEBUG(aText << "->" << str);
        return str;
    } else {
        return qtTrId("foilnotes-app_name");
    }
}

bool FoilNotes::foilPicsInstalled() const
{
    return iPrivate->iFoilPicsInstalled;
}

#include "FoilNotes.moc"
