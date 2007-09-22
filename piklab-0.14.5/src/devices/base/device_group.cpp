/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "device_group.h"

#include <qpainter.h>
#include <kglobal.h>

//----------------------------------------------------------------------------
QColor Device::statusColor(Device::Status status)
{
  switch (status) {
    case Device::Status_Future: return Qt::blue;
    case Device::Status_IP:     return Qt::green;
    case Device::Status_Mature:
    case Device::Status_NR:     return QColor("orange");
    case Device::Status_EOL:    return Qt::red;
    case Device::Status_Unknown:
    case Device::Nb_Status:     break;
  }
  return Qt::black;
}

QString coloredString(const QString &text, QColor color)
{
  return QString("<font color=\"") + color.name() + "\">" + text + "</font>";
}

QString supportedString(bool supported)
{
  return coloredString(supported ? i18n("Supported") : i18n("Unsupported"),
                       supported ? Qt::green :  Qt::red);
}

class Tick {
public:
  Tick() {}
  Tick(double value, double oValue) {
    s = KGlobal::locale()->formatNumber(value, 1);
    min = oValue;
  }
  QString s;
  double min;
};

class TickMap : public QMap<double, Tick>
{
public:
  TickMap() {}
  void add(double value, double oValue) {
    insert(value, Tick(value, oValue), false);
    (*this)[value].min = QMIN((*this)[value].min, oValue);
  }
};

QPixmap drawGraph(const QValueVector<Device::RangeBox> &boxes)
{
  const uint w = 300, h = 200;
  QPixmap pixmap(w, h);
  pixmap.fill(Qt::white);
  QPainter p(&pixmap);
  QFontMetrics f(p.font());
  TickMap xTicks, yTicks;
  xTicks.add(0.0, 0.0);
  yTicks.add(0.0, 0.0);
  for (uint i=0; i<boxes.count(); i++) {
//    qDebug("box #%i: %f=[%f %f] %f=[%f %f]", i, boxes[i].start.x, boxes[i].start.yMin,
//           boxes[i].start.yMax, boxes[i].end.x, boxes[i].end.yMin, boxes[i].end.yMax);
    xTicks.add(boxes[i].start.x, boxes[i].start.yMin);
    xTicks.add(boxes[i].start.x, boxes[i].start.yMax);
    xTicks.add(boxes[i].end.x, boxes[i].end.yMin);
    xTicks.add(boxes[i].end.x, boxes[i].end.yMax);
    yTicks.add(boxes[i].start.yMin, boxes[i].start.x);
    yTicks.add(boxes[i].start.yMax, boxes[i].start.x);
    yTicks.add(boxes[i].end.yMin, boxes[i].end.x);
    yTicks.add(boxes[i].end.yMax, boxes[i].end.x);
  }
  double xMax = 0.0, yMax = 0.0;
  int xStart = 0;
  int yStart = h-1 - f.lineSpacing();
  TickMap::const_iterator it = xTicks.begin();
  for (; it!=xTicks.end(); ++it) {
    xStart = QMAX(xStart, f.width(it.data().s));
    xMax = QMAX(xMax, it.key());
  }
  for (it = yTicks.begin(); it!=yTicks.end(); ++it)
    yMax = QMAX(yMax, it.key());
  int xEnd = w-1 - f.width(xTicks[xMax].s)/2;
  QRect rect = f.boundingRect(yTicks[yMax].s);
  int yEnd = rect.height()/2;

  // draw boxes
  p.setPen(Qt::lightGray);
  p.setBrush(Qt::lightGray);
  for (uint i=0; i<boxes.count(); i++) {
    double ax = double(xEnd - xStart)/xMax;
    double ay = double(yEnd - yStart)/yMax;
    QPointArray pa(4);
    pa.setPoint(0, qRound(ax*boxes[i].start.x), qRound(ay*boxes[i].start.yMin));
    pa.setPoint(1, qRound(ax*boxes[i].end.x), qRound(ay*boxes[i].end.yMin));
    pa.setPoint(2, qRound(ax*boxes[i].end.x), qRound(ay*boxes[i].end.yMax));
    pa.setPoint(3, qRound(ax*boxes[i].start.x), qRound(ay*boxes[i].start.yMax));
    pa.translate(xStart, yStart);
    p.drawPolygon(pa);
  }

  // draw axis
  p.setPen(Qt::black);
  p.drawLine(xStart, yStart, w-1, yStart);
  p.drawLine(xStart, yStart, xStart, 0);

  // draw ticks and lines
  p.setPen(Qt::DotLine);
  for (it = yTicks.begin(); it!=yTicks.end(); ++it) {
    int y1 = yStart + qRound(it.key()*(yEnd-yStart)/yMax);
    QRect rect = f.boundingRect(it.data().s);
    p.drawText(xStart/2-rect.width()/2 , y1+rect.height()/2, it.data().s);
    int xmin = xStart + qRound(it.data().min*(xEnd-xStart)/xMax);
    p.drawLine(xStart, y1, xmin, y1);
  }
  for (it = xTicks.begin(); it!=xTicks.end(); ++it) {
    int x1 = xStart + qRound(it.key()*(xEnd-xStart)/xMax);
    QRect rect = f.boundingRect(it.data().s);
    p.drawText(x1-rect.width()/2, h-1, it.data().s);
    int ymin = yStart + qRound(it.data().min*(yEnd-yStart)/yMax);
    p.drawLine(x1, yStart, x1, ymin);
  }

  return pixmap;
}

