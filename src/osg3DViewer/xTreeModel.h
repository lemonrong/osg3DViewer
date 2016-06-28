/******************************************************************************
   osg3DViewer: a 3D file viewer
   Copyright(C) 2015-2015  ronggenmiao@163.com

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software

 *******************************************************************************/

#ifndef _xTreeModel_H_
#define _xTreeModel_H_

// Qt
#include <QtCore/QAbstractItemModel>

// OSG
#include <osg/ref_ptr>

//  Forward declarations

namespace osg
{
	class Node;
	class Object;
}

//  Class Declaration

class xTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum
    {
        COL_NAME = 0,
        COL_TYPE,
        COL_DESCRIPTION,
        NB_COL
    };

    xTreeModel(QObject *parent = 0);
    virtual ~xTreeModel();

    void                setNode(osg::Node *node);
    osg::Node*          getNode();
    const osg::Node*    getNode() const;

    osg::Node*          getNode(const QModelIndex &index);

    void                clear() { reset(); }

    Qt::ItemFlags       flags(const QModelIndex &index) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool hasChildren(const QModelIndex &parent) const { return rowCount(parent) > 0; }

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void setEnableIndex(const QModelIndex &index, bool);

    QModelIndex searchForNode(osg::Node *node,const QModelIndex &parent = QModelIndex());
    QModelIndex searchForName(const QString &name, const QModelIndex &parent = QModelIndex());

protected:
    inline osg::Node* getPrivateData(const QModelIndex &index) const { return reinterpret_cast<osg::Node*>(index.internalPointer()); }

private:
    osg::ref_ptr<osg::Node> m_node;
    QHash<QString, QIcon> m_hashIcon;
};

#endif // _xTreeModel_H_
