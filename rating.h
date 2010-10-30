#ifndef RATING_H
#define RATING_H

#include <QMetaType>
#include <QtGui>

class Rating {
public:
	Rating(int _rating = 3);
	int get(void);
	void set(int _rating);
	void paint(QPainter *_painter, const QRect& _rect,
			   const QPalette& _palette, bool _isHighlighted) const;
	QSize sizeHint(void) const;

private:
	int m_rating;
	QPolygonF m_star;
};

Q_DECLARE_METATYPE(Rating)

class RatingEditor : public QWidget {
	Q_OBJECT

public:
	RatingEditor(int _rating);
	QSize sizeHint(void) const;
	void set(int _rating);
	int get(void);

protected:
	void paintEvent(QPaintEvent *_evt);
	void mouseReleaseEvent(QMouseEvent *_evt);
	void keyReleaseEvent(QKeyEvent *_evt);

private:
	Rating m_rating;
};

#endif
