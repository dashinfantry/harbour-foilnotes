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

#include "FoilNotesOrganizeModel.h"

#include "HarbourDebug.h"

// ==========================================================================
// FoilNotesOrganizeModel::Private
// ==========================================================================

class FoilNotesOrganizeModel::Private {
public:
    Private();

    int mapToSource(int aRow) const;
    int mapFromSource(int aRow) const;

public:
    int iDragIndex;
    int iDragPos;
};

FoilNotesOrganizeModel::Private::Private() :
    iDragIndex(-1),
    iDragPos(-1)
{
}

int FoilNotesOrganizeModel::Private::mapToSource(int aRow) const
{
    if (iDragIndex < iDragPos) {
        if (aRow < iDragIndex || aRow > iDragPos) {
            return aRow;
        } else if (aRow == iDragPos) {
            return iDragIndex;
        } else {
            return aRow + 1;
        }
    } else if (iDragPos < iDragIndex) {
        if (aRow < iDragPos || aRow > iDragIndex) {
            return aRow;
        } else if (aRow == iDragPos) {
            return iDragIndex;
        } else {
            return aRow - 1;
        }
    }
    return aRow;
}

int FoilNotesOrganizeModel::Private::mapFromSource(int aRow) const
{
    if (iDragIndex < iDragPos) {
        if (aRow < iDragIndex || aRow > iDragPos) {
            return aRow;
        } else if (aRow == iDragIndex) {
            return iDragPos;
        } else {
            return aRow - 1;
        }
    } else if (iDragPos < iDragIndex) {
        if (aRow < iDragPos || aRow > iDragIndex) {
            return aRow;
        } else if (aRow == iDragIndex) {
            return iDragPos;
        } else {
            return aRow + 1;
        }
    }
    return aRow;
}

// ==========================================================================
// FoilNotesOrganizeModel
// ==========================================================================

#define SUPER QSortFilterProxyModel

FoilNotesOrganizeModel::FoilNotesOrganizeModel(QObject* aParent) :
    SUPER(aParent),
    iPrivate(new Private)
{
}

FoilNotesOrganizeModel::~FoilNotesOrganizeModel()
{
    delete iPrivate;
}

void FoilNotesOrganizeModel::setSourceModelObject(QObject* aModel)
{
    setSourceModel(qobject_cast<QAbstractItemModel*>(aModel));
}

int FoilNotesOrganizeModel::dragIndex() const
{
    return iPrivate->iDragIndex;
}

int FoilNotesOrganizeModel::dragPos() const
{
    return iPrivate->iDragPos;
}

QModelIndex FoilNotesOrganizeModel::mapToSource(const QModelIndex& aIndex) const
{
    QAbstractItemModel* source = sourceModel();
    if (source) {
        return source->index(iPrivate->mapToSource(aIndex.row()), aIndex.column());
    }
    return aIndex;
}

QModelIndex FoilNotesOrganizeModel::mapFromSource(const QModelIndex& aIndex) const
{
    return index(iPrivate->mapFromSource(aIndex.row()), aIndex.column());
}

void FoilNotesOrganizeModel::setDragIndex(int aIndex)
{
    HDEBUG(aIndex);
    if (aIndex < 0) {
        if (iPrivate->iDragIndex >= 0) {
            // Drag is finished
            if (iPrivate->iDragPos != iPrivate->iDragIndex) {
                const int dragIndex = iPrivate->iDragIndex;
                const int dragPos = iPrivate->iDragPos;
                iPrivate->iDragPos = iPrivate->iDragIndex = -1;
                QAbstractItemModel* source = sourceModel();
                if (source) {
                    source->moveRow(QModelIndex(), dragIndex, QModelIndex(), dragPos);
                }
            } else {
                iPrivate->iDragPos = iPrivate->iDragIndex = -1;
            }
            Q_EMIT dragIndexChanged();
            Q_EMIT dragPosChanged();
        }
    } else if (aIndex != iPrivate->iDragIndex && aIndex < rowCount()) {
        // Drag is starting
        iPrivate->iDragPos = iPrivate->iDragIndex = aIndex;
        Q_EMIT dragIndexChanged();
        Q_EMIT dragPosChanged();
    }
}

void FoilNotesOrganizeModel::setDragPos(int aPos)
{
    if (aPos >= 0 && aPos < rowCount() &&
        iPrivate->iDragIndex >= 0 && iPrivate->iDragPos != aPos) {
        HDEBUG(aPos);
        const int dest = (aPos > iPrivate->iDragPos) ? (aPos + 1) : aPos;
        beginMoveRows(QModelIndex(), iPrivate->iDragPos, iPrivate->iDragPos,
            QModelIndex(), dest);
        iPrivate->iDragPos = aPos;
        endMoveRows();
        Q_EMIT dragPosChanged();
    }
}
