/***************************************************************************
 *   CT Hour Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CT_INITIALIZATION_ERROR
#define CT_INITIALIZATION_ERROR

#include <QString>

class CTInitializationError {
public:
	
	QString errorMessage() const {
		return error;
	}
	
	void setErrorMessage(const QString& errorMessage) {
		this->error = errorMessage;
	}
	
	bool hasErrorMessage() {
		if (error.isEmpty() == true)
			return false;
		
		return true;
	}
	
private:	
	QString error;

};


#endif // CT_INITIALIZATION_ERROR
