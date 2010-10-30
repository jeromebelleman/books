#include "rating.h"
#include <iostream>
#include <QPointF>
#include <QVector>
#include <math.h>

#define OFFSET 9
#define MAX 5
#define RADIUS1 8
#define RADIUS2 3

Rating::Rating(int _rating)
	: m_rating(_rating)
{
	for (int i = 0; i < 10; ++i) {
		if (i % 2 == 0) {
			m_star << QPointF(RADIUS1 * sin(i * .2 * 3.14) + OFFSET,
							  RADIUS1 * -cos(i * .2 * 3.14) + OFFSET);
		} else {
			m_star << QPointF(RADIUS2 * sin(i * .2 * 3.14) + OFFSET,
							  RADIUS2 * -cos(i * .2 * 3.14) + OFFSET);
		}
	}
}

QSize Rating::sizeHint(void) const
{
	return QSize(OFFSET + MAX * 3 * RADIUS1, 0);
}

void Rating::paint(QPainter *_painter, const QRect& _rect,
					  const QPalette& _palette, bool _isHighlighted) const
{
	_painter->save();
	_painter->setRenderHint(QPainter::Antialiasing, true);
	_painter->setPen(Qt::NoPen); /* Seems slow with a pen! */
	if (_isHighlighted) {
		_painter->setBrush(_palette.background());
	} else {
		_painter->setBrush(_palette.foreground());
	}

	_painter->translate(_rect.x(), _rect.y());

	if (m_rating != -1) {
		for (int i = 0; i < MAX; ++i) {
			if (i < m_rating) {
				_painter->drawPolygon(m_star);
			} else {
				_painter->drawPie(8, 8, 3, 3, 0, 5760);
			}
			_painter->translate(2 * RADIUS1, 0.);
		}
	}

	_painter->restore();
}

void Rating::set(int _rating)
{
	m_rating = _rating;
}

int Rating::get()
{
	return m_rating;
}

RatingEditor::RatingEditor(int _rating)
	: m_rating(_rating)
{
	setFocusPolicy(Qt::StrongFocus);
}

void RatingEditor::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.translate(0., OFFSET);
	m_rating.paint(&painter, rect(), palette(), false);
}

QSize RatingEditor::sizeHint(void) const
{
	return m_rating.sizeHint();
}

void RatingEditor::mouseReleaseEvent(QMouseEvent *_evt)
{
	int x;
	
	x = _evt->x();

	if (x < OFFSET + RADIUS1) {
		if (m_rating.get() == 1) {
			m_rating.set(0);
		} else {
			m_rating.set(1);
		}
	} else if (x > OFFSET + RADIUS1 and x < OFFSET + 3 * RADIUS1) {
		m_rating.set(2);
	} else if (x > OFFSET + RADIUS1 and x < OFFSET + 5 * RADIUS1) {
		m_rating.set(3);
	} else if (x > OFFSET + RADIUS1 and x < OFFSET + 7 * RADIUS1) {
		m_rating.set(4);
	} else if (x > OFFSET + RADIUS1 and x < OFFSET + 9 * RADIUS1) {
		m_rating.set(5);
	}
	update();
}

void RatingEditor::keyReleaseEvent(QKeyEvent *_evt)
{
	/* Propagate event */
	_evt->ignore();

	switch (_evt->key()) {
		case '0':
			m_rating.set(0);
			break;
		case '1':
			m_rating.set(1);
			break;
		case '2':
			m_rating.set(2);
			break;
		case '3':
			m_rating.set(3);
			break;
		case '4':
			m_rating.set(4);
			break;
		case '5':
			m_rating.set(5);
			break;
	}
	update();
}

void RatingEditor::set(int _rating)
{
	m_rating.set(_rating);
	update();
}

int RatingEditor::get(void)
{
	return m_rating.get();
}
