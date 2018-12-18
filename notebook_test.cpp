#include <QTest>
#include "notebook_app.hpp"
#include <QDebug>
#include "interpreter.hpp"
class NotebookTest : public QObject {
  Q_OBJECT
private:
	NotebookApp NoteApp;
private slots:
  void initTestCase();
  void testFindOutputByName();
  void testFindInputByName();
  void testFindStartButtonByName();
  void testFindStopButtonByName();
  void testFindResetButtonByName();
  void testFindInterruptButtonByName();
  void testChildrenSize();
  void testFindByType();
  void testSendText();
  void testSendTextNumber();
  void testNumber();
  void testMakePointSize40();
  void testComplexCalculation();
  void testListOfPoints();
  void testListOfLines();
  void testListOfTexts();
  void testDiscretePlotLayout();
  void testContinuousPlot();
  void testContinuousLinearPlot();
  void testContinuousOne();
  void testContinuousBigBoy();
  void testStopButton();
  void testInterruption();
};

void NotebookTest::testFindStartButtonByName()
{
	auto startButton = NoteApp.findChild<QPushButton *>("start");
	QVERIFY2(startButton, "Could not find input widget with name: 'start'");
}

void NotebookTest::testFindStopButtonByName()
{
	auto stopButton = NoteApp.findChild<QPushButton *>("stop");
	QVERIFY2(stopButton, "Could not find input widget with name: 'stop'");
}

void NotebookTest::testFindResetButtonByName()
{
	auto resetButton = NoteApp.findChild<QPushButton *>("reset");
	QVERIFY2(resetButton, "Could not find input widget with name: 'reset'");
}

void NotebookTest::testFindInterruptButtonByName()
{
	auto interruptButton = NoteApp.findChild<QPushButton *>("interrupt");
	QVERIFY2(interruptButton, "Could not find input widget with name: 'interrupt'");
}

void NotebookTest::testFindInputByName()
{
	auto op = NoteApp.findChild<InputWidget *>("input");
	QVERIFY2(op, "Could not find input widget with name: 'input'");
}

void NotebookTest::testFindOutputByName()
{
	auto op = NoteApp.findChild<OutputWidget *>("output");
	QVERIFY2(op, "Could not find output widget with name: 'output'");
}

void NotebookTest::testChildrenSize()
{
	auto childrenQPlainText = NoteApp.findChildren<QPlainTextEdit *>();
	auto childrenQgraphic = NoteApp.findChildren<QGraphicsView *>();
	auto childrenQPushButton = NoteApp.findChildren<QPushButton *>();
	QVERIFY2(childrenQPlainText.size() == 1, "Number QPlainText children is not incorrect");
	QVERIFY2(childrenQgraphic.size() == 1, "Number QGraphicsView children is not incorrect");
	QVERIFY2(childrenQPushButton.size() == 4, "Number childrenQPushButton children is not incorrect");
}

void NotebookTest::testFindByType()
{
	auto typeInput = NoteApp.findChild<QPlainTextEdit *>();
	auto typeView = NoteApp.findChild<QGraphicsView *>();
	auto stopButton = NoteApp.findChild<QPushButton *>("stop");
	auto startButton = NoteApp.findChild<QPushButton *>("start");
	auto resetButton = NoteApp.findChild<QPushButton *>("reset");
	auto interruptButton = NoteApp.findChild<QPushButton *>("interrupt");

	QVERIFY2(typeInput, "Could not find widget with type: QPlainTextEdit");
	QVERIFY2(typeView, "Could not find widget with type: QGraphicsView");
	QVERIFY2(stopButton, "Could not find widget with type: QPushButton stop");
	QVERIFY2(startButton, "Could not find widget with type: QPushButton start");
	QVERIFY2(resetButton, "Could not find widget with type: QPushButton reset");
	QVERIFY2(interruptButton, "Could not find widget with type: QPushButton interrupt");
}

void NotebookTest::testSendText()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(make-text \"hello world, this is Tran\")");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}


void NotebookTest::testSendTextNumber()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(make-text \"123\")");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::testNumber()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(+ 3 I)");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::testMakePointSize40()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(set-property \"size\" 40 (make-point 0 10))");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::testComplexCalculation()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(+ (/ 3 2) (^ e pi))");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::initTestCase(){
	NoteApp.show();
}

/*
findLines - find lines in a scene contained within a bounding box
			with a small margin
 */
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin) {

	QPainterPath selectPath;

	QMarginsF margins(margin, margin, margin, margin);
	selectPath.addRect(bbox.marginsAdded(margins));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
 */
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numpoints(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsEllipseItem::Type) {
			numpoints += 1;
		}
	}

	return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
		   rotation and string contents
 */
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents) {

	int numtext(0);
	foreach(auto item, scene->items(center)) {
		if (item->type() == QGraphicsTextItem::Type) {
			QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
			if ((text->toPlainText() == contents) &&
				(text->rotation() == rotation) &&
				(text->pos() + text->boundingRect().center() == center)) {
				numtext += 1;
			}
		}
	}

	return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
 */
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius) {

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
	scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()) {
		if (item->type() == QGraphicsLineItem::Type) {
			numlines += 1;
		}
	}

	return numlines;
}

