/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "theme.h"

#include <QDebug>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>

namespace customization
{
QString Theme::m_dataPath= "";

Theme::Theme(QObject* parent) : QObject(parent)
{
    loadData(m_dataPath);
}

StyleSheet* Theme::styleSheet(const QString& id)
{
    auto it= m_styleSheets.find(id);
    if(it == m_styleSheets.end())
        return nullptr;
    return it->second;
}

void Theme::setPath(const QString& path)
{
    m_dataPath= path;
}

Theme* Theme::instance()
{
    static Theme theme;
    return &theme;
}

void Theme::loadData(const QString& source)
{
    QFile file(source);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    StyleSheet* currentStyleSheet= nullptr;
    while(!in.atEnd())
    {
        auto line= file.readLine();
        qDebug() << line;
        if(line.isEmpty())
            continue;

        auto str= line;

        if(str.startsWith("["))
        {
            auto label= str.mid(1, str.length() - 3);
            currentStyleSheet= addStyleSheet(label);
        }
        else if(currentStyleSheet)
        {
            QRegularExpression exp("^(\\w+)=(.*)$");
            auto match= exp.match(str, 0, QRegularExpression::PartialPreferCompleteMatch);
            if(match.hasMatch())
            {
                auto key= match.captured(1);
                auto value= match.captured(2);
                bool ok;
                auto intVal= value.toInt(&ok);
                if(ok)
                    currentStyleSheet->insert(key, intVal);

                auto realVal= value.toDouble(&ok);
                if(ok)
                    currentStyleSheet->insert(key, realVal);
                else
                    currentStyleSheet->insert(key, value);
            }
        }
    }
}

StyleSheet* Theme::addStyleSheet(const QString& name)
{
    auto styleSheet= new StyleSheet(this);
    m_styleSheets.insert({name, styleSheet});
    return styleSheet;
}

StyleSheet::StyleSheet(Theme* parent) : QQmlPropertyMap(parent) {}

} // namespace customization
