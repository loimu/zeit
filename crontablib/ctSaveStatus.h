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

#ifndef CT_SAVE_STATUS
#define CT_SAVE_STATUS

#include <QString>

class CTSaveStatus {
public:
	
	CTSaveStatus() {
		this->errorStatus = false;
	}
	
	CTSaveStatus(const QString& errorMessage, const QString& detailErrorMessage) {
		this->errorStatus = true;
		this->error = errorMessage;
		this->detailError = detailErrorMessage;
	}
	
	QString errorMessage() const {
		return error;
	}

	QString detailErrorMessage() const {
		return detailError;
	}
	
	bool isError() const {
		return errorStatus;
	}
	
private:
	bool errorStatus;
	
	QString error;
	
	QString detailError;

};


#endif // CT_SAVE_STATUS
