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
 
#ifndef _X_SHADER_SETTING_DIALOG_
#define _X_SHADER_SETTING_DIALOG_
 
#include <QDialog>
#include <QString>
#include "ui_xShaderSettingDialog.h"
class xShaderSettingDialog : public QDialog
{
	Q_OBJECT;
	public:
		xShaderSettingDialog(QWidget *pParent = NULL, Qt::WindowFlags f=0);
		virtual ~xShaderSettingDialog();

		QString getVertexShaderPath();
		QString getFragmentShaderPath();

	protected slots:
		void slotLoadVertexShader();
		void slotLoadFragmentShader();
		void slotOK();
	private:
		Ui::xShaderSettingDialog ui;
};
 
 #endif