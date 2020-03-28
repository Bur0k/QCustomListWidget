#ifndef LIST_WIDGET_H
#define LIST_WIDGET_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMouseEvent>
#include <QList>
#include <QString>

class List_Widget : public QWidget
{
	Q_OBJECT

	QScrollArea *scroll;
	QWidget *scroll_widget;
	QVBoxLayout *main_layout;

	QColor selection_old_color;
	QWidget *selection = nullptr;
	QWidget *leftClick = nullptr;
	QWidget *rightClick = nullptr;

	int old_top_widget = 0;


	QList<QWidget *> widgets;
signals:
	void selection_changed(QWidget *to);
	void top_item_changed(int index);
	void double_clicked(QWidget *on);
	void right_clicked(QWidget *on, QPoint pos);

public slots:
	void on_vert_slider_change(int value);

public:
	explicit List_Widget(QWidget *parent = 0);

	void add_widget(QWidget *w);//Don't forget to setAttribute(Qt::WA_TransparentForMouseEvents); on all of the children of that widget
	void remove_widget(QWidget *w, bool delete_widget = false);
	void delete_widget(QWidget *w);
	void clear();
	QList<QWidget *> get_all_widgets();

	int get_top_item_index();
	int get_item_count_in_view();

	void scroll_to_top();

protected:
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // LIST_WIDGET_H
