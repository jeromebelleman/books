#ifndef NEWRATING_H
#define NEWRATING_H

#include <QMetaType>
#include <QtGui>

class NewRating {
public:
	NewRating(int _rating = 1);
	void paint(QPainter *_painter, const QRect& _rect,
			   const QPalette& _palette, bool _isHighlighted) const;
private:
	int m_rating;
	QPolygonF m_star;
};

Q_DECLARE_METATYPE(NewRating)

#endif