QPixmap Device::vddGraph(const QString &xLabel, const QString &yLabel,
                         const QValueVector<Device::RangeBox> &boxes)
{
  uint sp = 10;
  QPixmap graph = drawGraph(boxes);
  QPainter p;
  QFontMetrics f(p.font());
  QPixmap pixmap(graph.width() + sp + f.width(xLabel), graph.height() + sp + f.lineSpacing());
  pixmap.fill(Qt::white);
  copyBlt(&pixmap, 0, f.lineSpacing() + sp, &graph, 0, 0, graph.width(), graph.height());
  p.begin(&pixmap);
  p.setPen(Qt::black);
  p.drawText(0, f.lineSpacing(), yLabel);
  p.drawText(pixmap.width()-1-f.width(xLabel), pixmap.height()-1, xLabel);
  return pixmap;
}

const Device::Package *Device::barPackage(const char *name, const Device::Data &data)
{
  for (uint i=0; i<data.packages().count(); i++)
    for (uint k=0; k<data.packages()[i].types.count(); k++)
      if ( Package::TYPE_DATA[data.packages()[i].types[k]].name==name ) return &data.packages()[i];
  return 0;
}

QPixmap Device::pinsGraph(const Device::Package &package)
{
  QPixmap pixmap;
  QPainter p;
  QFontMetrics fm(p.font());
  uint nb = package.pins.count();
  const int hspacing = 3, wspacing = 3, wmark = 10, wpin = 4;
  int theight = fm.ascent() + (fm.ascent()%2==0 ? 1 : 0);
  int height = hspacing + (nb/2)*(hspacing + theight);
  int wnumber = fm.width("1");
  wnumber = QMAX(wnumber, fm.width(QString::number(nb/2)));
  wnumber = QMAX(wnumber, fm.width(QString::number(nb/2+1)));
  wnumber = QMAX(wnumber, fm.width(QString::number(nb)));
  int bwidth = 4*wspacing + 2*wnumber + wmark;
  int lwidth = 0, rwidth = 0;
  for (uint k=0; k<nb/2; k++) {
    lwidth = QMAX(lwidth, fm.width(package.pins[k]));
    rwidth = QMAX(rwidth, fm.width(package.pins[nb-k-1]));
  }
  int bx = lwidth + wspacing + wpin;
  int width = bx + bwidth + wpin + wspacing + rwidth;
  pixmap.resize(width, height);
  pixmap.fill(Qt::white);
  p.begin(&pixmap);
  p.setPen(QPen(Qt::black, 2));
  p.drawRect(bx, 1, bwidth, height-1);
  p.drawArc(bx+wspacing+wnumber+wspacing, -wmark/2+2, wmark, wmark, 0, -180*16);
  for (uint k=0; k<nb/2; k++) {
    int h = hspacing + theight/2 + k*(hspacing + theight);
    p.drawLine(bx-wpin-1, h, bx, h);
    p.drawLine(bx+bwidth, h, bx+bwidth+wpin, h);
    h += theight/2;
    QString label = package.pins[k];
    p.drawText(bx-wpin-wspacing-fm.width(label), h, label);
    p.drawText(bx+bwidth+wpin+wspacing, h, package.pins[nb-k-1]);
    uint pin = (k+1);
    if ( pin==1 || pin==(nb/2) ) {
      p.drawText(bx+wspacing, h, QString::number(pin));
      label = QString::number(nb-k);
      p.drawText(bx+bwidth-wspacing-fm.width(label), h, label);
    }
  }
  p.end();
  return pixmap;
}

