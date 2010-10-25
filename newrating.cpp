#include "newrating.h"
#include <iostream>
#include <QPointF>
#include <QVector>
#include <math.h>

#define OFFSET 9
#define RADIUS1 8
#define RADIUS2 3

NewRating::NewRating(int _rating)
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

void NewRating::paint(QPainter *_painter, const QRect& _rect,
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

	for (int i = -1; i < 5; ++i) {
		if (i < m_rating) {
			_painter->drawPolygon(m_star);
		} else {
			_painter->drawPie(8, 8, 3, 3, 0, 5760);
		}
		_painter->translate(15., 0.);
	}

	_painter->restore();
}
