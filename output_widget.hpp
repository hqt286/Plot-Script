#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QObject>
#include <string>
#include <QPainter>
#include <QVBoxLayout>
class OutputWidget : public QWidget
{
	Q_OBJECT

private:
	QGraphicsScene * scene;
	QGraphicsView * view;
public:
	OutputWidget(QWidget * parent = nullptr);
	QGraphicsView * outputBox();
	~OutputWidget();
protected:
	void resizeEvent(QResizeEvent *event);
public slots:
	void RecieveClearScene();
	void RecieveError(std::string error);
	void RecieveValidOutput(std::string message);
	void RecieveDrawPoint(double x, double y, double pointsize);
	void RecieveDrawLine(double x1, double y1, double x2, double y2, double thickness_size);
	void RecieveDrawString(double x, double y, double angle, double scale, std::string text_message);
};



#endif // !OUTPUT_WIDGET_HPP
