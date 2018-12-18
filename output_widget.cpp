#include "output_widget.hpp"
#include <iostream>
#include <cmath>
OutputWidget::OutputWidget(QWidget * parent) : QWidget(parent)
{
	scene = new QGraphicsScene;
	view = new QGraphicsView(scene);
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	auto layout = new QVBoxLayout();
	layout->addWidget(view);
	setLayout(layout);
	view->setParent(this);
	this->setObjectName("output");
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}	

OutputWidget::~OutputWidget()
{
	delete scene;
	delete view;
	scene = nullptr;
	view = nullptr;
}

void OutputWidget::resizeEvent(QResizeEvent * event)
{
	QWidget::resizeEvent(event); // this is to avoid warning unused variable
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

QGraphicsView * OutputWidget::outputBox()
{
	return view;
}

void OutputWidget::RecieveClearScene()
{
	scene->clear();
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::RecieveError(std::string error)
{
	scene->clear();
	scene->addText(QString::fromStdString(error));
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::RecieveValidOutput(std::string message)
{
	scene->addText(QString::fromStdString(message));
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}
#include <QDebug>
void OutputWidget::RecieveDrawPoint(double x, double y, double pointsize)
{
	//check nbounding box
	QPen blackpen(Qt::black);
	blackpen.setStyle(Qt::SolidLine);
	blackpen.setWidth(0);
	QRectF rec(x - (pointsize / 2), y - (pointsize / 2), pointsize, pointsize); // left top width heigh
	auto circle = new QGraphicsEllipseItem /*(x,y,pointsize,pointsize)*/;
	circle->setPen(blackpen);
	circle->setBrush(Qt::black);
	circle->setRect(rec);



	scene->addItem(circle);
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void OutputWidget::RecieveDrawLine(double x1, double y1, double x2, double y2, double thickness_size)
{
	auto line = new QGraphicsLineItem(x1,y1,x2,y2);
	QPen blackpen(Qt::black);	// draw in black color
	blackpen.setWidth(thickness_size);
	line->setPen(blackpen);

	scene->addItem(line);
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}


void OutputWidget::RecieveDrawString(double x, double y, double angle, double scale, std::string text_message)
{ 
	auto font = QFont("Monospace");
	font.setStyleHint(QFont::TypeWriter);
	font.setPointSize(1);

	auto text = new QGraphicsTextItem();

	text->setFont(font);
	text->setPlainText(QString::fromStdString(text_message));
	QPointF cntr_point_distance = text->boundingRect().center(); // get coordinate from the center

	text->setPos(x - cntr_point_distance.x(), y - cntr_point_distance.y());
	text->setScale(scale);
	text->setTransformOriginPoint(cntr_point_distance);
	text->setRotation(angle * 180 / std::atan2(0, -1));


	scene->addItem(text);
	view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

