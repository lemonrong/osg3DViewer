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


// Project
#include "xTreeModel.h"

// Qt
#include <QtGui/QIcon>

// OSG
#include <osg/LOD>
#include <osg/Billboard>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Group>
#include <osg/Switch>

//==============================================================================

xTreeModel::xTreeModel(QObject *parent) : QAbstractItemModel(parent)
{
    m_hashIcon.insert("LOD", QIcon(":/osg3DViewer/treeview/lod.png"));
    m_hashIcon.insert("Switch", QIcon(":/osg3DViewer/treeview/switch.png"));
    m_hashIcon.insert("Group", QIcon(":/osg3DViewer/treeview/group.png"));
    m_hashIcon.insert("MatrixTransform", QIcon(":/osg3DViewer/treeview/transform.png"));
    m_hashIcon.insert("Billboard", QIcon(":/osg3DViewer/treeview/billboard.png"));
    m_hashIcon.insert("Geode", QIcon(":/osg3DViewer/treeview/geode.png"));
    m_hashIcon.insert("PointLight", QIcon(":/osg3DViewer/treeview/pointlight.png"));
    m_hashIcon.insert("SpotLight", QIcon(":/osg3DViewer/treeview/spotlight.png"));
    m_hashIcon.insert("LightSource", QIcon(":/osg3DViewer/treeview/lightsource.png"));
    m_hashIcon.insert("Bone", QIcon(":/osg3DViewer/treeview/bone.png"));
    m_hashIcon.insert("Skeleton", QIcon(":/osg3DViewer/treeview/skeleton.png"));
    m_hashIcon.insert("Sequence", QIcon(":/osg3DViewer/treeview/sequence.png"));
}

//==============================================================================

xTreeModel::~xTreeModel()
{}

//==============================================================================

void xTreeModel::setNode(osg::Node *node)
{
    reset();
    m_node = node;
}

//==============================================================================

osg::Node* xTreeModel::getNode()
{
    return m_node.get();
}

//==============================================================================

const osg::Node* xTreeModel::getNode() const
{
    return m_node.get();
}

//==============================================================================

QModelIndex xTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex index;

    if (!parent.isValid())
    {
        index = createIndex(row, column, m_node.get());
    }
    else
    {
        osg::Group *grp = dynamic_cast<osg::Group*>(getPrivateData(parent));

        if (grp && row < (int)grp->getNumChildren())
        {
            index = createIndex(row, column, grp->getChild(row));
        }
        else
        {
            index = createIndex(row, column);
        }
    }

    return index;
}

//==============================================================================

QModelIndex xTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid() || getPrivateData(index) == m_node.get())
        return QModelIndex();

    if (getPrivateData(index) == NULL)
        return QModelIndex();

    osg::ref_ptr<osg::Node> node = dynamic_cast<osg::Node*>(getPrivateData(index));

    if (node.valid() && node->getNumParents() > 0)
    {
        osg::ref_ptr<osg::Group> parent = node->getParent(0);
        osg::ref_ptr<osg::Group> pp = parent->getParent(0);

        if (parent.valid() && pp.valid())
            return createIndex(pp->getChildIndex(parent.get()), 0, parent.get());
    }

    return QModelIndex();
}

//==============================================================================

int xTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 1;

    osg::ref_ptr<osg::Node> node = reinterpret_cast<osg::Node*>(parent.internalPointer());

    if (node.valid())
    {
        osg::Group *group = node->asGroup();

        if (group)
            return group->getNumChildren();
    }

    return 0;
}

//==============================================================================

int xTreeModel::columnCount(const QModelIndex & /*parent */) const
{
    return NB_COL;
}

//==============================================================================

QVariant xTreeModel::data(const QModelIndex &index, int role) const
{
    if (!getPrivateData(index))
        return QVariant();

    QVariant d;

    osg::ref_ptr<osg::Node> node = getPrivateData(index);
    QHash<QString, QIcon>::const_iterator it;

    if (role == Qt::DisplayRole) // name
    {
        if (index.column() == COL_NAME)
        {
            if (node->getName().empty())
                d = tr("<unnamed>");
            else
                d = QString::fromStdString(node->getName());
        }
        else if (index.column() == COL_TYPE)
        {
            d = QString::fromStdString(node->className());
        }
        else if (index.column() == COL_DESCRIPTION)
        {
            QString description;

            for (size_t i = 0; node->getNumDescriptions(); i++)
                description += QString::fromStdString(node->getDescription(i)) + "\n";
        }
    }
    else if (role == Qt::DecorationRole && (index.column() == COL_NAME)) // icon
    {
        it = m_hashIcon.find(node->className());
        if (it != m_hashIcon.end())
            d = *it;
    }
    else if (role == Qt::CheckStateRole && (index.column() == COL_NAME))
    {
        if (node.valid())
        {
            d = (node->getNodeMask() == 0 ? Qt::Unchecked : Qt::Checked);
        }
    }
    return d;
}

//==============================================================================

void xTreeModel::setEnableIndex(const QModelIndex &index, bool val)
{
    osg::ref_ptr<osg::Node> node = getPrivateData(index);
    if (node.valid())
    {
        if (val)
            node->setNodeMask(0xffffffff);
        else
            node->setNodeMask(0x0);
    }
}

//==============================================================================

QVariant xTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == COL_NAME)
            return QString("Name");
        else if (section == COL_TYPE)
            return QString("Type");
        else if (section == COL_DESCRIPTION)
            return QString("Description");
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

//==============================================================================

Qt::ItemFlags xTreeModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsUserCheckable |
           Qt::ItemIsSelectable |
           Qt::ItemIsEnabled |
           Qt::ItemIsTristate;
}

//==============================================================================

osg::Node* xTreeModel::getNode(const QModelIndex &index)
{
    if (index.isValid())
    {
        return getPrivateData(index);
    }

    return 0L;
}

//==============================================================================

QModelIndex xTreeModel::searchForNode(osg::Node *node, const QModelIndex &parent)
{
    // firstly check parent
    if (parent.isValid())
    {
        if (node == getNode(parent))
            return parent;
    }

    for (int i = 0; i < rowCount(parent); i++)
    {
        QModelIndex ind = index(i,0,parent);
        QModelIndex result = searchForNode(node,ind);
        if (result.isValid())
            return result;
    }
    return QModelIndex();
}

//==============================================================================

QModelIndex xTreeModel::searchForName(const QString &name, const QModelIndex &parent)
{
    // firstly check parent
    if (parent.isValid())
    {
        osg::ref_ptr<osg::Node> node = getPrivateData(parent);
        if (node.valid() && node->getName() == name.toStdString())
            return parent;
    }

    for (int i = 0; i < rowCount(parent); i++)
    {
        QModelIndex ind = index(i,0,parent);
        QModelIndex result = searchForName(name,ind);
        if (result.isValid())
            return result;
    }
    return QModelIndex();
}
