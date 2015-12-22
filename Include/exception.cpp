#include "Stdafx.h"
#include "exception.h"

namespace module_api
{
	module_api::Exception::Exception( ) : message( L"Exception occurred" )
	{ }

	module_api::Exception::Exception( const std::wstring &message ) : message( message )
	{ }

	std::wstring module_api::Exception::get_message( ) const
	{
		return message;
	}

	module_api::IndexOutOfRangeException::IndexOutOfRangeException( ) : Exception( L"Index out of range" )
	{ }

	module_api::IndexOutOfRangeException::IndexOutOfRangeException( const std::wstring &message ) : Exception( message )
	{ }

	module_api::BadInputException::BadInputException( ) : Exception( L"Invalid method input" )
	{ }

	module_api::BadInputException::BadInputException( const std::wstring &message ) : Exception( message )
	{ }

	module_api::ComputationException::ComputationException( ) : Exception( L"Method computations cannot be continued" )
	{ }

	module_api::ComputationException::ComputationException( const std::wstring &message ) : Exception( message )
	{ }

	module_api::NotImplementedException::NotImplementedException( ) : Exception( L"Functional is not implemented for now" )
	{ }

	module_api::NotImplementedException::NotImplementedException( const std::wstring &message ) : Exception( message )
	{ }

	module_api::UnexpectedException::UnexpectedException( ) : Exception( L"This exception should not have been thrown" )
	{ }

	module_api::UnexpectedException::UnexpectedException( const std::wstring &message ) : Exception( message )
	{ }
}