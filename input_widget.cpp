#include "input_widget.hpp"
#include <QDebug>
InputWidget::InputWidget(QWidget * parent) : QPlainTextEdit(parent)
{
	this->setObjectName("input");
}

void InputWidget::keyPressEvent(QKeyEvent * e)
{
	if ((e->key() == Qt::Key_Return) && (e->modifiers() == Qt::ShiftModifier))
	{
		emit Changed(this->toPlainText().toUtf8().constData());
	}
	QPlainTextEdit::keyPressEvent(e);
}