// SPDX-FileCopyrightText: 2026 Cutie Shell Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

/**
 * @brief Minimal mock CwlView for testing
 * 
 * This mock provides only the Qt meta-object system registration
 * needed for CwlView* to be used in signals/slots and QVariant.
 * It does NOT implement any actual CwlView functionality.
 * 
 * Used by tests that need to work with CwlView* pointers without
 * pulling in the full compositor dependencies.
 */
class CwlView : public QObject {
	Q_OBJECT

    public:
	explicit CwlView(QObject *parent = nullptr)
		: QObject(parent)
	{
	}
	virtual ~CwlView() = default;

	// Minimal interface - tests should not call these
	// They exist only to satisfy Qt's type system
};
