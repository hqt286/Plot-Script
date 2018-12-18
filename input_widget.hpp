#ifndef INPUT_WIDGET_HPP
#define INPUT_WIDGET_HPP
#include <QPlainTextEdit>
#include <QObject>
#include <string>
class InputWidget : public QPlainTextEdit
{
	Q_OBJECT
private:
public:
	InputWidget(QWidget * parent = nullptr);
protected:
	void keyPressEvent(QKeyEvent *e);
signals:
	void Changed(std::string Text); // connect to process in noteapp
};


#endif // !INPUT_WIDGET_HPP
