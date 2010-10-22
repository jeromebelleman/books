#ifndef RATING_H
#define RATING_H

#include <QtGui>

class Star : public QLabel
{
	Q_OBJECT

public:
	Star(int _i, bool _zero);
	void seton(bool _ison);
	void toggle(void);

signals:
	void clicked(int);

private:
	int m_i;
	bool m_zero;
	bool m_ison;
	QPixmap m_on;
	QPixmap m_off;
	QPixmap m_unrated;
	void mousePressEvent(QMouseEvent *_evt);
};

class Rating : public QWidget
{
	Q_OBJECT

public:
	Rating(int _n);

public slots:
	void rate(int);

private:
	QList<Star *> m_stars;
	int m_rating;
};


#endif