QString Device::htmlInfo(const Device::Data &data, const QString &deviceHref,
                         const QString &additionnalHtml, const QString &imagePrefix,
                         QMimeSourceFactory *msf)
{
  QString doc;

  // title
  doc += "<h1>";
  bool first = true;
  for (uint i=0; i<FrequencyRange::Nb_Specials; i++) {
    FrequencyRange::Special s = FrequencyRange::Special(i);
    for (uint k=0; k<data.frequencyRanges().count(); k++) {
      if ( data.frequencyRanges()[k].special!=s ) continue;
      if (first) first = false;
      else doc += " / ";
      doc += data.fname(s);
      break;
    }
  }
  doc += "</h1>";

  doc += "<table><tr>";
  doc += "<td>";

  doc += "<table>";
  QString status = coloredString(i18n(Device::STATUS_DATA[data.status()].label), statusColor(data.status()));
  doc += htmlTableRow(i18n("Status"), status);
  if ( data.alternatives().count() ) {
    QString s;
    for (uint i=0; i<data.alternatives().count(); i++) {
      if ( i!=0 ) s += ", ";
      QString href = deviceHref.arg(data.alternatives()[i].upper());
      s += QString("<a href=\"%1\">%2</a>").arg(href).arg(data.alternatives()[i].upper());
    }
    doc += htmlTableRow(i18n("Alternatives"), s);
  }
  QString url = "<a href=\"" + data.informationUrl() + "\">" + data.name() + "</a>";
  doc += htmlTableRow(i18n("Information Page"), url);
  doc += "</table>";

  doc += "<hr />";
  doc += "<table>";
  doc += data.informationHtml();
  QString s;
  for (uint i=0; i<data.packages().count(); i++)
    for (uint k=0; k<data.packages()[i].types.count(); k++)
      s += i18n(Package::TYPE_DATA[data.packages()[i].types[k]].label) + QString("[%1] ").arg(data.packages()[i].pins.count());
  doc += htmlTableRow(i18n("Packaging"), s);
  doc += "</table>";

  // additionnal info
  doc += additionnalHtml;

  // pins
  const Package *package = 0;
  for (uint i=0; Package::TYPE_DATA[i].name; i++) {
    if ( Package::TYPE_DATA[i].shape!=Package::Bar ) continue;
    package = barPackage(Package::TYPE_DATA[i].name, data);
    if (package) break;
  }
  if (package) {
    QPixmap pix = pinsGraph(*package);
    doc += "</td><td valign=\"top\">";
    doc += "<table cellpadding=\"3\"><tr bgcolor=\"gray\"><th align=\"center\">";
    for (uint k=0; k<package->types.count(); k++) {
      if ( k!=0 ) doc += " ";
      doc += i18n(Package::TYPE_DATA[package->types[k]].label);
      doc += "(" + QString::number(package->pins.count()) + ")";
    }
    doc += "</th></tr><tr><td align=\"center\">";
    QString label = data.name() + "_pins_graph.png";
    doc += "<img src=\"" + imagePrefix + label + "\" />";
    if (msf) msf->setPixmap(label, pix);
    doc += "</td></tr></table>";
    doc += "</td>";
  }

  doc += "</td>";
  doc += "</tr></table>";

  // vdd-frequency graphs
  doc += "<hr />";
  for (uint i=0; i<Device::FrequencyRange::Nb_Specials; i++) {
    Device::FrequencyRange::Special s = Device::FrequencyRange::Special(i);
    for (uint k=0; k<data.frequencyRanges().count(); k++) {
      const Device::FrequencyRange &fr = data.frequencyRanges()[k];
      if ( fr.special!=s ) continue;
      doc += "<h3>" + data.fname(s) + " - " + i18n("Temperature range: ")
	+ i18n(Device::FrequencyRange::TYPE_DATA[fr.type].label) + "</h3>";
      QString label = data.name() + "_" + data.fname(s) + "_" + Device::FrequencyRange::TYPE_DATA[fr.type].name + ".png";
      doc += "<img src=\"" + imagePrefix + label + "\" />";
      if (msf) msf->setPixmap(label, Device::vddGraph(i18n("F (MHz)"), i18n("Vdd (V)"), fr.vdds));
    }
  }


  return doc;
}
