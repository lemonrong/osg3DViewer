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

#include "xShaderSettingDialog.h"
#include <QFileDialog>

xShaderSettingDialog::xShaderSettingDialog(QWidget *pParent, Qt::WindowFlags f) : QDialog(pParent, f)
{
	ui.setupUi(this);
	connect(ui.pushButton_vertexShader, SIGNAL(clicked()), this, SLOT(slotLoadVertexShader()), Qt::UniqueConnection);
	connect(ui.pushButton_fragmentShader, SIGNAL(clicked()), this, SLOT(slotLoadFragmentShader()), Qt::UniqueConnection);
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(slotOK()), Qt::UniqueConnection);
}
xShaderSettingDialog::~xShaderSettingDialog()
{
}

void xShaderSettingDialog::slotLoadVertexShader()
{
	QString strShaderFile = QFileDialog::getOpenFileName(this, tr("Select vertex shader file"),  "", tr("Vertex Shader Files (*.vs)"));
	if (strShaderFile.isEmpty())
		return;
	ui.lineEdit_vertexShader->setText(strShaderFile);
}
void xShaderSettingDialog::slotLoadFragmentShader()
{
	QString strShaderFile = QFileDialog::getOpenFileName(this, tr("Select fragment shader file"),  "", tr("Fragment Shader Files (*.fs)"));
	if (strShaderFile.isEmpty())
		return;
	ui.lineEdit_fragmentShader->setText(strShaderFile);
}

void xShaderSettingDialog::slotOK()
{
	if (ui.lineEdit_vertexShader->text().isEmpty() || ui.lineEdit_fragmentShader->text().isEmpty())
	{
		rejected();
		return;
	}

	accepted();
}

QString xShaderSettingDialog::getVertexShaderPath()
{
	return ui.lineEdit_vertexShader->text();
}
QString xShaderSettingDialog::getFragmentShaderPath()
{
	return ui.lineEdit_fragmentShader->text();
}