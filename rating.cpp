#include "rating.h"

Star::Star(int _i, bool _zero)
	: m_i(_i), m_zero(_zero), m_ison(false), m_on(":/on.png"), 
	m_off(":/off.png"), m_unrated(":/unrated.png")
{
	setPixmap(m_unrated);
}

void Star::seton(bool _ison)
{
	if (_ison) {
		if (m_zero) {
			setPixmap(m_off);
		} else {
			setPixmap(m_on);
		}
		m_ison = true;
	} else {
		setPixmap(m_unrated);
		m_ison = false;
	}
}

void Star::toggle(void)
{
	if (m_ison) {
		setPixmap(m_unrated);
		m_ison = false;
	} else {
		if (m_zero) {
			setPixmap(m_off);
		} else {
			setPixmap(m_on);
		}
		m_ison = true;
	}
}

void Star::mousePressEvent(QMouseEvent *)
{
	emit clicked(m_i);
}

Rating::Rating(int _n = 5)
	: m_rating(-1)
{
	QHBoxLayout *layout;
	Star *star;

	layout = new QHBoxLayout();

	star = new Star(0, true);
	connect(star, SIGNAL(clicked(int)), this, SLOT(rate(int)));
	m_stars << star;
	layout->addWidget(star);

	for (int i = 1; i < _n; ++i) {
		star = new Star(i, false);
		m_stars << star;
		connect(star, SIGNAL(clicked(int)), this, SLOT(rate(int)));
		layout->addWidget(star);
	}

	setLayout(layout);
}

void Rating::rate(int _i)
{
	if (_i == 0 && m_rating == 0) {
		m_stars[0]->toggle();
	} else {
		for (int i = 0; i <= _i; ++i) {
			m_stars[i]->seton(true);
		}
	}
	for (int i = _i + 1; i < m_stars.size(); ++i) {
		m_stars[i]->seton(false);
	}
	m_rating = _i;
}
