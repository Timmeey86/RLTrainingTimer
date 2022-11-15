#pragma once
#include <filesystem>
#include <minwindef.h>
#include <minwinbase.h>
#include <stringapiset.h>
#include <winerror.h>
#include <commdlg.h>

class file_dialogs 
{
public:
	static std::wstring toWideString( const std::string &utf8String )
	{
		int wideSize = ::MultiByteToWideChar( CP_UTF8, 0, utf8String.c_str(), -1, NULL, 0 );
		if( wideSize == ERROR_NO_UNICODE_TRANSLATION ) {
			throw std::exception( "Invalid UTF-8 sequence." );
		}
		else if( wideSize == 0 ) {
			throw std::exception( "Error in UTF-8 to UTF-16 conversion." );
		}

		std::vector<wchar_t> resultString( wideSize );
		int convResult = ::MultiByteToWideChar( CP_UTF8, 0, utf8String.c_str(), -1, &resultString[0], wideSize );
		if( convResult != wideSize ) {
			throw std::exception( "Error in UTF-8 to UTF-16 conversion." );
		}

		return std::wstring( &resultString[0] );
	}

	static std::filesystem::path getOpenOrSaveFilePath(const std::filesystem::path & initialPath, std::vector<std::string> extensions, bool save = false)
	{
		namespace fs = std::filesystem;
		OPENFILENAMEW ofn;       // common dialog box structure
		wchar_t szFile[MAX_PATH];       // buffer for file name
		wchar_t extensionStr[10000];
		wchar_t initialPathStr[MAX_PATH];

		// Initialize OPENFILENAME
		::ZeroMemory( &ofn, sizeof(ofn) );
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = 0;
		ofn.lpstrFile = szFile;
	
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
		// use the contents of szFile to initialize itself.
		//
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof( szFile );
		if( extensions.empty() ) {
			ofn.lpstrFilter = L"All\0*.*\0";
		}
		else {
			size_t offset = 0;
		
			wcscpy( extensionStr, L"Supported Types" );
			offset += wcslen( extensionStr ) + 1;
			for( auto strIt = extensions.begin(); strIt != extensions.end(); ++strIt ) {
				wcscpy( extensionStr + offset, L"*." );
				offset += 2;
				wcscpy( extensionStr + offset, file_dialogs::toWideString( *strIt ).c_str() );
				offset += strIt->length();
				// append a semicolon to all but the last extensions
				if( strIt + 1 != extensions.end() ) {
					extensionStr[offset] = L';';
					offset += 1;
				}
				else {
					extensionStr[offset] = L'\0';
					offset += 1;
				}
			}

			extensionStr[offset] = 0;
			ofn.lpstrFilter = extensionStr;
		}
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		if( initialPath.empty() ) {
			ofn.lpstrInitialDir = NULL;
		}
		else if( fs::is_directory( initialPath ) ) {
			wcscpy( initialPathStr, initialPath.wstring().c_str() );
			ofn.lpstrInitialDir = initialPathStr;
		}
		else {
			wcscpy( initialPathStr, initialPath.parent_path().wstring().c_str() );
			ofn.lpstrInitialDir = initialPathStr;
			wcscpy( szFile, initialPath.filename().wstring().c_str() );
		}

		if(save) {
			ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;

			// Display the Save dialog box.
			if( ::GetSaveFileName( &ofn ) == TRUE ) {
				return fs::path( ofn.lpstrFile );
			}
		}
		else {
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box.
			if( ::GetOpenFileNameW( &ofn ) == TRUE ) {
				return fs::path( ofn.lpstrFile );
			}
		}

		return fs::path();
	}

	static std::filesystem::path getOpenFilePath( const std::filesystem::path &initialPath, std::vector<std::string> extensions )
	{
		return getOpenOrSaveFilePath(initialPath, extensions, false);
	}

	static std::filesystem::path getSaveFilePath( const std::filesystem::path &initialPath, std::vector<std::string> extensions )
	{
		return getOpenOrSaveFilePath(initialPath, extensions, true);
	}
};