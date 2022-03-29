#pragma once

#include <QObject>
#include <QFlags>
#include <QQmlEngine>

class FlagExample : public QObject
{

	Q_OBJECT
	QML_NAMED_ELEMENT(FlagExample)

	struct Private;
	QScopedPointer<Private> d;

public:
	explicit FlagExample(QObject* parent = nullptr);
	~FlagExample();

	// enum with our bitflags
	enum WhatToSetFlags {
		// << is a 'bit shift left', so we shift 0x1 by 1 bit to the left, making it 0b10
		SetColorScheme = 0x1 << 1,
		// shift 0x1 by 2 bits to the left, making it 0b100
		SetFont = 0x1 << 2,
		// and so on
		SetDesktopLayout = 0x1 << 3,
		SetYourMom = 0x1 << 4
		/* this means that every individual bit can be checked to densely pack a flag value
		   explanation of the binary mechanics available upon request
		*/
	};
	Q_DECLARE_FLAGS(WhatToSet /* the name of the fancy flag type */, WhatToSetFlags /* the raw enum Qt will make a fancy flag type for */)

	// declare a property
	Q_PROPERTY(WhatToSet whatToSet READ whatToSet WRITE setWhatToSet NOTIFY whatToSetChanged RESET resetWhatToSet)
	WhatToSet whatToSet() const;
	void setWhatToSet(WhatToSet value);
	Q_SIGNAL void whatToSetChanged();
	void resetWhatToSet();

};