void NotebookTest::testStopButton() {

	std::string program = R"( 
(discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";
	auto inputWidget = NoteApp.findChild<InputWidget *>("input");
	auto stopButton = NoteApp.findChild<QPushButton *>("stop");
	QTest::keyPress(stopButton, Qt::Key_Return);
	QTest::qWait(200);
	inputWidget->setPlainText(QString::fromStdString(program));
}

void NotebookTest::testInterruption() {

	std::string program = R"( 
(discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";
	auto inputWidget = NoteApp.findChild<InputWidget *>("input");
	auto interruptButton = NoteApp.findChild<QPushButton *>("interrupt");
	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyPress(interruptButton, Qt::Key_Return);
	QTest::qWait(200);

}

void NotebookTest::testDiscretePlotLayout() {

	std::string program = R"( 
(discrete-plot (list (list -1 -1) (list 1 1)) 
    (list (list "title" "The Title") 
          (list "abscissa-label" "X Label") 
          (list "ordinate-label" "Y Label") ))
)";
	auto inputWidget = NoteApp.findChild<InputWidget *>("input");
	inputWidget->setPlainText(QString::fromStdString(program));
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

	auto outputWidget = NoteApp.findChild<OutputWidget *>("output");
	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");
	QTest::qWait(2000);
	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	QCOMPARE(items.size(), 17);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 2.0;
	double scaley = 20.0 / 2.0;

	double xmin = scalex * -1;
	double xmax = scalex * 1;
	double ymin = scaley * -1;
	double ymax = scaley * 1;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-1")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("1")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

	// check the ordinate axis 
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	// check the point at (-1,-1)
	QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

	// check the point at (1,1)
	QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
}


void NotebookTest::testContinuousOne() {

	auto inputWidget = NoteApp.findChild<InputWidget *>("input");
	inputWidget->setPlainText("(begin	(define f(lambda(x) "
		"(/ 1 (+ 1 (^ e(- (* 20 x)))))))"
		"(continuous-plot f(list -1 1)))");
	QTest::keyClick(inputWidget, Qt::Key_Return, Qt::ShiftModifier);

	auto outputWidget = NoteApp.findChild<OutputWidget *>("output");
	auto view = outputWidget->findChild<QGraphicsView *>();
	QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");
	QTest::qWait(2000);
	auto scene = view->scene();

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	auto items = scene->items();
	QCOMPARE(items.size(), 59);

	// make them all selectable
	foreach(auto item, items) {
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0 / 2.0;
	double scaley = 20.0 / 2.0;

	double xmin = scalex * -1;
	double xmax = scalex * 1;
	double ymin = scaley * -1;
	double ymax = scaley * 1;
	double xmiddle = (xmax + xmin) / 2;
	double ymiddle = (ymax + ymin) / 2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 0);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 0);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 0);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("12.4")), 0);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("2.9")), 0);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("5")), 0);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 0);
}



void NotebookTest::testListOfPoints()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(list (set-property \"size\" 1 (make-point 0 0)) (set-property \"size\" 2 (make-point 4 0)) (set-property \"size\" 4 (make-point 8 0)) (set-property \"size\" 8 (make-point 16 0)) (set-property \"size\" 16 (make-point 32 0)) (set-property \"size\" 32 (make-point 64 0)))");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::testListOfLines()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(list (set-property \"thickness\" 3 (make-line (make-point 0 0) (make-point 0 20)))"
		" (set-property \"thickness\" 6 (make-line (make-point 10 0) (make-point 10 20)))"
		" (set-property \"thickness\" 9 (make-line (make-point 20 0) (make-point 20 20))))");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::testListOfTexts()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(begin ( define xloc 0) (define yloc 0) (list"
		"(set-property \"position\" (make-point (+ xloc 20) yloc) (make-text \"Hi\"))"
		"(set-property \"position\" (make-point (+ xloc 40) yloc) (make-text \"Hi\"))"
		"(set-property \"position\" (make-point (+ xloc 60) yloc) (make-text \"Hi\"))"
		"(set-property \"position\" (make-point (+ xloc 80) yloc) (make-text \"Hi\"))"
		"(set-property \"position\" (make-point (+ xloc 100) yloc) (make-text \":D\"))))");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::testContinuousPlot()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(begin	(define f (lambda(x) (^ e x)))"
		"(continuous-plot f (list -1 1)))");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::testContinuousLinearPlot()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(begin	(define f(lambda(x)"
		"(+ (* 2 x) 1))) (continuous-plot f(list -2 2)"
		"(list (list \"title\" \"A continuous linear function\")"
		"(list \"abscissa-label\" \"x\") (list \"ordinate-label\" \"y\"))))");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}

void NotebookTest::testContinuousBigBoy()
{
	auto input = NoteApp.findChild<InputWidget *>("input");
	input->setPlainText("(begin (define f (lambda(x) (sin x)))"
		"(continuous-plot f(list(- pi) pi)))");
	NoteApp.testNoteBook();
	QTest::qWait(200);
}


QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"


