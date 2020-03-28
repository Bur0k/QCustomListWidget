#include "list_widget.h"
#include <QCoreApplication>
#include <QScrollBar>
#include <QDebug>

List_Widget::List_Widget(QWidget *parent) :
	QWidget(parent), scroll(new QScrollArea()), scroll_widget(new QWidget()), main_layout(new QVBoxLayout())
{
	scroll->setWidget(scroll_widget);
	scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scroll->setWidgetResizable(true);

	scroll_widget->setLayout(main_layout);
	scroll_widget->layout()->setAlignment( Qt::AlignTop);
	scroll_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(scroll);

	connect(scroll->verticalScrollBar(), SIGNAL(valueChanged(int)),
			this, SLOT(on_vert_slider_change(int)));
}

void List_Widget::on_vert_slider_change(int value)
{
	int new_top_widget = get_top_item_index();
	if (new_top_widget == old_top_widget)
		return;

	old_top_widget = new_top_widget;
	emit top_item_changed(new_top_widget);
}

void List_Widget::add_widget(QWidget *w)
{
	w->setAutoFillBackground(true);
	widgets.push_back(w);
	main_layout->addWidget(w);
}

void List_Widget::remove_widget(QWidget *w, bool delete_widget)
{
	int index = widgets.indexOf(w);
	if(index != -1)
	{
		widgets.removeAt(index);
		main_layout->removeWidget(w);
		if(delete_widget)
			w->deleteLater();
	}
}

void List_Widget::delete_widget(QWidget *w)
{
	remove_widget(w, true);
}

void List_Widget::clear()
{
	for(auto w : widgets)
		delete_widget(w);
	widgets.clear();
	selection = nullptr;
}

QList<QWidget *> List_Widget::get_all_widgets()
{
	return widgets;
}

int List_Widget::get_top_item_index()
{
	if (widgets.size() == 0)
		return 0;

	const int viewport_height = scroll->viewport()->height();
	const int item_height = widgets.at(0)->size().height();
	const int spacing = main_layout->spacing();

	const float total_height_items = (item_height + spacing) * widgets.size() + spacing*2;
	const float value = scroll->verticalScrollBar()->value();
	//Remove the first spacing if possible
	const float value_without_first_spacing = (value - spacing >= 0) ? value - spacing : 0;

	// Iff there is a scrollbar:
	// (item_height + spacing) * widgets.size() + spacing*2  ==  viewport_height + scroll->verticalScrollBar()->maximum()

	if(total_height_items <= viewport_height)
		return 0;

	/*qDebug() << viewport_height
			 << " | " << item_height
			 << " | " << total_height_items
			 << " | " << total_height_layout
			 << " | " << value
			 << " | " << value_without_first_spacing/(item_height+spacing);*/

	return value_without_first_spacing/(item_height+spacing);
}

int List_Widget::get_item_count_in_view()
{
	if(widgets.size() == 0)
		return 0;
	const int viewport_height = scroll->viewport()->height();
	const int item_height = widgets.at(0)->size().height();
	// const int spacing = main_layout->spacing(); //Ignoring spacing here

	return viewport_height / item_height;
}

void List_Widget::scroll_to_top()
{
	scroll->ensureVisible(0,0,0,0);
}

void List_Widget::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		leftClick = childAt(event->pos());
		if(widgets.indexOf(leftClick) == -1)
		{
			leftClick = nullptr;
		}
	}
	else if(event->button() == Qt::RightButton)
	{
		rightClick = childAt(event->pos());
		if(widgets.indexOf(rightClick) == -1)
			rightClick = nullptr;
	}
}

void List_Widget::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(leftClick != nullptr && leftClick == childAt(event->pos()))
		{
			if(selection != nullptr)
			{
				QPalette p = selection->palette();
				p.setColor(QPalette::Window, selection_old_color);
				selection->setPalette(p);
			}

			QPalette p = leftClick->palette();
			selection_old_color = p.color(QPalette::Window);
			p.setColor(QPalette::Window, p.color(QPalette::Highlight));
			leftClick->setPalette(p);
			selection = leftClick;
			leftClick = nullptr;

			emit selection_changed(selection);
		}
	}
	else if(event->button() == Qt::RightButton)
	{
		if(rightClick != nullptr && rightClick == childAt(event->pos()))
		{
			emit right_clicked(rightClick, event->globalPos());
			rightClick = nullptr;
		}
	}
}

void List_Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
		emit double_clicked(childAt(event->pos()));
}
